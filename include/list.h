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
* list.h
*
*  Created on: Dec 12, 2016
*      Author: Aviad Gafni
*/

#ifndef __LIST_H__
#define __LIST_H__

#ifdef __cplusplus
extern "C" {
#endif


#include <stdlib.h> // size_t
#include <stdbool.h>

  typedef enum {
    LIST_SUCCESS,
    LIST_FAIL,
    LIST_NO_MEM,
    LIST_EINVAL,
    LIST_NOT_FOUND
  } ListStatus;

  typedef enum {
    LIST_ITERATOR_SUCCESS,
    LIST_ITERATOR_EINVAL,
    LIST_ITERATOR_END
  } ListIteratorStatus;

  typedef struct list_t *List;

  typedef struct list_iterator_t *ListIterator;

  /**
  * The generic data type the list holds.
  */
  typedef void ListData;

  /**
  * Pointer to a function to copy the data.
  */
  typedef ListData * (*ListCopyFunction)(const ListData*);

  /**
  * Pointer to a function to free the data.
  */
  typedef void(*ListFreeFunction)(ListData*);

  /**
  * Pointer to a function which compares two data elements.
  *
  * Function return values should be as follows:
  * If the two elements are identical, return value = 0.
  * If the first is less than the second, return value < 0.
  * If the first if greater than the second, return value > 0.
  */
  typedef int(*ListCompareFunction)(const ListData*, const ListData*);

  /**
  * for loops to iterate over the list, for user's convenience.
  */

#define LIST_FOREACH_FORWARD(type, variable, list) \
	for (const type variable = (type)list_get_first(list, 0); \
			variable != 0; \
			variable = (type)list_get_next(list, 0))

#define LIST_FOREACH_BACKWARD(type, variable, list) \
	for (const type variable = (type)list_get_last(list, 0); \
			variable != 0; \
			variable = (type)list_get_prev(list, 0))


  /**
  * list_create - creates a new list.
  *
  * @data_copy:	  	Pointer to a copy data function.
  * @data_free:	  	Pointer to a free data function.
  * @data_compare:	Pointer to a data compare function.
  *
  * return:	Pointer to the new list if it succeeds. NULL pointer otherwise.
  */
  List list_create(ListCopyFunction data_copy, ListFreeFunction data_free, ListCompareFunction data_compare);

  /**
  * list_copy - Makes an exact copy of a given list. Iterator is not initialized.
  *
  * @list:	The list to copy.
  *
  * return:	NULL pointer if there was an allocation failure, or if the list is
  * 			points to NULL pointer. Pointer to the copied list otherwise.
  */
  List list_copy(const List list);

  /**
  * list_destroy - Frees a list with all its elements. Pretty obvious.
  */
  void list_destroy(List list);



  /**                             Modifiers                                 **/

  /**
  * list_push_front - Adds a new data element as a first element in the list.
  *
  * @list:	The list to insert the data to.
  * @data:	The data to insert to the list.
  *
  * return:	LIST_EINVAL if any of the function arguments are NULL pointers.
  * 		  	LIST_NO_MEM if there was an allocation failure.
  * 		  	LIST_SUCCESS otherwise.
  */
  ListStatus list_push_front(List list, const ListData * data);

  /**
  * list_push_back - Adds a new data element as a last element in the list.
  *
  * @data:	The data to insert to the list.
  *
  * return:	LIST_EINVAL if any of the function arguments are NULL pointers.
  * 	  		LIST_NO_MEM if there was an allocation failure.
  * 	  		LIST_SUCCESS otherwise.
  */
  ListStatus list_push_back(List list, const ListData * data);

  /**
  * list_push_after - Adds a data after a list element the iterator points to.
  *
  * @list:		  	The list to insert the data to.
  * @before_this:	Pointer to data element in the list to insert before it.
  * @data:	  		Pointer to data element to insert.
  *
  * return:	LIST_EINVAL if one of the argument points to NULL pointer.
  * 		  	LIST_NO_MEM if there was an allocation failure.
  * 		  	LIST_SUCCESS otherwise.
  */
  ListStatus list_push_after(List list, const ListIterator iterator, const ListData * data);

  /**
  * list_push_before - Adds a data before a list element with a given data. If
  * 					         the given data does not exist in the list nothing changes.
  *
  * @list:			  The list to insert the data to.
  * @before_this:	Pointer to data element in the list to insert before it.
  * @data:		  	Pointer to data element to insert.
  *
  * return:	LIST_EINVAL if one of the argument points to NULL pointer.
  * 			  LIST_NOT_FOUND if the given data does not exist in the list.
  * 		  	LIST_NO_MEM if there was an allocation failure.
  * 		  	LIST_SUCCESS otherwise.
  */
  ListStatus list_push_before(List list, const ListIterator iterator, const ListData* data);

  /**
  * list_push_at - Adds a data element at a given index, stating from 0.
  *
  * @list: The list to add the data element to.
  * @n:    The position the data element will be inserted.
  * @data: The data element to insert.
  *
  * return: LIST_EINVAL if n < 0 or n > list size or if one of the arguments
  *         points to NULL pointer.
  *         LIST_NO_MEM if there was an allocation failure.
  *         LIST_SUCCESS otherwise.
  */
  ListStatus list_push_at(List list, size_t n, const ListData * data);

  /**
  * list_remove - Removes a data element from a list. If the given data exists
  *               in several elements in the list, it will remove he first one
  *               in forward order.
  *
  * @list:	The list to remove the data from.
  * @data:	The data to delete from the list.
  *
  * return:	LIST_EINVAL if one of the argument points to NULL pointer.
  * 			  LIST_NOT_FOUND if the given data does not exist in the list.
  * 		   	LIST_SUCCESS otherwise.
  */
  ListStatus list_remove(List list, const ListData* data);

  /**
  * list_remove_at - Removes a node at a given index, stating from 0.
  *
  * @list: The list to remove the node from.
  * @n:    The position of the node to be removed.
  *
  * return: LIST_EINVAL if n < 0 or n >= list size, or list is NULL pointer.
  *         LIST_SUCCESS in case of success.
  */
  ListStatus list_remove_at(List list, size_t n);

  /**
  * list_pop_front - Extracts the first element from the list.
  *                  NOTE: this element is allocated on the heap,
  *                  and thus should be free'd with ListFreeFunction.
  *
  * @list:  The list to pop the element from.
  *
  * return: The front element in case of success, or NULL pointer in case of
  *         failure.
  */
  ListData * list_pop_front(List list);

  /**
  * list_pop_back - Extracts the last element from the list.
  *                 NOTE: this element is allocated on the heap,
  *                 and thus should be free'd with ListFreeFunction.
  *
  * @list:  The list to pop the element from.
  *
  * return: The last element in case of success, or NULL pointer in case of
  *         failure.
  */
  ListData * list_pop_back(List list);
/**
* list_remove_iterator - Removes an element from a given list using an iterator.
*                        After the operation, the iterator is set to the next
*                        element, or NULL if there is no next element.
*
* @list:      The list to remove the element from.
* @iterator:  An iterator pointing to an element to be removed.
*
* return: LIST_EINVAL if one of the arguments is NULL pointer or if the iterator
          does not belong to the given list.
          LIST_SUCCESS otherwise.
*/
  ListStatus list_remove_iterator(List list, ListIterator iterator);

  /**
  * list_clear - Clears a list from all of its elements.
  */
  void list_clear(List list);

  /**
  * list_sort - Sorts a list (in an ascending order).
  *             Done in O(N*log(N)) worst case time complexity and O(N) space
  *             complexity.
  *
  * @list: The list to sort.
  *
  * return: LIST_FAIL in case of any sort of failure, yet the list is
  *         guaranteed to stay unaffected.
  *         LIST_SUCCESS in case of success.
  */
  ListStatus list_sort(List list);



  /**                         Element access                                **/

  /**
  * list_get_first - Gets the first data element in a list and sets an
  *                  iterator to it. If the iterator is not needed, pass
  *                  a NULL pointer in @iterator.
  *
  * @list:	    The list.
  * @iterator:  Pointer to store a new iterator to the first element.
  *
  * return:	If the list is not empty, the first data element in the
  * 			  list. NULL pointer otherwise.
  *         Also, as noted, if iterator is not NULL, sets @iterator to point
  *         to the first element, or NULL if the list is empty.
  */
  ListData * list_get_first(const List list, ListIterator iterator);

  /**
  * list_get_last - Gets the last data element in a list and sets an
  *                 iterator to it. If the iterator is not needed, pass
  *                 a NULL pointer in @iterator.
  *
  * @list:	    The list.
  * @iterator:  Pointer to store a new iterator to the last element.
  *
  * return:	If the list is not empty, the first data element in the
  * 			  list. NULL pointer otherwise.
  *         Also, as noted, if iterator is not NULL, sets @iterator to point
  *         to the last element, or NULL if the list is empty.
  */
  ListData * list_get_last(const List list, ListIterator iterator);

  /**
  * list_get_next - Gets the next data element in a list. Also advances the
  * 				        iterator to the next element.
  *
  * @list:	    The list.
  * @iterator:  An iterator of the given list.
  *
  * return:	If the list is not empty and there is a next element, the next data
  * 			  element. NULL pointer otherwise. NULL pointer is also returned when
  *         the iterator does not belong to the given list.
  */
  ListData * list_get_next(const List list, ListIterator iterator);

  /**
  * list_get_prev - Gets the previous data element in a list. Also regresses the
  * 				        iterator to the previous element.
  *
  * @list:	    The list.
  * @iterator:  An iterator of the given list.
  *
  * return:	If the list is not empty and there is a previous element, the next
  *         data element. NULL pointer otherwise. NULL pointer is also returned
  *         when the iterator does not belong to the given list.
  */
  ListData * list_get_prev(const List list, ListIterator iterator);

  /**
  * list_get_at - Gets the data element in a list at a given index,
  *               starting from 0.
  *
  * @list:	The list.
  * @n:     The position of the element to get
  *
  * return:	If the list is not empty and there is a next element, the next data
  * 			  element. NULL pointer otherwise.
  */
  ListData * list_get_at(const List list, size_t n);

  /**
  * list_find - Finds a data element in a list.
  *
  * @list: The list.
  * @data: The data to find.
  *
  * return: A pointer to the data element in the list if such element exists in
  *         the list, or NULL pointer otherwise.
  */
  ListData const * list_find(const List list, const ListData * data);



  /**                            Capacity                                   **/

  /**
  * list_get_size - Gets a list size.
  *
  * @list:	The list :/
  *
  * return:	The size of the list.
  */
  size_t list_get_size(const List list);

  /**
  * list_empty - Returns "true" if the list is empty and "false" if not.
  */
  bool list_empty(const List list);


  /**                            iterators                                  **/

  /**
  * list_iterator_create - Creates a new iterator pointing to the first element.
  *
  * @list: The list the iterator will belong to.
  *
  * return: A new iterator in case of success or NULL pointer otherwise.
  */
  ListIterator list_iterator_create(const List list);

  /**
  * list_iterator_copy - Creates a copy of a given iterator.
  *
  * @iterator: The iterator to copy.
  *
  * return: A new copy of @iterator, or NULL pointer on failure.
  */
  ListIterator list_iterator_copy(const ListIterator iterator);

  /**
  * list_iterator_first - Sets a given iterator to point to the first node.
  *
  * @iterator: The iterator to set.
  *
  * return: LIST_ITERATOR_EINVAL if the iterator is NULL.
  *         LIST_ITERATOR_END if the list is empty.
  *         LIST_ITERATOR_SUCCESS in case of success.
  */
  ListIteratorStatus list_iterator_first(ListIterator iterator);

  /**
  * list_iterator_last - Sets a given iterator to point to the last node.
  *
  * @iterator: The iterator to set.
  *
  * return: LIST_ITERATOR_EINVAL if the iterator is NULL.
  *         LIST_ITERATOR_END if the list is empty.
  *         LIST_ITERATOR_SUCCESS in case of success.
  */
  ListIteratorStatus list_iterator_last(ListIterator iterator);

  /**
  * list_iterator_next - Sets a given iterator to point to the next node.
  *
  * @iterator: The iterator to change.
  *
  * return: LIST_ITERATOR_EINVAL if the iterator is NULL.
  *         LIST_ITERATOR_END if the iterator reached to end of list. In this
  *         case, using list_iterator_get on the iterator will result NULL
  *         pointer.
  *         LIST_ITERATOR_SUCCESS in case of success.
  */
  ListIteratorStatus list_iterator_next(ListIterator iterator);

  /**
  * list_iterator_prev - Sets a given iterator to point to the previous iterator.
  *
  * @iterator: The iterator to change.
  *
  * return: LIST_ITERATOR_EINVAL if the iterator is NULL.
  *         LIST_ITERATOR_END if the iterator reached to start of list. In this
  *         case, using list_iterator_get on the iterator will result NULL
  *         pointer.
  *         LIST_ITERATOR_SUCCESS in case of success.
  */
  ListIteratorStatus list_iterator_prev(ListIterator iterator);

  /**
  * list_iterator_start - Sets a given iterator to point to start of list.
  *
  * return: LIST_ITERATOR_EINVAL if the iterator is NULL.
  *         LIST_ITERATOR_SUCCESS in case of success.
  */
  ListIteratorStatus list_iterator_start(ListIterator iterator);

  /**
  * list_iterator_end - Sets a given iterator to point to end of list.
  *
  * return: LIST_ITERATOR_EINVAL if the iterator is NULL.
  *         LIST_ITERATOR_SUCCESS in case of success.
  */
  ListIteratorStatus list_iterator_end(ListIterator iterator);

  /**
  * list_iterator_get - Gets the data element the iterator points to.
  *
  * @iterator: The iterator to get the data from.
  *
  * return: The data element the iterator points to, or NULL pointer in case
  *         of failure.
  */
  ListData * list_iterator_get(ListIterator iterator);

  /**
  * list_iterator_destroy - Destroys a given iterator.
  *
  * NOTE: this function needs to be called on ANY iterator created with
  *       list_iterator_create. i.e., even if the iterator reached to
  *       end/start of the list or no longer has valid data.
  */
  void list_iterator_destroy(ListIterator iterator);

  /**
  * list_iterator_equal - Checks if two iterators point to the same element.
  *
  * @first, @second: The two iterators to check.
  *
  * return: "true" if @first and @second point to the same element and "false"
            otherwise.
  */
  bool list_iterator_equal(const ListIterator first, const ListIterator second);

#ifdef __cplusplus
}
#endif

#endif /* __LIST_H__ */
