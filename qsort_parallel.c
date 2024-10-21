#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define THRESHOLD 5000000 // Threshold for switching to sequential quicksort

typedef struct
{
    int *array;
    int left;
    int right;
} ThreadArgs;

// Swap two elements
void swap(int *a, int *b)
{
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
void sequential_quicksort(int *array, int left, int right)
{
    if (left < right)
    {
        int pivotIndex = partition_hoare(array, left, right);
        sequential_quicksort(array, left, pivotIndex - 1);
        sequential_quicksort(array, pivotIndex + 1, right);
    }
}

// Parallel quicksort function
void *parallel_quicksort(void *args)
{
    ThreadArgs *threadArgs = (ThreadArgs *)args;
    int *array = threadArgs->array;
    int left = threadArgs->left;
    int right = threadArgs->right;

    if (left < right)
    {
        // If subarray is smaller than threshold, perform sequential quicksort
        if (right - left < THRESHOLD)
        {
            sequential_quicksort(array, left, right);
            return NULL;
        }

        // Partition the array
        int pivotIndex = partition_hoare(array, left, right);

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
        array[i] = rand() % 100000;
    }
}

// Main function to test the parallel quicksort
int main()
{
    int size[] = { 10, 15, 20, 25, 30}; // Size of the array
    printf("array size, sequential time, parallel time, speedup\n");
    for (int i = 0; i < 5; i++)
    {
        int n = 1 << size[i];
        int n2 = size[i];
        int *array = (int *)malloc(n * sizeof(int));

        // Generate a random array
        generate_random_array(array, n);
        // Copy the array for sequential quicksort
        
        // Measure time for parallel quicksort
        clock_t start_parallel = clock();
        quicksort(array, n);
        clock_t end_parallel = clock();
        double time_parallel = (double)(end_parallel - start_parallel) / CLOCKS_PER_SEC;

        // Print timing results
        printf("Parallel quicksort time for 2^%d: %f seconds\n", n2,  time_parallel);

        // Free allocated memory
        free(array);
    }

    return 0;
}
