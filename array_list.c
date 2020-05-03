#include "array_list.h"
#include <errno.h>
#include <malloc.h>
#include <memory.h>

struct array_list alist_create(size_t item_size, size_t initial_size) {
  struct array_list new = {
    .data = malloc(item_size * initial_size),
    .item_size = item_size,
    .size = 0,
    .capacity = initial_size,
  };
  new.original_start = new.data;
  return new;
}

void alist_free(struct array_list* list) {
  free(list->original_start);
  list->data = NULL;
  list->capacity = 0;
  list->size = 0;
  list->item_size = 0;
}

void reclaim_start_space(struct array_list* list, ptrdiff_t diff) {
  memmove(list->original_start, list->data, list->item_size * list->size);
  list->data = list->original_start;
  list->capacity += diff;
}

void* realloc_to_capacity(struct array_list* list) {
  void* new_memory = reallocarray(list->original_start, list->item_size, list->capacity);
  if ((new_memory == NULL && list->capacity > 0) || errno != 0) {
    perror("reallocarray");
    abort();
  }
  list->original_start = new_memory;
  return new_memory;
}

void alist_push(struct array_list* list, void* item) {
  if (list->size + 1 >= list->capacity) {
    ptrdiff_t diff = ((char*) list->data) - ((char*) list->original_start);
    if (diff != 0) {
      // Reclaim empty space at the start if available
      reclaim_start_space(list, diff);
    } else {
      // Expand storage
      list->capacity = (list->capacity + 1) * 2;
      realloc_to_capacity(list);
      list->data = ((char*) list->original_start) + diff;
    }
  }
  memcpy(((char*) list->data) + (list->item_size * list->size), item, list->item_size);
  list->size++;
}

void alist_shrink(struct array_list* list) {
  reclaim_start_space(list, ((char*) list->data) - ((char*) list->original_start));
  list->capacity = list->size;
  list->data = realloc_to_capacity(list);
}

void alist_remove(struct array_list* list, size_t idx) {
  if (idx == list->size - 1) {
    list->size--;
    return;
  } else if (idx == 0) {
    list->size--;
    list->capacity--;
    list->data = ((char*) list->data) + list->item_size;
    return;
  }
  char* ptr_to_removed = ((char*) list->data) + (list->item_size * idx);
  list->size--;
  memmove(ptr_to_removed, ptr_to_removed + list->item_size, list->item_size * list->size);
}

void alist_remove_item(struct array_list* list, void* item) {
  char* ptr = (char*) item;
  char* data_start = (char*) list->data;
  if (ptr < data_start || ptr >= data_start + list->size * list->item_size) {
    fprintf(stderr, "Item not in array\n");
    abort();
  }
  ptrdiff_t byte_offset = ptr - data_start;
  size_t index = byte_offset / list->item_size;
  alist_remove(list, index);
}

void alist_clear(struct array_list* list) {
  list->size = 0;
  list->data = list->original_start;
  ptrdiff_t diff = ((char*) list->data) - ((char*) list->original_start);
  list->capacity += diff / list->item_size;
}

void* alist_at(struct array_list* list, size_t idx) {
  if (idx >= list->size) { return NULL; }
  return ((char*) list->data) + idx * list->item_size;
}
