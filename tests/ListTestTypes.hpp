#include <cstring>

extern "C" {
#include "list.h"
}

// List of strings
int string_compare(const ListData* a, const ListData* b);
void string_free(ListData* s);
ListData* string_copy(const ListData* s);

// List of integers
int int_compare(const ListData * a, const ListData * b);
void int_free(ListData* i);
ListData * int_copy(const ListData* i);
