#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    TYPE_EMPTY,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_CHAR,
    TYPE_STRING
} DataType;

typedef union {
    int int_value;
    float float_value;
    char char_value;
    char *string_value;
} Data;

typedef struct {
    Data data;
    DataType type;
} Container;

void container_init(Container *c) {
    if (c == NULL) {
        return;
    }

    c->type = TYPE_EMPTY;
    c->data.string_value = NULL;
}

void container_clear(Container *c) {
    if (c == NULL) {
        return;
    }

    if (c->type == TYPE_STRING && c->data.string_value != NULL) {
        free(c->data.string_value);
        c->data.string_value = NULL;
    }

    c->type = TYPE_EMPTY;
    c->data.int_value = 0;
}

const char* container_type_name(DataType type) {
    if (type == TYPE_EMPTY) {
        return "EMPTY";
    } else if (type == TYPE_INT) {
        return "INT";
    } else if (type == TYPE_FLOAT) {
        return "FLOAT";
    } else if (type == TYPE_CHAR) {
        return "CHAR";
    } else if (type == TYPE_STRING) {
        return "STRING";
    }

    return "UNKNOWN";
}

void container_print(const Container *c) {
    if (c == NULL) {
        printf("[NULL Container]\n");
        return;
    }

    if (c->type == TYPE_EMPTY) {
        printf("[Type: EMPTY]  (empty)\n");
    } else if (c->type == TYPE_INT) {
        printf("[Type: INT]    %d\n", c->data.int_value);
    } else if (c->type == TYPE_FLOAT) {
        printf("[Type: FLOAT]  %.2f\n", c->data.float_value);
    } else if (c->type == TYPE_CHAR) {
        printf("[Type: CHAR]   '%c'\n", c->data.char_value);
    } else if (c->type == TYPE_STRING) {
        const char *s = (c->data.string_value != NULL) ? c->data.string_value : "";
        printf("[Type: STRING] \"%s\"\n", s);
    } else {
        printf("[Type: UNKNOWN]\n");
    }
}

int container_set_int(Container *c, int value) {
    if (c == NULL) {
        return 0;
    }

    container_clear(c);
    c->data.int_value = value;
    c->type = TYPE_INT;

    return 1;
}

int container_set_float(Container *c, float value) {
    if (c == NULL) {
        return 0;
    }

    container_clear(c);
    c->data.float_value = value;
    c->type = TYPE_FLOAT;

    return 1;
}

int container_set_char(Container *c, char value) {
    if (c == NULL) {
        return 0;
    }

    container_clear(c);
    c->data.char_value = value;
    c->type = TYPE_CHAR;

    return 1;
}

int container_set_string(Container *c, const char *value) {
    if (c == NULL || value == NULL) {
        return 0;
    }

    size_t len = strlen(value);
    char *new_str = (char *)malloc(len + 1);
    if (new_str == NULL) {
        return 0;
    }

    memcpy(new_str, value, len + 1);

    if (c->type == TYPE_STRING && c->data.string_value != NULL) {
        free(c->data.string_value);
    }

    c->data.string_value = new_str;
    c->type = TYPE_STRING;

    return 1;
}

int container_get_int(const Container *c, int *out_val) {
    if (c == NULL || out_val == NULL || c->type != TYPE_INT) {
        return 0;
    }

    *out_val = c->data.int_value;

    return 1;
}

int container_get_float(const Container *c, float *out_val) {
    if (c == NULL || out_val == NULL || c->type != TYPE_FLOAT) {
        return 0;
    }

    *out_val = c->data.float_value;

    return 1;
}

int container_get_char(const Container *c, char *out_val) {
    if (c == NULL || out_val == NULL || c->type != TYPE_CHAR) {
        return 0;
    }

    *out_val = c->data.char_value;

    return 1;
}

int container_get_string(const Container *c, const char **out_val) {
    if (c == NULL || out_val == NULL || c->type != TYPE_STRING) {
        return 0;
    }

    *out_val = c->data.string_value;

    return 1;
}

int container_copy(Container *destination, const Container *source) {
    if (destination == NULL || source == NULL) {
        return 0;
    }

    if (destination == source) {
        return 1;
    }

    if (source->type == TYPE_STRING) {
        if (source->data.string_value == NULL) {
            container_clear(destination);
            return 1;
        }

        size_t len = strlen(source->data.string_value);
        char *copy_buf = (char *)malloc(len + 1);
        if (copy_buf == NULL) {
            return 0;
        }

        memcpy(copy_buf, source->data.string_value, len + 1);

        container_clear(destination);
        destination->data.string_value = copy_buf;
        destination->type = TYPE_STRING;

        return 1;
    }

    container_clear(destination);
    destination->type = source->type;
    destination->data = source->data;

    return 1;
}

