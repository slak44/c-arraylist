#include "array_list.h"
#include <assert.h>
#include <stdio.h>
#include <criterion/criterion.h>

int* get_data(int data) {
  int* ptr = malloc(sizeof(*ptr));
  *ptr = data;
  return ptr;
}

Test(array_list, basic) {
  struct array_list list = alist_create(sizeof(int), 5);
  assert(alist_at(&list, -1) == NULL);
  assert(alist_at(&list, 0) == NULL);
  assert(alist_at(&list, 1) == NULL);

  alist_push(&list, get_data(123));
  alist_push(&list, get_data(7));
  alist_push(&list, get_data(2));
  alist_push(&list, get_data(56));
  alist_push(&list, get_data(123));
  alist_push(&list, get_data(-17));

  assert(list.size == 6);
  assert(list.capacity > 5);

  alist_remove(&list, 0);
  assert(list.size == 5);

  alist_push(&list, get_data(99));
  alist_push(&list, get_data(100));
  alist_push(&list, get_data(101));
  assert(list.size == 8);
  assert(list.capacity > 8);

  size_t old_cap = list.capacity;
  alist_remove(&list, list.size - 1);
  assert(list.size == 7);
  assert(old_cap == list.capacity);

  int* middle = alist_at(&list, 4);
  assert(*middle == -17);
  alist_remove(&list, 4);
  assert(*middle == 99);

  alist_free(&list);
}
