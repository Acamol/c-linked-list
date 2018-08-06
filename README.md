[![Build Status](https://travis-ci.org/Acamol/c-linked-list.svg?branch=master)](https://travis-ci.org/Acamol/c-linked-list)
[![license](https://img.shields.io/github/license/mashape/apistatus.svg)](https://github.com/Acamol/c-linked-list/blob/master/LICENSE)


List
========
Implementation of doubly linked-list with iterators in C.
Below is the public interface of `List` and `ListIterator`. For further information, read the full description in `list.h`.


List API
----
__list_create__ - creates a new list.
```
List list_create(ListCopyFunction data_copy, ListFreeFunction data_free, ListCompareFunction data_compare);
```
__list_copy__ - Makes an exact copy of a given list. Iterator is not initialized.
points to NULL pointer. Pointer to the copied list otherwise.
```
List list_copy(const List list);
```

__list_destroy__ - Free a list with all its elements.
```
void list_destroy(List list);
```

### Modifiers

__list_push_front__ - Adds a new data element as a first element in the list.
```
ListStatus list_push_front(List list, const ListData * data);
```

__list_push_back__ - Adds a new data element as a last element in the list.
```
ListStatus list_push_back(List list, const ListData * data);
```

__list_push_after__ - Adds a data after a list element the iterator points to.
```
ListStatus list_push_after(List list, const ListIterator iterator, const ListData * data);
```

__list_push_before__ - Adds a data before a list element the iterator points to.
```
ListStatus list_push_before(List list, const ListIterator iterator, const ListData* data);
```

__list_push_at__ - Adds a data element at a given index, stating from 0.
```
ListStatus list_push_at(List list, size_t n, const ListData * data);
```

__list_remove__ - Removes a data element from a list. If the given data exists in several elements in the list, it will remove he first one in forward order.
```
ListStatus list_remove(List list, const ListData* data);
```

__list_remove_at__ - Removes a node at a given index, stating from 0.
```
ListStatus list_remove_at(List list, size_t n);
```

__list_pop_front__ - Extracts the first element from the list.
```
ListData * list_pop_front(List list);
```

__list_pop_back__ - Extracts the last element from the list.
```
ListData * list_pop_back(List list);
```

__list_remove_iterator__ - Removes an element from a given list using an iterator. After the operation, the iterator is set to the next element, or `NULL` if there is no next element.
```
ListStatus list_remove_iterator(List list, ListIterator iterator);
```

__list_clear__ - Clears a list from all of its elements.
```
void list_clear(List list);
```

__list_sort__ - Sorts a list (in an ascending order).
Done in O(N*log(N)) worst case time complexity and O(N) space complexity.
```
ListStatus list_sort(List list);
```


### Element access
__list_get_first__ - Gets the first data element in a list and sets an iterator to it.
```
ListData * list_get_first(const List list, ListIterator iterator);
```
__list_get_last__ - Gets the last data element in a list and sets an iterator to it.
```
ListData * list_get_last(const List list, ListIterator iterator);
```
__list_get_next__ - Gets the next data element in a list. Also advances the iterator to the next element.
```
ListData * list_get_next(const List list, ListIterator iterator);
```

__list_get_prev__ - Gets the previous data element in a list. Also regresses the iterator to the previous element.
```
ListData * list_get_prev(const List list, ListIterator iterator);
```

 __list_get_at__ - Gets the data element in a list at a given index,
```
ListData * list_get_at(const List list, size_t n);
```

__list_find__ - Finds a data element in a list.
```
ListData const * list_find(const List list, const ListData * data);
```


### Capacity
__list_get_size__ - Returns a list size.
```
size_t list_get_size(const List list);
```

 __list_empty__ - Returns "true" if the list is empty and "false" if not.
 ```
bool list_empty(const List list);
```



Iterators
=========
A few notes on `ListIterator`'s behavior:
- An iterator should always be destroyed (`list_iterator_destroy`) before
  the program end.
- Iterator can point to start or end of list, but not both.
- When created on an empty `List`, the iterator points to start of list. Otherwise,
  it points to the first element.
- Getting next of iterator pointing to start of list results in the first
  element, or end of list if the list is empty. Similar behavior when getting
  previous of iterator pointing to end of list.
- Getting (`list_iterator_get`) iterator that points to start/end of list results
  in `NULL` pointer.
- After removing with an iterator (`list_remove_iterator`), the iterator points to
  the next element, or end of list if the list is empty.
- If the iterator points to a removed element, its behavior is undefined.

To iterate over a `List`, one can use the `LIST_FOREACH_*` macros, or directly using
the iterators like so (for example):
```c
// "List list" and "ListIterator it" were created somewhere before
for (ListIteratorStatus stat = list_iterator_first(it); stat != LIST_ITERATOR_END; stat = list_iterator_next(it)) {
  // ...do something with "it"...
}
// destroy "it" later
```
or:
```c
// list created eariler
ListIterator end = list_iterator_create(list);
list_iterator_end(end);
ListIterator it = list_Iterator_create(list);
for (it = list_iterator_copy(begin); !list_iterator_equal(it, end); list_iterator_next(it)) {
  // do something with it
}
// destroy "it" and "end" later
```

Range iteration can be done like so (for example):
```c
// "ListIterator begin, end" were created before
ListIterator it;
for (it = list_iterator_copy(begin); !list_iterator_equal(it, end); list_iterator_next(it)) {
  // do something with it
}
// Destroy "it", "begin" and "end" later
```

To better understand iterators, a `List` diagram can be visualized as follows:
```
+-----+   +-----+                      +----+   +----+
|start|   |first|        some          |last|   |end |
| of  +-->+node +--> ... nodes ... +-->+node+-->+ of |
|list |   |     |        here          |    |   |list|
+-----+   +-----+                      +----+   +----+
```

Iterators API
-------------
__list_iterator_create__ - Creates a new iterator pointing to the first element.
```
ListIterator list_iterator_create(const List list);
```

__list_iterator_copy__ - Creates a copy of a given iterator.
```
ListIterator list_iterator_copy(const ListIterator iterator);
```

__list_iterator_first__ - Sets a given iterator to point to the first node.
```
ListIteratorStatus list_iterator_first(ListIterator iterator);
```

__list_iterator_last__ - Sets a given iterator to point to the last node.
```
ListIteratorStatus list_iterator_last(ListIterator iterator);
```

__list_iterator_next__ - Sets a given iterator to point to the next node.
```
ListIteratorStatus list_iterator_next(ListIterator iterator);
```

__list_iterator_prev__ - Sets a given iterator to point to the previous iterator.
```
ListIteratorStatus list_iterator_prev(ListIterator iterator);
```

__list_iterator_start__ - Sets a given iterator to point to start of list.
```
ListIteratorStatus list_iterator_start(ListIterator iterator);
```

__list_iterator_end__ - Sets a given iterator to point to end of list.
```
ListIteratorStatus list_iterator_end(ListIterator iterator);
```

__list_iterator_get__ - Gets the data element the iterator points to.
```
ListData * list_iterator_get(ListIterator iterator);
```

__list_iterator_destroy__ - Destroys a given iterator.
```
void list_iterator_destroy(ListIterator iterator);
```

__list_iterator_equal__ - Checks if two iterators point to the same element.
```
bool list_iterator_equal(const ListIterator first, const ListIterator second);
```


Examples
--------
Below is a basic example of `List` of strings.
We define the following functions:
```C
ListData * string_copy(const ListData * s) {
  char * c = (char*)malloc(sizeof(*c) * strlen((char*)s) + 1);
  return strcpy(c, (char*)s);
}

void string_free(ListData * s) {
  free(s);
}

int string_compare(const ListData * a, const ListData * b) {
  return strcmp(((char*)a), (char*)b);
}
```

So we can create `List` like so:
```C
// This example prints to the standard output:
// What exactly are you here for?
// To see with eyes unclouded by hate.

List strings = list_create(string_copy, string_free, string_compare);
list_push_front(strings, "What exactly are you here for?");
list_push_back(strings, "To see with eyes unclouded by hate.");

LIST_FOREACH_FORWARD(char*, string, strings) {
  printf("%s\n", string);
}

list_destroy(strings);
```

For further examples, see `tests/iterator.cpp` and `tests/list.cpp`.


Install
-------
Build `list.c` and include `list.h` in your program.

Credit
------
Big thank you to [@bsamseth](https://github.com/bsamseth) for the GTest boiler plate.
