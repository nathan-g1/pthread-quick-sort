#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

// Structure to pass arguments to threads
typedef struct
{
    int *arr;
    int low;
    int high;
} thread_args;

// Function to perform partitioning
int partition(int *arr, int low, int high)
{
    int pivot = arr[low + (high - low) / 2]; // Median of Three pivot
    int i = low - 1;
    int j = high + 1;
    while (1)
    {
        do
        {
            i++;
        } while (arr[i] < pivot);
        do
        {
            j--;
        } while (arr[j] > pivot);
        if (i >= j)
            return j;
        int temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
}

// Function to perform quicksort
void *quicksort(void *args)
{
    thread_args arg = *(thread_args*) args;
    int *arr = arg->arr;
    int low = arg->low;
    int high = arg->high;
    if (low < high)
    {
        int pivot_index = partition(arr, low, high);
        // Create threads for parallelization
        if (pivot_index - 1 - low > high - pivot_index - 1)
        {
            pthread_t thread;
            thread_args thread_arg;
            thread_arg.arr = arr;
            thread_arg.low = low;
            thread_arg.high = pivot_index - 1;
            pthread_create(&thread, NULL, quicksort, &thread_arg);
            pthread_detach(thread);
            quicksort(arg);
        }
        else
        {
            pthread_t thread;
            thread_args thread_arg;
            thread_arg.arr = arr;
            thread_arg.low = pivot_index + 1;
            thread_arg.high = high;
            pthread_create(&thread, NULL, quicksort, &thread_arg);
            pthread_detach(thread);
            quicksort(arg);
        }
    }
    pthread_exit(NULL);
    return NULL;
}


// Sequential quicksort for small subarrays
void sequential_quicksort(int* array, int left, int right) {
    if (left < right) {
        int pivotIndex = partition(array, left, right);
        sequential_quicksort(array, left, pivotIndex - 1);
        sequential_quicksort(array, pivotIndex + 1, right);
    }
}

// Function to generate a random array of integers
void generate_random_array(int* array, int size) {
    for (int i = 0; i < size; i++) {
        array[i] = rand() % 10000000;
    }
}

// Driver code
int main()
{
    int size;
    printf("Enter array size: ");
    scanf("%d", &size);
    int *arr = (int *)malloc(size * sizeof(int));
    generate_random_array(arr, size);
    thread_args args;
    args.arr = arr;
    args.low = 0;
    args.high = size - 1;


    clock_t start_sequential = clock();
    sequential_quicksort(arr, 0, size - 1); // sequential
    clock_t end_sequential = clock();
    double time_sequential = (double)(end_sequential - start_sequential) / CLOCKS_PER_SEC;
    printf("Time taken for sequential quicksort: %f seconds\n", time_sequential);

    clock_t start = clock();
    quicksort(&args); // parallel
    clock_t end = clock();
    double time = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Time taken: %f seconds\n", time);

    // speedup
    printf("Speedup: %f\n", time_sequential / time);

    free(arr);
    return 0;
}
