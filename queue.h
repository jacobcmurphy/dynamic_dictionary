/******************************/
/*   Jacob Murphy             */
/*   jcmurphy@brandeis.edu    */
/*   2013-3-20                */
/*   COSI 175                 */
/******************************/

#include "mystring.h"

typedef struct wrapper wrapper;
typedef wrapper* Vwrap;



/* stores information about words in the Dictionary */
typedef struct node
{
	Word w;
	int idTag;
	int numKids;
	Word parent;	
	Vwrap wrapper;
	struct node *prev;
	struct node *next;
}   node;
typedef node* Vertex;
typedef Vertex LinkedList;


/* wrapper for a Vertex in the Queue */
typedef struct wrapper
{
	Vertex v;
	struct wrapper* prev;
	struct wrapper* next;
} wrapper;


/* creates and returns a wrapper for the given Vertex */
Vwrap NewVwrap(Vertex vert)
{
	Vwrap temp = malloc(sizeof(wrapper));
	temp->v = vert;
	temp->prev = NULL;
	temp->next = NULL;
	return temp;
}

/* a queue - used for LRU deletion */
typedef struct myqueue
{
	int size;
	Vwrap front;
	Vwrap end;
} myqueue;
typedef myqueue* Queue;

/* malloc, initialize, and return a new queue */
Queue NewQueue()
{
	Queue temp = malloc(sizeof(myqueue));
	temp->size = 0;
	temp->front = NULL;
	temp->end = NULL;
	return temp;
}

/* add the given Vwrap to the end of the queue */
void Enqueue(Vertex v, Queue q)
{
	v->wrapper = NewVwrap(v);
	if(q->size == 0) {q->front = v->wrapper; q->end = v->wrapper; }
	else
	{
		q->end->next = v->wrapper;
		v->wrapper->prev = q->end;
		v->wrapper->next = NULL;
		q->end = v->wrapper;
	}	
	q->size++;
}

/* return the first Vertex in the queue and update the queue */
Vertex Dequeue (Queue q)
{
	if(q->size < 1) {printf("Error: Queue underflow.\n"); exit(1); }
	Vertex temp = q->front->v;
	
	q->front->next->prev = NULL;
	q->front = q->front->next;
	
	q->size--;
	return temp;
}

/* remove a Vertex from anywhere in the queue 
   this is needed if the Vertex's numKids becomes greater than 0 */
void RemoveFromQueue(Vertex v, Queue q)
{
	if(q->size == 1) {q->front = NULL; q->end = NULL; }
	else if(q->front == v->wrapper) 
	{
		q->front = v->wrapper->next;
		v->wrapper->next->prev = NULL;
	}
	else if (q->end == v->wrapper)
	{
		q->end = v->wrapper->prev;
		v->wrapper->prev->next = NULL;
	}
	else
	{
		if(v->wrapper->next != NULL) { v->wrapper->next->prev = v->wrapper->prev; }
		v->wrapper->prev->next = v->wrapper->next;
	}
	q->size--;
}


/* if a Vertex is used again, it needs to be removed from its current position
   and added to the end of the queue */
void UpdateQueue(Vertex v, Queue q)
{
	if(v->wrapper == NULL)
	{
		Enqueue(v, q);
	}
	else
	{
		RemoveFromQueue(v, q);
		Enqueue(v, q);
	}
}

