#include <gtest/gtest.h>

extern "C" {
#include "list.h"
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

TEST(t_iterator, iterator) {
  List list = list_create(int_copy, int_free, int_compare);
  ListIterator iterator = list_iterator_create(list);
  ASSERT_NE(list, nullptr);
  ASSERT_NE(iterator, nullptr);
  EXPECT_EQ(LIST_ITERATOR_END, list_iterator_next(iterator));
  EXPECT_EQ(LIST_ITERATOR_END, list_iterator_prev(iterator));
  int insert = 0;
  list_push_back(list, &insert);
  EXPECT_EQ(LIST_ITERATOR_SUCCESS, list_iterator_next(iterator));
  EXPECT_EQ(0, *(int*)list_iterator_get(iterator));
  EXPECT_EQ(LIST_ITERATOR_END, list_iterator_next(iterator));
  insert = 1;
  list_push_back(list, &insert);

  for (ListIteratorStatus stat = list_iterator_first(iterator); stat != LIST_ITERATOR_END; stat = list_iterator_next(iterator)) {
    EXPECT_EQ(insert - 1, *(int*)list_iterator_get(iterator));
    ++insert;
  }
  EXPECT_EQ(LIST_EINVAL, list_push_after(list, iterator, &insert)); // iterator pointing to end
  EXPECT_EQ(LIST_SUCCESS, list_push_before(list, iterator, &insert));
  EXPECT_NE(nullptr, list_pop_back(list));
  for (ListIteratorStatus stat = list_iterator_last(iterator); stat != LIST_ITERATOR_END; stat = list_iterator_prev(iterator)) {
    EXPECT_EQ(insert - 2, *(int*)list_iterator_get(iterator));
    --insert;
  }
  EXPECT_EQ(LIST_EINVAL, list_push_before(list, iterator, &insert)); // iterator pointing to start
  EXPECT_EQ(LIST_SUCCESS, list_push_after(list, iterator, &insert));
  EXPECT_NE(nullptr, list_pop_front(list));

  // check again the list
  LIST_FOREACH_FORWARD(int*, i, list) {
    EXPECT_EQ(insert - 1, *i);
    ++insert;
  }

  insert = 1;
  // insert with iterator
  for (size_t i = 0; i < 3; ++i) {
    list_iterator_last(iterator);
    EXPECT_EQ(LIST_SUCCESS, list_push_after(list, iterator, &(++insert)));
  }

  // check again the list
  int count = 0;
  LIST_FOREACH_FORWARD(int*, i, list) {
    EXPECT_EQ(count, *i);
    ++count;
  }

  insert = 0;
  // insert with iterator
  for (size_t i = 0; i < 3; ++i) {
    list_iterator_first(iterator);
    EXPECT_EQ(LIST_SUCCESS, list_push_before(list, iterator, &(--insert)));
  }

  // check again the list
  count = -3;
  LIST_FOREACH_FORWARD(int*, i, list) {
    EXPECT_EQ(count, *i);
    ++count;
  }

  insert = 1;
  list_clear(list);
  list_push_front(list, &insert);
  list_push_back(list, &(insert += 4));
  // insert with iterator
  list_iterator_first(iterator);
  for (size_t i = 2; i < 5; ++i) {
    EXPECT_EQ(LIST_SUCCESS, list_push_after(list, iterator, &i));
    list_iterator_next(iterator);
  }

  // check again the list
  count = 1;
  LIST_FOREACH_FORWARD(int*, i, list) {
    EXPECT_EQ(count, *i);
    ++count;
  }

  // remove the elements we added in the last for-loop with iterator
  list_iterator_first(iterator);
  for (list_iterator_next(iterator); *(int*)list_iterator_get(iterator) < 5; ) {
    EXPECT_EQ(LIST_SUCCESS, list_remove_iterator(list, iterator));
    ++insert;
  }

  // check again the list
  EXPECT_EQ(1, *(int*)list_get_first(list, 0));
  EXPECT_EQ(5, *(int*)list_get_last(list, 0));

  // adding them back with reverse iterator
  list_iterator_last(iterator);
  for (size_t i = 4; i >= 2; --i) {
    EXPECT_EQ(LIST_SUCCESS, list_push_before(list, iterator, &i));
    list_iterator_prev(iterator);
  }

  // check again the list
  count = 1;
  LIST_FOREACH_FORWARD(int*, i, list) {
    EXPECT_EQ(count, *i);
    ++count;
  }

  // range iteration
  // create the iterators
  ListIterator begin = list_iterator_create(list);
  list_iterator_next(begin);
  ListIterator end = list_iterator_create(list);
  list_iterator_last(end);
  int i = 2;
  // the actual loop
  ListIterator it;
  for (it = list_iterator_copy(begin); !list_iterator_equal(it, end); list_iterator_next(it)) {
    EXPECT_EQ(i++, *(int*)list_iterator_get(it));
  }
  list_iterator_destroy(it);
  list_iterator_destroy(begin);
  list_iterator_destroy(end);

  list_iterator_destroy(iterator);
  list_destroy(list);
}
