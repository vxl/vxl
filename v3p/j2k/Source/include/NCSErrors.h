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
** FILE:   	NCSError.h
** CREATED:	Thu Feb 25 09:19:00 WST 1999
** AUTHOR: 	Simon Cope
** PURPOSE:	NCS Error handling enum/routines.
** EDITS:
   [01] ny	26Apr00 Added 	NCS_NET_PACKET_RECV_ZERO_LENGTH error 
   [02] sjc 30May00 Merge Mac SDK port in to main source stream
   [03] ny  12May00 Added lost of connection error
   [04] ny  07Jul00 Added Metabase query errors
   [05] ny  19Jul00 Added Timeout errors
   [06] mjs 25Aug00 Added errors for Java JNI functionality
   [07] jmp 31Aug00 Moved extra symbol server errors to end of enum
   [08] sjc 12Oct00 Added NCS_MAX_ERROR_NUMBER to enable range check
   [10]  ny	30Oct00	Merge WinCE/Palm SDK
   [11] rar	19Jan01 Mac port changes
   [12] sjc 06Mar01 Strings were swapped
   [13] jmp 17May01 Added SetExtents error
   [14] jmp 25Sep01 Layer errors
   [15] ajd 22Nov01 added 1.65 gdt errors
   [16] ajd 15Jan02 added NCS_GDT_TRANSFORM_OUT_OF_BOUNDS
   [17]	jmp 17Jan02 Layer errors
   [18] jmp 24Apr02 Layer Parameter Errors
   [19] mjs 15Nov02 Added PIPE errors for CGI server
   [20] rar 20Nov02 Added errors for NCSMakeDir
   [21] tfl 17Nov04 Added errors for JPEG 2000 geodata I/O
   [22] tfl 25Nov05 Moved #include <crtdbg.h> outside extern "C" {} to fix new/delete override issue
   [23] tfl 19Jan06 Added temporary error condition for trying to do tiled views on progressive reads
 *******************************************************/


#ifndef NCSERRORS_H
#define NCSERRORS_H

/* Define some debugging macros */

#if (defined(WIN32) && !defined(_WIN32_WCE)) /**[10]**/
#include <crtdbg.h>
#else
#define _CRT_WARN 1
#if !defined(_DEBUG) && !defined(DEBUG)
#ifndef _ASSERT
#define _ASSERT(a)
#endif
#define _RPT0(a, b)
#define _RPT1(a, b, a1)
#define _RPT2(a, b, a1, a2)
#define _RPT3(a, b, a1, a2, a3)	
#define _RPT4(a, b, a1, a2, a3, a4)
#else	/* _DEBUG */
#ifndef _ASSERT
#define _ASSERT(a)
#endif
#define _RPT0(a, b)			fprintf(stderr, b)
#define _RPT1(a, b, a1)			fprintf(stderr, b, a1)
#define _RPT2(a, b, a1, a2)		fprintf(stderr, b, a1, a2)
#define _RPT3(a, b, a1, a2, a3)		fprintf(stderr, b, a1, a2, a3)
#define _RPT4(a, b, a1, a2, a3, a4)	fprintf(stderr, b, a1, a2, a3, a4)
#endif	/* _DEBUG */
#endif	/* WIN32 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NCSDEFS_H
#include "NCSDefs.h"
#endif

#ifdef PALM
#include <PalmTypes.h>
#include <FileStream.h>
#endif

/*
** Error Enum.
*/

/*
** IMPORTANT: Add new errors to the end of this list so we can retain some 
**			  backwards binary compatibilty. Also, don't forget to add the 
**			  error text!
*/

/** 
 *	@enum
 *	An enumerated type specifying all the errors associated with the SDK.  Each error
 *	code is associated with a string of explanatory text.
 */
