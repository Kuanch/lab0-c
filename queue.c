#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Helper functions */
/*
 * e_new(char *s) - Create a new element
 * @s: string would be put into the element
 * Return: the pointer to the new element, NULL for allocation failed
 */
static inline element_t *e_new(char *s)
{
    if (!s)
        return NULL;

    element_t *new_e = malloc(sizeof(element_t));
    if (!new_e)
        return NULL;
    INIT_LIST_HEAD(&new_e->list);

    size_t slen = strlen(s) + 1;
    new_e->value = malloc(slen);
    if (!new_e->value) {
        free(new_e);
        return NULL;
    }
    memcpy(new_e->value, s, slen);

    return new_e;
}

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *new_q = malloc(sizeof(struct list_head));
    if (new_q)
        INIT_LIST_HEAD(new_q);
    return new_q;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;
    for (struct list_head *node = l->next, *next; node != l; node = next) {
        next = node->next;
        element_t *e = list_entry(node, element_t, list);
        free(e->value);
        free(e);
    }
    free(l);
    return;
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!s)
        return false;
    element_t *new_e = e_new(s);
    if (!new_e)
        return false;
    list_add(&new_e->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!s)
        return false;
    element_t *new_e = e_new(s);
    if (!new_e)
        return false;
    list_add_tail(&new_e->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *e = list_entry(head->next, element_t, list);
    list_del_init(head->next);
    if (sp && bufsize) {
        size_t min = strlen(e->value) + 1;
        min = min > bufsize ? bufsize : min;
        memcpy(sp, e->value, min);
        sp[min - 1] = '\0';
    }
    return e;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *e = list_entry(head->prev, element_t, list);
    list_del_init(head->prev);
    if (sp && bufsize) {
        size_t min = strlen(e->value) + 1;
        min = min > bufsize ? bufsize : min;
        memcpy(sp, e->value, min);
        sp[min - 1] = '\0';
    }
    return e;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    int qlen = 0;
    for (struct list_head *p = head->next; p != head; p = p->next)
        qlen += 1;
    return qlen;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    struct list_head *slow = head->next;
    struct list_head *fast = head->next;
    for (; fast && fast->next != head;
         slow = slow->next, fast = fast->next->next)
        ;
    if (slow == head)
        return false;
    list_del(slow);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    for (struct list_head *p = head->next, *next, *prev; p != head; p = next) {
        next = p->next;
        prev = p->prev;
        p->prev = next;
        p->next = prev;
    }
    struct list_head *next = head->next;
    head->next = head->prev;
    head->prev = next;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend) {}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