int main(void) {
    printf("        SAFE HETEROGENEOUS CONTAINER DEMONSTRATION             \n");

    Container items[6];
    for (int i = 0; i < 6; i++) {
        container_init(&items[i]);
    }

    printf("--- Populating 6 Containers with Mixed Types ---\n");
    container_set_int(&items[0], 10);
    container_set_float(&items[1], 3.14f);
    container_set_char(&items[2], 'A');
    container_set_string(&items[3], "Hello");
    container_set_int(&items[4], 0);
    container_set_string(&items[5], "Data");

    for (int i = 0; i < 6; i++) {
        printf("Index [%d] | Type: %-6s | ", i, container_type_name(items[i].type));
        container_print(&items[i]);
    }

    printf("            DEEP COPY & INDEPENDENCE DEMONSTRATION             \n");

    Container copy_target;
    container_init(&copy_target);

    printf("Deep copying items[5] (\"Data\") into copy_target...\n");
    container_copy(&copy_target, &items[5]);

    printf("Original items[5] before change: ");
    container_print(&items[5]);
    printf("Copy container                 : ");
    container_print(&copy_target);

    printf("\nChanging original items[5] to \"Data Modified\"...\n");
    container_set_string(&items[5], "Data Modified");

    printf("Original items[5] after change : ");
    container_print(&items[5]);
    printf("Copy container (remains intact): ");
    container_print(&copy_target);

    container_clear(&copy_target);

    printf("     TYPE TRANSITIONS (STRING -> INT -> STRING -> FLOAT)       \n");

    Container morph;
    container_init(&morph);

    printf("1. Setting to STRING: \"Initial String\"\n");
    container_set_string(&morph, "Initial String");
    container_print(&morph);

    printf("2. Morphing to INT: 42 (old string memory freed)\n");
    container_set_int(&morph, 42);
    container_print(&morph);

    printf("3. Morphing to STRING: \"Second String\"\n");
    container_set_string(&morph, "Second String");
    container_print(&morph);

    printf("4. Morphing to FLOAT: 99.99 (second string memory freed)\n");
    container_set_float(&morph, 99.99f);
    container_print(&morph);

    container_clear(&morph);

    printf("                   EDGE CASE VERIFICATION                      \n");

    Container test;
    container_init(&test);

    printf("1. Integer zero value check:\n");
    container_set_int(&test, 0);
    int int_val = -1;
    if (container_get_int(&test, &int_val)) {
        printf("   Retrieved integer zero successfully: %d\n", int_val);
    }

    printf("\n2. Wrong-type getter on INT container (asking for float):\n");
    float float_val = 0.0f;
    if (!container_get_float(&test, &float_val)) {
        printf("   Safely rejected with error status.\n");
    }

    printf("\n3. Querying an empty container:\n");
    container_clear(&test);
    int empty_val = 0;
    if (!container_get_int(&test, &empty_val)) {
        printf("   Getter on empty container safely rejected.\n");
    }

    printf("\n4. Storing empty string (\"\"):\n");
    container_set_string(&test, "");
    const char *empty_str = NULL;
    if (container_get_string(&test, &empty_str)) {
        printf("   Empty string retrieved successfully: \"%s\"\n", empty_str);
    }

    printf("\n5. Testing repeated container_clear calls:\n");
    container_clear(&test);
    container_clear(&test);
    container_clear(&test);
    printf("   Repeated clear executed safely with no issues.\n");

    printf("\n6. Testing self-copy:\n");
    container_set_string(&test, "Self Copy String");
    if (container_copy(&test, &test)) {
        printf("   Self-copy successful: ");
        container_print(&test);
    }

    printf("\n7. Replacing string using pointer to its own current string:\n");
    const char *cur_ptr = NULL;
    container_get_string(&test, &cur_ptr);
    if (container_set_string(&test, cur_ptr)) {
        printf("   In-place string replacement successful: ");
        container_print(&test);
    }

    container_clear(&test);
    for (int i = 0; i < 6; i++) {
        container_clear(&items[i]);
    }

    printf("\nAll containers cleared and memory freed successfully.\n");

    return 0;
}