typedef enum {
	/* NCS Raster Errors */
	NCS_SUCCESS					= 0,		/**< No error */
	NCS_QUEUE_NODE_CREATE_FAILED,			/**< Queue node creation failed */
	NCS_FILE_OPEN_FAILED,					/**< File open failed */
	NCS_FILE_LIMIT_REACHED,					/**< The Image Web Server's licensed file limit has been reached */
	NCS_FILE_SIZE_LIMIT_REACHED,			/**< The requested file is larger than is permitted by the license on this Image Web Server */
	NCS_FILE_NO_MEMORY,						/**< Not enough memory for new file */
	NCS_CLIENT_LIMIT_REACHED,				/**< The Image Web Server's licensed client limit has been reached */
	NCS_DUPLICATE_OPEN,						/**< Detected duplicate open from net layer */
	NCS_PACKET_REQUEST_NYI,					/**< Packet request type not yet implemented */
	NCS_PACKET_TYPE_ILLEGAL,				/**< Packet type is illegal */
	NCS_DESTROY_CLIENT_DANGLING_REQUESTS,	/**< Client closed while requests outstanding */

	/* NCS Network Errors */
	NCS_UNKNOWN_CLIENT_UID,					/**< Client UID unknown */
	NCS_COULDNT_CREATE_CLIENT,				/**< Could not create new client */
	NCS_NET_COULDNT_RESOLVE_HOST,			/**< Could not resolve address of Image Web Server */
	NCS_NET_COULDNT_CONNECT,				/**< Could not connect to host */
	NCS_NET_RECV_TIMEOUT,					/**< Receive timeout */
	NCS_NET_HEADER_SEND_FAILURE,			/**< Error sending header */
	NCS_NET_HEADER_RECV_FAILURE,			/**< Error receiving header */
	NCS_NET_PACKET_SEND_FAILURE,			/**< Error sending packet */
	NCS_NET_PACKET_RECV_FAILURE,			/**< Error receiving packet */
	NCS_NET_401_UNAUTHORISED,				/**< 401 Unauthorised: SDK doesn't do authentication so this suggests a misconfigured server */			
	NCS_NET_403_FORBIDDEN,					/**< 403 Forbidden: could be a 403.9 from IIS or PWS meaning that the maximum simultaneous request limit has been reached */
	NCS_NET_404_NOT_FOUND,					/**< 404 Not Found: this error suggests that the server hasn't got Image Web Server installed */
	NCS_NET_407_PROXYAUTH,					/**< 407 Proxy Authentication: the SDK doesn't do proxy authentication yet either, so this also suggests misconfiguration */
	NCS_NET_UNEXPECTED_RESPONSE,			/**< Unexpected HTTP response could not be handled */
	NCS_NET_BAD_RESPONSE,					/**< HTTP response received outside specification */
	NCS_NET_ALREADY_CONNECTED,				/**< Already connected */
	NCS_INVALID_CONNECTION,					/**< Connection is invalid */
	NCS_WINSOCK_FAILURE,					/**< A Windows sockets failure occurred */

	/* NCS Symbol Errors */
	NCS_SYMBOL_ERROR,			/**< Symbology error */
	NCS_OPEN_DB_ERROR,			/**< Could not open database */
	NCS_DB_QUERY_FAILED,		/**< Could not execute the requested query on database */
	NCS_DB_SQL_ERROR,			/**< SQL statement could not be executed */
	NCS_GET_LAYER_FAILED,		/**< Open symbol layer failed */
	NCS_DB_NOT_OPEN,			/**< The database is not open */
	NCS_QT_TYPE_UNSUPPORTED,	/**< This type of quadtree is not supported */

	/* Preference errors */
	NCS_PREF_INVALID_USER_KEY,		/**< Invalid local user key name specified */
	NCS_PREF_INVALID_MACHINE_KEY,	/**< Invalid local machine key name specified */
	NCS_REGKEY_OPENEX_FAILED,		/**< Failed to open registry key */
	NCS_REGQUERY_VALUE_FAILED,		/**< Registry query failed */
	NCS_INVALID_REG_TYPE,			/**< Type mismatch in registry variable */

	/* Misc Errors */
	NCS_INVALID_ARGUMENTS,		/**< Invalid arguments passed to function */
	NCS_ECW_ERROR,				/**< ECW error */
	/* unspecified, but coming out of ecw */
	NCS_SERVER_ERROR,			/**< Server error */
	/* unspecified server error */
	NCS_UNKNOWN_ERROR,			/**< Unknown error */
	NCS_EXTENT_ERROR,			/**< Extent conversion failed */
	NCS_COULDNT_ALLOC_MEMORY,	/**< Could not allocate enough memory */
	NCS_INVALID_PARAMETER,		/**< An invalid parameter was used */

	/* Compression Errors */
	NCS_FILEIO_ERROR,						/**< Error reading or writing file */
	NCS_COULDNT_OPEN_COMPRESSION,			/**< Compression task could not be initialised */
	NCS_COULDNT_PERFORM_COMPRESSION,		/**< Compression task could not be processed */
	NCS_GENERATED_TOO_MANY_OUTPUT_LINES,	/**< Trying to generate too many output lines */
	NCS_USER_CANCELLED_COMPRESSION,			/**< Compression task was cancelled by client application */
	NCS_COULDNT_READ_INPUT_LINE,			/**< Could not read line from input data */
	NCS_INPUT_SIZE_EXCEEDED,				/**< Input image size was exceeded for this version of the SDK */

	/* Decompression Errors */
	NCS_REGION_OUTSIDE_FILE,	/**< Specified image region is outside image extents */
	NCS_NO_SUPERSAMPLE,			/**< Supersampling is not supported by the SDK functions */
	NCS_ZERO_SIZE,				/**< Specified image region has a zero width or height */
	NCS_TOO_MANY_BANDS,			/**< More bands specified than exist in the input file */
	NCS_INVALID_BAND_NR,		/**< An invalid band number has been specified */

	/* NEW Compression Error */
	NCS_INPUT_SIZE_TOO_SMALL,	/**< Input image size is too small to compress - for ECW compression there is a minimum output file size */

	/* NEW Network error */
	NCS_INCOMPATIBLE_PROTOCOL_VERSION,	/**< The ECWP client version is incompatible with this server */
	NCS_WININET_FAILURE,				/**< Windows Internet Client error */
	NCS_COULDNT_LOAD_WININET,			/**< wininet.dll could not be loaded - usually indicates Internet Explorer should be upgraded */

	/* NCSFile && NCSRenderer class errors */
	NCS_FILE_INVALID_SETVIEW,			/**< The parameters specified for setting a file view were invalid, or the view was not set */
	NCS_FILE_NOT_OPEN,					/**< No file is open */
	
	/* NEW JNI Java Errors */
	NCS_JNI_REFRESH_NOT_IMPLEMENTED,	/**< Class does not implement ECWProgressiveDisplay interface */
	/* A class is trying to use RefreshUpdate() method, but has not implemented ECWProgressiveDisplay*/

	/* NEW Coordinate Errors*/
	NCS_INCOMPATIBLE_COORDINATE_SYSTEMS,	/**< Incompatible coordinate systems */
	NCS_INCOMPATIBLE_COORDINATE_DATUM,		/**< Incompatible coordinate datum types */
	NCS_INCOMPATIBLE_COORDINATE_PROJECTION,	/**< Incompatible coordinate projection types */
	NCS_INCOMPATIBLE_COORDINATE_UNITS,		/**< Incompatible coordinate units types */
	NCS_COORDINATE_CANNOT_BE_TRANSFORMED,	/**< Non-linear coordinate systems not supported */
	NCS_GDT_ERROR,							/**< Error involving the GDT database */
	
	/* NEW NCScnet error */
	NCS_NET_PACKET_RECV_ZERO_LENGTH,	/**< Zero length packet received */
	/**[01]**/
	
	/* Macintosh SDK specific errors */
	NCS_UNSUPPORTEDLANGUAGE,			/**< Must use Japanese version of the ECW SDK */
	/**[02]**/

	/* Loss of connection */
	NCS_CONNECTION_LOST,				/**< Connection to server was lost */
	/**[03]**/

	NCS_COORD_CONVERT_ERROR,			/**< NCSGDT coordinate conversion failed */

	/* Metabase Stuff */
	NCS_METABASE_OPEN_FAILED,			/**< Failed to open metabase */
	/**[04]**/
	NCS_METABASE_GET_FAILED,			/**< Failed to get value from metabase */
	/**[04]**/
	NCS_NET_HEADER_SEND_TIMEOUT,		/**< Timeout sending header */
	/**[05]**/
	
	NCS_JNI_ERROR,						/**< Java JNI error */
	/**[06]**/

	NCS_DB_INVALID_NAME,				/**< No data source passed */
	/**[07]**/
	NCS_SYMBOL_COULDNT_RESOLVE_HOST,	/**< Could not resolve address of Image Web Server Symbol Server Extension */
	/**[07]**/
	
	NCS_INVALID_ERROR_ENUM,				/**< The value of an NCSError error number was invalid! */
	/**[08]**/
	
	/* NCSFileIO errors [10] */
	NCS_FILE_EOF,						/**< End of file reached */
	NCS_FILE_NOT_FOUND,					/**< File not found */
	NCS_FILE_INVALID,					/**< File was invalid or corrupt */
	NCS_FILE_SEEK_ERROR,				/**< Attempted to read, write or seek past file limits */
	NCS_FILE_NO_PERMISSIONS,			/**< Permissions not available to access file */
	NCS_FILE_OPEN_ERROR,				/**< Error opengin file */
	NCS_FILE_CLOSE_ERROR,				/**< Error closing file */
	NCS_FILE_IO_ERROR,					/**< Miscellaneous error involving file input or output */

	NCS_SET_EXTENTS_ERROR,				/**< Illegal or invalid world coordinates supplied */
	/**[09]**/ 

	NCS_FILE_PROJECTION_MISMATCH,		/**< Image projection does not match that of the controlling layer */
	/** 1.65 gdt errors [15]**/
	NCS_GDT_UNKNOWN_PROJECTION,		/**< Unknown map projection */
	NCS_GDT_UNKNOWN_DATUM,			/**< Unknown geodetic datum */
	NCS_GDT_USER_SERVER_FAILED,		/**< User specified Geographic Projection Database data server failed */
	NCS_GDT_REMOTE_PATH_DISABLED,	/**< Remote Geographic Projection Database file downloading has been disabled and no local GDT data is available */
	NCS_GDT_BAD_TRANSFORM_MODE,		/**< Invalid mode of transform */
	NCS_GDT_TRANSFORM_OUT_OF_BOUNDS,/**< Coordinate to be transformed is out of bounds */
	NCS_LAYER_DUPLICATE_LAYER_NAME,	/**< A layer already exists with the specified name */
	/**[17]**/
	NCS_LAYER_INVALID_PARAMETER,	/**< The specified layer does not contain the specified parameter */
	/**[18]**/
	NCS_PIPE_CREATE_FAILED,			/**< Failed to create pipe */
	/**[19]**/

	/* Directory creation errors */
	NCS_FILE_MKDIR_EXISTS,			/**< Directory to be created already exists */ /*[20]*/
	NCS_FILE_MKDIR_PATH_NOT_FOUND,	/**< The path specified for directory creation does not exist */ /*[20]*/
	NCS_ECW_READ_CANCELLED,			/**< File read was cancelled */

	/* JP2 georeferencing errors */
	NCS_JP2_GEODATA_READ_ERROR,		/**< Error reading geodata from a JPEG 2000 file */ /*[21]*/
	NCS_JP2_GEODATA_WRITE_ERROR,    /**< Error writing geodata to a JPEG 2000 file */	/*[21]*/
	NCS_JP2_GEODATA_NOT_GEOREFERENCED, /**< JPEG 2000 file not georeferenced */			/*[21]*/

	/* Progressive view too large error */
	NCS_PROGRESSIVE_VIEW_TOO_LARGE, /**< Progressive views are limited to 4000x4000 in size */ /*[23]*/
	
	// Insert new errors before here!
	
	NCS_MAX_ERROR_NUMBER			/**< The maximum error value in this enumerated type - should not itself be reported, must always be defined last */ /*[08]*/
} NCSError;


