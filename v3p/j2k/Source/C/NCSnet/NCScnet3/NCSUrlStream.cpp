/********************************************************
** Copyright 1999 Earth Resource Mapping Pty Ltd.
** This document contains unpublished source code of
** Earth Resource Mapping Pty Ltd. This notice does
** not indicate any intention to publish the source
** code contained herein.
**
** FILE:   	NCScnet3\NCSUrlStream.cpp
** CREATED:	14-5-2001
** AUTHOR: 	Russell Rogers
** PURPOSE:	
** EDITS:
** [01]
*******************************************************/


#include "cnet.h"
#include "NCScnet.h"
#ifdef WIN32
#include "wininet.h"
#else
#define INTERNET_DEFAULT_HTTP_PORT 80
#endif // WIN32

#include "NCSRequest.h"
#include "NCSUrlStream.h"

#ifndef BUFSIZ
#  define BUFSIZ 1024
#endif

//Stream Buffer
CNCSUrlBuffer::CNCSUrlBuffer(char *pUrl, char *pHeader, char *pBody, BOOLEAN post)
{
	if (pUrl)
		m_pUrl = NCSStrDup(pUrl);

	if (pHeader)
		m_pHeader = NCSStrDup(pHeader);

	if (pBody)
		m_pBody = NCSStrDup(pBody);

	m_bPost = post;

	NCSMutexInit(&m_nBufferMutex);
	m_pStrStreamBuffer = new strstreambuf;
}

CNCSUrlBuffer::CNCSUrlBuffer(const CNCSUrlBuffer &sb)
{
	if (sb.m_pUrl)
		m_pUrl = NCSStrDup(sb.m_pUrl);

	if (sb.m_pHeader)
		m_pHeader = NCSStrDup(sb.m_pHeader);

	if (sb.m_pBody)
		m_pBody = NCSStrDup(sb.m_pBody);

	m_bPost = sb.m_bPost;

	NCSMutexInit(&m_nBufferMutex);
	if (m_pStrStreamBuffer)
		delete m_pStrStreamBuffer;
}

CNCSUrlBuffer& CNCSUrlBuffer::operator=(const CNCSUrlBuffer &sb)
{
	if (this != &sb)
	{
		if (m_pUrl)	NCSFree(m_pUrl);
		if (m_pHeader) NCSFree(m_pHeader);
		if (m_pBody) NCSFree(m_pBody);

		if (sb.m_pUrl)
			m_pUrl = NCSStrDup(sb.m_pUrl);

		if (sb.m_pHeader)
			m_pHeader = NCSStrDup(sb.m_pHeader);

		if (sb.m_pBody)
			m_pBody = NCSStrDup(sb.m_pBody);

		m_bPost = sb.m_bPost;
	}

	return *this;
}

CNCSUrlBuffer::~CNCSUrlBuffer()
{
	overflow(EOF);
	delete [] base();

	if (m_pUrl)
		NCSFree(m_pUrl);

	if (m_pHeader)
		NCSFree(m_pHeader);

	if (m_pBody)
		NCSFree(m_pBody);

	NCSMutexFini(&m_nBufferMutex);
	NCSThreadFini();
}

/**
 *	Opens the connection and starts reading back data.
 *  Fills the buffer with the data read back from the connection.
 */
BOOLEAN CNCSUrlBuffer::Connect()
{
	NCSThreadInit();
	BOOLEAN bStarted = NCSThreadSpawn(&m_tUrlConn, &ThreadProcessing, (void*)this, FALSE);
	if (bStarted)
		while (!NCSThreadIsRunning(&m_tUrlConn));

	return bStarted;
}

int CNCSUrlBuffer::underflow()
{
	//if (no more data from connection) return EOF;
  
	//if still more characters in buffer
	if (gptr () < egptr ()) return *(unsigned char*)gptr ();
  
	//if no buffer and deallocation fails
	if (base () == 0 && doallocate () == 0) return EOF;
  
	//read in some data from the conenction
	int bufsz = unbuffered () ? 1: BUFSIZ;
	int rval = Read(base(), bufsz);	//read specified bytes from buffer
	if (rval == EOF)
	{
		return EOF;
	}
	else if (rval == 0)
		return EOF;
	
	setg(eback (), base (), base () + rval);	//set the get buffer pointer
	return *(unsigned char*)gptr ();			//return character at gptr;
}

