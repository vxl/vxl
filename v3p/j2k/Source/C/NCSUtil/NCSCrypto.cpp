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
** FILE:   	NCSUtil\NCSCrypto.cpp
** CREATED:	08Mar00
** AUTHOR: 	Nicholas Yue
** PURPOSE:	Encryption and decryption functions
** EDITS:
 *******************************************************/

#include "NCSUtil.h"
#include "NCSCrypto.h"

/*
** 02Mar00 : The current design strategy for the encryption
**           stuff is to have a standard API entry point
**           and use rot13 while working on more advanced
**           stuff like RSA PKI. We need to be aware of two
**           important considerations
**           (1) Export control
**           (2) I18N (wide character set)
*/

static int NCSCryptoMethod = NCSCryptoNULL;

/*
** Although this algorithm has a common internet name
** known as rotate13, I am purposely avoided using that
** name so that when some scan the symbols, the name is
** not obvious.
** Someone with any cryptographic knowledge can still
** guess the function from the input argument
** type signature.
*/
void rotate(char *inputtext, char *outputtext)
{
	int len = (int)strlen(inputtext);
	for (int i=0;i<len;i++) {
		char a = ~inputtext[i];
		outputtext[i] = (char)(~a-1/(~(a|32)/13*2-11)*13);
	}
	outputtext[len] = '\0';
}

/*
** Initialize the cryptographics stuff :
** Currently nothing needs to be done, when RSA PKI
** type of encryption is used, MS-Windows's CryptoAPI
** functions needs to be called to initialize the
** certificate management stuff.
*/
void NCSSetCryptoMethod(int cryptomethod)
{
	NCSCryptoMethod = cryptomethod;
}

/*
** Find out the current Cryptographic methods
*/
int NCSGetCryptoMethod(void)
{
	return NCSCryptoMethod;
}

int NCSEncryptData(UINT8 * plaintext, UINT8 * ciphertext)	/**[01]**/
{
	switch(NCSCryptoMethod) {
	case NCSCryptoRot13 :
		rotate((char *)plaintext,(char *)ciphertext);
		break;
	default:
		return -1;
	}
	return 0;
}

int NCSDecryptData(UINT8 * ciphertext, UINT8 * plaintext)	/**[01]**/
{
	switch(NCSCryptoMethod) {
	case NCSCryptoRot13 :
	rotate((char *)ciphertext,(char *)plaintext);
		break;
	default:
		return -1;
	}
	return 0;
}
