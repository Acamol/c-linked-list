#include <gtest/gtest.h>

#include <string>
#include <cstring>

extern "C" {
#include "list.h"
}

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


TEST(t_list, general_correctness) {
  List list = list_create(string_copy, string_free, string_compare);
  ASSERT_NE(list, nullptr);
  EXPECT_EQ(0, list_get_size(list));
  EXPECT_EQ(LIST_SUCCESS, list_push_front(list, "The fox is in the hat"));
  EXPECT_EQ(1, list_get_size(list));
  EXPECT_TRUE(strcmp("The fox is in the hat", (char*)list_get_first(list, 0)) == 0);
  EXPECT_TRUE(strcmp("The fox is in the hat", (char*)list_get_last(list, 0)) == 0);
  EXPECT_EQ(LIST_SUCCESS, list_remove(list, list_get_last(list, 0)));
  EXPECT_EQ(0, list_get_size(list));
  for (std::size_t i = 0; i < 10000; ++i) {
    std::string s("string #" + std::to_string(i));
    list_push_front(list, s.c_str());
  }
  EXPECT_EQ(10000, list_get_size(list));

  ListIterator iterator = list_iterator_create(list);
  EXPECT_STREQ("string #9999", (char*)list_iterator_get(iterator));
  EXPECT_STREQ("string #9999", (char*)list_get_first(list, iterator));
  EXPECT_STREQ("string #9998", (char*)list_get_next(list, iterator));
  EXPECT_STREQ("string #9998", (char*)list_iterator_get(iterator));
  EXPECT_STREQ("string #0", (char*)list_get_last(list, iterator));
  EXPECT_STREQ("string #0", (char*)list_iterator_get(iterator));
  EXPECT_STREQ("string #1", (char*)list_get_prev(list, iterator));
  EXPECT_STREQ("string #1", (char*)list_iterator_get(iterator));

  EXPECT_STREQ("string #0", (char*)list_get_last(list, iterator));
  EXPECT_EQ(0, (char*)list_get_next(list, iterator));
  EXPECT_STREQ("string #0", (char*)list_get_prev(list, iterator));

  std::size_t i = 10000;
  LIST_FOREACH_FORWARD(char*, it, list) {
    --i;
    std::string s("string #" + std::to_string(i));
    EXPECT_STREQ(s.c_str(), it);
  }

  LIST_FOREACH_BACKWARD(char*, it, list) {
    std::string s("string #" + std::to_string(i));
    EXPECT_STREQ(s.c_str(), it);
    ++i;
  }

  list_clear(list);
  EXPECT_TRUE(list_empty(list));

  list_iterator_destroy(iterator);
  list_destroy(list);
}

TEST(t_list, sort) {
  List list = list_create(int_copy, int_free, int_compare);
  ASSERT_NE(list, nullptr);
  for (size_t i = 0; i < 50; ++i) {
    int tmp = i % 11;
    list_push_front(list, &tmp);
  }

  list_sort(list);
  int prev = -1;
  LIST_FOREACH_FORWARD(int*, iterator, list) {
    EXPECT_LE(prev, *iterator);
    prev = *iterator;
  }

  list_destroy(list);
}

TEST(t_list, find) {
  int num[] = { 15, 17, -1, 3, 19, 4 };
  size_t num_size = sizeof(num) / sizeof(num[0]);
  List list = list_create(int_copy, int_free, int_compare);
  ASSERT_NE(list, nullptr);
  for (size_t i = 0; i < num_size; ++i) {
    EXPECT_EQ(LIST_SUCCESS, list_push_back(list, &num[i]));
  }

  for (size_t i = 0; i < num_size; ++i) {
    EXPECT_NE(list_find(list, &num[i]), nullptr);
  }
  int tmp = 666;
  EXPECT_EQ(list_find(list, &tmp), nullptr);

  list_destroy(list);
}

TEST(t_list, push_at) {
  List list = list_create(int_copy, int_free, int_compare);
  ASSERT_NE(list, nullptr);
  int num[] = { 1, 2, 4, 5, 6 };
  size_t num_size = sizeof(num) / sizeof(num[0]);
  for (size_t i = 0; i < num_size; ++i) {
    list_push_back(list, &num[i]);
  }

  int insert = 3;
  ASSERT_EQ(LIST_SUCCESS, list_push_at(list, 2, &insert));

  int i = 1;
  LIST_FOREACH_FORWARD(int*, iterator, list) {
    EXPECT_EQ(i++, *iterator);
  }

  list_destroy(list);
}

TEST(t_list, remove_at) {
  List list = list_create(int_copy, int_free, int_compare);
  ASSERT_NE(list, nullptr);
  int num[] = { 0, 1, 2, 3, 4, 5, 6 };
  size_t num_size = sizeof(num) / sizeof(num[0]);
  for (size_t i = 0; i < num_size; ++i) {
    list_push_back(list, &num[i]);
  }

  EXPECT_EQ(LIST_SUCCESS, list_remove_at(list, 0)); // 1->2->3->4->5->6
  EXPECT_EQ(1, *(int*)list_get_first(list, 0));
  EXPECT_EQ(LIST_SUCCESS, list_remove_at(list, list_get_size(list) - 1)); // 1->2->3->4->5
  EXPECT_EQ(5, *(int*)list_get_last(list, 0));
  EXPECT_EQ(LIST_SUCCESS, list_remove_at(list, 2)); // 1->2->4->5

  int res[] = { 1, 2, 4, 5 };
  size_t i = 0;
  LIST_FOREACH_FORWARD(int*, iterator, list) {
    EXPECT_EQ(res[i++], *iterator);
  }

  i = 3;
  ASSERT_EQ(LIST_SUCCESS, list_push_at(list, 2, &i)); // 1->2->3->4->5
  i = 1;
  LIST_FOREACH_FORWARD(int*, iterator, list) {
    EXPECT_EQ(i++, *iterator);
  }

  list_destroy(list);
}

TEST(t_list, get_at) {
  List list = list_create(int_copy, int_free, int_compare);
  ASSERT_NE(list, nullptr);
  int num[] = { 0, 1, 2, 3, 4, 5, 6 };
  size_t num_size = sizeof(num) / sizeof(num[0]);
  for (size_t i = 0; i < num_size; ++i) {
    list_push_back(list, &num[i]);
  }

  EXPECT_EQ(0, *(int*)list_get_at(list, 0));
  EXPECT_EQ(6, *(int*)list_get_at(list, 6));
  EXPECT_EQ(4, *(int*)list_get_at(list, 4));

  list_destroy(list);
}

TEST(t_list, pop_and_push) {
  List list = list_create(int_copy, int_free, int_compare);
  ASSERT_NE(list, nullptr);
  for (size_t i = 0; i < 50; ++i) {
    EXPECT_EQ(LIST_SUCCESS, list_push_back(list, &i));
  }
  EXPECT_EQ(50, list_get_size(list));
  EXPECT_EQ(25, *(int*)list_get_at(list, 25));

  list_clear(list);
  EXPECT_EQ(0, list_get_size(list));

  for (size_t i = 0; i < 50; ++i) {
    EXPECT_EQ(LIST_SUCCESS, list_push_front(list, &i));
  }
  EXPECT_EQ(50, list_get_size(list));
  EXPECT_EQ(19, *(int*)list_get_at(list, 30));

  list_clear(list);
  int i = 12;
  list_push_front(list, &i);
  ListIterator iterator = list_iterator_create(list);
  list_remove_iterator(list, iterator);

  list_iterator_destroy(iterator);
  list_destroy(list);
}
