#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

void bubbleSort(int arr[], int n);
void selectionSort(int arr[], int n);
void insertionSort(int arr[], int n);
void mergeSort(int arr[], int low, int high);
void bogoSort(int arr[], int n);

void printHelp();

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printHelp();
        return 1;
    }

    char *algorithm = argv[1];
    char *filename = argv[2];

    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Unable to open file %s\n", filename);
        return 1;
    }

    int size;
    fscanf(file, "%d", &size);
    int *arr = malloc(size * sizeof(int));
    for (int i = 0; i < size; i++) {
        fscanf(file, "%d", &arr[i]);
    }
    fclose(file);

    clock_t start = clock();

    if (strcmp(algorithm, "-b") == 0) {
        printf("Time complexity: Best Case - O(n), Worst Case - O(n^2)\n");
        bubbleSort(arr, size);
    } else if (strcmp(algorithm, "-s") == 0) {
        printf("Time complexity: Best Case - O(n^2), Worst Case - O(n^2)\n");
        selectionSort(arr, size);
    } else if (strcmp(algorithm, "-i") == 0) {
        printf("Time complexity: Best Case - O(n), Worst Case - O(n^2)\n");
        insertionSort(arr, size);
    } else if (strcmp(algorithm, "-m") == 0) {
        printf("Time complexity: Best Case - O(n log n), Worst Case - O(n log n)\n");
        mergeSort(arr, 0, size - 1);
    } else if (strcmp(algorithm, "-l") == 0) {
        printf("Time complexity: Best Case - O(n), Worst Case - Infinite\n");
        bogoSort(arr, size);
    } else if (strcmp(algorithm, "--help") == 0) {
        printHelp();
        return 0;
    } else {
        printf("Unknown algorithm\n");
        return 1;
    }

    clock_t end = clock();
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;

    printf("Sorted numbers:\n");
    for (int i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    printf("Execution time: %f seconds\n", time_taken);

    free(arr);
    return 0;
}

void bubbleSort(int arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

void selectionSort(int arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        int minIndex = i;
        for (int j = i + 1; j < n; j++) {
            if (arr[j] < arr[minIndex]) {
                minIndex = j;
            }
        }
        int temp = arr[i];
        arr[i] = arr[minIndex];
        arr[minIndex] = temp;
    }
}

void insertionSort(int arr[], int n) {
    for (int i = 1; i < n; i++) {
        int key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

void merge(int arr[], int low, int mid, int high);
void mergeSort(int arr[], int low, int high);

void mergeSort(int arr[], int low, int high) {
    if (low < high) {
        int mid = low + (high - low) / 2;
        mergeSort(arr, low, mid);
        mergeSort(arr, mid + 1, high);
        merge(arr, low, mid, high);
    }
}

void merge(int arr[], int low, int mid, int high) {
    int n1 = mid - low + 1;
    int n2 = high - mid;

    int left[n1], right[n2];
    for (int i = 0; i < n1; i++)
        left[i] = arr[low + i];
    for (int j = 0; j < n2; j++)
        right[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = low;
    while (i < n1 && j < n2) {
        if (left[i] <= right[j]) {
            arr[k] = left[i];
            i++;
        } else {
            arr[k] = right[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        arr[k] = left[i];
        i++;
        k++;
    }

    while (j < n2) {
        arr[k] = right[j];
        j++;
        k++;
    }
}

bool isSorted(int arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        if (arr[i] > arr[i + 1]) {
            return false;
        }
    }
    return true;
}

void shuffle(int arr[], int n) {
    for (int i = 0; i < n; i++) {
        int j = rand() % n;
        int temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
}

void bogoSort(int arr[], int n) {
    while (!isSorted(arr, n)) {
        shuffle(arr, n);
    }
}

void printHelp() {
    printf("Usage: ./sort <algorithm> <numbers.txt>\n");
    printf("Sorting algorithms:\n");
    printf("-b : Bubble Sort\n");
    printf("-s : Selection Sort\n");
    printf("-i : Insertion Sort\n");
    printf("-m : Merge Sort\n");
    printf("-l : Bogo Sort\n");
    printf("--help : Display this help message\n");
}
