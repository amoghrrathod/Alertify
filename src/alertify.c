#include "../include/jansson/jansson.h" // Include the Jansson library for JSON handling
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>      // Include for time functions
#include <uuid/uuid.h> // For UUID generation

#define FILE_NAME "reminders.json" // JSON file name

// ANSI color codes
#define COLOR_RESET "\033[94m"
#define COLOR_GREEN "\033[32m"   // Completed
#define COLOR_YELLOW "\033[33m"  // Pending
#define COLOR_RED "\033[31m"     // Overdue
#define COLOR_MAGENTA "\033[35m" // Header

void print_usage() {
  printf(COLOR_YELLOW
         "Usage: alertify [options]\n" COLOR_RESET "Options:\n"
         " -a, --add REMINDER Add a new reminder\n"
         " -d, --due DATE Set due date (YYYY-MM-DD)\n"
         " -p, --priority PRIORITY Set priority (Low, Medium, High)\n"
         " -r, --remove SERIAL Remove a reminder by serial number\n"
         " -u, --update SERIAL Update a reminder by serial number\n"
         " -s, --set-status SERIAL Set the status of a reminder (Pending, "
         "Completed, Overdue) by serial number\n"
         " -x, --reset Reset the reminders file\n"
         " -b, --backup Backup reminders to reminders_backup.json\n"
         " -R, --restore Restore reminders from reminders_backup.json\n"
         " -h, --help Show this help message\n");
}

void print_header() {
  printf(COLOR_MAGENTA "==============================\n");
  printf("          Reminders           \n");
  printf("==============================\n" COLOR_RESET);
}

void reset_file() {
  FILE *file = fopen(FILE_NAME, "w");
  if (file) {
    fprintf(file, "[]");
    fclose(file);
    printf(COLOR_GREEN "Reminders file reset successfully.\n" COLOR_RESET);
  } else {
    fprintf(stderr, COLOR_RED "Failed to reset reminders file.\n" COLOR_RESET);
  }
}

void load_reminders(json_t **reminders) {
  json_error_t error;
  *reminders = json_load_file(FILE_NAME, 0, &error);
  if (!*reminders) {
    if (error.line == 1 && error.column == 1) {
      *reminders = json_array(); // Create an empty array if file doesn't exist
      json_dump_file(*reminders, FILE_NAME, 0);
    } else {
      fprintf(stderr, COLOR_RED "Error loading reminders: %s\n" COLOR_RESET,
              error.text);
      exit(1);
    }
  }
}

void save_reminders(json_t *reminders) {
  json_dump_file(reminders, FILE_NAME, JSON_INDENT(4));
}

void generate_uuid(char *uuid_str) {
  uuid_t uuid;
  uuid_generate(uuid);
  uuid_unparse_lower(uuid, uuid_str);
}

void add_reminder(const char *reminder_text, const char *due_date,
                  const char *priority) {
  json_t *reminders;
  load_reminders(&reminders);

  char uuid_str[37]; // UUID string length
  generate_uuid(uuid_str);

  json_t *new_reminder = json_object();
  json_object_set_new(new_reminder, "id", json_string(uuid_str));
  json_object_set_new(new_reminder, "reminder", json_string(reminder_text));
  json_object_set_new(new_reminder, "due", json_string(due_date));
  json_object_set_new(new_reminder, "priority", json_string(priority));
  json_object_set_new(new_reminder, "status", json_string("Pending"));

  json_array_append(reminders, new_reminder);
  save_reminders(reminders);

  printf(COLOR_GREEN "Reminder added: %s\n" COLOR_RESET, reminder_text);

  json_decref(reminders);
}

// Function to check if the date is overdue
int is_overdue(const char *due_date) {
  if (strcmp(due_date, "N/A") == 0) {
    return 0; // Not overdue if no due date
  }

  struct tm tm_due;
  time_t now;
  time_t due;
  double seconds;

  // Convert due_date to struct tm
  strptime(due_date, "%Y-%m-%d", &tm_due);
  due = mktime(&tm_due);

  // Get current time
  time(&now);

  // Calculate difference in seconds
  seconds = difftime(now, due);

  return seconds > 0;
}

