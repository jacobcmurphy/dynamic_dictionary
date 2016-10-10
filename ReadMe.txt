Jacob Murphy
jcmurphy@brandeis.edu
2013-3-20
Brandeis Universtiy COSI 175
-----------------------------------------------------------------------------------------------

How to use the program to compress:
	From terminal, type ./comp
	You will be prompted to enter the file name, enter the name exactly; 
	if it is in a different directory, you will have to provide the path.
	The compressed file will be called <original name>.bin

How to use the program to decompress:
	From terminal, type ./decomp
	You will be prompted to enter the file name, enter the name exactly; 
	if it is in a different directory, you will have to provide the path.
	The decompressed file will be called unc<original name>

Settings:
	The program can compress has 2 settings for updating the dictionary 
	and 3 settings to delete from the dictionarys. The dictionary can have a size
	between 256 words and 65535 words. These settings can be changed
	in the settings.txt file - the first line is for update method
	and the second line is for the deletion method.
	
	Updating methods:
		"FC" (First character): Add the previous match followed by 
					the first character of the current match.
		"CM" (Current match): Add the previous match followed by the current match.

	Deletion methods:
		"FREEZE":  Stops adding new words when the dictionary is full.
		"RESTART": Restart learning when the dictionary is full. 
			   Resets the dictionary to the base alphabet.
		"LRU": Delete the least recently used entry that will allow 
		       the prefix property to be preserved.

***NOTE: LRU currently does NOT work

-----------------------------------------------------------------------------------------------
How it works:

This program works by reading in characters from the input file,
comparing the string of read characters to what is currently in the dicionary,
and outputting a number to reference them. It then updates the dictionary based on
the setting picked from the above options. If the dictionary is full, space is made
using the deletion method picked from above.

To decompress, the reference number is read and then the word is found in the dictionary.
To decompression program matches the process of the compression program so 
it updates and deletes from the dictionary in the same order so reference numbers 
remain applicable and the file can be recovered.

-----------------------------------------------------------------------------------------------
Structures used:

Vertex:
- stores a word in a string
- an id tag is used to reference the Vertex
- tracks the number of string based off of it in the numKids field
  to help preserve the prefix property
- parentStr stores the string it was based off of before being added
- has pointers for the previous and next Vertexes used in a list

LinkedList:
- chain of Vertex structures organized by their prev and next fields

Vwrap
- stores a Vertex and previous/next pointers to other Vwraps

Queue
- a linked list of Vwraps
- add to the end of the Queue using Enqueue
- take from the beginning of the Queue using Dequeue
- used to aid in the LRU deletion method

Dictionary:
- Table of vertexes that are placed using open hashing
- Tracks the number of words it contains
- Assists in tracking the order words are accessed


* An array of Vertexes is also used to give direct access to a Vertex by
  its idTag field instead of hashing the word stored in it. This is used
  in both compress and decompress.

























