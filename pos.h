#ifndef POS_H
#define POS_H

struct pos {
    unsigned int r, c;
};

typedef struct pos pos;


typedef struct pq_entry pq_entry;

struct pq_entry {
    pos p;
    pq_entry *next, *prev;
};


struct posqueue {
    pq_entry *head, *tail;
    unsigned int len;
};

typedef struct posqueue posqueue;

/*Given a row and column coordinate, this creates a position*/
pos make_pos(unsigned int r, unsigned int c);

/*Initializes a new empty queue*/
posqueue* posqueue_new();

/*Adds a new element behind the tail of the queue, and makes that element he new tail*/
void pos_enqueue(posqueue* q, pos p);

/*Takes away head of queue, and returns its position*/
pos pos_dequeue(posqueue* q);

/*Takes away tail of queue and returns its pos*/
pos posqueue_remback(posqueue* q);

/*Entirely frees the memory allocated to a queue and its contents*/
void posqueue_free(posqueue* q);

#endif /* POS_H */