#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int roll_number;
    char name[50];
    int age;
    float gpa;
} Student;

typedef struct {
    Student *students;
    int count;
    int capacity;
} StudentList;

void clear_input_buffer(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {
    }
}

int init_list(StudentList *list, int initial_capacity) {
    if (list == NULL) {
        return 0;
    }

    if (initial_capacity <= 0 || initial_capacity > 5000) {
        printf("Error: Initial capacity must be between 1 and 5000.\n");
        list->students = NULL;
        list->count = 0;
        list->capacity = 0;
        return 0;
    }

    list->students = (Student *)malloc((size_t)initial_capacity * sizeof(Student));
    if (list->students == NULL) {
        printf("Error: Memory allocation failed!\n");
        list->count = 0;
        list->capacity = 0;
        return 0;
    }

    list->count = 0;
    list->capacity = initial_capacity;

    return 1;
}

void free_list(StudentList *list) {
    if (list != NULL && list->students != NULL) {
        free(list->students);
        list->students = NULL;
        list->count = 0;
        list->capacity = 0;
    }
}

int find_student(const StudentList *list, int roll_number) {
    if (list == NULL || list->students == NULL) {
        return -1;
    }

    for (int i = 0; i < list->count; i++) {
        if (list->students[i].roll_number == roll_number) {
            return i;
        }
    }

    return -1;
}

int grow_list_if_full(StudentList *list) {
    if (list == NULL) {
        return 0;
    }

    if (list->count < list->capacity) {
        return 1;
    }

    if (list->capacity >= 5000) {
        printf("Error: Cannot add more students, maximum capacity reached.\n");
        return 0;
    }

    int new_capacity = list->capacity * 2;
    if (new_capacity > 5000) {
        new_capacity = 5000;
    }

    Student *temp = (Student *)realloc(list->students, (size_t)new_capacity * sizeof(Student));
    if (temp == NULL) {
        printf("Error: Memory reallocation failed! Keeping old capacity.\n");
        return 0;
    }

    list->students = temp;
    list->capacity = new_capacity;
    printf("(Notice: Array capacity automatically doubled to %d)\n", new_capacity);

    return 1;
}

int add_student(StudentList *list, int roll_number, const char *name, int age, float gpa) {
    if (list == NULL) {
        return 0;
    }

    if (find_student(list, roll_number) != -1) {
        printf("Error: Roll number %d already exists! Duplicate not allowed.\n", roll_number);
        return 0;
    }

    if (!grow_list_if_full(list)) {
        return 0;
    }

    Student *s = &list->students[list->count];
    s->roll_number = roll_number;
    strncpy(s->name, name, 49);
    s->name[49] = '\0';
    s->age = age;
    s->gpa = gpa;

    list->count++;
    printf("Success: Student with Roll Number %d added.\n", roll_number);

    return 1;
}

void print_student(const Student *s) {
    if (s == NULL) {
        return;
    }

    printf("Roll No: %-6d | Name: %-20s | Age: %-3d | GPA: %.2f\n",
           s->roll_number, s->name, s->age, s->gpa);
}

void display_all_students(const StudentList *list) {
    if (list == NULL || list->count == 0) {
        printf("Notice: Student list is currently empty.\n");
        return;
    }

    printf("\n--- List of All Students (Total: %d, Capacity: %d) ---\n", list->count, list->capacity);
    for (int i = 0; i < list->count; i++) {
        printf("[%d] ", i + 1);
        print_student(&list->students[i]);
    }
}

int update_student(StudentList *list, int roll_number, const char *new_name, int new_age, float new_gpa) {
    if (list == NULL || list->count == 0) {
        printf("Notice: List is empty. Nothing to update.\n");
        return 0;
    }

    int index = find_student(list, roll_number);
    if (index == -1) {
        printf("Error: Student with Roll Number %d not found.\n", roll_number);
        return 0;
    }

    Student *s = &list->students[index];
    strncpy(s->name, new_name, 49);
    s->name[49] = '\0';
    s->age = new_age;
    s->gpa = new_gpa;

    printf("Success: Record for Roll Number %d updated successfully.\n", roll_number);

    return 1;
}

