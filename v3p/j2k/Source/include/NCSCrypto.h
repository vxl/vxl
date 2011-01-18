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
** FILE:   	include\NCSCrypto.h
** CREATED:	08Mar00
** AUTHOR: 	Nicholas Yue
** PURPOSE:	Encryption utilities heeader
** EDITS:
** [01] 14Nov00	 ny Mac port changes 
 *******************************************************/
#include "NCSTypes.h"	/**[01]**/


#ifdef __cplusplus
extern "C" {
#endif

#ifndef NCSCRYPTO_H
#define NCSCRYPTO_H

#define NCSCryptoNULL	0x0000
#define NCSCryptoRot13	0x0001
#define NCSCryptoRSA	0x0002

extern void NCSSetCryptoMethod(int cryptomethod);
extern int NCSGetCryptoMethod(void);
extern int NCSEncryptData(UINT8 * plaintext, UINT8 * ciphertext);
extern int NCSDecryptData(UINT8 * ciphertext, UINT8 * plaintext);

#endif // NCSCRYPTO_H

#ifdef __cplusplus
}
#endif