#ifdef NCSERROR_DEFINE_GLOBALS

static char *NCSErrorTextArray[] = {"No Error",							/* NCS_SUCCESS						*/
	/* NCS Raster Errors */
	"Queue node creation failed",						/* NCS_QUEUE_NODE_CREATE_FAILED		*/
	"File open failed",									/* NCS_FILE_OPEN_FAILED				*/
	"The Image Web Server's licensed file limit has been reached",								/* NCS_FILE_LIMIT_REACHED			*/
	"The requested file is larger than is permitted by the license on this Image Web Server",	/* NCS_FILE_SIZE_LIMIT_REACHED		*/
	"Not enough memory for new file",															/* NCS_FILE_NO_MEMORY				*/
	"The Image Web Server's licensed client limit has been reached",							/* NCS_CLIENT_LIMIT_REACHED			*/
	"Detected duplicate open from net layer",			/* NCS_DUPLICATE_OPEN				*/
	"Packet request type not yet implemented",			/* NCS_PACKET_REQUEST_NYI			*/
	"Packet type is illegal",							/* NCS_PACKET_TYPE_ILLEGAL			*/
	"Client closed while requests outstanding",			/* NCS_DESTROY_CLIENT_DANGLING_REQUESTS */

	/* NCS Network Errors */
	"Client UID unknown",								/* NCS_UNKNOWN_CLIENT_UID			*/
	"Could not create new client",						/* NCS_COULDNT_CREATE_CLIENT		*/
	"Could not resolve address of Image Web Server",		/* NCS_NET_COULDNT_RESOLVE_HOST		*/
	"Could not connect to host",							/* NCS_NET_COULDNT_CONNECT			*/
	"Receive timeout",									/* NCS_NET_RECV_TIMEOUT				*/
	"Error sending header",								/* NCS_NET_HEADER_SEND_FAILURE		*/
	"Error receiving header",							/* NCS_NET_HEADER_RECV_FAILURE		*/
	"Error sending packet",								/* NCS_NET_PACKET_SEND_FAILURE		*/
	"Error receiving packet",							/* NCS_NET_PACKET_RECV_FAILURE		*/
	"401 Unauthorised",									/* NCS_NET_401_UNAUTHORISED			*/
	"403 Forbidden",									/* NCS_NET_403_FORBIDDEN			*/
	"Is the host an Image Web Server?",					/* NCS_NET_404_NOT_FOUND			*/
	"Your HTTP proxy requires authentication",	/*	NCS_NET_407_PROXYAUTH */
	"Unexpected HTTP response",							/* NCS_NET_UNEXPECTED_RESPONSE		*/
	"Bad HTTP response",								/* NCS_NET_BAD_RESPONSE				*/
	"Already connected",								/* NCS_NET_ALREADY_CONNECTED		*/
	"The connection is invalid",						/* NCS_INVALID_CONNECTION			*/
	"Windows sockets failure",							/* NCS_WINSOCK_FAILURE				*/

	/* NCS Symbol Errors */
	"Symbology error",									/* NCS_SYMBOL_ERROR					*/
	"Could not open database",							/* NCS_OPEN_DB_ERROR				*/
	"Could not execute the requested query on database",/* NCS_DB_QUERY_FAILED				*/
	"SQL statement could not be executed",				/* NCS_DB_SQL_ERROR					*/
	"Open symbol layer failed",							/* NCS_GET_LAYER_FAILED				*/
	"The database is not open",							/* NCS_DB_NOT_OPEN					*/
	"This type of quad tree is not supported",			/* NCS_QT_TYPE_UNSUPPORTED			*/
	/* Preference errors */
	"Invalid local user key name specified",			/* NCS_PREF_INVALID_USER_KEY		*/
	"Invalid local machine key name specified",			/* NCS_PREF_INVALID_MACHINE_KEY		*/
	"Failed to open registry key",						/* NCS_REGKEY_OPENEX_FAILED			*/
	"Registry query failed",							/* NCS_REGQUERY_VALUE_FAILED		*/
	"Type mismatch in registry variable",				/* NCS_INVALID_REG_TYPE				*/
	/* Misc errors */
	"Invalid arguments passed to function",				/* NCS_INVALID_ARGUMENTS			*/
	"ECW error",										/* NCS_ECW_ERROR					*/
	"Server error",										/* NCS_SERVER_ERROR					*/
	"Unknown error",									/* NCS_UNKNOWN_ERROR				*/
	"Extent conversion failed",							/* NCS_EXTENT_ERROR					*/
	"Could not allocate enough memory",					/* NCS_COULDNT_ALLOC_MEMORY	[12]	*/
	"An invalid parameter was used",					/* NCS_INVALID_PARAMETER			*/
	/* Compress errors */
	"Could not perform Read/Write on file",				/* NCS_FILEIO_ERROR					*/
	"Could not open compression task",					/* NCS_COULDNT_OPEN_COMPRESSION		*/
	"Could not perform compression",					/* NCS_COULDNT_PERFORM_COMPRESSION	*/
	"Trying to generate too many output lines",			/* NCS_GENERATED_TOO_MANY_OUTPUT_LINES */
	"User cancelled compression",						/* NCS_USER_CANCELLED_COMPRESSION	*/
	"Could not read line from input image file",		/* NCS_COULDNT_READ_INPUT_LINE		*/
	"Input image size exceeded for this version",		/* NCS_INPUT_SIZE_EXCEEDED			*/
		/* Decompression Errors */
	"Specified image region is outside image area",		/* NCS_REGION_OUTSIDE_FILE			*/
	"Supersampling not supported",						/* NCS_NO_SUPERSAMPLE				*/
	"Specified image region has a zero width or height",/* NCS_ZERO_SIZE					*/
	"More bands specified than exist in this file",		/* NCS_TOO_MANY_BANDS				*/
	"An invalid band number has been specified",		/* NCS_INVALID_BAND_NR				*/
	/* New Compression Error */
	"Input image size is too small to compress",		/* NCS_INPUT_SIZE_TOO_SMALL			*/
	/* NEW Network error */
	"The ECWP client version is incompatible with this server", /* NCS_INCOMPATIBLE_PROTOCOL_VERSION */
	"Windows Internet Client error",					/* NCS_WININET_FAILURE				*/
	"Could not load wininet.dll",						/* NCS_COULDNT_LOAD_WININET			*/

	/* NCSFile && NCSRenderer class errors */
	"Invalid SetView parameters or SetView not called.",/* NCS_FILE_INVALID_SETVIEW */
	"There is no open ECW file.",						/* NCS_FILE_NOT_OPEN */
	
	/* NEW JNI Java Errors */
	"Class does not implement ECWProgressiveDisplay interface.",/* NCS_JNI_REFRESH_NOT_IMPLEMENTED */

	/* NEW Coordinate Errors */
	"Incompatible coordinate systems",					/* NCS_INCOMPATIBLE_COORDINATE_SYSTEMS	*/
	"Incompatible coordinate datum types",				/* NCS_INCOMPATIBLE_COORDINATE_DATUM	*/
	"Incompatible coordinate projection types",			/* NCS_INCOMPATIBLE_COORDINATE_PROJECTION*/
	"Incompatible coordinate units types",				/* NCS_INCOMPATIBLE_COORDINATE_UNITS	*/
	"Non-linear coordinate systems not supported",		/* NCS_COORDINATE_CANNOT_BE_TRANSFORMED	*/
	"GDT Error",										/* NCS_GDT_ERROR */
	"Zero length packet",								/* NCS_NET_PACKET_RECV_ZERO_LENGTH */	/**[01]**/
	"Must use Japanese version of the ECW SDK",			/* NCS_UNSUPPORTEDLANGUAGE */			/**[02]**/
	"Loss of connection to server",						/* NCS_CONNECTION_LOST */				/**[03]**/
	"NCSGdt coordinate conversion failed",				/* NCS_COORD_CONVERT_ERROR */
	"Failed to open metabase",							/* NCS_METABASE_OPEN_FAILED*/				/**[04]**/
	"Failed to get value from metabase",				/* NCS_METABASE_GET_FAILED*/				/**[04]**/
	"Timeout sending header",							/* NCS_NET_HEADER_SEND_TIMEOUT*/			/**[05]**/
	"Java JNI error",									/* NCS_JNI_ERROR */						/**[06]**/
	"No data source passed",							/* NCS_DB_INVALID_NAME				*/	/**[07]**/
	"Could not resolve address of Image Web Server Symbol Server Extension",		/* NCS_SYMBOL_COULDNT_RESOLVE_HOST*/  /**[07]**/
	"Invalid NCSError value!",							/* NCS_INVALID_ERROR_ENUM */			/**[08]**/
	/* NCSFileIO errors [10] */
	"End Of File reached",								/* NCS_FILE_EOF							[10]	*/
	"File not found",									/* NCS_FILE_NOT_FOUND					[10]	*/
	"File is invalid or corrupt",						/* NCS_FILE_INVALID						[10]	*/
	"Attempted to read, write or seek past file limits",/* NCS_FILE_SEEK_ERROR					[10]	*/
	"Permissions not available to access file",			/* NCS_FILE_NO_PERMISSIONS				[10]	*/
	"File open error",									/* NCS_FILE_OPEN_ERROR					[10]	*/
	"File close error",									/* NCS_FILE_CLOSE_ERROR					[10]	*/
	"File IO error",									/* NCS_FILE_IO_ERROR					[10]	*/
	
	"Illegal World Coordinates",						/* NCS_SET_EXTENTS_ERROR				[13]	*/

	"Image projection doesn't match controlling layer", /* NCS_FILE_PROJECTION_MISMATCH		[14]	*/

	/** 1.65 gdt errors [15]**/
	"Unknown map projection",							/*NCS_GDT_UNKNOWN_PROJECTION,*/
	"Unknown datum",									/*NCS_GDT_UNKNOWN_DATUM,*/
	"User specified Geographic Projection Database data server failed",	/*NCS_GDT_USER_SERVER_FAILED*/
	"Remote Geographic Projection Database file downloading has been disable and no Geographic Projection Database data is locally available",	/*NCS_GDT_REMOTE_PATH_DISABLED*/
	"Invalid transform mode",							/*NCS_GDT_BAD_TRANSFORM_MODE,*/

	"coordinate to be transformed is out of bounds",	/*NCS_GDT_TRANSFORM_OUT_OF_BOUNDS*/

	"Layer already exists with this layer name",		/*NCS_LAYER_DUPLICATE_LAYER_NAME*/	/**[17]**/
	"Layer does not contain this parameter",			/*NCS_LAYER_INVALID_PARAMETER*/		/**[18]**/

	"Failed to create pipe",							/*NCS_PIPE_CREATE_FAILED*/ /**[19]**/

	/* Directory creation errors */
	"Directory already exists",							/*[20] NCS_FILE_MKDIR_EXISTS, Directory already exists */
	"The path was not found",							/*[20] NCS_FILE_MKDIR_PATH_NOT_FOUND, The path for the location of the new dir does not exist */

	"The read was cancelled",
	"Error reading georeferencing data from JPEG 2000 file", /*[21] NCS_JP2_GEODATA_READ_ERROR*/
	"Error writing georeferencing data to JPEG 2000 file", /*[21] NCS_JP2_GEODATA_READ_ERROR*/
	"JPEG 2000 file is not or should not be georeferenced", /*[21] NCS_JP2_GEODATA_NOT_GEOREFERENCED*/

	"View size is limited to 4000x4000 or 64 high in progressive mode", /*[23] NCS_PROGRESSIVE_VIEW_TOO_LARGE*/
	// Insert new errors before here!
	"Max NCSError enum value!",							/* NCS_MAX_ERROR_NUMBER */				/**[08]**/
	""};

