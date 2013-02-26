#ifndef _TRACE_H_
#define _TRACE_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void Trace(
	char* strSource
)
{
	char strDest[40];
	strcpy(strDest, strSource);
	
	FILE* fp;
	fp = fopen("c:\trace.txt","w");
	if (fp == NULL)
		exit(1);
    
	fprintf(fp, "%s\n", strDest);
	
	fclose(fp);
}  

#endif
