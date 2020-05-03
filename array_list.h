#pragma once

#include <stdlib.h>

/**
 * Automatically resizing array. Is almost a deque: it supports copy-less O(1) removal at both ends, but only supports
 * amortized O(1) insertion at one end. Of course, as an array, it has O(1) random access to all elements.
 */
struct array_list {
  void* original_start;
  void* data;
  size_t item_size;
  size_t size;
  size_t capacity;
};

struct array_list alist_create(size_t item_size, size_t initial_size);
void alist_free(struct array_list*);

/**
 * Push a copy to the end of the list.
 */
void alist_push(struct array_list* list, void* item);

/**
 * Remove the idx'th element. Invalidates all pointers after that index.
 */
void alist_remove(struct array_list*, size_t idx);

/**
 * Remove the element pointed to by item. Invalidates all pointers after that item. The item pointer must be inside the
 * array list, otherwise it aborts. The "item" is a pointer returned by alist_at.
 */
void alist_remove_item(struct array_list*, void* item);
__always_inline void alist_pop_front(struct array_list* list) { alist_remove(list, 0); }

/**
 * Reduce capacity to exactly size. Deallocate unused memory.
 */
void alist_shrink(struct array_list*);

void alist_clear(struct array_list*);

void* alist_at(struct array_list* list, size_t);
__always_inline void* alist_front(struct array_list* list) { return alist_at(list, 0); }

#define ALIST_FOR_EACH(type, item, list)                                                                               \
  size_t item##array_list_for_each_index = 0;                                                                          \
  for (type* item = alist_at((list), 0); item##array_list_for_each_index < (list)->size;                               \
       item = alist_at((list), ++item##array_list_for_each_index))

#define ALIST_FIND(type, item, list, condition)                                                                        \
  type* item = NULL;                                                                                                   \
  do {                                                                                                                 \
    _Pragma("clang diagnostic push");                                                                                  \
    _Pragma("clang diagnostic ignored \"-Wshadow\"");                                                                  \
    for (size_t i = 0; i < (list)->size; i++) {                                                                        \
      item = alist_at((list), i);                                                                                      \
      if (condition) break;                                                                                            \
      if (i + 1 == (list)->size) {                                                                                     \
        item = NULL;                                                                                                   \
        break;                                                                                                         \
      }                                                                                                                \
    }                                                                                                                  \
    _Pragma("clang diagnostic pop");                                                                                   \
  } while (0)
