/******************************/
/*   Jacob Murphy             */
/*   jcmurphy@brandeis.edu    */
/*   2-21-2013                */
/*   COSI 175                 */
/******************************/

#include "supportfunctions.h"
#include "queue.h"

#define DICTSIZE  65535          /* maximum number of words in the dictionary                 */
#define MAXFILENAMESIZE 100      /* largest allowed file name path for the file to compress   */
#define SETTINGSFILE "settings.txt"



/* creates, initializes, and returns a new Vertex */
Vertex NewVertex(Word w, int tagNum, Word parentWord)
{
	Vertex tempV = malloc(sizeof(node));
	if(tempV == NULL) { printf("Malloc failed.\n"); exit(1); }
	tempV->w = w;
	tempV->idTag = tagNum;
	tempV->numKids = 0;
	tempV->parent = parentWord;
	tempV->wrapper = NULL;
	tempV->prev = NULL;
	tempV->next = NULL;
	return tempV;
}


/* structure containing all of the possible words */
typedef struct dict
{
	LinkedList* table;
	Queue lruQ;
	char* ADDMETHOD;
	char* DELETEMETHOD;
	int maxSize;
	int wordCount;
} dict;
typedef dict* Dictionary;

/* creates, initializes and returns a Dictionary */
Dictionary NewDictionary(int size, char* insert, char* del)
{
	if(size<256)        { size = 256;  }
	else if(size>65535) { size = 65535;}
	Dictionary tempD = malloc(sizeof(dict));
	tempD->table = malloc(sizeof(LinkedList) * size);
	tempD->lruQ = NewQueue();
	tempD->ADDMETHOD = insert;
	tempD->DELETEMETHOD = del;
	tempD->maxSize = size;
	tempD->wordCount = 0;

	/* adds all ASCII characters as the base alphabet */
	int i;
	for(i=0; i<256; i++)
	{
		Word w = CharToWord((char) i);
		Vertex tempV = NewVertex(w, i, NULL);
		tempD->table[i] = tempV;
		tempD->wordCount++;
	}

	/* sets all positions after ASCII values to NULL */
	int j;
	for(j=256; j<tempD->maxSize; j++)
	{
		tempD->table[j] = NULL;
	}

	return tempD;
}


/* returns int based on the given string */
int Hash(Word w)
{
	unsigned int count = 0;
	int i;
	for(i=0; i < w->length; i++)
	{
		count = (count*33 + (unsigned int)w->str[i]) % DICTSIZE;
	}
	return count % DICTSIZE;
}


/* deletes the given Vertex from the Dictionary */
void RemoveVertex(Vertex v, Dictionary d)
{
	int index = Hash(v->w);
	if(d->table[index] == v) { d->table[index] = v->next; }
	if(v->next != NULL) { v->next->prev = v->prev; }
	if(v->prev != NULL) { v->prev->next = v->next; }	
}


/* returns the id tag of a word if it is in the Dictionary */
int Member(Word w, Dictionary d)
{
	int index = Hash(w);
	Vertex tempV = d->table[index];
	while(tempV != NULL)
	{	
		if (WordEquals(tempV->w, w)) { return tempV->idTag; }
		tempV = tempV->next;
	}
	return -1;
}


/* removes all entries in the given Dictionary */
void EmptyDictionary(Dictionary d)
{
	int i;
        for(i = 0; i<d->maxSize; i++)
        {
		Vertex tempV = d->table[i];

		while(tempV != NULL)
		{
			Vertex tempV2 = tempV;
			tempV = tempV->next;
			free(tempV2->w);
			free(tempV2);
		}
	}
	free(d);
}

/* prints all of the words in the given Dictionary */
void PrintDictionary(Dictionary d)
{
	Vertex v = NULL;
	int i;
	for(i = 0; i < d->maxSize; i++)
	{
		v = d->table[i];
		while(v != NULL)
		{
			printf("Index: %d, Word: %s, Id: %d\n", i, v->w->str, v->idTag);
			v = v->next;
		}
	}
}

/* deletes entries from the given dictionary based on DELETEMETHOD set above */
int DictDelete (Dictionary d)
{
    /* resets the dictionary to the base alphabet */
    if(strequals(d->DELETEMETHOD, "RESTART") == true)
    {
	char* insert = d->ADDMETHOD;
	char* del = d->DELETEMETHOD;
	EmptyDictionary(d);
	d = NewDictionary(DICTSIZE, insert, del);
	
	return 256;

    }
    
   /* remove the least recently used word in the dictionary */
    else if(strequals(d->DELETEMETHOD, "LRU") == true)
    {
	Vertex temp = Dequeue(d->lruQ);
	int val = temp->idTag;
	RemoveVertex(temp, d);

	d->wordCount--;
	return val;
    }

    /* if DELETEMETHOD is "FREEZE" it does nothing to remove a word */
    return d->maxSize;
}


/* creates and returns a Vertex for the given string and adds it to the Dictionary */
Vertex AddToDict (Word w, int tag, Word parent, Dictionary d)
{
	if(d->wordCount < d->maxSize) 
	{
		int index = Hash(w);
		Vertex v = NewVertex(w, tag, parent);
		if(d->table[index] != NULL)
		{
			d->table[index]->prev = v; 
			v->next = d->table[index];
		}
		d->table[index] = v;
		d->wordCount++;

		return v;
	}
	return NULL;
}
