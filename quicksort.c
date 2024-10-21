#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>

#define THRESHOLD 5000000 // Threshold for switching to sequential quicksort
#define MAX_THREADS 14     // Maximum number of threads

typedef struct
{
    int *array;
    int left;
    int right;
} Task;

// Task queue
Task taskQueue[256];
int taskCount = 0;
bool shutdown = false;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // Mutex for task queue
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;   // Condition variable for task queue

// Swap two elements
void swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Partition the array: Hoare partition
int partition_hoare(int *arr, int low, int high)
{
    int pivot = arr[low];
    int i = low - 1, j = high + 1;

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
        sequential_quicksort(array, left, pivotIndex);
        sequential_quicksort(array, pivotIndex + 1, right);
    }
}

// Worker function for the thread pool
void *worker(void *arg)
{
    while (1)
    {
        pthread_mutex_lock(&mutex);
        while (taskCount == 0 && !shutdown)
        {
            pthread_cond_wait(&cond, &mutex);
        }
        if (shutdown)
        {
            pthread_mutex_unlock(&mutex);
            break;
        }
        // Get the next task from the queue
        Task task = taskQueue[--taskCount];
        pthread_mutex_unlock(&mutex);

        // Perform the quicksort task
        int *array = task.array;
        int left = task.left;
        int right = task.right;

        if (right - left < THRESHOLD)
        {
            sequential_quicksort(array, left, right);
        }
        else
        {
            int pivotIndex = partition_hoare(array, left, right);
            // Add left and right tasks to the queue
            pthread_mutex_lock(&mutex);
            taskQueue[taskCount++] = (Task){array, left, pivotIndex};
            taskQueue[taskCount++] = (Task){array, pivotIndex + 1, right};
            pthread_mutex_unlock(&mutex);
            pthread_cond_signal(&cond);
        }
    }
    return NULL;
}

// Function to initialize and start parallel quicksort using a thread pool
void quicksort(int *array, int size)
{
    pthread_t threadPool[MAX_THREADS];

    // Start the worker threads
    for (int i = 0; i < MAX_THREADS; i++)
    {
        pthread_create(&threadPool[i], NULL, worker, NULL);
    }

    // Add the initial task to the queue
    pthread_mutex_lock(&mutex);
    taskQueue[taskCount++] = (Task){array, 0, size - 1};
    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&cond);

    // Wait for the tasks to complete
    pthread_mutex_lock(&mutex);
    while (taskCount > 0)
    {
        pthread_mutex_unlock(&mutex);
        pthread_mutex_lock(&mutex);
    }
    shutdown = true;
    pthread_mutex_unlock(&mutex);
    pthread_cond_broadcast(&cond);

    // Join the worker threads
    for (int i = 0; i < MAX_THREADS; i++)
    {
        pthread_join(threadPool[i], NULL);
    }
}

// Function to generate a random array of integers
void generate_random_array(int *array, int size)
{
    for (int i = 0; i < size; i++)
    {
        array[i] = rand() % 100;
    }
}

bool is_sorted(int *arr, int size) {
    for (int i = 0; i < size; i++) {
        if (arr[i] > arr[i + 1]) {
            // print the two elements that are not in order
            printf("arr[%d] = %d, arr[%d] = %d\n", i, arr[i], i + 1, arr[i + 1]);
            return false;
        }
    }
    return true;
}

// print the array
void print_array(int *arr, int size) {
    for (int i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

// Main function to test the parallel quicksort
int main(void) {
    int n = 1 << 15;
    int *array = (int *)malloc(n * sizeof(int));

    generate_random_array(array, n);
    print_array(array, 10);
    clock_t start_parallel = clock();
    quicksort(array, n);
    clock_t end_parallel = clock();
    double time_parallel = (double)(end_parallel - start_parallel) / CLOCKS_PER_SEC;

    printf("Parallel quicksort time for 2^%d: %f seconds\n", 15, time_parallel);
    print_array(array, 10);
    is_sorted(array, n) ? printf("Array is sorted\n") : printf("Array is not sorted\n");
    free(array);
    return 0;
}
