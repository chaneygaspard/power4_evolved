#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pos.h"

pos make_pos(unsigned int r, unsigned int c) {
    pos p = {r, c};
    return p;
}

posqueue* posqueue_new() {
    posqueue* q = (posqueue*)malloc(sizeof(posqueue));
    if (q == NULL) {
        return NULL;
    }

    q->head = NULL;
    q->tail = NULL;
    q->len = 0;

    return q;
}


void pos_enqueue(posqueue* q, pos p) {
    pq_entry* new_tail = (pq_entry*)malloc(sizeof(pq_entry));

    new_tail->p = p;
    new_tail->next = NULL;
    new_tail->prev = q->tail;

    if (q->tail != NULL) {
        q->tail->next = new_tail;
    }
    else {
        q->head = new_tail;
    }

    q->tail = new_tail;
    q->len++;
}


pos pos_dequeue(posqueue* q) {
    if (q == NULL || q->head == NULL) {
        fprintf(stderr, "Error: Cannot dequeue from an empty queue.\n");
        exit(1);
    }

    pq_entry* front_element = q->head;
    pos return_pos = front_element->p;

    q->head = front_element->next;
    if (q->head != NULL) {
        q->head->prev = NULL;
    }
    else {
        q->tail = NULL;
    }
    free(front_element);

    q->len -= 1;
    return return_pos;
}


pos posqueue_remback(posqueue* q) {
    if (q == NULL || q->tail == NULL) {
        fprintf(stderr, "Error: Cannot take element out of an empty queue.\n");
        exit(1);
    }

    pq_entry* back_element = q->tail;
    pos return_pos = back_element->p;

    q->tail = back_element->prev;
    if (q->tail != NULL) {
        q->tail->next = NULL;
    }
    else {
        q->head = NULL;
    }
    free(back_element);

    q->len -= 1;
    return return_pos;
}


void posqueue_free(posqueue* q) {
    if (q == NULL) {
        return;
    }

    pq_entry* index = q->head;
    while (index != NULL) {
        pq_entry* new_index = index->next;
        free(index);
        index = new_index;
    }

    free(q);
}