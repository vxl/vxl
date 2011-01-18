/********************************************************
** Copyright 1999 Earth Resource Mapping Ltd.
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
** FILE:   	NCSMalloc.h
** CREATED:	Thu Feb 25 09:19:00 WST 1999
** AUTHOR: 	Simon Cope
** PURPOSE:	NCS Memory handling routines.
** EDITS:
 *******************************************************/

#ifndef NCSARRAY_H
#define NCSARRAY_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#ifndef NCSMALLOC_H
#include "NCSMalloc.h"
#endif

/*
** Set to cause arrays to grow/shrink by a granular factor instead of for each element.
*/
#define NCS_ARRAY_GRANULAR

#ifdef NCS_ARRAY_GRANULAR

/*
** Granular array factor (nelements to grow/shrink)
*/
#define NCS_ARRAY_GRANULARITY 16
		/*
		((void *)pArray) = (void *)NCSRealloc((void *)(pArray),	\
			((nElements) + NCS_ARRAY_GRANULARITY) *		\
			sizeof(*(pArray)),				\
			FALSE);						\
		*/

#define NCSArrayInsertElement(pArray, nElements, nIndex, pElement)	\
	if(nElements % NCS_ARRAY_GRANULARITY == 0) {			\
		void *pData = (void *)NCSRealloc((void *)(pArray),	\
			((nElements) + NCS_ARRAY_GRANULARITY) *		\
			sizeof(*(pArray)),				\
			FALSE);						\
		pArray = pData;						\
	}								\
	if((nIndex) < (nElements)) {					\
		memmove(&((pArray)[(nIndex) + 1]),			\
			&((pArray)[(nIndex)]),				\
			((nElements) - (nIndex)) * sizeof(*(pArray)));	\
	}								\
	if(pElement) {							\
		(pArray)[(nIndex)] = *(pElement);			\
	} else {							\
		memset(&((pArray)[(nIndex)]), 0, sizeof(*pArray));	\
	}								\
	(nElements) += 1

#define NCSArrayAppendElement(pArray, nElements, pElement)		\
	NCSArrayInsertElement(pArray, nElements, nElements, pElement)

#define NCSArrayRemoveElement(pArray, nElements, nIndex)		\
	if((nIndex) < (nElements) - 1) {				\
		memmove(&((pArray)[(nIndex)]),				\
			&((pArray)[(nIndex) + 1]),			\
			((nElements) - (nIndex) - 1) * sizeof(*(pArray)));	\
	}								\
	(nElements) -= 1;						\
	if(nElements % NCS_ARRAY_GRANULARITY == 0) {			\
		if((nElements) > 0) {					\
			void *pData = (void *)NCSRealloc((pArray),	\
				(nElements) * sizeof(*(pArray)),	\
				FALSE);					\
			pArray = pData;					\
		} else {						\
			NCSFree((pArray));				\
			pArray = (void*)NULL;				\
		}							\
	}

#else /* NCS_ARRAY_GRANULAR */

#define NCSArrayInsertElement(pArray, nElements, nIndex, pElement)	\
				(pArray) = NCSRealloc((pArray),										\
									  (nElements + 1) * sizeof(*(pArray)),			\
									  TRUE);										\
				if((nIndex) < (nElements)) {										\
					memmove(&((pArray)[(nIndex) + 1]),								\
							&((pArray)[(nIndex)]),									\
							((nElements) - (nIndex)) * sizeof(*(pArray)));			\
				}																	\
				if(pElement) {														\
					(pArray)[(nIndex)] = *(pElement);								\
				}																	\
				(nElements) += 1

#define NCSArrayAppendElement(pArray, nElements, pElement)							\
				NCSArrayInsertElement(pArray, nElements, nElements, pElement)

#define NCSArrayRemoveElement(pArray, nElements, nIndex)							\
				if((nIndex) < (nElements) - 1) {									\
					memmove(&((pArray)[(nIndex)]),									\
						    &((pArray)[(nIndex) + 1]),								\
							((nElements) - (nIndex) - 1) * sizeof(*(pArray)));		\
				}																	\
				(nElements) -= 1;													\
				if((nElements) > 0) {												\
					(pArray) = NCSRealloc((pArray),									\
										  (nElements) * sizeof(*(pArray)),			\
										  FALSE);									\
				} else {															\
					NCSFree((pArray));												\
					(pArray) = (void*)NULL;											\
				}

#endif /* NCS_ARRAY_GRANULAR */

#ifdef __cplusplus
}
#endif

#endif /* NCSARRAY_H */
