#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int id;
    float width;
    float height;
    float area;
    float perimeter;
} Rectangle;

typedef struct {
    Rectangle *rectangles;
    int count;
    int capacity;
} RectangleInventory;

void clear_input_buffer(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {
    }
}

float float_abs(float value) {
    if (value < 0.0f) {
        return -value;
    }
    return value;
}

int is_square(float width, float height) {
    float diff = float_abs(width - height);
    if (diff < 0.0001f) {
        return 1;
    }
    return 0;
}

void calculate_geometry(Rectangle *r) {
    if (r == NULL) {
        return;
    }

    r->area = r->width * r->height;
    r->perimeter = 2.0f * (r->width + r->height);
}

int init_inventory(RectangleInventory *inv, int initial_capacity) {
    if (inv == NULL) {
        return 0;
    }

    if (initial_capacity <= 0 || initial_capacity > 1000) {
        printf("Error: Initial capacity must be between 1 and 1000.\n");
        inv->rectangles = NULL;
        inv->count = 0;
        inv->capacity = 0;
        return 0;
    }

    inv->rectangles = (Rectangle *)malloc((size_t)initial_capacity * sizeof(Rectangle));
    if (inv->rectangles == NULL) {
        printf("Error: Memory allocation failed!\n");
        inv->count = 0;
        inv->capacity = 0;
        return 0;
    }

    inv->count = 0;
    inv->capacity = initial_capacity;

    return 1;
}

void free_inventory(RectangleInventory *inv) {
    if (inv != NULL && inv->rectangles != NULL) {
        free(inv->rectangles);
        inv->rectangles = NULL;
        inv->count = 0;
        inv->capacity = 0;
    }
}

int find_rectangle(const RectangleInventory *inv, int id) {
    if (inv == NULL || inv->rectangles == NULL) {
        return -1;
    }

    for (int i = 0; i < inv->count; i++) {
        if (inv->rectangles[i].id == id) {
            return i;
        }
    }

    return -1;
}

int grow_inventory_if_full(RectangleInventory *inv) {
    if (inv == NULL) {
        return 0;
    }

    if (inv->count < inv->capacity) {
        return 1;
    }

    if (inv->capacity >= 1000) {
        printf("Error: Maximum inventory limit of 1000 reached.\n");
        return 0;
    }

    int new_capacity = inv->capacity * 2;
    if (new_capacity > 1000) {
        new_capacity = 1000;
    }

    Rectangle *temp = (Rectangle *)realloc(inv->rectangles, (size_t)new_capacity * sizeof(Rectangle));
    if (temp == NULL) {
        printf("Error: Memory reallocation failed! Keeping old capacity.\n");
        return 0;
    }

    inv->rectangles = temp;
    inv->capacity = new_capacity;
    printf("(Notice: Inventory capacity automatically doubled to %d)\n", new_capacity);

    return 1;
}

int add_rectangle(RectangleInventory *inv, int id, float width, float height) {
    if (inv == NULL) {
        return 0;
    }

    if (find_rectangle(inv, id) != -1) {
        printf("Error: Rectangle ID %d already exists! Duplicate ID rejected.\n", id);
        return 0;
    }

    if (!grow_inventory_if_full(inv)) {
        return 0;
    }

    Rectangle *r = &inv->rectangles[inv->count];
    r->id = id;
    r->width = width;
    r->height = height;
    calculate_geometry(r);

    inv->count++;
    printf("Success: Rectangle ID %d added. Area = %.2f, Perimeter = %.2f (%s)\n",
           id, r->area, r->perimeter, is_square(width, height) ? "Square" : "Non-square");

    return 1;
}

void print_rectangle(const Rectangle *r) {
    if (r == NULL) {
        return;
    }

    printf("ID: %-5d | Width: %-6.2f | Height: %-6.2f | Area: %-7.2f | Perimeter: %-7.2f | Shape: %s\n",
           r->id, r->width, r->height, r->area, r->perimeter,
           is_square(r->width, r->height) ? "Square" : "Non-square");
}

void display_all_rectangles(const RectangleInventory *inv) {
    if (inv == NULL || inv->count == 0) {
        printf("Notice: Rectangle inventory is currently empty.\n");
        return;
    }

    printf("\n--- List of All Rectangles (Total: %d, Capacity: %d) ---\n", inv->count, inv->capacity);
    for (int i = 0; i < inv->count; i++) {
        printf("[%d] ", i + 1);
        print_rectangle(&inv->rectangles[i]);
    }
}

int update_rectangle(RectangleInventory *inv, int id, float new_width, float new_height) {
    if (inv == NULL || inv->count == 0) {
        printf("Notice: Inventory is empty. Nothing to update.\n");
        return 0;
    }

    int index = find_rectangle(inv, id);
    if (index == -1) {
        printf("Error: Rectangle ID %d not found.\n", id);
        return 0;
    }

    Rectangle *r = &inv->rectangles[index];
    r->width = new_width;
    r->height = new_height;
    calculate_geometry(r);

    printf("Success: Rectangle ID %d updated. New Area = %.2f, Perimeter = %.2f (%s)\n",
           id, r->area, r->perimeter, is_square(new_width, new_height) ? "Square" : "Non-square");

    return 1;
}