int CNCSUrlBuffer::overflow(int c)
{
// if c == EOF, return flush_output();
// if c == '\n' and linebuffered, insert c and
// return (flush_output()==EOF)? EOF: c;     
// otherwise insert c into the buffer and return c
	if (c == EOF) return flush_output();	//as forcing an error, destroy any data
  
	//if (xflags () & _S_NO_WRITES) return EOF;	//if nothing to write
  
	//if (pbase () == 0 && doallocate () == 0) return EOF;	// if no base and can't allocate
  
	if (pptr () >= epptr() && flush_output () == EOF)
		return EOF;

	//xput_char (c);
	if ((unbuffered () && c == '\n' || pptr () >= epptr ())
		&& flush_output () == EOF)
			return EOF;
  
	return c;
}

int CNCSUrlBuffer::flush_output()
// return 0 when there is nothing to flush or when the flush is a success
// return EOF when it could not flush
{
  if (pptr () <= pbase ()) return 0;
  return EOF;
}

int CNCSUrlBuffer::sync ()
{
  return flush_output ();
}

int CNCSUrlBuffer::doallocate()
{
  if (!base ()) {
    char*	buf = new char[2*BUFSIZ];
    setb (buf, buf+BUFSIZ);
    setg (buf, buf, buf);
    
    buf += BUFSIZ;
    setp (buf, buf+BUFSIZ);
    return 1;
  }
  return 0;
}

BOOLEAN CNCSUrlBuffer::Write(void *pBuf, int len)
{

	NCSMutexBegin(&m_nBufferMutex);
	m_pStrStreamBuffer->sputn((char *) pBuf, len);
	NCSMutexEnd(&m_nBufferMutex);
	return TRUE;
}

int CNCSUrlBuffer::Read(void *pBuf, int len)
{
	BOOLEAN bNoData;
	int retVal;
	static int i = 0;


	if (((++i%10) == 0) && (i>10)) {
		i=0;
		NCSThreadYield(); // Yeild this thread every 30 scanlines to free up cpu time for drawing.
	}

	bNoData = TRUE;
	retVal = EOF;
	while (NCSThreadIsRunning(&m_tUrlConn) && bNoData)
	{
		NCSMutexBegin(&m_nBufferMutex);
		retVal = m_pStrStreamBuffer->sgetn((char *) pBuf, len);
		NCSMutexEnd(&m_nBufferMutex);
		if (retVal != EOF && retVal != 0)
			bNoData = FALSE;
		else
			NCSSleep(100);
	}

	//check for data if thread not running
	if (!NCSThreadIsRunning(&m_tUrlConn) && bNoData)
	{
		NCSMutexBegin(&m_nBufferMutex);
		retVal = m_pStrStreamBuffer->sgetn((char *) pBuf, len);
		NCSMutexEnd(&m_nBufferMutex);
	}

	return retVal;
}

void CNCSUrlBuffer::ThreadProcessing(void *pData)
{
	CNCSUrlBuffer *pBuffer = (CNCSUrlBuffer *) pData;
	//char *pResponses=NULL;
	//int nLen;
	
	pBuffer->PostURLBuffered( );
}

NCSError CNCSUrlBuffer::PostURLBuffered( )
{
	NCSError eError = NCS_SUCCESS;
	//HINTERNET hInternet;
	char *p = 0;
	char *pOpenVerb = NULL;

	CNCSRequest request;
	string sMethod;
	string sHeader = m_pHeader;

	if (m_bPost)
		sMethod = "POST";
	else
		sMethod = "GET";

	if( request.SendARequest( m_pUrl, INTERNET_DEFAULT_HTTP_PORT, sMethod, sHeader, m_pBody ) ) {
		char *pData = (char*)NCSMalloc(1024, TRUE);

		while(true) {
			unsigned long nThisRead;
			nThisRead = request.Read( (char*)pData, 1024 );
			if(nThisRead != 0) {
				Write((char *)pData, nThisRead);
			} else {
				break;
			}
		}
	}
	else {
		eError = NCS_NET_COULDNT_CONNECT;
	}
	return(eError);
}


//stream
CNCSUrlStream::~CNCSUrlStream()
{
  //delete ios::rdbuf ();
  init (0);
}

void CNCSUrlStream::error(const char* errmsg) const
{
	//return some kind of error message
}
