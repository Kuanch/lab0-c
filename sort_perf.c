#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <x86intrin.h>

#include "dudect/cpucycles.h"
#include "queue.h"

void static inline q_sort_wrapper(struct list_head *head)
{
    bool predefined_descend = true;  // or false, depending on your requirement
    q_sort(head, predefined_descend);
}

int main(int argc, char **argv)
{
    if (argc != 2) {  // Corrected condition to check if exactly one argument is
                      // provided
        printf("Usage: %s <sort_name>\n", argv[0]);
        return 1;
    }
    char *sort_name = argv[1];
    char *save_file;
    void (*sort_func)(struct list_head * head);
    if (strcmp(sort_name, "qsort") == 0) {
        sort_func = &q_sort_wrapper;
        save_file = "qsort_perf.txt";
    } else if (strcmp(sort_name, "listsort") == 0) {
        sort_func = list_sort;
        save_file = "listsort_perf.txt";
    } else {
        printf("Invalid sort name: %s\n", sort_name);
        return 1;
    }
    struct list_head *q = q_new();
    FILE *fileout = fopen(save_file, "w");
    for (int num_data = 10; num_data <= 100; num_data += 10) {
        for (int i = 0; i < num_data; i++) {
            char s = (char) (rand() % 26 + 'a');
            q_insert_tail(q, &s);
        }
        // _mm_mfence();
        int64_t start = cpucycles();
        sort_func(q);
        int64_t end = cpucycles();
        // save the result to text for analysis later
        fprintf(fileout, "%d %ld\n", num_data, end - start);
        q = q_new();
    }
}
