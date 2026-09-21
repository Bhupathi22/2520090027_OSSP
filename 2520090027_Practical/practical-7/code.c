#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Global variables
int global_initialized = 42;    // Initialized Data Segment (.data)
int global_uninitialized;       // Uninitialized Data Segment (.bss)

void print_addresses(void) {
    // Local static variables
    static int static_initialized = 100; // Data Segment (.data)
    static int static_uninitialized;      // BSS Segment (.bss)

    // Stack variable
    int stack_var = 10;

    // Heap allocation
    int *heap_var = (int *)malloc(sizeof(int));
    if (!heap_var) {
        perror("Allocation failed");
        return;
    }

    // Convert function pointer to standard void pointer safely
    void *code_ptr;
    *(void **)(&code_ptr) = (void *)print_addresses;

    printf("================ Memory Address Analysis ================\n");
    printf("Process PID          : %d\n\n", getpid());

    printf("Code (Text) Segment  :\n");
    printf("  Function Address   : %p\n\n", code_ptr);

    printf("Data Segment (Initialized):\n");
    printf("  Global variable    : %p\n", (void *)&global_initialized);
    printf("  Static variable    : %p\n\n", (void *)&static_initialized);

    printf("BSS Segment (Uninitialized):\n");
    printf("  Global variable    : %p\n", (void *)&global_uninitialized);
    printf("  Static variable    : %p\n\n", (void *)&static_uninitialized);

    printf("Heap Segment         :\n");
    printf("  Allocated memory   : %p\n\n", (void *)heap_var);

    printf("Stack Segment        :\n");
    printf("  Local variable     : %p\n", (void *)&stack_var);
    printf("=========================================================\n");

    free(heap_var);
}

int main(void) {
    print_addresses();

    printf("\nPaused to inspect memory maps.\n");
    printf("Run 'cat /proc/%d/maps' in another terminal.\n", getpid());
    printf("Press ENTER to exit...");
    getchar();

    return 0;
}
