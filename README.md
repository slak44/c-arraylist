# c-arraylist

A simple C11 array list implementation. Automatically resizable, provides bounds
checking.

### API

| Action    | Complexity      | Function/Macro                                |
|-----------|-----------------|-----------------------------------------------|
| indexing  | O(1)            | `alist_at`                                    |
| front     | O(1)            | `alist_front`                                 |
| push      | O(1), amortized | `alist_push`                                  |
| pop front | O(1)            | `alist_pop_front`/`alist_remove` (at index 0) |
| pop back  | O(1)            | `alist_remove` (at last index)                |
| remove    | O(n)            | `alist_remove`/`alist_remove_item`            |
| search    | O(n)            | `ALIST_FIND`                                  |

Other functions:
- `alist_create`: create an array list
- `alist_free`: free an existing array list
- `alist_shrink`: frees allocated but unused memory
- `alist_clear`: marks all elements removed, but does not free memory

Useful macros:
- `ALIST_FOR_EACH(type, item, list) body`: iterate all values
- `ALIST_FIND(type, item, list, condition)`: find first value matching
condition, or null

### Usage

To use, simply include `array_list.h` and compile `array_list.c`.

Example:

```c
#include <array_list.h>
#include <stdio.h>

int main() {
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
  return 0;
}
```

### Tests

A bunch of tests can be found in `tests/array_list_test.c`.
