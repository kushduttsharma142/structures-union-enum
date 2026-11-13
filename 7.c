#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    STATUS_PENDING,
    STATUS_DONE
} TaskStatus;

typedef struct {
    int id;
    char description[100];
    TaskStatus status;
} Task;

typedef struct {
    Task *tasks;
    int count;
    int capacity;
} TaskManager;

typedef enum {
    CMD_ADD,
    CMD_DELETE,
    CMD_VIEW,
    CMD_UPDATE,
    CMD_COMPLETE,
    CMD_FIND,
    CMD_HELP,
    CMD_EXIT,
    CMD_UNKNOWN
} CommandType;

void clear_input_buffer(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {
    }
}

char to_lowercase(char ch) {
    if (ch >= 'A' && ch <= 'Z') {
        return ch + ('a' - 'A');
    }
    return ch;
}

int case_insensitive_equal(const char *s1, const char *s2) {
    if (s1 == NULL || s2 == NULL) {
        return 0;
    }

    while (*s1 != '\0' && *s2 != '\0') {
        if (to_lowercase(*s1) != to_lowercase(*s2)) {
            return 0;
        }
        s1++;
        s2++;
    }

    return (*s1 == '\0' && *s2 == '\0');
}

CommandType get_command(const char *cmd_text) {
    if (cmd_text == NULL) {
        return CMD_UNKNOWN;
    }

    if (case_insensitive_equal(cmd_text, "ADD")) {
        return CMD_ADD;
    }
    if (case_insensitive_equal(cmd_text, "DELETE")) {
        return CMD_DELETE;
    }
    if (case_insensitive_equal(cmd_text, "VIEW")) {
        return CMD_VIEW;
    }
    if (case_insensitive_equal(cmd_text, "UPDATE")) {
        return CMD_UPDATE;
    }
    if (case_insensitive_equal(cmd_text, "COMPLETE")) {
        return CMD_COMPLETE;
    }
    if (case_insensitive_equal(cmd_text, "FIND")) {
        return CMD_FIND;
    }
    if (case_insensitive_equal(cmd_text, "HELP")) {
        return CMD_HELP;
    }
    if (case_insensitive_equal(cmd_text, "EXIT")) {
        return CMD_EXIT;
    }

    return CMD_UNKNOWN;
}

int init_task_manager(TaskManager *tm, int initial_capacity) {
    if (tm == NULL) {
        return 0;
    }

    if (initial_capacity <= 0) {
        initial_capacity = 2;
    }

    tm->tasks = (Task *)malloc((size_t)initial_capacity * sizeof(Task));
    if (tm->tasks == NULL) {
        printf("Error: Memory allocation failed!\n");
        tm->count = 0;
        tm->capacity = 0;
        return 0;
    }

    tm->count = 0;
    tm->capacity = initial_capacity;

    return 1;
}

void free_task_manager(TaskManager *tm) {
    if (tm != NULL && tm->tasks != NULL) {
        free(tm->tasks);
        tm->tasks = NULL;
        tm->count = 0;
        tm->capacity = 0;
    }
}

int find_task(const TaskManager *tm, int id) {
    if (tm == NULL || tm->tasks == NULL) {
        return -1;
    }

    for (int i = 0; i < tm->count; i++) {
        if (tm->tasks[i].id == id) {
            return i;
        }
    }

    return -1;
}

int grow_tasks_if_full(TaskManager *tm) {
    if (tm == NULL) {
        return 0;
    }

    if (tm->count < tm->capacity) {
        return 1;
    }

    if (tm->capacity >= 1000) {
        printf("Error: Maximum task capacity reached.\n");
        return 0;
    }

    int new_capacity = tm->capacity * 2;
    if (new_capacity > 1000) {
        new_capacity = 1000;
    }

    Task *temp = (Task *)realloc(tm->tasks, (size_t)new_capacity * sizeof(Task));
    if (temp == NULL) {
        printf("Error: Memory reallocation failed! Keeping old capacity.\n");
        return 0;
    }

    tm->tasks = temp;
    tm->capacity = new_capacity;
    printf("(Notice: Storage capacity doubled to %d tasks)\n", new_capacity);

    return 1;
}

int add_task(TaskManager *tm, int id, const char *description) {
    if (tm == NULL || description == NULL) {
        return 0;
    }

    if (find_task(tm, id) != -1) {
        printf("Error: Task ID %d already exists! Duplicate ID rejected.\n", id);
        return 0;
    }

    if (!grow_tasks_if_full(tm)) {
        return 0;
    }

    Task *t = &tm->tasks[tm->count];
    t->id = id;
    strncpy(t->description, description, 99);
    t->description[99] = '\0';
    t->status = STATUS_PENDING;

    tm->count++;
    printf("Success: Task %d added with status [PENDING].\n", id);

    return 1;
}

void print_task(const Task *t) {
    if (t == NULL) {
        return;
    }

    const char *status_str = (t->status == STATUS_DONE) ? "DONE" : "PENDING";
    printf("ID: %-5d | Status: %-8s | Description: %s\n", t->id, status_str, t->description);
}

void display_all_tasks(const TaskManager *tm) {
    if (tm == NULL || tm->count == 0) {
        printf("Notice: Task list is currently empty.\n");
        return;
    }

    printf("\n--- Task List (Total: %d, Capacity: %d) ---\n", tm->count, tm->capacity);
    for (int i = 0; i < tm->count; i++) {
        printf("[%d] ", i + 1);
        print_task(&tm->tasks[i]);
    }
}

