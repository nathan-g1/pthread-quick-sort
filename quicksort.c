#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define THRESHOLD 10000 // Threshold for switching to sequential quicksort

typedef struct {
    int* array;
    int left;
    int right;
} ThreadArgs;

// Swap two elements
void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Partition the array
int partition(int* array, int left, int right) {
    int pivot = array[right];
    int i = left - 1;

    for (int j = left; j < right; j++) {
        if (array[j] < pivot) {
            i++;
            swap(&array[i], &array[j]);
        }
    }
    swap(&array[i + 1], &array[right]);
    return i + 1;
}

// Sequential quicksort for small subarrays
void sequential_quicksort(int* array, int left, int right) {
    if (left < right) {
        int pivotIndex = partition(array, left, right);
        sequential_quicksort(array, left, pivotIndex - 1);
        sequential_quicksort(array, pivotIndex + 1, right);
    }
}

// Parallel quicksort function
void* parallel_quicksort(void* args) {
    ThreadArgs* threadArgs = (ThreadArgs*)args;
    int* array = threadArgs->array;
    int left = threadArgs->left;
    int right = threadArgs->right;

    if (left < right) {
        // If subarray is smaller than threshold, perform sequential quicksort
        if (right - left < THRESHOLD) {
            sequential_quicksort(array, left, right);
            return NULL;
        }

        // Partition the array
        int pivotIndex = partition(array, left, right);

        // Create thread arguments for left and right subarrays
        ThreadArgs leftArgs = {array, left, pivotIndex - 1};
        ThreadArgs rightArgs = {array, pivotIndex + 1, right};

        // Create threads for the left and right subarrays
        pthread_t leftThread, rightThread;
        pthread_create(&leftThread, NULL, parallel_quicksort, &leftArgs);
        pthread_create(&rightThread, NULL, parallel_quicksort, &rightArgs);

        // Wait for both threads to finish
        pthread_join(leftThread, NULL);
        pthread_join(rightThread, NULL);
    }

    return NULL;
}

// Function to initialize and start parallel quicksort
void quicksort(int* array, int size) {
    ThreadArgs args = {array, 0, size - 1};
    parallel_quicksort(&args);
}

// Function to generate a random array of integers
void generate_random_array(int* array, int size) {
    for (int i = 0; i < size; i++) {
        array[i] = rand() % 10000000;
    }
}

// Function to print the array (for debugging)
void print_array(int* array, int size) {
    for (int i = 0; i < size; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");
}

// Main function to test the parallel quicksort
int main() {
    int n = 1 << 20; // Array size (2^20)
    // int n = 30;
    int* array = (int*)malloc(n * sizeof(int));
    int* array_copy = (int*)malloc(n * sizeof(int));

    // Generate a random array
    generate_random_array(array, n);
    // Copy the array for sequential quicksort
    for (int i = 0; i < n; i++) {
        array_copy[i] = array[i];
    }

    // Print the array
    printf("Unsorted array:\n");
    print_array(array, n);

    // Measure time for sequential quicksort
    clock_t start_sequential = clock();
    sequential_quicksort(array_copy, 0, n - 1);
    clock_t end_sequential = clock();
    double time_sequential = (double)(end_sequential - start_sequential) / CLOCKS_PER_SEC;

    // Measure time for parallel quicksort
    clock_t start_parallel = clock();
    quicksort(array, n);
    clock_t end_parallel = clock();
    double time_parallel = (double)(end_parallel - start_parallel) / CLOCKS_PER_SEC;

    // Print sorted array
    printf("Sorted array (parallel):\n");
    print_array(array, n);

    printf("Sorted array (sequential):\n");
    print_array(array_copy, n);

    // Print sorted array (optional, for debugging purposes)
    printf("Sorted array:\n");
    print_array(array, n);

       // Print timing results
    printf("Time taken for parallel quicksort: %f seconds\n", time_parallel);
    printf("Time taken for sequential quicksort: %f seconds\n", time_sequential);
    printf("Speedup: %f\n", time_sequential / time_parallel);

    // Free allocated memory
    free(array);
    free(array_copy);
    return 0;
}
