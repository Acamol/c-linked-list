/*
*  MIT License
*
*  Copyright (c) 2018 Aviad Gafni
*
*  Permission is hereby granted, free of charge, to any person obtaining a copy
*  of this software and associated documentation files (the "Software"), to deal
*  in the Software without restriction, including without limitation the rights
*  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*  copies of the Software, and to permit persons to whom the Software is
*  furnished to do so, subject to the following conditions:
*
*  The above copyright notice and this permission notice shall be included in all
*  copies or substantial portions of the Software.
*
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*  SOFTWARE.
*/

/*
* list.c
*
*  Created on: Dec 12, 2016
*      Author: Aviad Gafni
*/

#include <stdlib.h> // malloc, free
#include "list.h"
#include "listConfig.h"

typedef struct node_t {
  ListData* data;
  struct node_t *next, *prev;
} Node;

struct list_t {
  size_t size;
  ListCopyFunction data_copy;
  ListFreeFunction data_free;
  ListCompareFunction data_compare;
  Node* iterator;
  Node* head;
};

struct list_iterator_t {
  List list;
  Node * node;
  bool end_edge;   // iterator reached edges of list
  bool start_edge;
};


/******************************************************************************
*                    Functions that works on a node                           *
******************************************************************************/

typedef enum {
  NODE_SUCCESS,
  NODE_NO_MEM,
  NODE_EINVAL
} NodeStatus;

static Node* node_create() {
  Node* new = malloc(sizeof(*new));
  if (new == 0) {
    return 0;
  }

  new->data = new->prev = new->next = 0;

  return new;
}

static void node_destroy(Node* node, ListFreeFunction data_free) {
  if (node != 0) {
    // avoid freeing when there's nothing to free.
    // maybe the user supplied "data_free" function cannot handle NULL pointer.
    if (node->data != 0)
      data_free(node->data);
    free(node);
  }
}

// not used at the moment.
static Node* node_copy(Node* to_copy, ListCopyFunction data_copy, ListFreeFunction data_free) {
  if (to_copy == 0 || data_copy == 0) {
    return 0;
  }

  Node* new = node_create();
  if (new == 0) {
    return 0;
  }

  new->data = data_copy(to_copy->data);
  if (new->data == 0) {
    node_destroy(new, data_free);
  }

  return new;
}

static NodeStatus node_set(Node* node, const ListData* data, ListCopyFunction data_copy) {
  if (node == 0 || data == 0 || data_copy == 0) {
    return NODE_EINVAL;
  }

  node->data = data_copy(data);
  if (node->data == 0) {
    return NODE_NO_MEM;
  }

  return NODE_SUCCESS;
}


/******************************************************************************
*                  Functions that works on a list                             *
******************************************************************************/

static Node* __list_get_first(List list) {
  return list->head->next;
}

static Node* __list_get_last(List list) {
  return list->head->prev;
}

// this is used internally to iterate over all the nodes in the list.
#define list_foreach(iterator, list) \
	for (iterator = __list_get_first(list); \
		iterator->data != 0;\
		iterator = iterator->next )

static Node* __find_node(const List list, const ListData* data) {
  Node* iterator = 0;
  list_foreach(iterator, list) {
    if (list->data_compare(data, iterator->data) == 0) {
      break;
    }
  }

  return iterator;
}

static ListStatus NodeStatus_to_ListStatus(NodeStatus status) {
  switch (status) {
  case NODE_SUCCESS:
    return LIST_SUCCESS;
  case NODE_NO_MEM:
    return LIST_NO_MEM;
  case NODE_EINVAL:
    return LIST_EINVAL;
  default:
    return LIST_SUCCESS;
  }
}

List list_create(ListCopyFunction data_copy, ListFreeFunction data_free, ListCompareFunction data_compare) {
  if (data_copy == 0 || data_free == 0 || data_compare == 0) {
    return 0;
  }

  List new_list = malloc(sizeof(*new_list));
  if (new_list == 0) {
    return 0;
  }

  new_list->size = 0;
  new_list->iterator = new_list->head = 0;
  new_list->data_copy = data_copy;
  new_list->data_free = data_free;
  new_list->data_compare = data_compare;
  new_list->head = node_create();
  if (new_list->head == 0) {
    free(new_list);
    return 0;
  }
  new_list->head->next = new_list->head->prev = new_list->head;
  // list head data is initialized already to NULL pointer - this is very
  // important, since in order to destroy the head without any issues free
  // needs a NULL pointer.
  new_list->iterator = new_list->head;

  return new_list;
}