static char *NCSErrorTextArrayEx[] = {
	"No error",												/* NCS_SUCCESS											*/
	/* NCS Raster Errors */
	"Queue node creation failed",							/* NCS_QUEUE_NODE_CREATE_FAILED							*/
	"Could not open file \"%s\" %s",						/* NCS_FILE_OPEN_FAILED									*/
	"The Image Web Server's licensed file limit has been reached %s",							/* NCS_FILE_LIMIT_REACHED (license name eg office) */
	"The requested file is larger than is permitted by the license on this Image Web Server %s",/* NCS_FILE_SIZE_LIMIT_REACHED (license name eg office)	*/
	"Not enough memory for new file",															/* NCS_FILE_NO_MEMORY				*/
	"The Image Web Server's licensed client limit has been reached %s",							/* NCS_CLIENT_LIMIT_REACHED	(license name eg enterprise) */
	"Detected duplicate open from net layer",				/* NCS_DUPLICATE_OPEN									*/
	"Packet request type not yet implemented %d",			/* NCS_PACKET_REQUEST_NYI (packet type num)				*/
	"Packet type is illegal %d",							/* NCS_PACKET_TYPE_ILLEGAL (packet type num)			*/
	"Client closed while requests outstanding",				/* NCS_DESTROY_CLIENT_DANGLING_REQUESTS					*/

	/* NCS Network Errors */
	"Client UID unknown",									/* NCS_UNKNOWN_CLIENT_UID								*/
	"Could not create new client %s",						/* NCS_COULDNT_CREATE_CLIENT (reason)					*/
	"Could not resolve address of Image Web Server %s",		/* NCS_NET_COULDNT_RESOLVE_HOST	(ip or hostname)		*/
	"Could not connect to host %s",							/* NCS_NET_COULDNT_CONNECT (reason)						*/
	"Receive timeout",										/* NCS_NET_RECV_TIMEOUT									*/
	"Error sending header %s",								/* NCS_NET_HEADER_SEND_FAILURE (reason)					*/
	"Error receiving header %s",							/* NCS_NET_HEADER_RECV_FAILURE (reason)					*/
	"Error sending packet",									/* NCS_NET_PACKET_SEND_FAILURE							*/
	"Error receiving packet",								/* NCS_NET_PACKET_RECV_FAILURE							*/
	"401 Unauthorised",										/* NCS_NET_401_UNAUTHORISED								*/
	"403 Forbidden",										/* NCS_NET_403_FORBIDDEN								*/
	"Is the host an Image Web Server?",						/* NCS_NET_404_NOT_FOUND								*/
	"Your HTTP proxy requires authentication,\nthis is presently unsupported by the Image Web Server control",	/*	NCS_NET_407_PROXYAUTH */
	"Unexpected HTTP response %s",								/* NCS_NET_UNEXPECTED_RESPONSE (resonse # or string)*/
	"Bad HTTP response %s",									/* NCS_NET_BAD_RESPONSE	(resonse # or string)			*/
	"Already connected",									/* NCS_NET_ALREADY_CONNECTED							*/
	"The connection is invalid",							/* NCS_INVALID_CONNECTION								*/
	"Windows sockets failure %s",							/* NCS_WINSOCK_FAILURE (reason (GetLastError()) or wininet version)	*/

	/* NCS Symbol Errors */
	"Symbology error",										/* NCS_SYMBOL_ERROR										*/
	"Could not open database",								/* NCS_OPEN_DB_ERROR									*/
	"Could not execute the requested query on database",	/* NCS_DB_QUERY_FAILED									*/
	"SQL statement could not be executed",					/* NCS_DB_SQL_ERROR										*/
	"Open symbol layer failed",								/* NCS_GET_LAYER_FAILED									*/
	"The database is not open",								/* NCS_DB_NOT_OPEN										*/
	"This type of quad tree is not supported",				/* NCS_QT_TYPE_UNSUPPORTED								*/

	/* Preference errors */
	"Invalid local user key name specified %s",				/* NCS_PREF_INVALID_USER_KEY (key name)					*/
	"Invalid local machine key name specified %s",			/* NCS_PREF_INVALID_MACHINE_KEY	(local machine key)		*/
	"Failed to open registry key %s",						/* NCS_REGKEY_OPENEX_FAILED	(key name)					*/
	"Registry query failed %s",								/* NCS_REGQUERY_VALUE_FAILED (reason)					*/
	"Type mismatch in registry variable",					/* NCS_INVALID_REG_TYPE									*/
	
	/* Misc errors */
	"Invalid arguments passed to function %s",				/* NCS_INVALID_ARGUMENTS (function name)				*/
	"ECW error %s",											/* NCS_ECW_ERROR (reason)								*/
	"Server error %s",										/* NCS_SERVER_ERROR (reason)							*/
	"Unknown error %s",										/* NCS_UNKNOWN_ERROR (reason)							*/
	"Extent conversion failed",								/* NCS_EXTENT_ERROR										*/
	"Could not allocate enough memory %s",					/* NCS_COULDNT_ALLOC_MEMORY	(what trying to malloc)	[12]*/
	"An invalid parameter was used %s",						/* NCS_INVALID_PARAMETER (reason)						*/
	
	/* Compress errors */
	"Could not perform Read/Write on file %s",				/* NCS_FILEIO_ERROR	(filename)							*/
	"Could not open compression task %s",					/* NCS_COULDNT_OPEN_COMPRESSION	(reason)				*/
	"Could not perform compression %s",						/* NCS_COULDNT_PERFORM_COMPRESSION	(reason)			*/
	"Trying to generate too many output lines",				/* NCS_GENERATED_TOO_MANY_OUTPUT_LINES					*/
	"User cancelled compression",							/* NCS_USER_CANCELLED_COMPRESSION						*/
	"Could not read line from input image file",			/* NCS_COULDNT_READ_INPUT_LINE							*/
	"Input image size exceeded for this version",			/* NCS_INPUT_SIZE_EXCEEDED								*/
	
	/* Decompression Errors */
	"Specified image region is outside image area",			/* NCS_REGION_OUTSIDE_FILE								*/
	"Supersampling not supported",							/* NCS_NO_SUPERSAMPLE									*/
	"Specified image region has a zero width or height",	/* NCS_ZERO_SIZE										*/
	"More bands specified than exist (%d) in this file (%d)",/* NCS_TOO_MANY_BANDS	(bands passed, bands in file)	*/
	"An invalid band number has been specified %s",			/* NCS_INVALID_BAND_NR (bandnr)							*/
	
	/* New Compression Error */
	"Input image size is too small to compress",			/* NCS_INPUT_SIZE_TOO_SMALL								*/
	/* NEW Network error */
	"The ECWP client version (%s) is incompatible with this server (%s)", /* NCS_INCOMPATIBLE_PROTOCOL_VERSION (clientver, server ver) */
	"Windows Internet Client error %s",						/* NCS_WININET_FAILURE	(reason)						*/
	"Could not load wininet.dll %s",						/* NCS_COULDNT_LOAD_WININET	(reason)					*/

	/* NCSFile && NCSRenderer class errors */
	"Invalid SetView parameters or SetView not called.",	/* NCS_FILE_INVALID_SETVIEW								*/
	"There is no open ECW file.",							/* NCS_FILE_NOT_OPEN									*/
	
	/* NEW JNI Java Errors */
	"Class does not implement ECWProgressiveDisplay interface.",/* NCS_JNI_REFRESH_NOT_IMPLEMENTED */

	/* NEW Coordinate Errors */
	"Incompatible coordinate systems",						/* NCS_INCOMPATIBLE_COORDINATE_SYSTEMS					*/
	"Incompatible coordinate datum types",					/* NCS_INCOMPATIBLE_COORDINATE_DATUM					*/
	"Incompatible coordinate projection types",				/* NCS_INCOMPATIBLE_COORDINATE_PROJECTION				*/
	"Incompatible coordinate units types",					/* NCS_INCOMPATIBLE_COORDINATE_UNITS					*/
	"Non-linear coordinate systems not supported",			/* NCS_COORDINATE_CANNOT_BE_TRANSFORMED					*/
	"GDT Error : %s",										/* NCS_GDT_ERROR (reason)								*/
	"Zero length packet : %s",								/* NCS_NET_PACKET_RECV_ZERO_LENGTH (reason)             */ /**[01]**/
	"Must use Japanese version of the ECW SDK",				/* NCS_UNSUPPORTEDLANGUAGE								*/ /**[02]**/
	"Lost of connection to server : %s",					/* NCS_CONNECTION_LOST									*/ /**[03]**/
	"NCSGdt coordinate conversion failed : %s",				/* NCS_COORD_CONVERT_ERROR */
	"Failed to open metabase : %s",							/* NCS_METABASE_OPEN_FAILED*/				/**[04]**/
	"Failed to get value from metabase : %s",				/* NCS_METABASE_GET_FAILED*/				/**[04]**/
	"Timeout sending header : %s",							/* NCS_NET_HEADER_SEND_TIMEOUT*/			/**[05]**/
	"Java JNI error : %s",									/* NCS_JNI_ERROR */						/**[06]**/
	"No data source passed",								/* NCS_DB_INVALID_NAME									*/
	"Could not resolve address of Image Web Server Symbol Server Extension",		/* NCS_SYMBOL_COULDNT_RESOLVE_HOST		*/
	"Invalid NCSError value!",							/* NCS_INVALID_ERROR_ENUM */			/**[08]**/
	/* NCSFileIO errors [10] */
	"End Of File reached : %s",									/* NCS_FILE_EOF					[10]	*/
	"File not found : %s",										/* NCS_FILE_NOT_FOUND			[10]	*/
	"File is invalid or corrupt : %s",							/* NCS_FILE_INVALID				[10]	*/
	"Attempted to read, write or seek past file limits : %s",	/* NCS_FILE_SEEK_ERROR			[10]	*/
	"Permissions not available to access file : %s",			/* NCS_FILE_NO_PERMISSIONS		[10]	*/
	"File open error : %s",										/* NCS_FILE_OPEN_ERROR			[10]	*/
	"File close error : %s",									/* NCS_FILE_CLOSE_ERROR			[10]	*/
	"File IO error : %s",										/* NCS_FILE_IO_ERROR			[10]	*/
	
	"Illegal World Coordinates : %s",							/* NCS_SET_EXTENTS_ERROR		[13]	*/

	"Image projection doesn't match controlling layer : %s",	/* NCS_FILE_PROJECTION_MISMATCH		[14]	*/

	/** 1.65 gdt errors [15]**/
	"Unknown map projection: %s",							/*NCS_GDT_UNKNOWN_PROJECTION,*/
	"Unknown datum: %s",									/*NCS_GDT_UNKNOWN_DATUM,*/
	"User specified Geographic Projection Database data server failed while loading %s.  Please check your network connection and if the problem persists contact the website administrator.",			/*NCS_GDT_USER_SERVER_FAILED*/
	"Remote Geographic Projection Database file downloading has been disable and no Geographic Projection Database data is locally available",	/*NCS_GDT_REMOTE_PATH_DISABLED*/
	"Invalid transform mode: %s",							/*NCS_GDT_BAD_TRANSFORM_MODE,			[17]**/

	"coordinate to be transformed is out of bounds: %s",	/*NCS_GDT_TRANSFORM_OUT_OF_BOUNDS*/

	"Layer already exists with this name : %s",				/*NCS_LAYER_DUPLICATE_LAYER_NAME*/	/**[17]**/
	"Layer does not contain this parameter : %s",			/*NCS_LAYER_INVALID_PARAMETER*/		/**[18]**/

	"Failed to create pipe : %s",							/*NCS_PIPE_CREATE_FAILED*/ /**[19]**/

	/* Directory creation errors */
	"Directory already exists : %s",						/*[20] NCS_FILE_MKDIR_EXISTS, Directory already exists */
	"The path was not found : %s",							/*[20] NCS_FILE_MKDIR_PATH_NOT_FOUND, The path for the location of the new dir does not exist */

	"The read was cancelled",
	"Error reading georeferencing data from JPEG 2000 file: %s", /*[21] NCS_JP2_GEODATA_READ_ERROR*/
	"Error writing georeferencing data to JPEG 2000 file: %s", /*[21] NCS_JP2_GEODATA_READ_ERROR*/
	"JPEG 2000 file is not or should not be georeferenced: %s", /*[21] NCS_JP2_GEODATA_NOT_GEOREFERENCED*/

	"View size is limited to 4000x4000 or 64 high in progressive mode", /*[23] NCS_PROGRESSIVE_VIEW_TOO_LARGE*/

	// Insert new errors before here!
	"Max NCSError enum value!",							/* NCS_MAX_ERROR_NUMBER */				/**[08]**/
	""};

