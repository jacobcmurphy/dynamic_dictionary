/******************************/
/*   Jacob Murphy             */
/*   jcmurphy@brandeis.edu    */
/*   2-21-2013                */
/*   COSI 175                 */
/******************************/

#include <stdio.h>
#include <stdlib.h>
#include "dictionary.h"


/* reads input from the source character-by-character */
char* GetInput(FILE* source)
{
    char* name = malloc((MAXFILENAMESIZE+1) * sizeof(char));
    if(name == NULL) { printf("Malloc failed.\n"); exit(1); }
    char ch;
    int charCount = 0;
    while((ch = (char) fgetc(source)) != '\n' && charCount<MAXFILENAMESIZE)
    {
        name[charCount++] = ch;
    }
    name[charCount] = '\0';
    return name;
}

/* takes input name and creates output name by adding ".bin" */
char* SetOutName(char* name)
{
    char* newName = malloc((strlength(name)+4) * sizeof(char));
    if(newName == NULL) { printf("Malloc failed.\n"); exit(1); }

    int end =  strlength(name);
    int j;
    for(j=0; j<end; j++)
    {
        newName[j] = name[j];
    }

    newName[end] = '.';
    newName[end+1] = 'b';
    newName[end+2] = 'i';
    newName[end+3] = 'n';
    newName[end+4] = '\0';

    return newName;
}

/* outputs i to the compressed file */
void SendIndex(FILE* fOut, unsigned int i)
{
   unsigned char ch1 = (i & 0xFF);
   unsigned char ch2 = (i >> 8);
   putc(ch1, fOut);
   putc(ch2, fOut);

}

/*sets the starting helper array */
void SetArray(Vertex* arr, Dictionary d)
{
	int a;
	for(a=0; a<256; a++)
	{
		arr[a] = d->table[a];
	}
	int b;
	for(b=256; b<DICTSIZE; b++)
	{
		arr[b] = NULL;
	}
}



int main()
{
	FILE* settings = fopen(SETTINGSFILE, "r");
	if(settings == NULL) { printf("settings malloc failed\n"); }
	char* insertMethod = GetInput(settings);
	printf("Insert method: %s\n", insertMethod);
	char* deleteMethod = GetInput(settings);
	printf("Delete method: %s\n", deleteMethod);
	fclose(settings);

	Dictionary d = NewDictionary(DICTSIZE, insertMethod, deleteMethod);

	Vertex* vArr = malloc(sizeof(node) * DICTSIZE);
	SetArray(vArr, d);


	/* gets name of file to be compressed */
	printf("Please enter the file path for the file to compress: \n");
	char* inName = GetInput(stdin);
	FILE* fIn =  fopen(inName, "rb");
	while(fIn == NULL)
	{
	    printf("Invalid name given. Please enter another file name: \n");
	    inName = GetInput(stdin);
	    fIn = fopen(inName, "rb");
	}

	/* names and creates output file */
	char* outName = SetOutName(inName);
	FILE* fOut = fopen(outName, "wb");

	Word prevMatch = NULL;
	int tagNum = 255;
	int ch = getc(fIn);

	while (ch != EOF)
	{
		Word w = NewWord();
		if(w == NULL) { printf("Malloc failed.\n"); exit(1); }
		w->str[0] = (unsigned char) ch;

		w->str[1] = '\0';
		w->length = 1;
		int tag = Member(w, d);
		int prevTag=tag;
		while(tag != -1 && ch!=EOF && w->length < MAXWORDSIZE)
		{

			ch =  getc(fIn);
			w->str[w->length] = (char) ch;
			w->length++;
			w->str[w->length] = '\0';
			prevTag = tag;
			tag = Member(w,d);
		}
		w->str[--w->length] = '\0';

		/* print the idTag of the matched word to the output file */
		SendIndex(fOut, prevTag);	

		/* update LRU queue here */ 
		if(prevTag>255 && strequals(d->DELETEMETHOD, "LRU") == true)
		{ 
			UpdateQueue(vArr[prevTag], d->lruQ);
		}

		/*add a new word to the dictionary */
		if(prevMatch != NULL)
		{
			/*create the new word based on FC or CM updating method */
			Word addWord = NULL;
			if(strequals(d->ADDMETHOD, "FC") == true)
			{
				addWord = WordCopy(prevMatch);
				if(addWord == NULL) { printf("Malloc failed.\n"); exit(1); }
				addWord->str[addWord->length++] = w->str[0];
				addWord->str[addWord->length] = '\0';

			}
			else if(strequals(d->ADDMETHOD, "CM") == true)
			{
				addWord = WordCat(prevMatch, w);
			}

			/* insert the newly created word into the dictionary */
			if(tagNum < d->maxSize && Member(addWord, d) == -1)
			{
				/* if the dictionary needs more space, use the deletion method */
				if(d->wordCount >= d->maxSize)
				{
					if(strequals(d->DELETEMETHOD, "RESTART") == true)
					{
						tagNum = DictDelete(d);
						SetArray(vArr, d);
						printf("RESTARTING.\n");
						w = NULL;
					}
					if(strequals(d->DELETEMETHOD, "LRU") == true)
					{
						/* decrement parent's numKids - 
						   if it becomes 0, enqueue parent */
						tagNum = DictDelete(d);
						printf("LRU TEST\n");
						if(vArr[tagNum]->parent != NULL)
						{

							int index = Hash(vArr[tagNum]->parent);
							Vertex p = d->table[index];
							while(WordEquals(p->w, vArr[tagNum]->parent) == false)
							{
								p = p->next;
							}
							p->numKids--;
							if(p->numKids == 0) { Enqueue(p, d->lruQ); }
						}
						/* prevent possible problems from deletion */
						vArr[tagNum] = NULL;
						tagNum--;
						w = NULL;
					}					
				}
				tagNum++;
				vArr[tagNum] = AddToDict(addWord, tagNum, prevMatch, d);
				if(strequals(d->DELETEMETHOD, "LRU") == true)
				/* enqueue the added word and update its parent's numKids */
				{ 
					Enqueue(vArr[tagNum], d->lruQ); 
				}
			}
		}
		
		free(prevMatch);
		prevMatch = w;
	}

	/* close files and cleanup */
	fclose(fIn);
	fclose(fOut);
	EmptyDictionary(d);
	free(inName);
	free(outName);
	printf("\nCompression now finished.\n");
	return 0;
}