int update_task(TaskManager *tm, int id, const char *new_description) {
    if (tm == NULL || tm->count == 0) {
        printf("Notice: Task list is empty. Nothing to update.\n");
        return 0;
    }

    int index = find_task(tm, id);
    if (index == -1) {
        printf("Error: Task ID %d not found.\n", id);
        return 0;
    }

    strncpy(tm->tasks[index].description, new_description, 99);
    tm->tasks[index].description[99] = '\0';

    printf("Success: Description for Task %d updated.\n", id);

    return 1;
}

int complete_task(TaskManager *tm, int id) {
    if (tm == NULL || tm->count == 0) {
        printf("Notice: Task list is empty.\n");
        return 0;
    }

    int index = find_task(tm, id);
    if (index == -1) {
        printf("Error: Task ID %d not found.\n", id);
        return 0;
    }

    tm->tasks[index].status = STATUS_DONE;
    printf("Success: Task %d marked as [DONE].\n", id);

    return 1;
}

int delete_task(TaskManager *tm, int id) {
    if (tm == NULL || tm->count == 0) {
        printf("Notice: Task list is empty. Nothing to delete.\n");
        return 0;
    }

    int index = find_task(tm, id);
    if (index == -1) {
        printf("Error: Task ID %d not found.\n", id);
        return 0;
    }

    for (int i = index; i < tm->count - 1; i++) {
        tm->tasks[i] = tm->tasks[i + 1];
    }

    tm->count--;
    printf("Success: Task %d deleted.\n", id);

    return 1;
}

void show_help(void) {
    printf("\n=== Available Commands (Case-Insensitive) ===\n");
    printf("  ADD      - Add a new task\n");
    printf("  DELETE   - Delete an existing task\n");
    printf("  VIEW     - View all tasks\n");
    printf("  UPDATE   - Update a task's description\n");
    printf("  COMPLETE - Mark a task as DONE\n");
    printf("  FIND     - Search and display a task by ID\n");
    printf("  HELP     - Display this help message\n");
    printf("  EXIT     - Free memory and exit\n");
}

int main(void) {
    TaskManager tm;
    if (!init_task_manager(&tm, 2)) {
        return 1;
    }

    printf("Welcome to Interactive Task Manager!\n");
    printf("Type 'HELP' to view available commands.\n");

    char command_input[32];

    while (1) {
        printf("\nTaskManager> ");
        if (scanf("%31s", command_input) != 1) {
            printf("\nInput ended (EOF). Exiting...\n");
            break;
        }

        CommandType cmd = get_command(command_input);

        if (cmd == CMD_ADD) {
            int id = 0;
            char desc[100];

            printf("Enter positive Task ID: ");
            if (scanf("%d", &id) != 1 || id <= 0) {
                printf("Invalid Task ID!\n");
                clear_input_buffer();
                continue;
            }

            if (find_task(&tm, id) != -1) {
                printf("Error: Task ID %d already exists!\n", id);
                continue;
            }

            printf("Enter Task Description: ");
            clear_input_buffer();
            if (scanf(" %99[^\n]", desc) != 1) {
                printf("Invalid description!\n");
                continue;
            }

            add_task(&tm, id, desc);

        } else if (cmd == CMD_DELETE) {
            if (tm.count == 0) {
                printf("Notice: Task list is empty.\n");
                continue;
            }

            int id = 0;
            printf("Enter Task ID to delete: ");
            if (scanf("%d", &id) != 1 || id <= 0) {
                printf("Invalid Task ID!\n");
                clear_input_buffer();
                continue;
            }

            delete_task(&tm, id);

        } else if (cmd == CMD_VIEW) {
            display_all_tasks(&tm);

        } else if (cmd == CMD_UPDATE) {
            if (tm.count == 0) {
                printf("Notice: Task list is empty.\n");
                continue;
            }

            int id = 0;
            printf("Enter Task ID to update: ");
            if (scanf("%d", &id) != 1 || id <= 0) {
                printf("Invalid Task ID!\n");
                clear_input_buffer();
                continue;
            }

            int index = find_task(&tm, id);
            if (index == -1) {
                printf("Task ID %d does not exist.\n", id);
                continue;
            }

            printf("Current task: ");
            print_task(&tm.tasks[index]);

            char new_desc[100];
            printf("Enter New Description: ");
            clear_input_buffer();
            if (scanf(" %99[^\n]", new_desc) != 1) {
                printf("Invalid description!\n");
                continue;
            }

            update_task(&tm, id, new_desc);

        } else if (cmd == CMD_COMPLETE) {
            if (tm.count == 0) {
                printf("Notice: Task list is empty.\n");
                continue;
            }

            int id = 0;
            printf("Enter Task ID to mark complete: ");
            if (scanf("%d", &id) != 1 || id <= 0) {
                printf("Invalid Task ID!\n");
                clear_input_buffer();
                continue;
            }

            complete_task(&tm, id);

        } else if (cmd == CMD_FIND) {
            if (tm.count == 0) {
                printf("Notice: Task list is empty.\n");
                continue;
            }

            int id = 0;
            printf("Enter Task ID to find: ");
            if (scanf("%d", &id) != 1 || id <= 0) {
                printf("Invalid Task ID!\n");
                clear_input_buffer();
                continue;
            }

            int index = find_task(&tm, id);
            if (index != -1) {
                printf("Task found: ");
                print_task(&tm.tasks[index]);
            } else {
                printf("Task ID %d not found.\n", id);
            }

        } else if (cmd == CMD_HELP) {
            show_help();

        } else if (cmd == CMD_EXIT) {
            printf("Exiting Task Manager. Freeing resources...\n");
            break;

        } else {
            printf("Unknown command '%s'. Type 'HELP' to see supported commands.\n", command_input);
        }
    }

    free_task_manager(&tm);

    return 0;
}
