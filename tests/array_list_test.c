#include "array_list.h"
#include <criterion/criterion.h>
#include <stdio.h>
#include <stdint.h>
#include <signal.h>

#define INITIAL_CAPACITY 5

Test(array_list_create, create_and_free) {
  size_t init_size = INITIAL_CAPACITY;
  struct array_list list = alist_create(sizeof(int), init_size);

  cr_assert(list.data != NULL, "Initial allocation must be correct");
  cr_assert(list.original_start == list.data, "data is original_start, initially");
  cr_assert(list.size == 0 && list.capacity == init_size, "Initial size is correct");
  cr_assert(list.item_size == sizeof(int), "Item size is correct");

  alist_free(&list);

  cr_assert(list.data == NULL && list.original_start == NULL, "Pointers were free'd");
  cr_assert(list.item_size == 0 && list.size == 0 && list.capacity == 0, "Sizes are zero");
}

Test(array_list_create, create_and_free_empty) {
  struct array_list list = alist_create(sizeof(int), 0);

  cr_assert(list.data == NULL && list.original_start == NULL, "No initial allocation");
  cr_assert(list.item_size == sizeof(int), "Item size is correct");

  alist_free(&list);

  cr_assert(list.data == NULL && list.original_start == NULL, "Pointers are still null");
  cr_assert(list.item_size == 0 && list.size == 0 && list.capacity == 0, "Sizes are zero");
}

Test(array_list_create, abort_when_no_mem, .signal = SIGABRT) {
  struct array_list list = alist_create(sizeof(int), UINTMAX_MAX);
  alist_free(&list);
}

struct array_list list;

void setup() { list = alist_create(sizeof(int), INITIAL_CAPACITY); }

void teardown() { alist_free(&list); }

TestSuite(array_list, .init = setup, .fini = teardown);

Test(array_list, empty_list_at) {
  cr_assert(alist_at(&list, INT32_MIN) == NULL, "Empty list at is NULL");
  cr_assert(alist_at(&list, -1) == NULL, "Empty list at is NULL");
  cr_assert(alist_at(&list, 0) == NULL, "Empty list at is NULL");
  cr_assert(alist_at(&list, 1) == NULL, "Empty list at is NULL");
  cr_assert(alist_at(&list, INT32_MAX) == NULL, "Empty list at is NULL");
}

Test(array_list, push_works) {
  alist_push(&list, &(int) {123});
  alist_push(&list, &(int) {7});
  alist_push(&list, &(int) {2});
  alist_push(&list, &(int) {56});
  alist_push(&list, &(int) {-45});

  cr_assert(list.size == 5, "Size for 5 pushes is 5");
}

Test(array_list, push_makes_copy) {
  int integer = 45;
  alist_push(&list, &integer);
  alist_push(&list, &integer);
  alist_push(&list, &integer);

  int* front = alist_at(&list, 0);
  cr_assert(*front == integer, "Element was copied correctly");
  integer = 98;
  front = alist_at(&list, 0);
  cr_assert(*front != integer, "A copy was made, value now differs");
  *front = 50000;
  int* second = alist_at(&list, 1);
  cr_assert(*front != *second, "A copy was made between elements, value now differs");
}

Test(array_list, push_realloc_works) {
  const int pushes = INITIAL_CAPACITY + 1;
  for (int i = 0; i < pushes; ++i) {
    alist_push(&list, &(int) {123});
  }

  cr_assert(list.size == pushes, "Size equal to push count");
  cr_assert(list.capacity > INITIAL_CAPACITY, "Capacity was increased");
}

Test(array_list, at_works) {
  const int pushes = 100;
  for (int i = 0; i < pushes; ++i) {
    alist_push(&list, &i);
  }
  for (int i = 0; i < pushes; ++i) {
    int* value = alist_at(&list, i);
    cr_assert(*value == i, "Value must be same as inserted");
  }
}

