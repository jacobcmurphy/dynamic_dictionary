/******************************/
/*   Jacob Murphy             */
/*   jcmurphy@brandeis.edu    */
/*   2013-3-18                */
/*   COSI 175                 */
/******************************/

#define MAXWORDSIZE  100 /* longest word allowed in the dictionary */

typedef struct myw
{
	unsigned char* str;
	int length;
} myword;
typedef myword* Word;

/* malloc and return a new Word */
Word NewWord()
{
	Word temp = malloc(sizeof(myword));
	temp->str = malloc(sizeof(unsigned char) * MAXWORDSIZE);
	temp->length = 0;
	return temp;
}

/* prints given word character by character */
void PrintWord(Word w, FILE* out)
{
	int i;
	for(i=0; i< w->length; i++)
	{
		putc(w->str[i], out);
	}
}

/* creates a new Word with exact same data as given word */
Word WordCopy(Word w)
{
	if(w != NULL)
	{
		Word temp = NewWord();
		int i;
		for(i=0; i < w->length; i++)
		{
			temp->str[i] = w->str[i];
		}
		temp->length = w->length;
		temp->str[temp->length] = '\0';
		return temp;
	}
	return NULL;
}


/* creates and returns a new Word that represents w1 and w2 combined */
Word WordCat (Word w1, Word w2)
{
	Word temp = NewWord();
	if(temp == NULL) { printf("word malloc failed.\n"); }
	temp->length = w1->length + w2->length;

	int i;
	for(i=0; i < w1->length; i++)
	{
		temp->str[i] = w1->str[i];
	}

	int j;
	for(j=0; j < w2->length; j++)
	{
		temp->str[i+j] = w2->str[j];
	}
	temp->str[i+j] = '\0';

	return temp;
}

/* determines if two Words contain the same data */
bool WordEquals (Word w1, Word w2)
{
	if(w1==NULL && w2==NULL) { return true;  }
	else if(w1==NULL && w2!=NULL) { return false; }
	else if(w1!=NULL && w2==NULL) { return false; }
	else if(w1->length != w2->length) { return false; }
	else
	{
		int i;
		for(i=0; i < w1->length; i++)
		{
			if(w1->str[i] != w2->str[i]) { return false; }
		}
	}
	return true;
}


/* converts the given character into a Word */
Word CharToWord(char ch)
{
	Word temp = NewWord();
	temp->length = 1;
	temp->str[0] = ch;
	temp->str[1] = '\0';
	return temp;
}

