/********************************************************** 
** Copyright 2004 Earth Resource Mapping Ltd.
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
** FILE:   	ecw_jni_config.c
** CREATED:	10 May 2004
** AUTHOR: 	Mark Sheridan
** PURPOSE:	Java wrappers for JNI usage of the ECW config functions NCSecwSetConfig() && NCSecwGetConfig()
**			For some bizarre reason those calls were implemented as var args, since Java doesnt have var_args
**			it makes this code very verbose and prone to errors.
** EDITS:
**
**
*******************************************************/

#include "NCSErrors.h"
#include "NCSECW.h"
#include "JNCSEcwConfig.h"

#ifndef JNI_VERSION_1_2
#error You must compile this class against a 1.2 virtual machine specification
#endif

static void ThrowRuntimeEx(JNIEnv *pEnv, char *pMessage)
{
	// throw an exception of the type "java.lang.RuntimeException"
	jclass runTimeExCls = (*pEnv)->FindClass(pEnv, "java/lang/RuntimeException");

	if (runTimeExCls != NULL)
	{
		(*pEnv)->ThrowNew(pEnv, runTimeExCls, (const char *)pMessage); 
	}

	(*pEnv)->DeleteLocalRef(pEnv, runTimeExCls);
}

/*
 * Class:     com_ermapper_ecw_JNCSEcwConfig
 * Method:    SetTextureDither
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL Java_com_ermapper_ecw_JNCSEcwConfig_setTextureDither
  (JNIEnv *pEnv, jclass clazz, jboolean bValue)
{
	if (NCSecwSetConfig(NCSCFG_TEXTURE_DITHER, bValue) != NCS_SUCCESS)
	{
		ThrowRuntimeEx(pEnv, "Error setting texture dither property.");
	}
}

/*
 * Class:     com_ermapper_ecw_JNCSEcwConfig
 * Method:    GetTextureDither
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_ermapper_ecw_JNCSEcwConfig_getTextureDither
  (JNIEnv *pEnv, jclass clazz)
{
	jboolean bValue = FALSE;
	if (NCSecwGetConfig(NCSCFG_TEXTURE_DITHER, &bValue) != NCS_SUCCESS)
	{
		ThrowRuntimeEx(pEnv, "Error getting texture dither property.");
	}
	return bValue;
}

/*
 * Class:     com_ermapper_ecw_JNCSEcwConfig
 * Method:    SetForceFileReopen
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL Java_com_ermapper_ecw_JNCSEcwConfig_setForceFileReopen
  (JNIEnv *pEnv, jclass clazz, jboolean  bValue)
{
	if (NCSecwSetConfig(NCSCFG_FORCE_FILE_REOPEN, bValue) != NCS_SUCCESS)
	{
		ThrowRuntimeEx(pEnv, "Error setting force file reopen property.");
	}
}

/*
 * Class:     com_ermapper_ecw_JNCSEcwConfig
 * Method:    GetForceFileReopen
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_ermapper_ecw_JNCSEcwConfig_getForceFileReopen
  (JNIEnv *pEnv, jclass clazz)
{
	jboolean bValue = FALSE;
	if (NCSecwGetConfig(NCSCFG_FORCE_FILE_REOPEN, &bValue) != NCS_SUCCESS)
	{
		ThrowRuntimeEx(pEnv, "Error getting force file reopen property.");
	}
	return bValue;
}


/*
 * Class:     com_ermapper_ecw_JNCSEcwConfig
 * Method:    SetCacheMaxMem
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_com_ermapper_ecw_JNCSEcwConfig_setCacheMaxMem
  (JNIEnv *pEnv, jclass clazz, jlong value)
{
	if (NCSecwSetConfig(NCSCFG_CACHE_MAXMEM, value) != NCS_SUCCESS)
	{
		ThrowRuntimeEx(pEnv, "Error setting cache max mem property.");
	}
}


/*
 * Class:     com_ermapper_ecw_JNCSEcwConfig
 * Method:    GetCacheMaxMen
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_com_ermapper_ecw_JNCSEcwConfig_getCacheMaxMen
  (JNIEnv *pEnv, jclass clazz)
{
	jlong value = 0;
	if (NCSecwGetConfig(NCSCFG_CACHE_MAXMEM, &value) != NCS_SUCCESS)
	{
		ThrowRuntimeEx(pEnv, "Error getting cache max mem property.");
	}
	return value;
}


/*
 * Class:     com_ermapper_ecw_JNCSEcwConfig
 * Method:    SetCacheMaxOpenFiles
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_ermapper_ecw_JNCSEcwConfig_setCacheMaxOpenFiles
  (JNIEnv *pEnv, jclass clazz, jint value)
{
	if (NCSecwSetConfig(NCSCFG_CACHE_MAXOPEN, value) != NCS_SUCCESS)
	{
		ThrowRuntimeEx(pEnv, "Error setting cache max open files property.");
	}
}


/*
 * Class:     com_ermapper_ecw_JNCSEcwConfig
 * Method:    GetCacheMaxOpenFiles
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_ermapper_ecw_JNCSEcwConfig_getCacheMaxOpenFiles
  (JNIEnv *pEnv, jclass clazz)
{
	jint value = 0;
	if (NCSecwGetConfig(NCSCFG_CACHE_MAXOPEN, &value) != NCS_SUCCESS)
	{
		ThrowRuntimeEx(pEnv, "Error getting cache max open files property.");
	}
	return value;
}


/*
 * Class:     com_ermapper_ecw_JNCSEcwConfig
 * Method:    SetBlockingTimeMS
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_ermapper_ecw_JNCSEcwConfig_setBlockingTimeMS
  (JNIEnv *pEnv, jclass clazz, jint value)
{
	if (NCSecwSetConfig(NCSCFG_BLOCKING_TIME_MS, value) != NCS_SUCCESS)
	{
		ThrowRuntimeEx(pEnv, "Error setting cache max open files property.");
	}
}


/*
 * Class:     com_ermapper_ecw_JNCSEcwConfig
 * Method:    GetBlockingTimeMS
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_ermapper_ecw_JNCSEcwConfig_getBlockingTimeMS
  (JNIEnv *pEnv, jclass clazz)
{
	jint value = 0;
	if (NCSecwGetConfig(NCSCFG_BLOCKING_TIME_MS, &value) != NCS_SUCCESS)
	{
		ThrowRuntimeEx(pEnv, "Error getting cache max open files property.");
	}
	return value;
}

/*
 * Class:     com_ermapper_ecw_JNCSEcwConfig
 * Method:    SetRefreshTimeMS
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_ermapper_ecw_JNCSEcwConfig_setRefreshTimeMS
  (JNIEnv *pEnv, jclass clazz, jint value)
{
	if (NCSecwSetConfig(NCSCFG_REFRESH_TIME_MS, value) != NCS_SUCCESS)
	{
		ThrowRuntimeEx(pEnv, "Error setting refresh time property.");
	}
}


/*
 * Class:     com_ermapper_ecw_JNCSEcwConfig
 * Method:    GetRefreshTimeMS
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_ermapper_ecw_JNCSEcwConfig_getRefreshTimeMS
  (JNIEnv *pEnv, jclass clazz)
{
	jint value = 0;
	if (NCSecwGetConfig(NCSCFG_REFRESH_TIME_MS, &value) != NCS_SUCCESS)
	{
		ThrowRuntimeEx(pEnv, "Error getting refresh time property.");
	}
	return value;
}


/*
 * Class:     com_ermapper_ecw_JNCSEcwConfig
 * Method:    SetPurgeDelayMS
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_ermapper_ecw_JNCSEcwConfig_setPurgeDelayMS
  (JNIEnv *pEnv, jclass clazz, jint value)
{
	if (NCSecwSetConfig(NCSCFG_PURGE_DELAY_MS, value) != NCS_SUCCESS)
	{
		ThrowRuntimeEx(pEnv, "Error setting purge delay property.");
	}
}


/*
 * Class:     com_ermapper_ecw_JNCSEcwConfig
 * Method:    GetPurgeDelayMS
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_ermapper_ecw_JNCSEcwConfig_getPurgeDelayMS
  (JNIEnv *pEnv, jclass clazz)
{
	jint value = 0;
	if (NCSecwGetConfig(NCSCFG_REFRESH_TIME_MS, &value) != NCS_SUCCESS)
	{
		ThrowRuntimeEx(pEnv, "Error getting purge delay property.");
	}
	return value;
}


/*
 * Class:     com_ermapper_ecw_JNCSEcwConfig
 * Method:    SetFilePurgeDelayMS
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_ermapper_ecw_JNCSEcwConfig_setFilePurgeDelayMS
  (JNIEnv *pEnv, jclass clazz, jint value)
{
	if (NCSecwSetConfig(NCSCFG_FILE_PURGE_DELAY_MS, value) != NCS_SUCCESS)
	{
		ThrowRuntimeEx(pEnv, "Error setting file purge delay property.");
	}
}


/*
 * Class:     com_ermapper_ecw_JNCSEcwConfig
 * Method:    GetFilePurgeDelayMS
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_ermapper_ecw_JNCSEcwConfig_getFilePurgeDelayMS
  (JNIEnv *pEnv, jclass clazz)
{
	jint value = 0;
	if (NCSecwGetConfig(NCSCFG_FILE_PURGE_DELAY_MS, &value) != NCS_SUCCESS)
	{
		ThrowRuntimeEx(pEnv, "Error getting file purge delay property.");
	}
	return value;
}


/*
 * Class:     com_ermapper_ecw_JNCSEcwConfig
 * Method:    SetMinPurgeDelayMS
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_ermapper_ecw_JNCSEcwConfig_setMinPurgeDelayMS
  (JNIEnv *pEnv, jclass clazz, jint value)
{
	if (NCSecwSetConfig(NCSCFG_MIN_FILE_PURGE_DELAY_MS, value) != NCS_SUCCESS)
	{
		ThrowRuntimeEx(pEnv, "Error setting min file purge delay property.");
	}
}


/*
 * Class:     com_ermapper_ecw_JNCSEcwConfig
 * Method:    GetMinPurgeDelayMS
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_ermapper_ecw_JNCSEcwConfig_getMinPurgeDelayMS
  (JNIEnv *pEnv, jclass clazz)
{
	jint value = 0;
	if (NCSecwGetConfig(NCSCFG_MIN_FILE_PURGE_DELAY_MS, &value) != NCS_SUCCESS)
	{
		ThrowRuntimeEx(pEnv, "Error getting min file purge delay property.");
	}
	return value;
}


/*
 * Class:     com_ermapper_ecw_JNCSEcwConfig
 * Method:    SetProxy
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_ermapper_ecw_JNCSEcwConfig_setECWPProxy
  (JNIEnv *pEnv, jclass clazz, jstring value)
{
	const char *proxy = (*pEnv)->GetStringUTFChars(pEnv, value, (jboolean *)NULL);

	if (proxy != NULL)
	{
		if (NCSecwSetConfig(NCSCFG_ECWP_PROXY, proxy) != NCS_SUCCESS)
		{
			ThrowRuntimeEx(pEnv, "Error setting proxy property.");
		}

		(*pEnv)->ReleaseStringUTFChars(pEnv, value, proxy);
	}
}


/*
 * Class:     com_ermapper_ecw_JNCSEcwConfig
 * Method:    GetProxy
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_ermapper_ecw_JNCSEcwConfig_getECWPProxy
  (JNIEnv *pEnv, jclass clazz)
{
	char *pvalue = NULL;

	if (NCSecwGetConfig(NCSCFG_ECWP_PROXY, &pvalue) != NCS_SUCCESS)
	{
		ThrowRuntimeEx(pEnv, "Error getting proxy property.");
	}
	else
	{
		return (*pEnv)->NewStringUTF(pEnv, pvalue);
	}
	return (*pEnv)->NewStringUTF(pEnv, "");
}


/*
 * Class:     com_ermapper_ecw_JNCSEcwConfig
 * Method:    SetForceLowMemCompress
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL Java_com_ermapper_ecw_JNCSEcwConfig_setForceLowMemCompress
  (JNIEnv *pEnv, jclass clazz, jboolean bValue)
{
	if (NCSecwSetConfig(NCSCFG_FORCE_LOWMEM_COMPRESS, bValue) != NCS_SUCCESS)
	{
		ThrowRuntimeEx(pEnv, "Error setting low mem compress property.");
	}
}


/*
 * Class:     com_ermapper_ecw_JNCSEcwConfig
 * Method:    GetForceLowMemCompress
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_ermapper_ecw_JNCSEcwConfig_getForceLowMemCompress
  (JNIEnv *pEnv, jclass clazz)
{
	jboolean bValue = FALSE;
	if (NCSecwGetConfig(NCSCFG_FORCE_LOWMEM_COMPRESS, &bValue) != NCS_SUCCESS)
	{
		ThrowRuntimeEx(pEnv, "Error getting low mem compress property.");
	}
	return bValue;
}


/*
 * Class:     com_ermapper_ecw_JNCSEcwConfig
 * Method:    SetECWPReconnect
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL Java_com_ermapper_ecw_JNCSEcwConfig_setECWPReconnect
  (JNIEnv *pEnv, jclass clazz, jboolean bValue)
{
	if (NCSecwSetConfig(NCSCFG_TRY_ECWP_RECONNECT, bValue) != NCS_SUCCESS)
	{
		ThrowRuntimeEx(pEnv, "Error setting ECWP reconnect property.");
	}
}


/*
 * Class:     com_ermapper_ecw_JNCSEcwConfig
 * Method:    GetECWPReconnect
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_ermapper_ecw_JNCSEcwConfig_getECWPReconnect
  (JNIEnv *pEnv, jclass clazz)
{
	jboolean bValue = FALSE;
	if (NCSecwGetConfig(NCSCFG_TRY_ECWP_RECONNECT, &bValue) != NCS_SUCCESS)
	{
		ThrowRuntimeEx(pEnv, "Error getting ECWP reconnect property.");
	}
	return bValue;
}