Test(array_list, at_out_of_bounds) {
  const int pushes = 100;
  for (int i = 0; i < pushes; ++i) {
    alist_push(&list, &i);
  }
  cr_assert(alist_at(&list, -1) == NULL, "Negative index invalid");
  cr_assert(alist_at(&list, -1235) == NULL, "Negative index invalid");
  cr_assert(alist_at(&list, pushes + 1) == NULL, "Large index invalid");
  cr_assert(alist_at(&list, pushes * 60) == NULL, "Large index invalid");
}

Test(array_list, for_each_macro) {
  const int pushes = 100;
  for (int i = 0; i < pushes; ++i) {
    alist_push(&list, &i);
  }
  int counter = 0;
  ALIST_FOR_EACH(int, my_int, &list) {
    cr_assert(*my_int == counter, "Value (%d) must be same as inserted (%d)", *my_int, counter);
    counter++;
  }
}

Test(array_list, find_macro) {
  const int pushes = 100;
  for (int i = 0; i < pushes; ++i) {
    alist_push(&list, &i);
  }
  ALIST_FIND(int, needle, &list, *needle > 50 && *needle % 2 == 0);
  cr_assert(*needle == 52, "Found item must respect condition");
}

Test(array_list, remove_idx_works) {
  alist_push(&list, &(int) {1});
  alist_push(&list, &(int) {2});
  alist_push(&list, &(int) {3});

  cr_assert(list.size == 3, "Size for 3 pushes is 3");
  alist_remove(&list, 1);
  cr_assert(list.size == 2, "Size reduced by 1");
}

Test(array_list, remove_front) {
  alist_push(&list, &(int) {1});
  alist_push(&list, &(int) {2});
  alist_push(&list, &(int) {3});

  void* original = alist_at(&list, 2);

  alist_remove(&list, 0);
  cr_assert(list.size == 2, "Size reduced by 1");
  cr_assert(alist_at(&list, 1) == original, "remove(0) is O(1), does not move data");

  alist_pop_front(&list);
  cr_assert(list.size == 1, "Size reduced by 1");
  cr_assert(alist_at(&list, 0) == original, "pop_front() is O(1), does not move data");
}

Test(array_list, remove_end) {
  const int value = 0xDEADBEEF;
  for (int i = 0; i < INITIAL_CAPACITY; ++i) {
    alist_push(&list, &(int) {value});
  }

  int* original = alist_at(&list, list.size - 1);

  alist_remove(&list, list.size - 1);
  cr_assert(list.size == INITIAL_CAPACITY - 1, "Size reduced by 1");
  cr_assert(*original == value, "remove(last) is O(1), does not move data");
}

Test(array_list, data_moved_correctly) {
  const int pushes = 100;
  const int hole_at = 50;
  for (int i = 0; i < pushes; ++i) {
    alist_push(&list, &i);
  }
  alist_remove(&list, hole_at);
  for (int i = hole_at; i < pushes - 1; ++i) {
    int* value = alist_at(&list, i);
    cr_assert(*value == i + 1, "Elements offset by 1 due to removal");
  }
}

Test(array_list, reclaim_front) {
  for (int i = 0; i < INITIAL_CAPACITY; ++i) {
    alist_push(&list, &i);
  }
  cr_assert(list.size == list.capacity, "List is full (cap %d)", list.capacity);

  alist_pop_front(&list);
  alist_pop_front(&list);
  cr_assert(list.data != list.original_start, "pop_front() leaves unused space");
  cr_assert(list.size == list.capacity, "List is still full, because of unused space (cap %d)", list.capacity);

  alist_push(&list, &(int) {1});
  alist_push(&list, &(int) {1});
  cr_assert(list.capacity == INITIAL_CAPACITY, "Unused space was reclaimed (cap %d)", list.capacity);
}

