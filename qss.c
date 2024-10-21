#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define PARALLEL_THRESHOLD 10000 // Threshold below which to switch to sequential sorting
#define SMALL_THRESHOLD 50       // Threshold below which to use insertion sort

// Structure to pass data to threads
typedef struct {
    int *arr;
    int left;
    int right;
} QuicksortArgs;

// Function to perform insertion sort
void insertion_sort(int arr[], int left, int right) {
    for (int i = left + 1; i <= right; i++) {
        int key = arr[i];
        int j = i - 1;
        while (j >= left && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

// Partition function for quicksort
int partition(int arr[], int left, int right) {
    int pivot = arr[right];
    int i = left - 1;
    for (int j = left; j < right; j++) {
        if (arr[j] <= pivot) {
            i++;
            int temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
        }
    }
    int temp = arr[i + 1];
    arr[i + 1] = arr[right];
    arr[right] = temp;
    return i + 1;
}

// Sequential quicksort
void sequential_quicksort(int arr[], int left, int right) {
    if (left < right) {
        if (right - left < SMALL_THRESHOLD) {
            insertion_sort(arr, left, right);
        } else {
            int pivot = partition(arr, left, right);
            sequential_quicksort(arr, left, pivot - 1);
            sequential_quicksort(arr, pivot + 1, right);
        }
    }
}

// Parallel quicksort function
void *parallel_quicksort(void *args) {
    QuicksortArgs *qsArgs = (QuicksortArgs *)args;
    int left = qsArgs->left;
    int right = qsArgs->right;
    int *arr = qsArgs->arr;

    if (left < right) {
        if (right - left < PARALLEL_THRESHOLD) {
            sequential_quicksort(arr, left, right);
        } else {
            int pivot = partition(arr, left, right);

            // Prepare arguments for the two recursive calls
            QuicksortArgs leftArgs = {arr, left, pivot - 1};
            QuicksortArgs rightArgs = {arr, pivot + 1, right};

            pthread_t leftThread, rightThread;

            // Create threads for parallel sorting
            pthread_create(&leftThread, NULL, parallel_quicksort, &leftArgs);
            pthread_create(&rightThread, NULL, parallel_quicksort, &rightArgs);

            // Wait for both threads to complete
            pthread_join(leftThread, NULL);
            pthread_join(rightThread, NULL);
        }
    }
    return NULL;
}

// Helper function to initiate parallel quicksort
void quicksort(int arr[], int size) {
    QuicksortArgs args = {arr, 0, size - 1};
    parallel_quicksort(&args);
}

// Function to generate a random array
void generate_random_array(int arr[], int size) {
    srand(time(NULL));
    for (int i = 0; i < size; i++) {
        arr[i] = rand() % 1000000;
    }
}

void benchmark_sorts(int n) {
    int size = 1 << n;
    int *arr = malloc(size * sizeof(int));

    generate_random_array(arr, size);

    // Measure time for sequential quicksort
    clock_t start_sequential = clock();
    sequential_quicksort(arr, 0, size - 1);
    clock_t end_sequential = clock();
    double sequential_time = (double)(end_sequential - start_sequential) / CLOCKS_PER_SEC;
    printf("Sequential quicksort time for 2^%d: %f seconds\n", n, sequential_time);

    // write the sequential time to a file
    FILE *f = fopen("sequential_time.csv", "a");
    fprintf(f, "2^%d,%f\n", n, sequential_time);
    fclose(f);

    // Cleanup
    free(arr);
}

int main() {
    int size[] = { 10, 15, 20, 25, 30}; // Size of the array
    for (int i = 0; i < 5; i++) {
        benchmark_sorts(size[i]);
    }

    return 0;
}
