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
** FILE:   	NCSUtil\NCSBase64.cpp
** CREATED:	16Feb00
** AUTHOR: 	Nicholas Yue
** PURPOSE:	Base64 wrapper functions
** EDITS:
** [01] ny  08Mar00 Coding standard conformance edit.
**                  Change Base64Coder to CNCSBase64Coder
** [02] ny  31Oct00 Merge WinCE/PALM SDK changes
** [03] ny  10Nov00 Mac port changes, authentication broken
 *******************************************************/

#include <stdio.h>
#include "NCSBase64.h"
#include "CNCSBase64Coder.h"
#include "NCSUtil.h"
/*!
 * Decodes a Base 64 encrypted message
 * \param inputMessage the encoded text
 * \param outputMessage the decoded text
 * \see NCSEncodeBase64Message
 * \version [02] ny  08Mar00 Coding standard conformance edit.
 * \version [01] ny  08Mar00 Change Base64Coder to CNCSBase64Coder
*/
void NCSDecodeBase64Message(char *inputMessage,char *outputMessage)
{
	CNCSBase64Coder Coder;

	Coder.Decode((UINT8 *)inputMessage); /**[03]**/

	strcpy(outputMessage,(const char *)Coder.DecodedMessage());
}

/*!
 * Encodes a Base 64 encrypted message
 * \param inputMessage the text to be encoded
 * \param outputMessage the eecoded text
 * \see NCSDecodeBase64Message
 * \version [02] ny  08Mar00 Coding standard conformance edit.
 * \version [01] ny  08Mar00 Change Base64Coder to CNCSBase64Coder
 */
void NCSEncodeBase64Message(char *inputMessage,char *outputMessage)
{
	CNCSBase64Coder Coder;

	Coder.Encode((UINT8 *)inputMessage);	/**[03]**/

	strcpy(outputMessage,(const char *)Coder.EncodedMessage());
}