int delete_student(StudentList *list, int roll_number) {
    if (list == NULL || list->count == 0) {
        printf("Notice: List is empty. Nothing to delete.\n");
        return 0;
    }

    int index = find_student(list, roll_number);
    if (index == -1) {
        printf("Error: Student with Roll Number %d not found.\n", roll_number);
        return 0;
    }

    for (int i = index; i < list->count - 1; i++) {
        list->students[i] = list->students[i + 1];
    }

    list->count--;
    printf("Success: Student with Roll Number %d deleted.\n", roll_number);

    return 1;
}

void show_analytics(const StudentList *list) {
    if (list == NULL || list->count == 0) {
        printf("Notice: Analytics unavailable because the student list is empty.\n");
        return;
    }

    int highest_index = 0;
    int lowest_index = 0;
    float total_gpa = 0.0f;

    for (int i = 0; i < list->count; i++) {
        total_gpa += list->students[i].gpa;

        if (list->students[i].gpa > list->students[highest_index].gpa) {
            highest_index = i;
        }

        if (list->students[i].gpa < list->students[lowest_index].gpa) {
            lowest_index = i;
        }
    }

    float average_gpa = total_gpa / (float)list->count;

    printf("\n--- Academic Analytics Summary ---\n");
    printf("Total Students : %d\n", list->count);
    printf("Average GPA    : %.2f\n", average_gpa);
    printf("Highest GPA    : %.2f by %s (Roll No: %d)\n",
           list->students[highest_index].gpa,
           list->students[highest_index].name,
           list->students[highest_index].roll_number);
    printf("Lowest GPA     : %.2f by %s (Roll No: %d)\n",
           list->students[lowest_index].gpa,
           list->students[lowest_index].name,
           list->students[lowest_index].roll_number);
}

void display_sorted_by_gpa(const StudentList *list) {
    if (list == NULL || list->count == 0) {
        printf("Notice: Cannot sort because the list is empty.\n");
        return;
    }

    Student *copy = (Student *)malloc((size_t)list->count * sizeof(Student));
    if (copy == NULL) {
        printf("Error: Could not allocate memory to sort students.\n");
        return;
    }

    for (int i = 0; i < list->count; i++) {
        copy[i] = list->students[i];
    }

    for (int i = 0; i < list->count - 1; i++) {
        for (int j = 0; j < list->count - 1 - i; j++) {
            int swap_needed = 0;

            if (copy[j].gpa < copy[j + 1].gpa) {
                swap_needed = 1;
            } else if (copy[j].gpa == copy[j + 1].gpa) {
                if (copy[j].roll_number > copy[j + 1].roll_number) {
                    swap_needed = 1;
                }
            }

            if (swap_needed == 1) {
                Student temp = copy[j];
                copy[j] = copy[j + 1];
                copy[j + 1] = temp;
            }
        }
    }

    printf("\n--- Students Sorted by GPA (Highest to Lowest) ---\n");
    for (int i = 0; i < list->count; i++) {
        printf("[%d] ", i + 1);
        print_student(&copy[i]);
    }

    free(copy);
}