int delete_rectangle(RectangleInventory *inv, int id) {
    if (inv == NULL || inv->count == 0) {
        printf("Notice: Inventory is empty. Nothing to delete.\n");
        return 0;
    }

    int index = find_rectangle(inv, id);
    if (index == -1) {
        printf("Error: Rectangle ID %d not found.\n", id);
        return 0;
    }

    for (int i = index; i < inv->count - 1; i++) {
        inv->rectangles[i] = inv->rectangles[i + 1];
    }

    inv->count--;
    printf("Success: Rectangle ID %d deleted.\n", id);

    return 1;
}

void show_geometry_summary(const RectangleInventory *inv) {
    if (inv == NULL || inv->count == 0) {
        printf("Notice: Summary unavailable because inventory is empty.\n");
        return;
    }

    int min_idx = 0;
    int max_idx = 0;
    float total_area = 0.0f;
    int square_count = 0;

    for (int i = 0; i < inv->count; i++) {
        float a = inv->rectangles[i].area;
        total_area += a;

        if (a < inv->rectangles[min_idx].area) {
            min_idx = i;
        }

        if (a > inv->rectangles[max_idx].area) {
            max_idx = i;
        }

        if (is_square(inv->rectangles[i].width, inv->rectangles[i].height)) {
            square_count++;
        }
    }

    float avg_area = total_area / (float)inv->count;

    printf("\n--- Geometry Summary Analysis ---\n");
    printf("Total Rectangles : %d (Squares: %d, Non-squares: %d)\n",
           inv->count, square_count, inv->count - square_count);
    printf("Total Area       : %.2f\n", total_area);
    printf("Average Area     : %.2f\n", avg_area);
    printf("Smallest Area    : %.2f (ID: %d, Dimensions: %.2f x %.2f)\n",
           inv->rectangles[min_idx].area, inv->rectangles[min_idx].id,
           inv->rectangles[min_idx].width, inv->rectangles[min_idx].height);
    printf("Largest Area     : %.2f (ID: %d, Dimensions: %.2f x %.2f)\n",
           inv->rectangles[max_idx].area, inv->rectangles[max_idx].id,
           inv->rectangles[max_idx].width, inv->rectangles[max_idx].height);
}

void display_sorted_by_area(const RectangleInventory *inv) {
    if (inv == NULL || inv->count == 0) {
        printf("Notice: Cannot sort because inventory is empty.\n");
        return;
    }

    Rectangle *copy = (Rectangle *)malloc((size_t)inv->count * sizeof(Rectangle));
    if (copy == NULL) {
        printf("Error: Memory allocation failed for sorting.\n");
        return;
    }

    for (int i = 0; i < inv->count; i++) {
        copy[i] = inv->rectangles[i];
    }

    for (int i = 0; i < inv->count - 1; i++) {
        for (int j = 0; j < inv->count - 1 - i; j++) {
            int swap_needed = 0;

            if (copy[j].area > copy[j + 1].area) {
                swap_needed = 1;
            } else if (copy[j].area == copy[j + 1].area) {
                if (copy[j].id > copy[j + 1].id) {
                    swap_needed = 1;
                }
            }

            if (swap_needed == 1) {
                Rectangle temp = copy[j];
                copy[j] = copy[j + 1];
                copy[j + 1] = temp;
            }
        }
    }

    printf("\n--- Rectangles Sorted by Area (Lowest to Highest) ---\n");
    for (int i = 0; i < inv->count; i++) {
        printf("[%d] ", i + 1);
        print_rectangle(&copy[i]);
    }

    free(copy);
}

void filter_by_minimum_area(const RectangleInventory *inv, float min_threshold) {
    if (inv == NULL || inv->count == 0) {
        printf("Notice: Cannot filter because inventory is empty.\n");
        return;
    }

    printf("\n--- Rectangles with Area > %.2f ---\n", min_threshold);
    int matches = 0;

    for (int i = 0; i < inv->count; i++) {
        if (inv->rectangles[i].area > min_threshold) {
            matches++;
            printf("[%d] ", matches);
            print_rectangle(&inv->rectangles[i]);
        }
    }

    if (matches == 0) {
        printf("No rectangles found with area greater than %.2f.\n", min_threshold);
    }
}