ListData * list_get_first(const List list, ListIterator iterator) {
  if (list == 0) {
    return 0;
  }

  list->iterator = __list_get_first(list);
  if (iterator != 0) {
    if (iterator->list != list) {
      return 0;
    }

    iterator->node = list->iterator;
    if (iterator->node == list->head) {
      iterator->start_edge = true;
    } else {
      iterator->start_edge = false;
    }
  }

  // if list->iterator points to the head it's ok, since head's data is
  // always NULL.
  return list->iterator->data;
}

ListData * list_get_last(const List list, ListIterator iterator) {
  if (list == 0) {
    return 0;
  }

  list->iterator = __list_get_last(list);;
  if (iterator != 0) {
    if (iterator->list != list) {
      return 0;
    }

    iterator->node = list->iterator;
    if (iterator->node == list->head) {
      iterator->end_edge = true;
    } else {
      iterator->end_edge = false;
    }
  }

  // if list->iterator points to the head it's ok, since head's data is
  // always NULL.
  return list->iterator->data;
}

ListData * list_get_next(const List list, ListIterator iterator) {
  // note that if we reached to end of list, the iterator does not advance,
  // and we return NULL.
  if (list == 0) {
    return 0;
  }

  if (iterator != 0) {
    // if the iterator is on an edge, we return NULL pointer.
    if (list != iterator->list || iterator->end_edge) {
      return 0;
    }

    iterator->node = iterator->node->next;
    if (iterator->node == list->head) {
      iterator->end_edge = true;
    }
  }

  list->iterator = list->iterator->next;

  // if list->iterator points to the head it's ok, since head's data is
  // always NULL.
  return list->iterator->data;
}

ListData * list_get_prev(const List list, ListIterator iterator) {
  // note that if we reached to start of list, the iterator does not regress,
  // and we return NULL.
  if (list == 0) {
    return 0;
  }

  if (iterator != 0) {
    // if the iterator is on an edge, we return NULL pointer.
    if (list != iterator->list || iterator->start_edge) {
      return 0;
    }

    iterator->node = iterator->node->prev;
    if (iterator->node == list->head) {
      iterator->start_edge = true;
    }
  }

  list->iterator = list->iterator->prev;

  // if list->iterator points to the head it's ok, since head's data is
  // always NULL.
  return list->iterator->data;
}

ListData * list_get_at(const List list, size_t n) {
  if (n >= list->size) {
    return 0;
  }

  Node * iterator;
  list_foreach(iterator, list) {
    if (n-- == 0) {
      break;
    }
  }

  return iterator->data;
}

ListStatus list_push_front(List list, const ListData* data) {
  if (list == 0 || data == 0) {
    return LIST_EINVAL;
  }

  ListIterator iterator = list_iterator_create(list);
  if (iterator == 0) {
    return LIST_NO_MEM;
  }

  iterator->node = iterator->list->head;
  ListStatus res = list_push_after(list, iterator, data);
  list_iterator_destroy(iterator);

  return res;
}

ListStatus list_push_back(List list, const ListData* data) {
  if (list == 0 || data == 0) {
    return LIST_EINVAL;
  }

  ListIterator iterator = list_iterator_create(list);
  if (iterator == 0) {
    return LIST_NO_MEM;
  }

  iterator->node = iterator->list->head;
  iterator->start_edge = false;
  ListStatus res = list_push_before(list, iterator, data);
  list_iterator_destroy(iterator);

  return res;
}

ListStatus list_push_after(List list, const ListIterator iterator, const ListData* data) {
  if (list == 0 || data == 0 || iterator == 0 || list != iterator->list || iterator->end_edge) {
    return LIST_EINVAL;
  }

  Node* new = node_create();
  if (new == 0) {
    return LIST_NO_MEM;
  }

  NodeStatus res = node_set(new, data, list->data_copy);
  if (res != NODE_SUCCESS) {
    node_destroy(new, list->data_free);
    return NodeStatus_to_ListStatus(res);
  }

  // link the new node
  Node * next = iterator->node->next;
  next->prev = new;
  new->next = next;
  new->prev = iterator->node;
  iterator->node->next = new;
  ++list->size;

  return LIST_SUCCESS;
}

