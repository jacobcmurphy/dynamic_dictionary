/******************************/
/*   Jacob Murphy             */
/*   jcmurphy@brandeis.edu    */
/*   2-21-2013                */
/*   COSI 175                 */
/******************************/

#include <stdio.h>
#include <stdlib.h>
#include "dictionary.h"


/* reads input from source character-by-character */
char* GetTextInput(FILE* source)
{
    char* name = malloc((MAXFILENAMESIZE) * sizeof(char));
    char ch;
    int charCount = 0;
    while((ch = (char) fgetc(source)) != '\n' && charCount<MAXFILENAMESIZE)
    {
        name[charCount++] = ch;
    }
    name[charCount] = '\0';
    return name;
}

/* takes input name and creates output name by adding "unc" it and removing the ".bin" extension */
char* SetOutName(char* name)
{
	char* newName = malloc((strlength(name)+4) * sizeof(char));
	if(newName == NULL) { printf("Malloc failed.\n"); exit(1); }

	int end = strlength(name) - 4;

	int j;
	for(j=0; j<end; j++)
	{
		newName[j] = name[j];
	}

	int i;
	for(i= strlength(newName); i>=0; i--)
	{
		newName[i+3] = newName[i];
	}
	newName[0] = 'u';
	newName[1] = 'n';
	newName[2] = 'c';

	return newName;
}


/* reads in two characters from the file and returns avalue equal to an idTag of a Vertex */
int GetInput(FILE* f)
{
    int c1 = getc(f);
    if(c1 == EOF) { return -1;}
    int c2 = getc(f);
    if(c2 == EOF) { return -1;}
    unsigned char ch1 = (unsigned char) c1;
    unsigned char ch2 = (unsigned char) c2;
    unsigned int val = (ch1 &0xFF) | (ch2<<8);
    return val;
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
	char* insertMethod = GetTextInput(settings);
	printf("Insert method: %s\n", insertMethod);
	char* deleteMethod = GetTextInput(settings);
	printf("Delete method: %s\n", deleteMethod);
	fclose(settings);

	Dictionary d = NewDictionary(DICTSIZE, insertMethod, deleteMethod);

	Vertex* vArr = malloc(sizeof(node) * DICTSIZE);
	SetArray(vArr, d);

	/* gets name of file to be compressed */
	printf("Please enter the file path for the file to decompress: \n");
	char* inName = GetTextInput(stdin);
	FILE* fIn =  fopen(inName, "rb");
	while(fIn == NULL)
	{
		printf("Invalid name given. Please enter another file name: \n");
		inName = GetTextInput(stdin);
		fIn = fopen(inName, "rb");
	}

	/* names and creates output file */
	char* outName = SetOutName(inName);
	FILE* fOut = fopen(outName, "wb");

	Word prevMatch = NULL;
	int tagNum = 255;
	int index = GetInput(fIn);

	while(index != -1)
	{
		Word currentMatch = vArr[index]->w;

		/* print the word to the output file */
	        PrintWord(currentMatch, fOut);

		/* update LRU queue here */ 
		if(index>255 && strequals(d->DELETEMETHOD, "LRU") == true)
		{ 
			UpdateQueue(vArr[index], d->lruQ);
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
				addWord->str[addWord->length++] = currentMatch->str[0];
				addWord->str[addWord->length] = '\0';
			}
			else if(strequals(d->ADDMETHOD, "CM") == true)
			{
				addWord = WordCat(prevMatch, currentMatch);
			}

			/* insert the newly created word into the dictionary */
			if(tagNum < d->maxSize  && Member(addWord, d) == -1)
			{
				/* if the dictionary needs more space, use the deletion method */
				if(d->wordCount >= d->maxSize)
				{
					if(strequals(d->DELETEMETHOD, "RESTART") == true)
					{
						tagNum = DictDelete(d);
						SetArray(vArr, d);
						printf("RESTARTING.\n");
						currentMatch = NULL; 
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
						currentMatch = NULL;
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
		prevMatch = currentMatch;
		index = GetInput(fIn);
	}

	/* close files and cleanup */
	fclose(fIn);
	fclose(fOut);
	EmptyDictionary(d);
	free(inName);
	free(outName);
	printf("\nDecompression now finished.\n");
	return 0;
}
