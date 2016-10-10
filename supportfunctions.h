/******************************/
/*   Jacob Murphy             */
/*   jcmurphy@brandeis.edu    */
/*   2013-3-1                 */
/*   COSI 175                 */
/******************************/


typedef int bool;
#define true 1
#define false 0


/* returns the length of the given string */
int strlength (char* s)
{
    if(s == NULL) { return 0;}
    int length = 0;

    while (s[length] != '\0')
    {
        length++;
    }
    return length;
}

/* returns true if the strings are equal, otherwise it returns false */
bool strequals (char* s1, char* s2)
{
    if(s1==NULL && s2==NULL) {return true;}
    else if(s1==NULL && s2!=NULL) {return false;}
    else if(s2==NULL && s1!=NULL) {return false;}
    else if(strlength(s1) != strlength(s2)) {return false;}
    else
    {
        int i;
        for(i=0; i<strlength(s1); i++)
        {
            if(s1[i] != s2[i]) {return false;}
        }
        return true;
    }
}

/* copies one string to another */
void strcopy(char* newStr, char* oldStr)
{
    if(newStr!= NULL && oldStr != NULL)
    {
    	int i;
	    for(i=0; i<=strlength(oldStr); i++)
	    {
        	newStr[i] = oldStr[i];
	    }
	    newStr[i] = '\0';
    }
}

/* combines two strings */
char* strcat(char* s1, char* s2)
{
    if(s1 == NULL) {return s2;}
    else if(s2 == NULL) {return s1;}
    else
    {
        char* newStr = malloc(sizeof(char) * (strlength(s1) + strlength(s2) + 1));
	if(newStr == NULL) { printf("Malloc failed.\n"); exit(1); }
        int i;
        for(i=0; i<strlength(s1); i++)
        {
            newStr[i] = s1[i];
        }
        int j;
        for(j=0; j<strlength(s2); j++)
        {
            newStr[j+strlength(s1)] = s2[j];
        }
        newStr[j+strlength(s1)+1] = '\0';
        return newStr;
    }
}


/* converts a given character to a string */
char* CharToStr(char c)
{
    char* s = malloc(sizeof(char)*2);
    if(s == NULL) { printf("Malloc failed.\n"); exit(1); }
    s[0] = c;
    s[1] = '\0';
    return s;
}
