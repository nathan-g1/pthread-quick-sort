#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_THREADS 8
#define THRESHOLD 10000

typedef struct {
    int* array;
    int size;
    int threadNumber;
} PQuicksortArgs;

typedef struct {
    int* array;
    int left;
    int right;
    int pivot;
} ParallelPartitionArgs;

// Custom barrier implementation
typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int count;
    int tripCount;
} CustomBarrier;

CustomBarrier barrier;

// Custom barrier initialization
void barrier_init(CustomBarrier* barrier, int count) {
    pthread_mutex_init(&barrier->mutex, NULL);
    pthread_cond_init(&barrier->cond, NULL);
    barrier->count = 0;
    barrier->tripCount = count;
}

// Custom barrier wait function
void barrier_wait(CustomBarrier* barrier) {
    pthread_mutex_lock(&barrier->mutex);
    barrier->count++;
    if (barrier->count >= barrier->tripCount) {
        barrier->count = 0; // Reset for reuse
        pthread_cond_broadcast(&barrier->cond);
    } else {
        pthread_cond_wait(&barrier->cond, &barrier->mutex);
    }
    pthread_mutex_unlock(&barrier->mutex);
}

// Custom barrier destroy function
void barrier_destroy(CustomBarrier* barrier) {
    pthread_mutex_destroy(&barrier->mutex);
    pthread_cond_destroy(&barrier->cond);
}

// Swap function
void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}


// Partition function using the selected pivot
int partition(int* array, int left, int right, int pivot) {
    int i = left - 1;
    int j = right + 1;
    while (1) {
        do {
            i++;
        } while (array[i] < pivot);

        do {
            j--;
        } while (array[j] > pivot);

        if (i >= j) {
            return j;
        }
        swap(&array[i], &array[j]);
    }
}

// Sequential sorting (Insertion Sort for simplicity)
void sequential_sort(int* array, int left, int right) {
    // for (int i = left + 1; i <= right; i++) {
    //     int key = array[i];
    //     int j = i - 1;
    //     while (j >= left && array[j] > key) {
    //         array[j + 1] = array[j];
    //         j--;
    //     }
    //     array[j + 1] = key;
    // }
    if (left < right)
    {

        int pivotIndex = partition(array, left, right, array[(left + right) / 2]);
        sequential_sort(array, left, pivotIndex - 1);
        sequential_sort(array, pivotIndex + 1, right);
    }
}

// Pivot selection (simple middle-element)
int choose_pivot(int* array, int left, int right) {
    return array[(left + right) / 2];
}

// Parallel partition function
void* parallel_partition(void* args) {
    ParallelPartitionArgs* partitionArgs = (ParallelPartitionArgs*)args;
    int* array = partitionArgs->array;
    int left = partitionArgs->left;
    int right = partitionArgs->right;
    int pivot = partitionArgs->pivot;

    int split = partition(array, left, right, pivot);
    return (void*)(long)split; // Returning the split index
}

// Recursive parallel quicksort function
void* PQuicksort(void* args) {
    PQuicksortArgs* sortArgs = (PQuicksortArgs*)args;
    int* array = sortArgs->array;
    int size = sortArgs->size;
    int threadNumber = sortArgs->threadNumber;

    // If only one thread, perform sequential sorting
    if (threadNumber == 1 || size < THRESHOLD) {
        sequential_sort(array, 0, size - 1);
        return NULL;
    }

    int pivot = choose_pivot(array, 0, size - 1);

    // Parallel partitioning phase
    pthread_t partitionThread;
    ParallelPartitionArgs partitionArgs = {array, 0, size - 1, pivot};
    pthread_create(&partitionThread, NULL, parallel_partition, &partitionArgs);

    // Wait for partitioning to complete
    void* partitionResult;
    pthread_join(partitionThread, &partitionResult);
    int split = (int)(long)partitionResult;

    // Thread splitting
    int threadSplit = threadNumber / 2;

    // Barrier to synchronize threads
    barrier_wait(&barrier);

    // Recursive calls
    PQuicksortArgs leftArgs = {array, split + 1, threadSplit};
    PQuicksortArgs rightArgs = {array + split + 1, size - split - 1, threadNumber - threadSplit};

    pthread_t leftThread, rightThread;
    pthread_create(&leftThread, NULL, PQuicksort, &leftArgs);
    pthread_create(&rightThread, NULL, PQuicksort, &rightArgs);

    pthread_join(leftThread, NULL);
    pthread_join(rightThread, NULL);

    return NULL;
}

// Function to initialize parallel quicksort
void parallel_quicksort(int* array, int size) {
    PQuicksortArgs args = {array, size, MAX_THREADS};
    barrier_init(&barrier, MAX_THREADS);
    PQuicksort(&args);
    barrier_destroy(&barrier);
}

// Utility function to generate random data
void generate_random_array(int* array, int size) {
    for (int i = 0; i < size; i++) {
        array[i] = rand() % 1000;
    }
}

// Main function
// int main() {
//     int n = 1 << 10; // Array size (2^20)
//     int* array = (int*)malloc(n * sizeof(int));

//     // Generate random data
//     generate_random_array(array, n);

//     // Perform parallel quicksort
//     clock_t start_parallel = clock();
//     parallel_quicksort(array, n);
//     clock_t end_parallel = clock();

//     // Print timing results
//     double time_parallel = (double)(end_parallel - start_parallel) / CLOCKS_PER_SEC;
//     printf("Parallel Quicksort Time: %f\n", time_parallel);
//     // Free memory
//     free(array);

//     return 0;
// }


// Function to print the array (for debugging)
void print_array(int* array, int size) {
    for (int i = 0; i < size; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");
}

int main() {
    // int sizeArray[] = {1 << 15, 1 << 15, 1 << 20};
    // int sizeArray[] = {1 << 15};
    int sizeArray[] = {1 << 10};
    int n = sizeArray[0];
    int *array = (int *)malloc(n * sizeof(int));
    int *array_copy = (int *)malloc(n * sizeof(int));
    // Generate a random array
    generate_random_array(array, n);
    // Copy the array for sequential quicksort
    for (int i = 0; i < n; i++)
    {
        array_copy[i] = array[i];
    }
    printf("q6\n");
    printf("array size, sequential time, parallel time, speedup\n");

    // // Measure time for sequential quicksort
    print_array(array, n);
    clock_t start_sequential = clock();
    // parallel_quicksort(array, n);
    sequential_sort(array, 0, n - 1);
    clock_t end_sequential = clock();
    double time_sequential = (double)(end_sequential - start_sequential) / CLOCKS_PER_SEC;
    printf("after\n");

    print_array(array, n);
    // Measure time for parallel quicksort
    // clock_t start_parallel = clock();
    // parallel_quicksort(array, n);
    // clock_t end_parallel = clock();
    // double time_parallel = (double)(end_parallel - start_parallel) / CLOCKS_PER_SEC;

    // Print timing results
    // printf("%d, %f, %f, %f\n", n, 0.0, time_parallel, 0.0);
    printf("%d, %f, %f, %f\n", n, time_sequential, 0.0, 0.0);
    // printf("%d, %f, %f, %f\n", n, time_sequential, time_parallel, time_sequential / time_parallel);

    // Free allocated memory
    free(array);
    free(array_copy);

    return 0;
}