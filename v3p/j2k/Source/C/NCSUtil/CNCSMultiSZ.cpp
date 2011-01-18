/********************************************************
** Copyright 2000 Earth Resource Mapping Ltd.
** This document contains proprietary source code of
** Earth Resource Mapping Ltd, and can only be used under
** one of the three licenses as described in the 
** license.txt file supplied with this distribution. 
** See separate license.txt file for license details 
** and conditions.
**
** This software is covered by US patent #6,442,298,
** #6,102,897 and #6,633,688.  Rights to use these patents 
** is included in the license agreements.
**
** FILE:   	NCSUtil\CNCSMultiSZ.cpp
** CREATED:	10Apr00
** AUTHOR: 	Nicholas Yue
** PURPOSE:	MultiSZ string manipulation code
**              Although the class is MultiSZ, for convenience,
**              all incoming data is converted to wchar_t and
**              stored as such internally.
** EDITS:
** [01] 20Apr00 ny Fix the logic of strstr, the arguments were swapped
 *******************************************************/

#include <windows.h>
#include <mbstring.h>
#include <memory.h>
#include <atlbase.h>
#include "CNCSMultiSZ.h"

CNCSMultiSZ::CNCSMultiSZ()
{
	wcLineCount = 0;
}

CNCSMultiSZ::~CNCSMultiSZ()
{
}

#ifdef _DEBUG

void CNCSMultiSZ::print()
{
	USES_CONVERSION;
	for (register int i=0;i<wcLineCount;i++)
		printf("printMultisz : %s\n",W2A(wcApps[i]));
}

#endif

void CNCSMultiSZ::assign(unsigned char *chBuffer)
{
	memcpy(input_multiSZBuffer,chBuffer,szlength(chBuffer));
	disassemble(chBuffer);
}

void CNCSMultiSZ::disassemble(unsigned char *chBuffer)
{
	USES_CONVERSION;
	size_t l = 0;
	wchar_t *ptr,strBuffer[1024],*result;
	int length,index=0;
	ptr = (wchar_t *)chBuffer;
	length = (int)wcslen((const wchar_t *)chBuffer);
	while ((*ptr)!=NULL) {
		result = wcscpy(strBuffer,ptr);
		result = wcscpy(wcApps[index],ptr);
		strcpy(szApps[index],W2A(ptr));
		length = (int)wcslen(strBuffer);
		ptr += (length+1);
		l += (length+1);
		index++;
	}
	wcLineCount = index;
}

void CNCSMultiSZ::assemble()
{
	USES_CONVERSION;
	register int i;
	unsigned char *ptr = output_multiSZBuffer;
	wchar_t *result;
	int length;

	for (i=0;i<wcLineCount;i++) {
		result = wcscpy((wchar_t *)ptr,wcApps[i]);
		length = (int)wcslen(wcApps[i]);
		ptr += (length+1)*sizeof(wchar_t);
	}

	// Must add the additional NULL character required by Microsoft's multisz format.
	*ptr = '\0';
	ptr++;
	*ptr = '\0';
}

size_t CNCSMultiSZ::szlength(unsigned char *chBuffer)
{
	USES_CONVERSION;
	size_t l = 0;
	wchar_t *ptr,strBuffer[1024],*result;
	int length;
	ptr = (wchar_t *)chBuffer;
	length = (int)wcslen((const wchar_t *)chBuffer);
	while ((*ptr)!=NULL) {
		result = wcscpy(strBuffer,ptr);
		length = (int)wcslen(strBuffer);
		ptr += (length+1);
		l += (length+1);
	}
	return 2*(l+1);
}

int CNCSMultiSZ::strstr(char *entryName)
{
	USES_CONVERSION;
	register int i;
//	unsigned char *ptr = input_multiSZBuffer;

	for (i=0;i<wcLineCount;i++)
		if (wcsstr(wcApps[i],A2W(entryName))!=(wchar_t *)NULL) /**[01]**/
			return i;
	return -1;
}

void CNCSMultiSZ::get(unsigned char *ptr)
{
	assemble();
	memcpy(ptr,output_multiSZBuffer,szlength(output_multiSZBuffer));
}

/*
** This function assumes that ptr has been allocated with the
** required ammount of storage
*/
BOOLEAN CNCSMultiSZ::get(int index, wchar_t *ptr)	/**[01]**/
{													/**[01]**/
	if (index<wcLineCount) {						/**[01]**/
		if (!wcscpy(ptr,wcApps[index]))				/**[01]**/
			return FALSE;							/**[01]**/
	} else											/**[01]**/
		return FALSE;								/**[01]**/
	return TRUE;									/**[01]**/
}													/**[01]**/

void CNCSMultiSZ::append(char *newEntryName)
{
	USES_CONVERSION;
	wchar_t *result;
	if (wcLineCount<MAX_MSZ_NUM_LINE) {
		result = wcscpy(wcApps[wcLineCount],A2W(newEntryName));
		wcLineCount++;
	}
}

void CNCSMultiSZ::replace(char *newEntryName, int index)
{
	USES_CONVERSION;
	wchar_t *result;
	if (index<MAX_MSZ_NUM_LINE)
		result = wcscpy(wcApps[index],A2W(newEntryName));
}