ListStatus list_push_before(List list, const ListIterator iterator, const ListData* data) {
  if (list == 0 || data == 0 || iterator == 0 || list != iterator->list || iterator->start_edge) {
    return LIST_EINVAL;
  }

  Node* new = node_create();
  if (new == 0) {
    return LIST_NO_MEM;
  }

  NodeStatus res = node_set(new, data, list->data_copy);
  if (res != NODE_SUCCESS) {
    node_destroy(new, list->data_free);
    return NodeStatus_to_ListStatus(res);
  }

  Node* prev = iterator->node->prev;
  prev->next = new;
  new->prev = prev;
  new->next = iterator->node;
  iterator->node->prev = new;
  ++list->size;

  return LIST_SUCCESS;
}


ListStatus list_push_at(List list, size_t n, const ListData * data) {
  if (n == 0) {
    return list_push_front(list, data);
  }

  if (list == 0 || data == 0 || list->size < n) {
    return LIST_EINVAL;
  }

  Node * new = node_create();
  if (new == 0) {
    return LIST_NO_MEM;
  }

  NodeStatus res = node_set(new, data, list->data_copy);
  if (res != NODE_SUCCESS) {
    node_destroy(new, list->data_free);
    return NodeStatus_to_ListStatus(res);
  }


  Node * iterator;
  list_foreach(iterator, list) {
    if (n-- == 1) {
      break;
    }
  }

  new->next = iterator->next;
  new->prev = iterator;
  iterator->next = new;
  iterator->next->prev = new;
  ++list->size;

  return LIST_SUCCESS;
}

ListStatus list_remove(List list, const ListData* data) {
  if (list == 0 || data == 0) {
    return LIST_EINVAL;
  }

  Node* iterator = 0;
  if ((iterator = __find_node(list, data)) == 0) {
    return LIST_NOT_FOUND;
  }

  if (list->iterator == iterator) {
    list->iterator = list->iterator->next;
  }

  Node* next = iterator->next;
  Node* prev = iterator->prev;
  prev->next = next;
  next->prev = prev;

  node_destroy(iterator, list->data_free);
  --list->size;

  return LIST_SUCCESS;
}

ListData * list_pop_front(List list) {
  if (list == 0 || list->size == 0) {
    return 0;
  }

  Node * first_node = __list_get_first(list);
  list->head->next = first_node->next;
  first_node->next->prev = list->head;
  ListData * data = first_node->data;
  free(first_node);
  --list->size;

  return data;
}

ListData * list_pop_back(List list) {
  if (list == 0 || list->size == 0) {
    return 0;
  }

  Node * last_node = __list_get_last(list);
  list->head->prev = last_node->prev;
  last_node->prev->next = list->head;
  ListData * data = last_node->data;
  free(last_node);
  --list->size;

  return data;
}

ListStatus list_remove_at(List list, size_t n) {
  if (list == 0 || n >= list->size) {
    return LIST_EINVAL;
  }

  Node * iterator;
  list_foreach(iterator, list) {
    if (n-- == 0) {
      break;
    }
  }

  // fix the iterator if it points to the removed element
  if (list->iterator == iterator) {
    list->iterator = list->iterator->next;
  }

  iterator->prev->next = iterator->next;
  iterator->next->prev = iterator->prev;
  node_destroy(iterator, list->data_free);
  --list->size;

  return LIST_SUCCESS;
}

ListStatus list_remove_iterator(List list, ListIterator iterator) {
  if (list == 0 || iterator == 0 || list != iterator->list) {
    return LIST_EINVAL;
  }

  Node * prev = iterator->node->prev;
  Node * next = iterator->node->next;
  prev->next = next;
  next->prev = prev;
  node_destroy(iterator->node, list->data_free);
  --list->size;

  // fix the iterator to point to next element
  iterator->node = next;
  if (iterator->node == list->head) {
    iterator->end_edge = true;
  }

  return LIST_SUCCESS;
}

