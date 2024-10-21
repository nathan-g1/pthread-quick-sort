#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define THRESHOLD 5000000 // Threshold for switching to sequential quicksort

#define MAX_THREADS 9    // Maximum number of threads to avoid oversubscription

// Global counter to keep track of the number of threads
int active_threads = 0;
pthread_mutex_t thread_count_lock = PTHREAD_MUTEX_INITIALIZER;

typedef struct
{
    int *array;
    int left;
    int right;
} ThreadArgs;

// Swap two elements
void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // Mutex for thread-safe increment

// Partition the array Lomuto partition
int partition_lomuto(int *array, int left, int right)
{
    int pivot = array[right];
    int i = left - 1;

    for (int j = left; j < right; j++)
    {
        if (array[j] < pivot)
        {
            i++;
            swap(&array[i], &array[j]);
        }
    }
    swap(&array[i + 1], &array[right]);
    return i + 1;
}
// Partition the array: Hoare partition
int partition_hoare(int *arr, int low, int high)
{
    int pivot = arr[low];
    int i = low - 1, j = high + 1;

    while (1)
    {

        // Find leftmost element greater
        // than or equal to pivot
        do
        {
            i++;
        } while (arr[i] < pivot);

        // Find rightmost element smaller
        // than or equal to pivot
        do
        {
            j--;
        } while (arr[j] > pivot);

        // If two pointers met
        if (i >= j)
            return j;

        // Swap arr[i] and arr[j]
        int temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
}
// Partition the array: Median of Three partition
int partition_median_of_three(int *arr, int low, int high)
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
        swap(&arr[i], &arr[j]);
    }
}

// Sequential quicksort for small subarrays
// Sequential quicksort for small subarrays
void sequential_quicksort(int* array, int left, int right) {
    if (left < right) {
        int pivotIndex = partition_hoare(array, left, right);
        sequential_quicksort(array, left, pivotIndex);
        sequential_quicksort(array, pivotIndex + 1, right);
    }
}

// Parallel quicksort function with optimizations
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
        int pivotIndex = partition_hoare(array, left, right);

        // Create thread arguments for left and right subarrays
        ThreadArgs leftArgs = {array, left, pivotIndex};
        ThreadArgs rightArgs = {array, pivotIndex + 1, right};

        pthread_t leftThread, rightThread;
        int left_thread_created = 0;
        int right_thread_created = 0;

        // Lock the global thread counter
        pthread_mutex_lock(&thread_count_lock);
        if (active_threads < MAX_THREADS) {
            // Create a thread for the left partition if allowed
            active_threads++;
            left_thread_created = 1;
            pthread_create(&leftThread, NULL, parallel_quicksort, &leftArgs);
        }
        pthread_mutex_unlock(&thread_count_lock);

        if (!left_thread_created) {
            // If a new thread was not created, sort the left partition sequentially
            parallel_quicksort(&leftArgs);
        }

        // Lock the global thread counter again
        pthread_mutex_lock(&thread_count_lock);
        if (active_threads < MAX_THREADS) {
            // Create a thread for the right partition if allowed
            active_threads++;
            right_thread_created = 1;
            pthread_create(&rightThread, NULL, parallel_quicksort, &rightArgs);
        }
        pthread_mutex_unlock(&thread_count_lock);

        if (!right_thread_created) {
            // If a new thread was not created, sort the right partition sequentially
            parallel_quicksort(&rightArgs);
        }

        // Join threads if they were created
        if (left_thread_created) {
            pthread_join(leftThread, NULL);
            pthread_mutex_lock(&thread_count_lock);
            active_threads--;
            pthread_mutex_unlock(&thread_count_lock);
        }

        if (right_thread_created) {
            pthread_join(rightThread, NULL);
            pthread_mutex_lock(&thread_count_lock);
            active_threads--;
            pthread_mutex_unlock(&thread_count_lock);
        }
    }

    return NULL;
}

// Function to initialize and start parallel quicksort
void quicksort(int *array, int size)
{
    ThreadArgs args = {array, 0, size - 1};
    parallel_quicksort(&args);
}

// Function to generate a random array of integers
void generate_random_array(int *array, int size)
{
    for (int i = 0; i < size; i++)
    {
        array[i] = rand() % 1000;
    }
}

// Function to print the array (for debugging)
void print_array(int *array, int size)
{
    for (int i = 0; i < size; i++)
    {
        printf("%d ", array[i]);
    }
    printf("\n");
}

// Main function to test the parallel quicksort
int main()
{
    // Accept the array size from the user
    // int n;
    // printf("Enter the size of the array: ");
    // scanf("%d", &n);

    // int *array = (int *)malloc(n * sizeof(int));
    // int *array_copy = (int *)malloc(n * sizeof(int));

    // // Generate a random array
    // generate_random_array(array, n);
    // // Copy the array for sequential quicksort
    // for (int i = 0; i < n; i++)
    // {
    //     array_copy[i] = array[i];
    // }

    // // Print the array size
    // printf("Array size: %d\n", n);

    // // Measure time for sequential quicksort
    // clock_t start_sequential = clock();
    // sequential_quicksort(array_copy, 0, n - 1);
    // clock_t end_sequential = clock();
    // double time_sequential = (double)(end_sequential - start_sequential) / CLOCKS_PER_SEC;

    // // Measure time for parallel quicksort
    // clock_t start_parallel = clock();
    // quicksort(array, n);
    // clock_t end_parallel = clock();
    // double time_parallel = (double)(end_parallel - start_parallel) / CLOCKS_PER_SEC;

    // // Print timing results
    // printf("\n");
    // printf("Time taken for parallel quicksort: %f seconds\n", time_parallel);
    // printf("Time taken for sequential quicksort: %f seconds\n", time_sequential);
    // printf("Speedup: %f\n", time_sequential / time_parallel);
    int sizeArray[] = {1 << 10, 1 << 15, 1 << 20, 1 << 25, 1 << 30};
    for (int i = 0; i < 5; i++)
    {
        int n = sizeArray[i];
        int *array = (int *)malloc(n * sizeof(int));
        int *array_copy = (int *)malloc(n * sizeof(int));

        // Generate a random array
        generate_random_array(array, n);
        // Copy the array for sequential quicksort
        for (int i = 0; i < n; i++)
        {
            array_copy[i] = array[i];
        }

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

        // Print timing results
        printf("%d, %f, %f, %f, \n", n, time_sequential, time_parallel, time_sequential / time_parallel);

        // Free allocated memory
        free(array);
        free(array_copy);
    }

    return 0;
}
