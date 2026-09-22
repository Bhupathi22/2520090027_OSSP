#include <stdio.h>
#include <stdlib.h>

int main() {

    int i;

    printf("===== DYNAMIC MEMORY ALLOCATION =====\n");

    /* 1. malloc() */
    printf("\n1. malloc() demonstration\n");

    int *arr = (int *)malloc(5 * sizeof(int));

    if (arr == NULL) {
        printf("malloc failed.\n");
        return 1;
    }

    for (i = 0; i < 5; i++) {
        arr[i] = (i + 1) * 10;
    }

    printf("malloc allocated 5 integers:\n");

    for (i = 0; i < 5; i++) {
        printf("%d ", arr[i]);
    }

    printf("\n");


    /* 2. calloc() */
    printf("\n2. calloc() demonstration\n");

    int *zeroArr = (int *)calloc(5, sizeof(int));

    if (zeroArr == NULL) {
        printf("calloc failed.\n");
        free(arr);
        return 1;
    }

    printf("calloc initialized values to:\n");

    for (i = 0; i < 5; i++) {
        printf("%d ", zeroArr[i]);
    }

    printf("\n");


    /* 3. realloc() */
    printf("\n3. realloc() demonstration\n");

    int *temp = (int *)realloc(arr, 10 * sizeof(int));

    if (temp == NULL) {
        printf("realloc failed.\n");
        free(arr);
        free(zeroArr);
        return 1;
    }

    arr = temp;

    for (i = 5; i < 10; i++) {
        arr[i] = (i + 1) * 10;
    }

    printf("After realloc, array contains:\n");

    for (i = 0; i < 10; i++) {
        printf("%d ", arr[i]);
    }

    printf("\n");


    /* 4. free() */
    printf("\n4. free() demonstration\n");

    free(arr);
    free(zeroArr);

    printf("Allocated memory successfully released.\n");

    printf("\n===== PROGRAM COMPLETED =====\n");

    return 0;
}