void list_clear(List list) {
  if (list != 0) {
    Node *to_delete;
    list->iterator = list->head->next;
    while (list->iterator != list->head) {
      to_delete = list->iterator;
      list->iterator = list->iterator->next;
      node_destroy(to_delete, list->data_free);
    }

    // finished freeing. now fix the list to empty.
    list->head->next = list->head->prev = list->head;
    list->size = 0;
    list->iterator = list->head;
  }
}

void list_destroy(List list) {
  if (list != 0) {
    list_clear(list);
    node_destroy(list->head, list->data_free);
    free(list);
  }
}

List list_copy(const List list) {
  if (list == 0) {
    return 0;
  }

  List new = list_create(list->data_copy, list->data_free, list->data_compare);
  if (new == 0) {
    return 0;
  }

  Node* iterator;
  list_foreach(iterator, list) {
    if (list_push_back(new, iterator->data) != LIST_SUCCESS) {
      list_destroy(new);
      return 0;
    }
  }

  return new;
}

ListData const * list_find(const List list, const ListData * data) {
  if (list == 0 || data == 0) {
    return 0;
  }

  return __find_node(list, data)->data;
}


static ListData ** __merge(ListData ** a, size_t size_a, ListData ** b, size_t size_b, ListCompareFunction data_compare) {
  size_t a_idx = 0, b_idx = 0, merged_idx = 0;
  ListData ** merged = malloc(sizeof(*merged) * (size_a + size_b));
  if (merged == 0) {
    return 0;
  }

  while (a_idx < size_a && b_idx < size_b) {
    if (data_compare(a[a_idx], b[b_idx]) <= 0) {
      merged[merged_idx++] = a[a_idx++];
    } else {
      merged[merged_idx++] = b[b_idx++];
    }
  }

  while (a_idx < size_a) merged[merged_idx++] = a[a_idx++];
  while (b_idx < size_b) merged[merged_idx++] = b[b_idx++];

  return merged;
}

// return 0 in case of success and 1 in case of failure (bad allocation).
static int __merge_sort(ListData ** a, size_t size, ListCompareFunction data_compare) {
  if (size <= 1) return 0;

  int res = 0;
  res += __merge_sort(a, size / 2, data_compare);
  res += __merge_sort(a + size / 2, size - size / 2, data_compare);
  ListData ** sorted = __merge(a, size / 2, a + size / 2, size - size / 2, data_compare);
  if (sorted == 0) {
    return 1;
  }
  for (size_t i = 0; i < size; ++i) {
    a[i] = sorted[i];
  }

  free(sorted);
  return (res == 0 ? 0 : 1); // 0 for success, 1 for failure
}

// the idea is to transfer the list elements to an array, sort the array using
// merge-sort, and then update the list nodes using the sorted array.
// this gives an O(n*log(n)) worst case sorting to the list.
// the additional mess is due to memory managment, and asserting that in case
// of an error, the list stays intact.
ListStatus list_sort(List list) {
  if (list == 0) {
    return LIST_EINVAL;
  }

  List sorted_list = list_copy(list);
  if (sorted_list == 0) {
    return LIST_FAIL;
  }

  // from this point onward, we only use sorted_list.
  // this is done to ensure that if some error should occur,
  // the original list will stay intact.
  ListData ** listArray = malloc(sizeof(*listArray) * sorted_list->size);
  if (listArray == 0) {
    list_destroy(sorted_list);
    return LIST_FAIL;
  }

  Node* iterator;
  size_t i = 0;
  list_foreach(iterator, sorted_list) {
    listArray[i++] = sorted_list->data_copy(iterator->data);
    if (listArray[i - 1] == 0) {
      for (size_t j = 0; j < i - 1; ++j) {
        sorted_list->data_free(listArray[j]);
      }
      free(listArray);
      list_destroy(sorted_list);
      return LIST_FAIL;
    }
  }

  int res = __merge_sort(listArray, sorted_list->size, sorted_list->data_compare);

  // if res == 1, some error occurred during __merge_sort
  if (res) {
    free(listArray);
    list_destroy(sorted_list);
    return LIST_FAIL;
  }

  // update sorted_list (to be sorted)
  i = 0;
  list_foreach(iterator, sorted_list) {
    sorted_list->data_free(iterator->data);
    iterator->data = listArray[i++];
  }
  free(listArray);

  // sorting went fine. it is safe to swap the lists.
  Node * tmp_head = list->head;
  list->head = sorted_list->head;
  list->iterator = list->head;
  sorted_list->head = tmp_head;
  list_destroy(sorted_list);

  return LIST_SUCCESS;
}