int main(void) {
    RectangleInventory inv;
    inv.rectangles = NULL;
    inv.count = 0;
    inv.capacity = 0;

    int initial_cap = 0;
    printf("Welcome to Rectangle Inventory and Geometry Analysis System\n");

    while (1) {
        printf("Enter initial inventory capacity (1 to 1000): ");
        if (scanf("%d", &initial_cap) == 1 && initial_cap > 0 && initial_cap <= 1000) {
            break;
        }
        printf("Invalid input! Please enter an integer between 1 and 1000.\n");
        clear_input_buffer();
    }

    if (!init_inventory(&inv, initial_cap)) {
        printf("Program exiting due to initialization failure.\n");
        return 1;
    }

    int choice = 0;

    while (1) {
        printf("           RECTANGLE MENU                \n");
        printf("1. Add Rectangle\n");
        printf("2. Display All Rectangles\n");
        printf("3. Search Rectangle by ID\n");
        printf("4. Update Rectangle Dimensions\n");
        printf("5. Delete Rectangle by ID\n");
        printf("6. View Geometry Summary Analysis\n");
        printf("7. Display Rectangles Sorted by Area\n");
        printf("8. Filter Rectangles by Area Threshold\n");
        printf("9. Exit\n");
        printf("Enter your choice (1-9): ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input! Please enter a number between 1 and 9.\n");
            clear_input_buffer();
            continue;
        }

        if (choice == 1) {
            int id = 0;
            float width = 0.0f;
            float height = 0.0f;

            printf("Enter positive Rectangle ID: ");
            if (scanf("%d", &id) != 1 || id <= 0) {
                printf("Invalid ID! Must be a positive integer.\n");
                clear_input_buffer();
                continue;
            }

            if (find_rectangle(&inv, id) != -1) {
                printf("Error: Rectangle ID %d is already in use!\n", id);
                continue;
            }

            printf("Enter positive Width: ");
            if (scanf("%f", &width) != 1 || width <= 0.0f) {
                printf("Invalid width! Must be a positive number.\n");
                clear_input_buffer();
                continue;
            }

            printf("Enter positive Height: ");
            if (scanf("%f", &height) != 1 || height <= 0.0f) {
                printf("Invalid height! Must be a positive number.\n");
                clear_input_buffer();
                continue;
            }

            add_rectangle(&inv, id, width, height);

        } else if (choice == 2) {
            display_all_rectangles(&inv);

        } else if (choice == 3) {
            if (inv.count == 0) {
                printf("Notice: Inventory is empty.\n");
                continue;
            }

            int id = 0;
            printf("Enter Rectangle ID to search: ");
            if (scanf("%d", &id) != 1 || id <= 0) {
                printf("Invalid ID!\n");
                clear_input_buffer();
                continue;
            }

            int index = find_rectangle(&inv, id);
            if (index != -1) {
                printf("Rectangle found:\n");
                print_rectangle(&inv.rectangles[index]);
            } else {
                printf("Rectangle with ID %d not found.\n", id);
            }

        } else if (choice == 4) {
            if (inv.count == 0) {
                printf("Notice: Inventory is empty.\n");
                continue;
            }

            int id = 0;
            printf("Enter Rectangle ID to update: ");
            if (scanf("%d", &id) != 1 || id <= 0) {
                printf("Invalid ID!\n");
                clear_input_buffer();
                continue;
            }

            int index = find_rectangle(&inv, id);
            if (index == -1) {
                printf("Rectangle with ID %d not found.\n", id);
                continue;
            }

            printf("Current values: ");
            print_rectangle(&inv.rectangles[index]);

            float new_width = 0.0f;
            float new_height = 0.0f;

            printf("Enter New Width: ");
            if (scanf("%f", &new_width) != 1 || new_width <= 0.0f) {
                printf("Invalid width! Must be a positive number.\n");
                clear_input_buffer();
                continue;
            }

            printf("Enter New Height: ");
            if (scanf("%f", &new_height) != 1 || new_height <= 0.0f) {
                printf("Invalid height! Must be a positive number.\n");
                clear_input_buffer();
                continue;
            }

            update_rectangle(&inv, id, new_width, new_height);

        } else if (choice == 5) {
            if (inv.count == 0) {
                printf("Notice: Inventory is empty.\n");
                continue;
            }

            int id = 0;
            printf("Enter Rectangle ID to delete: ");
            if (scanf("%d", &id) != 1 || id <= 0) {
                printf("Invalid ID!\n");
                clear_input_buffer();
                continue;
            }

            delete_rectangle(&inv, id);

        } else if (choice == 6) {
            show_geometry_summary(&inv);

        } else if (choice == 7) {
            display_sorted_by_area(&inv);

        } else if (choice == 8) {
            if (inv.count == 0) {
                printf("Notice: Inventory is empty.\n");
                continue;
            }

            float threshold = 0.0f;
            printf("Enter minimum area threshold: ");
            if (scanf("%f", &threshold) != 1) {
                printf("Invalid threshold value!\n");
                clear_input_buffer();
                continue;
            }

            filter_by_minimum_area(&inv, threshold);

        } else if (choice == 9) {
            printf("Exiting program. Freeing memory...\n");
            break;

        } else {
            printf("Invalid choice! Please select an option between 1 and 9.\n");
        }
    }

    free_inventory(&inv);

    return 0;
}
