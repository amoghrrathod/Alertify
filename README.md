# Alertify - A Simple CLI Reminder Tool

**Alertify** is a lightweight and efficient command-line tool to manage reminders. With features such as adding, updating, removing, and listing reminders, it helps you stay on top of your tasks. Written in C, it supports color-coded statuses, including **Pending**, **Completed**, and **Overdue** reminders, offering a clean and easy-to-read output.

## Features
- Add reminders with custom due dates and priority levels.
- List reminders with status and due date tracking.
- Update reminders by serial number.
- Color-coded reminder statuses (Pending, Completed, Overdue).
- Backup and restore reminders easily.
- Works with JSON for simple data management.

## Getting Started

### Prerequisites
Ensure you have the following installed:
- **Homebrew** (for macOS users)
- **Make** and a C compiler (GCC/Clang)

### Installing Alertify

#### Option 1: Using Homebrew (macOS)
```bash
brew install amoghrrathod/alertify/alertify
```
#### Option 2: Manual Installation (All platforms)
##### Clone the repository:
```bash
git clone https://github.com/amoghrrathod/alertify.git
```
##### Build and install Alertify:
```bash
cd alertify
make && sudo make install
```

### Usage
#### Once installed, you can use `alertify` with various options:
```bash
alertify [options]
```

##### Common Commands:
Add a new reminder:
```bash
alertify --add "Buy groceries" --due 2024-10-05 --priority High
```
List all reminders:
```bash
alertify
```
Update an existing reminder:
```bash
alertify --update 1 --due 2024-10-10 --priority Medium
```
Remove a reminder by serial number:
```bash
alertify --remove 1
```
Set the status of a reminder:
```bash
alertify --set-status 2 Completed
```
Backup reminders:
```bash
alertify --backup
```
Restore reminders from a backup:
```bash
alertify --restore
```
### Uninstalling Alertify
#### Option 1: If Installed Using Homebrew
To uninstall Alertify via Homebrew:
```bash
brew uninstall alertify
```
#### Option 2: Manual Uninstall
Remove the installed binary:
```bash
sudo rm -f /usr/local/bin/alertify
rm -rf /path/to/cloned/alertify
```

### License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