size_t list_get_size(const List list) {
  return list->size;
}

bool list_empty(const List list) {
  return list_get_size(list) == 0;
}



/******************************************************************************
*               Functions that works on iterator                              *
******************************************************************************/

ListIterator list_iterator_create(const List list) {
  if (list == 0) {
    return 0;
  }

  ListIterator iterator = malloc(sizeof(*iterator));
  if (iterator == 0) {
    return 0;
  }

  iterator->list = list;
  iterator->node = __list_get_first(list);
  iterator->end_edge = false;
  iterator->start_edge = (list->size == 0) ? true : false;

  return iterator;
}

ListIterator list_iterator_copy(const ListIterator iterator) {
  if (iterator == 0) {
    return 0;
  }

  ListIterator new = malloc(sizeof(*new));
  if (new == 0) {
    return 0;
  }

  new->list = iterator->list;
  new->node = iterator->node;
  new->start_edge = iterator->start_edge;
  new->end_edge = iterator->end_edge;

  return new;
}

ListIteratorStatus list_iterator_first(ListIterator iterator) {
  if (iterator == 0) {
    return LIST_ITERATOR_EINVAL;
  }

  iterator->node = __list_get_first(iterator->list);
  if (iterator->node == iterator->list->head) {
    iterator->start_edge = true;
    iterator->end_edge = false;
    return LIST_ITERATOR_END;
  }
  iterator->start_edge = false;
  iterator->end_edge = false;

  return LIST_ITERATOR_SUCCESS;
}

ListIteratorStatus list_iterator_last(ListIterator iterator) {
  if (iterator == 0) {
    return LIST_ITERATOR_EINVAL;
  }

  iterator->node = __list_get_last(iterator->list);
  if (iterator->node == iterator->list->head) {
    iterator->start_edge = false;
    iterator->end_edge = true;
    return LIST_ITERATOR_END;
  }
  iterator->start_edge = false;
  iterator->end_edge = false;

  return LIST_ITERATOR_SUCCESS;
}

ListIteratorStatus list_iterator_next(ListIterator iterator) {
  if (iterator == 0 || iterator->end_edge) {
    return LIST_ITERATOR_EINVAL;
  }

  iterator->start_edge = false;
  iterator->node = iterator->node->next;
  if (iterator->node == iterator->list->head) {
    iterator->end_edge = true;
    return LIST_ITERATOR_END;
  }

  return LIST_ITERATOR_SUCCESS;
}

ListIteratorStatus list_iterator_prev(ListIterator iterator) {
  if (iterator == 0 || iterator->start_edge) {
    return LIST_ITERATOR_EINVAL;
  }

  iterator->end_edge = false;
  iterator->node = iterator->node->prev;
  if (iterator->node == iterator->list->head) {
    iterator->start_edge = true;
    return LIST_ITERATOR_END;
  }

  return LIST_ITERATOR_SUCCESS;
}

ListIteratorStatus list_iterator_start(ListIterator iterator) {
  if (iterator == 0) {
    return LIST_ITERATOR_EINVAL;
  }

  iterator->node = iterator->list->head;
  iterator->start_edge = true;
  iterator->end_edge = false;

  return LIST_ITERATOR_SUCCESS;
}

ListIteratorStatus list_iterator_end(ListIterator iterator) {
  if (iterator == 0) {
    return LIST_ITERATOR_EINVAL;
  }

  iterator->node = iterator->list->head;
  iterator->start_edge = false;
  iterator->end_edge = true;

  return LIST_ITERATOR_SUCCESS;
}

ListData * list_iterator_get(ListIterator iterator) {
  if (iterator == 0) {
    return 0;
  }

  return iterator->node->data;
}

void list_iterator_destroy(ListIterator iterator) {
  if (iterator != 0) {
    free(iterator);
  }
}

bool list_iterator_equal(const ListIterator first, const ListIterator second) {
  if (first->node == second->node) {
    return (first->start_edge == second->start_edge && first->end_edge == second->end_edge);
  }

  return false;
}
