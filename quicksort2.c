#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define ARRAY_SIZE (1 << 30) // Array size

typedef struct {
    int *array;
    int left;
    int threshold;
    int right;
} ThreadArgs;

// Swap two elements
void swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Partition the array
int partition(int *array, int left, int right)
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

// Sequential quicksort for small subarrays
void sequential_quicksort(int *array, int left, int right)
{
    if (left < right)
    {
        int pivotIndex = partition(array, left, right);
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
    int threshold = threadArgs->threshold;

    if (left < right)
    {
        // If subarray is smaller than threshold, perform sequential quicksort
        if (right - left < threshold)
        {
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

// Function to print the array (for debugging)
void print_array(int *array, int size)
{
    for (int i = 0; i < size; i++)
    {
        printf("%d ", array[i]);
    }
    printf("\n");
}

// Main function to test the parallel quicksort with different thresholds
int main()
{
    int *array = (int *)malloc(ARRAY_SIZE * sizeof(int));
    int *array_copy = (int *)malloc(ARRAY_SIZE * sizeof(int));

    // Generate a random array
    generate_random_array(array, ARRAY_SIZE);

    // Define different threshold values to test
    int thresholds[] = {100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000, 20000, 30000, 40000, 50000, 60000, 70000, 80000, 90000, 100000, 200000, 300000, 400000, 500000, 600000, 700000, 800000, 900000, 1000000, 2000000, 3000000, 4000000, 5000000, 6000000, 7000000, 8000000, 9000000, 10000000};
    int num_thresholds = sizeof(thresholds) / sizeof(thresholds[0]);

    // Loop through each threshold value
    for (int i = 0; i < num_thresholds; i++)
    {
        int threshold = thresholds[i];

        // Copy the array for parallel quicksort
        for (int j = 0; j < ARRAY_SIZE; j++)
        {
            array_copy[j] = array[j];
        }

        // Measure time for parallel quicksort
        clock_t start_parallel = clock();
        quicksort(array_copy, ARRAY_SIZE);
        clock_t end_parallel = clock();
        double time_parallel = (double)(end_parallel - start_parallel) / CLOCKS_PER_SEC;

        // Print timing results
        printf("2_30, %d, %f\n", threshold, time_parallel);
    }

    // Free allocated memory
    free(array);
    return 0;
}