#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef union {
    int i;
    float f;
    char str[20];
} UnionData;

typedef struct {
    int i;
    float f;
    char str[20];
} StructData;

typedef enum {
    VALUE_INT,
    VALUE_FLOAT,
    VALUE_TEXT
} ValueType;

typedef struct {
    UnionData data;
    ValueType tag;
} TaggedRecord;

int set_int(TaggedRecord *rec, int value) {
    if (rec == NULL) {
        return 0;
    }

    rec->data.i = value;
    rec->tag = VALUE_INT;

    return 1;
}

int set_float(TaggedRecord *rec, float value) {
    if (rec == NULL) {
        return 0;
    }

    rec->data.f = value;
    rec->tag = VALUE_FLOAT;

    return 1;
}

int set_text(TaggedRecord *rec, const char *str) {
    if (rec == NULL || str == NULL) {
        return 0;
    }

    if (strlen(str) >= 20) {
        printf("Error: Text length (%zu characters) exceeds maximum limit of 19 characters!\n", strlen(str));
        return 0;
    }

    strncpy(rec->data.str, str, 19);
    rec->data.str[19] = '\0';
    rec->tag = VALUE_TEXT;

    return 1;
}

void display_record(const TaggedRecord *rec) {
    if (rec == NULL) {
        return;
    }

    if (rec->tag == VALUE_INT) {
        printf("[Type: INT]   Value = %d\n", rec->data.i);
    } else if (rec->tag == VALUE_FLOAT) {
        printf("[Type: FLOAT] Value = %.2f\n", rec->data.f);
    } else if (rec->tag == VALUE_TEXT) {
        printf("[Type: TEXT]  Value = \"%s\"\n", rec->data.str);
    } else {
        printf("[Type: UNKNOWN]\n");
    }
}

int main(void) {
    printf("        PART 1: STRUCTURE VS UNION MEMORY LAYOUT               \n");

    StructData sdata;
    UnionData udata;

    printf("Size of UnionData  = %zu bytes\n", sizeof(UnionData));
    printf("Size of StructData = %zu bytes\n", sizeof(StructData));

    size_t offset_i = (size_t)((char *)&sdata.i - (char *)&sdata);
    size_t offset_f = (size_t)((char *)&sdata.f - (char *)&sdata);
    size_t offset_str = (size_t)((char *)&sdata.str - (char *)&sdata);

    printf("\nStructure Member Offsets:\n");
    printf("  sdata.i   offset = %zu bytes (address: %p)\n", offset_i, (void *)&sdata.i);
    printf("  sdata.f   offset = %zu bytes (address: %p)\n", offset_f, (void *)&sdata.f);
    printf("  sdata.str offset = %zu bytes (address: %p)\n", offset_str, (void *)&sdata.str);

    printf("\nUnion Member Addresses (Notice they are identical):\n");
    printf("  udata.i   address = %p\n", (void *)&udata.i);
    printf("  udata.f   address = %p\n", (void *)&udata.f);
    printf("  udata.str address = %p\n", (void *)&udata.str);
    printf("  Base union address: %p\n", (void *)&udata);

    size_t struct_100 = 100 * sizeof(StructData);
    size_t union_100 = 100 * sizeof(UnionData);
    printf("\nMemory required for 100 Structure records: %zu bytes\n", struct_100);
    printf("Memory required for 100 Union records    : %zu bytes\n", union_100);
    printf("Memory saved by using Union             : %zu bytes\n", struct_100 - union_100);

    printf("         PART 2: SAFE TAGGED UNION DEMONSTRATION               \n");
    printf("Size of TaggedRecord = %zu bytes (UnionData + enum tag + padding)\n\n", sizeof(TaggedRecord));

    TaggedRecord records[5];

    set_int(&records[0], 10);
    set_float(&records[1], 220.50f);
    set_text(&records[2], "Union Programming");
    set_int(&records[3], 42);
    set_text(&records[4], "Safe Record");

    for (int i = 0; i < 5; i++) {
        printf("Record [%d]: ", i + 1);
        display_record(&records[i]);
    }

    printf("             PART 3: EDGE CASE TESTING                         \n");

    TaggedRecord test;

    printf("1. Testing valid 19-character string (\"1234567890123456789\"):\n");
    if (set_text(&test, "1234567890123456789")) {
        printf("   Accepted: ");
        display_record(&test);
    }

    printf("\n2. Testing overlong 20-character string (\"12345678901234567890\"):\n");
    if (!set_text(&test, "12345678901234567890")) {
        printf("   Safely rejected without modifying record.\n");
        printf("   Current record state: ");
        display_record(&test);
    }

    printf("\n3. Testing dynamic type transitions on a single record:\n");
    set_int(&test, 100);
    printf("   Set to INT:   ");
    display_record(&test);

    set_text(&test, "Changed to text");
    printf("   Set to TEXT:  ");
    display_record(&test);

    set_float(&test, 99.95f);
    printf("   Set to FLOAT: ");
    display_record(&test);

    printf("                     CONCLUSION                                \n");
    printf("1. Structures allocate separate memory for each member.\n");
    printf("2. Unions allocate shared memory for all members (size = largest member).\n");
    printf("3. A union saves memory when only ONE type of value is needed at a time.\n");
    printf("4. Using an enum tag ensures we only read the currently stored active member.\n");

    return 0;
}