Test(array_list, remove_item_ptr) {
  alist_push(&list, &(int) {1});
  alist_push(&list, &(int) {2});
  alist_push(&list, &(int) {3});

  alist_remove_item(&list, alist_at(&list, 1));
  cr_assert(list.size == 2, "Size reduced by 1");
}

Test(array_list, remove_bad_item_ptr_aborts, .signal = SIGABRT) {
  alist_push(&list, &(int) {1});
  alist_push(&list, &(int) {2});
  alist_push(&list, &(int) {3});

  alist_remove_item(&list, (void*) 0xFFF123);
}

Test(array_list, clear) {
  const int pushed = INITIAL_CAPACITY * 20;
  for (int i = 0; i < pushed; ++i) {
    alist_push(&list, &i);
  }
  alist_clear(&list);
  cr_assert(list.capacity >= pushed, "Capacity not reduced after clear");
}

Test(array_list, shrink_capacity) {
  const int pushed = 100;
  const int removed = 80;
  for (int i = 0; i < pushed; ++i) {
    alist_push(&list, &i);
  }
  for (int i = 0; i < removed; ++i) {
    alist_pop_front(&list);
  }
  alist_shrink(&list);
  cr_assert(list.capacity == (pushed - removed), "Capacity reduced after shrink");
}

Test(array_list, shrink_to_zero) {
  const int pushed = 100;
  for (int i = 0; i < pushed; ++i) {
    alist_push(&list, &i);
  }
  for (int i = 0; i < pushed; ++i) {
    alist_pop_front(&list);
  }
  alist_shrink(&list);
  cr_assert(list.capacity == 0, "Capacity reduced after shrink");
}

Test(array_list, clear_reclaims_front) {
  for (int i = 0; i < INITIAL_CAPACITY; ++i) {
    alist_push(&list, &i);
  }
  alist_pop_front(&list);
  alist_pop_front(&list);
  alist_clear(&list);
  cr_assert(list.capacity == INITIAL_CAPACITY, "Capacity not reduced by pop_front (cap %d)", list.capacity);
}

Test(array_list_struct, push_works) {
  struct big_struct {
    uint32_t test;
    int x;
    bool bla;
    char name[50];
    double trouble;
    char data[100];
  };
  struct big_struct instance = {
      .data = "afsdjgfjasdlfakf;alekf",
      .name = "sdjgiejdfkeaslfk",
      .bla = true,
      .test = 42,
      .trouble = 5.5,
      .x = 2,
  };
  struct array_list structs = alist_create(sizeof(struct big_struct), INITIAL_CAPACITY);
  for (int i = 0; i < INITIAL_CAPACITY; ++i) {
    alist_push(&structs, &instance);
  }
  cr_assert(structs.size == INITIAL_CAPACITY, "Big structs were pushed");
}

Test(array_list_sample, readme_sample_compiles) {
  struct array_list list = alist_create(sizeof(int), 5);
  alist_push(&list, &(int) {1});
  alist_push(&list, &(int) {2});
  alist_push(&list, &(int) {3});
  alist_push(&list, &(int) {4});
  alist_push(&list, &(int) {5});
  alist_push(&list, &(int) {6});
  printf("size is %d, capacity is %d\n", list.size, list.capacity);

  alist_remove(&list, 1);
  printf("size is %d, capacity is %d\n", list.size, list.capacity);

  alist_shrink(&list);
  printf("size is %d, capacity is %d\n", list.size, list.capacity);

  int* second = alist_at(&list, 1);
  printf("second item is %d\n", *second);

  ALIST_FOR_EACH(int, value, &list) {
    printf("%d\n", *value);
    if (*value == 5) break;
  }

  ALIST_FIND(int, needle, &list, *needle > 4 && *needle % 2 == 0);
  printf("needle is %d\n", *needle);

  ALIST_FIND(int, big_number, &list, *big_number > 10000);
  printf("was big number found?: %s\n", big_number != NULL ? "yes" : "no");

  alist_free(&list);
}