#endif /* NCSERROR_DEFINE_GLOBALS */

/** 
 *	@def Macro for checking success status on error return values
 */
#define NCS_SUCCEEDED(nError) (nError == NCS_SUCCESS)
/** 
 *	@def Macro for checking fail status on error return values
 */
#define NCS_FAILED(nError) (nError != NCS_SUCCESS)

void NCSErrorInit(void);
void NCSErrorFini(void);

/** 
 *	Construct a message box displaying the error test for the last reported error
 *	@param		nError		The error
 *	@param		pWindow		The parent window of the message box
 */
void NCSGetLastErrorTextMsgBox(NCSError nError, void *pWindow);
/**
 *	Return formatted error text for a series of errors
 *	@param		nError		Variable length list of error arguments
 *	@return					(char *) value, formatted error text
 */
const char * NCSFormatErrorText(NCSError nError, ...);
/**
 *	Return error text for the last error 
 *	@param		nError		Error number
 *	@return					(char *) value, error text
 */
const char * NCSGetLastErrorText(NCSError nError);
/**	
 *	Get the error number of the last error that has occurred
 *	@return					NCSError value, the last error number
 */
NCSError NCSGetLastErrorNum(void);

#ifdef WIN32 // [11]

DWORD NCSDbgGetExceptionInfoMsg(EXCEPTION_POINTERS *pExceptionPtr, char *pMessage);

#endif	/* MACINTOSH */

// Deprecated
extern const char * NCSGetErrorText(NCSError eError);

#ifdef WIN32
#ifdef _WIN32_WCE
#define MSGBOX_WARNING_FLAGS	(MB_OK|MB_ICONINFORMATION)
#define MSGBOX_ERROR_FLAGS		(MB_OK|MB_ICONERROR)
#else
#define MSGBOX_WARNING_FLAGS	(MB_OK|MB_ICONINFORMATION|MB_TASKMODAL)
#define MSGBOX_ERROR_FLAGS		(MB_OK|MB_ICONERROR|MB_TASKMODAL)
#endif
#elif defined PALM
NCSError NCSPalmGetNCSError(Err eErr);
#endif

#ifdef __cplusplus
}
#endif

#endif /* NCSERRORS_H */