int main(void) {
    StudentList list;
    list.students = NULL;
    list.count = 0;
    list.capacity = 0;

    int initial_cap = 0;
    printf("Welcome to Student Record Management System\n");

    while (1) {
        printf("Enter initial capacity of student list: ");
        if (scanf("%d", &initial_cap) == 1 && initial_cap > 0) {
            break;
        }
        printf("Please enter a positive integer greater than 0.\n");
        clear_input_buffer();
    }

    if (!init_list(&list, initial_cap)) {
        printf("Program exiting due to initialization failure.\n");
        return 1;
    }

    int choice = 0;

    while (1) {
        printf("             STUDENT MENU                \n");
        printf("1. Add Student\n");
        printf("2. Display All Students\n");
        printf("3. Search Student by Roll Number\n");
        printf("4. Update Student Details\n");
        printf("5. Delete Student Record\n");
        printf("6. View Academic Analytics\n");
        printf("7. Display Students Sorted by GPA\n");
        printf("8. Exit\n");
        printf("Enter your choice (1-8): ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input! Please enter a number between 1 and 8.\n");
            clear_input_buffer();
            continue;
        }

        if (choice == 1) {
            int roll = 0;
            char name[50];
            int age = 0;
            float gpa = 0.0f;

            printf("Enter positive Roll Number: ");
            if (scanf("%d", &roll) != 1 || roll <= 0) {
                printf("Invalid roll number! Must be a positive integer.\n");
                clear_input_buffer();
                continue;
            }

            if (find_student(&list, roll) != -1) {
                printf("Error: Roll number %d already exists!\n", roll);
                continue;
            }

            printf("Enter Student Name: ");
            clear_input_buffer();
            if (scanf(" %49[^\n]", name) != 1) {
                printf("Invalid name!\n");
                continue;
            }

            printf("Enter Age (16 to 100): ");
            if (scanf("%d", &age) != 1 || age < 16 || age > 100) {
                printf("Invalid age! Age must be between 16 and 100.\n");
                clear_input_buffer();
                continue;
            }

            printf("Enter GPA (0.00 to 4.00): ");
            if (scanf("%f", &gpa) != 1 || gpa < 0.0f || gpa > 4.0f) {
                printf("Invalid GPA! GPA must be between 0.00 and 4.00.\n");
                clear_input_buffer();
                continue;
            }

            add_student(&list, roll, name, age, gpa);

        } else if (choice == 2) {
            display_all_students(&list);

        } else if (choice == 3) {
            if (list.count == 0) {
                printf("Notice: Student list is empty.\n");
                continue;
            }

            int roll = 0;
            printf("Enter Roll Number to search: ");
            if (scanf("%d", &roll) != 1 || roll <= 0) {
                printf("Invalid roll number!\n");
                clear_input_buffer();
                continue;
            }

            int index = find_student(&list, roll);
            if (index != -1) {
                printf("Student found:\n");
                print_student(&list.students[index]);
            } else {
                printf("Student with Roll Number %d not found.\n", roll);
            }

        } else if (choice == 4) {
            if (list.count == 0) {
                printf("Notice: Student list is empty.\n");
                continue;
            }

            int roll = 0;
            printf("Enter Roll Number to update: ");
            if (scanf("%d", &roll) != 1 || roll <= 0) {
                printf("Invalid roll number!\n");
                clear_input_buffer();
                continue;
            }

            int index = find_student(&list, roll);
            if (index == -1) {
                printf("Student with Roll Number %d does not exist.\n", roll);
                continue;
            }

            printf("Current details: ");
            print_student(&list.students[index]);

            char new_name[50];
            int new_age = 0;
            float new_gpa = 0.0f;

            printf("Enter New Name: ");
            clear_input_buffer();
            if (scanf(" %49[^\n]", new_name) != 1) {
                printf("Invalid name!\n");
                continue;
            }

            printf("Enter New Age (16 to 100): ");
            if (scanf("%d", &new_age) != 1 || new_age < 16 || new_age > 100) {
                printf("Invalid age! Age must be between 16 and 100.\n");
                clear_input_buffer();
                continue;
            }

            printf("Enter New GPA (0.00 to 4.00): ");
            if (scanf("%f", &new_gpa) != 1 || new_gpa < 0.0f || new_gpa > 4.0f) {
                printf("Invalid GPA! GPA must be between 0.00 and 4.00.\n");
                clear_input_buffer();
                continue;
            }

            update_student(&list, roll, new_name, new_age, new_gpa);

        } else if (choice == 5) {
            if (list.count == 0) {
                printf("Notice: Student list is empty.\n");
                continue;
            }

            int roll = 0;
            printf("Enter Roll Number to delete: ");
            if (scanf("%d", &roll) != 1 || roll <= 0) {
                printf("Invalid roll number!\n");
                clear_input_buffer();
                continue;
            }

            delete_student(&list, roll);

        } else if (choice == 6) {
            show_analytics(&list);

        } else if (choice == 7) {
            display_sorted_by_gpa(&list);

        } else if (choice == 8) {
            printf("Exiting program. Freeing memory...\n");
            break;

        } else {
            printf("Invalid choice! Please select an option from 1 to 8.\n");
        }
    }

    free_list(&list);

    return 0;
}
