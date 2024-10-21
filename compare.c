#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to compare two integers (used for qsort)
int compare(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

// Function to parse a line into an array of integers
int parse_line(char *line, int *array) {
    int count = 0;
    char *token = strtok(line, ",");
    while (token != NULL) {
        array[count++] = atoi(token);
        token = strtok(NULL, ",");
    }
    return count;
}

// Function to check if the second line is a sorted version of the first line
void check_sorted_version(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    char line1[256], line2[256];
    if (fgets(line1, sizeof(line1), file) == NULL || fgets(line2, sizeof(line2), file) == NULL) {
        perror("Error reading lines");
        fclose(file);
        return;
    }
    fclose(file);

    int array1[256], array2[256];
    int n1 = parse_line(line1, array1);
    int n2 = parse_line(line2, array2);

    if (n1 != n2) {
        printf("The second line is not a sorted version of the first line.\n");
        return;
    }

    qsort(array1, n1, sizeof(int), compare);

    for (int i = 0; i < n1; i++) {
        if (array1[i] != array2[i]) {
            printf("The second line is not a sorted version of the first line.\n");
            return;
        }
    }

    printf("The second line is a sorted version of the first line.\n");
}

int main() {
    check_sorted_version("array.txt");
    return 0;
}