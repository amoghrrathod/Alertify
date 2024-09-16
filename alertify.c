#include <getopt.h>
#include <jansson.h> // Include the Jansson library for JSON handling
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>      // For date comparison
#include <uuid/uuid.h> // For UUID generation

#define FILE_NAME "reminders.json" // JSON file name

// ANSI color codes
#define COLOR_RESET "\033[0m"
#define COLOR_GREEN "\033[32m"   // Completed
#define COLOR_YELLOW "\033[33m"  // Pending
#define COLOR_RED "\033[31m"     // Overdue
#define COLOR_MAGENTA "\033[35m" // Header

void print_usage() {
  printf(
      COLOR_YELLOW
      "Usage: alertify [options]\n" COLOR_RESET "Options:\n"
      " -a, --add REMINDER      Add a new reminder\n"
      " -d, --due DATE          Set due date (YYYY-MM-DD)\n"
      " -p, --priority PRIORITY Set priority (Low, Medium, High)\n"
      " -r, --remove ID         Remove a reminder by ID\n"
      " -u, --update ID         Update a reminder by ID\n"
      " -s, --set-status ID     Set the status of a reminder (Pending, "
      "Completed, Overdue)\n"
      " -x, --reset             Reset the reminders file\n"
      " -b, --backup            Backup reminders to reminders_backup.json\n"
      " -R, --restore           Restore reminders from reminders_backup.json\n"
      " -h, --help              Show this help message\n");
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

void list_reminders() {
  json_t *reminders;
  load_reminders(&reminders);

  size_t index;
  json_t *value;

  if (json_array_size(reminders) == 0) {
    printf(COLOR_YELLOW "No reminders found.\n" COLOR_RESET);
    return;
  }

  json_array_foreach(reminders, index, value) {
    printf("%s%s (Due: %s, Priority: %s)\n", COLOR_GREEN,
           json_string_value(json_object_get(value, "reminder")),
           json_string_value(json_object_get(value, "due")),
           json_string_value(json_object_get(value, "priority")));
  }

  printf(COLOR_RESET); // Reset color after listing
}

void remove_reminder(const char *uuid) {
  json_t *reminders;
  load_reminders(&reminders);

  size_t index;
  json_t *value;

  int found = 0;

  for (index = 0; index < json_array_size(reminders); index++) {
    value = json_array_get(reminders, index);

    if (strcmp(json_string_value(json_object_get(value, "id")), uuid) == 0) {
      found = 1;
      json_array_remove(reminders, index);
      break;
    }
  }

  if (!found) {
    fprintf(stderr, COLOR_RED "No reminder found with ID %s.\n" COLOR_RESET,
            uuid);
    return;
  }

  save_reminders(reminders);

  printf(COLOR_RED "Reminder %s deleted.\n" COLOR_RESET, uuid);

  json_decref(reminders);
}

void update_reminder(const char *uuid, const char *due_date,
                     const char *priority) {
  json_t *reminders;
  load_reminders(&reminders);

  size_t index;
  json_t *value;

  int found = 0;

  for (index = 0; index < json_array_size(reminders); index++) {
    value = json_array_get(reminders, index);

    if (strcmp(json_string_value(json_object_get(value, "id")), uuid) == 0) {
      found = 1;

      if (due_date != NULL) {
        json_object_set(value, "due", json_string(due_date));
      }

      if (priority != NULL) {
        json_object_set(value, "priority", json_string(priority));
      }

      printf(COLOR_GREEN "Reminder %s updated.\n" COLOR_RESET, uuid);
      break;
    }
  }

  if (!found) {
    fprintf(stderr, COLOR_RED "No reminder found with ID %s.\n" COLOR_RESET,
            uuid);
  }

  save_reminders(reminders);

  json_decref(reminders);
}

void set_status(const char *uuid, const char *status) {
  json_t *reminders;
  load_reminders(&reminders);

  size_t index;
  json_t *value;

  int found = 0;

  for (index = 0; index < json_array_size(reminders); index++) {
    value = json_array_get(reminders, index);

    if (strcmp(json_string_value(json_object_get(value, "id")), uuid) == 0) {
      found = 1;
      json_object_set(value, "status", json_string(status));
      break;
    }
  }

  if (!found) {
    fprintf(stderr, COLOR_RED "No reminder found with ID %s.\n" COLOR_RESET,
            uuid);
  } else {
    printf(COLOR_GREEN "Status of reminder %s set to %s.\n" COLOR_RESET, uuid,
           status);
    save_reminders(reminders);
  }

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
  char *reminder = NULL;
  char *due = NULL;
  char *priority = NULL;
  char *uuid = NULL;
  char *status = NULL;

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
      uuid = optarg;
      break;
    case 'u':
      uuid = optarg;
      break;
    case 's':
      uuid = optarg;
      status = argv[optind]; // Get status
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
    if (!due)
      due = "N/A";
    if (!priority)
      priority = "Medium";
    add_reminder(reminder, due, priority);
  }

  if (uuid && status) {
    set_status(uuid, status);
  }

  if (uuid && due && priority) {
    update_reminder(uuid, due, priority);
  }

  if (uuid) {
    remove_reminder(uuid);
  }

  list_reminders();
  return 0;
}
