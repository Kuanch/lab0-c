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
    bool predefined_descend = false;
    q_sort(head, predefined_descend);
}

void static inline random_data(struct list_head *head)
{
    for (int i = 0; i < 10; i++) {
        char s = (char) (rand() % 26 + 'a');
        q_insert_tail(head, &s);
    }
}

void static inline almost_sorted_data(struct list_head *head)
{
    for (int i = 0; i < 10; i++) {
        char s = (char) (rand() % 26 + 'a');
        q_insert_tail(head, &s);
    }
    q_sort(head, false);
    for (int i = 0; i < 10; i++) {
        char s = (char) (rand() % 26 + 'a');
        q_insert_tail(head, &s);
    }
}

void static inline reverse_sorted_data(struct list_head *head)
{
    for (int i = 0; i < 10; i++) {
        char s = (char) (rand() % 26 + 'a');
        q_insert_head(head, &s);
    }
    q_sort(head, true);
}

void print_list(struct list_head *q)
{
    if (!q)
        return;
    for (struct list_head *node = q->next; node != q; node = node->next) {
        element_t *e = list_entry(node, element_t, list);
        printf("%s ", e->value);
    }
    printf("\n");
}

char *concat(const char *s1, const char *s2)
{
    char *result =
        malloc(strlen(s1) + strlen(s2) + 1);  // +1 for the null-terminator
    memcpy(result, s1, strlen(s1));
    memcpy(result + strlen(s1), s2, strlen(s2) + 1);
    return result;
}

int main(int argc, char **argv)
{
    if (argc != 3) {
        printf("Usage: %s <sort_name> <data_mode>\n", argv[0]);
        return 1;
    }
    char *sort_name = argv[1];
    char *data_mode = argv[2];
    char *save_file;
    void (*sort_func)(struct list_head * head);
    if (strcmp(sort_name, "qsort") == 0) {
        sort_func = &q_sort_wrapper;
        save_file = "qsort_dist";
    } else if (strcmp(sort_name, "listsort") == 0) {
        sort_func = list_sort;
        save_file = "listsort_dist";
    } else {
        printf("Invalid sort name: %s\n", sort_name);
        return 1;
    }

    void (*data_prod)(struct list_head * head);
    if (strcmp(data_mode, "random") == 0) {
        data_prod = random_data;
        save_file = concat(save_file, "_random.txt");
    } else if (strcmp(data_mode, "almost_sort") == 0) {
        data_prod = almost_sorted_data;
        save_file = concat(save_file, "_almost_sorted.txt");
    } else if (strcmp(data_mode, "reverse_sort") == 0) {
        data_prod = reverse_sorted_data;
        save_file = concat(save_file, "_reverse_sorted.txt");
    } else {
        printf("Invalid data mode: %s\n", data_mode);
        return 1;
    }
    struct list_head *q = q_new();
    FILE *fileout = fopen(save_file, "w");
    int num_test = 1;
    for (int test = 0; test < num_test; test++) {
        data_prod(q);
        print_list(q);
        _mm_mfence();
        int64_t start = cpucycles();
        sort_func(q);
        int64_t end = cpucycles();
        printf("Cycles: %ld\n", end - start);
        print_list(q);
        fprintf(fileout, "%ld ", end - start);
        q = q_new();
    }
    fclose(fileout);
}
