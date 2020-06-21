#include "mystring.h"
#include <stdio.h>
#include <stdlib.h>

/*
 *   Implement the following functions: 
 * 
 *   You are NOT allowed to use any standard string functions such as 
 *   strlen, strcpy or strcmp or any other string function.
 */

/*
 *  mystrlen() calculates the length of the string s, 
 *  not including the terminating character '\0'.
 *  Returns: length of s.
 */
int mystrlen (const char *s) 
{
	const char *temp=s;
	while(*temp!='\0'){
		temp++;
	}
	return temp-s;
}	

/*
 *  mystrcpy()  copies the string pointed to by src (including the terminating 
 *  character '\0') to the array pointed to by dst.
 *  Returns: a  pointer to the destination string dst.
 */
char  *mystrcpy (char *dst, const char *src)
{
	if(src==NULL||dst==NULL)
		return NULL;
	char *temp=dst;
	while(*src!='\0'){
		*dst=*src;		
		dst++;
		src++;
	}	
	*dst='\0';
	return temp;
}

/*
 * mystrcmp() compares two strings alphabetically
 * Returns: 
 * 	-1 if s1  < s2
 *  	0 if s1 == s2
 *  	1 if s1 > s2
 */
int mystrcmp(const char *s1, const char *s2)
{
	while(*s1==*s2){
		if(*s1=='\0')
			return 0;
		s1++;
		s2++;
	}
	if(*s1<*s2)
		return -1;
	else
		return 1;
}

/*
 * mystrdup() creates a duplicate of the string pointed to by s. 
 * The space for the new string is obtained using malloc.  
 * If the new string can not be created, a null pointer is returned.
 * Returns:  a pointer to a new string (the duplicate) 
 	     or null If the new string could not be created for 
	     any reason such as insufficient memory.
 */
char *mystrdup(const char *s1)
{
	char *s2=malloc(mystrlen(s1)+1);
	if(s2)
		mystrcpy(s2,s1);
	else
		return NULL;
	return s2;
}

