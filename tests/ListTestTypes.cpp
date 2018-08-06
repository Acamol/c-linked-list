#include <cstring>

// List of strings
int string_compare(const ListData* a, const ListData* b) {
  return strcmp(((char*)a), (char*)b);
}

void string_free(ListData* s) {
  free(s);
}

ListData* string_copy(const ListData* s) {
  char* c = (char*)malloc(sizeof(*c) * strlen((char*)s) + 1);
  return strcpy(c, (char*)s);
}

// List of integers
int int_compare(const ListData * a, const ListData * b) {
  return *(int*)a - *(int*)b;
}

void int_free(ListData* i) {
  free(i);
}

ListData * int_copy(const ListData* i) {
  int* c = (int*)malloc(sizeof(*c));
  *c = *(int*)i;
  return c;
}