void list_reminders() {
  json_t *reminders;
  load_reminders(&reminders);

  size_t index;
  json_t *value;
  int serial_number = 1;

  if (json_array_size(reminders) == 0) {
    printf(COLOR_YELLOW "No reminders found.\n" COLOR_RESET);
    json_decref(reminders);
    return;
  }

  // Temporary array to hold indices of reminders to be removed
  json_t *to_remove = json_array();

  // Print and collect reminders to remove
  json_array_foreach(reminders, index, value) {
    const char *status = json_string_value(json_object_get(value, "status"));
    const char *reminder_text =
        json_string_value(json_object_get(value, "reminder"));
    const char *due_date = json_string_value(json_object_get(value, "due"));
    const char *priority =
        json_string_value(json_object_get(value, "priority"));

    // Determine color based on status
    const char *status_color;

    if (is_overdue(due_date) && strcmp(status, "Pending") == 0) {
      status_color = COLOR_RED; // Overdue reminders in red
    } else {
      status_color = COLOR_YELLOW; // Pending reminders in yellow
    }

    // If status is "Completed", add index to removal list
    if (strcmp(status, "Completed") == 0) {
      json_array_append_new(to_remove, json_integer(index));
    } else {
      printf("%s%02d. %s (Due: %s, Priority: %s, Status: %s)%s\n", status_color,
             serial_number++, reminder_text, due_date, priority, status,
             COLOR_RESET);
    }
  }

  // Remove completed reminders from the main list
  for (size_t i = 0; i < json_array_size(to_remove); i++) {
    size_t remove_index = json_integer_value(json_array_get(to_remove, i));
    json_array_remove(reminders, remove_index);
  }

  save_reminders(reminders);
  json_decref(reminders);
  json_decref(to_remove);
}
void update_reminder_by_serial(int serial_number, const char *due_date,
                               const char *priority) {
  json_t *reminders;
  load_reminders(&reminders);

  size_t index;
  int found = 0;

  for (index = 0; index < json_array_size(reminders); index++) {
    if (index == (size_t)(serial_number - 1)) {
      found = 1;
      json_t *reminder = json_array_get(reminders, index);

      if (due_date != NULL) {
        printf("Updating due date to %s\n", due_date); // Debug
        json_object_set(reminder, "due", json_string(due_date));
      }

      if (priority != NULL) {
        printf("Updating priority to %s\n", priority); // Debug
        json_object_set(reminder, "priority", json_string(priority));
      }

      printf(COLOR_GREEN "Reminder %d updated.\n" COLOR_RESET, serial_number);
      break;
    }
  }

  if (!found) {
    fprintf(stderr,
            COLOR_RED "No reminder found with serial number %d.\n" COLOR_RESET,
            serial_number);
  }

  save_reminders(reminders);
  json_decref(reminders);
}

void set_status_by_serial(int serial_number, const char *status) {
  json_t *reminders;
  load_reminders(&reminders);

  if (serial_number <= 0 || serial_number > json_array_size(reminders)) {
    printf(COLOR_RED "No reminder found with serial number %d.\n" COLOR_RESET,
           serial_number);
    json_decref(reminders);
    return;
  }

  json_t *reminder = json_array_get(reminders, serial_number - 1);
  json_object_set(reminder, "status", json_string(status));

  printf(COLOR_GREEN "Status of reminder %d set to %s.\n" COLOR_RESET,
         serial_number, status);

  save_reminders(reminders);
  json_decref(reminders);
}

void backup_reminders() {
  if (rename(FILE_NAME, "reminders_backup.json") == 0) {
    printf(COLOR_GREEN "Backup successful.\n" COLOR_RESET);
  } else {
    fprintf(stderr, COLOR_RED "Failed to backup reminders.\n" COLOR_RESET);
  }
}

void restore_reminders() {
  if (rename("reminders_backup.json", FILE_NAME) == 0) {
    printf(COLOR_GREEN "Restore successful.\n" COLOR_RESET);
  } else {
    fprintf(stderr, COLOR_RED "Failed to restore reminders.\n" COLOR_RESET);
  }
}
int main(int argc, char *argv[]) {
  print_header(); // Print the header at the start

  FILE *file = fopen(FILE_NAME, "r");
  if (!file) {
    reset_file();
  } else {
    fclose(file);
  }

  int opt;
  int serial_number = -1; // Initialize with an invalid value
  char *reminder = NULL;
  char *due = NULL;
  char *priority = NULL;
  char *status = NULL;

  if (argc == 1) {
    list_reminders();
    return 0;
  }

  static struct option long_options[] = {
      {"add", required_argument, NULL, 'a'},
      {"due", required_argument, NULL, 'd'},
      {"priority", required_argument, NULL, 'p'},
      {"remove", required_argument, NULL, 'r'},
      {"update", required_argument, NULL, 'u'},
      {"set-status", required_argument, NULL, 's'},
      {"backup", no_argument, NULL, 'b'},
      {"restore", no_argument, NULL, 'R'},
      {"reset", no_argument, NULL, 'x'},
      {"help", no_argument, NULL, 'h'},
      {0, 0, 0, 0}};

  while ((opt = getopt_long(argc, argv, "a:d:p:r:u:s:bxRh", long_options,
                            NULL)) != -1) {
    switch (opt) {
    case 'a':
      reminder = optarg;
      break;

    case 'd':
      due = optarg;
      break;

    case 'p':
      priority = optarg;
      break;

    case 'r':
      serial_number = atoi(optarg);
      break;

    case 'u':
      serial_number = atoi(optarg);
      break;

    case 's':
      serial_number = atoi(optarg);
      status = argv[optind];
      break;

    case 'b':
      backup_reminders();
      exit(0);

    case 'R':
      restore_reminders();
      exit(0);

    case 'x':
      reset_file();
      exit(0);

    case 'h':
    default:
      print_usage();
      exit(0);
    }
  }

  if (reminder) {
    add_reminder(reminder, due ? due : "N/A", priority ? priority : "Medium");
  }

  if (serial_number > 0) {
    if (status) {
      set_status_by_serial(serial_number, status);
    } else {
      update_reminder_by_serial(serial_number, due, priority);
    }
  }

  list_reminders();
  return 0;
}
