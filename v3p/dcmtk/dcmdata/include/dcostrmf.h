/*
 *
 *  Copyright (C) 1994-2002, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmdata
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: DcmOutputFileStream and related classes,
 *    implements streamed output to files.
 *
 */

#ifndef DCOSTRMF_H
#define DCOSTRMF_H

#include "osconfig.h"
#include "dcostrma.h"

#define INCLUDE_CSTDIO
#include "ofstdinc.h"


/** consumer class that stores data in a plain file.
 */
class DcmFileConsumer: public DcmConsumer
{
public:
  /** constructor
   *  @param filename name of file to be created, must not be NULL or empty
   */
  DcmFileConsumer(const char *filename);

  /// destructor
  virtual ~DcmFileConsumer();

  /** returns the status of the consumer. Unless the status is good,
   *  the consumer will not permit any operation.
   *  @return status, true if good
   */
  virtual OFBool good() const;

  /** returns the status of the consumer as an OFCondition object.
   *  Unless the status is good, the consumer will not permit any operation.
   *  @return status, EC_Normal if good
   */
  virtual OFCondition status() const;

  /** returns true if the consumer is flushed, i.e. has no more data
   *  pending in it's internal state that needs to be flushed before
   *  the stream is closed.
   *  @return true if consumer is flushed, false otherwise
   */
  virtual OFBool isFlushed() const;

  /** returns the minimum number of bytes that can be written with the
   *  next call to write(). The DcmObject write methods rely on avail
   *  to return a value > 0 if there is no I/O suspension since certain
   *  data such as tag and length are only written "en bloc", i.e. all
   *  or nothing.
   *  @return minimum of space available in consumer
   */
  virtual Uint32 avail() const;

  /** processes as many bytes as possible from the given input block.
   *  @param buf pointer to memory block, must not be NULL
   *  @param buflen length of memory block
   *  @return number of bytes actually processed.
   */
  virtual Uint32 write(const void *buf, Uint32 buflen);

  /** instructs the consumer to flush its internal content until
   *  either the consumer becomes "flushed" or I/O suspension occurs.
   *  After a call to flush(), a call to write() will produce undefined
   *  behaviour.
   */
  virtual void flush();

private:

  /// private unimplemented copy constructor
  DcmFileConsumer(const DcmFileConsumer&);

  /// private unimplemented copy assignment operator
  DcmFileConsumer& operator=(const DcmFileConsumer&);

  /// the file we're actually writing to
  FILE *file_;

  /// status
  OFCondition status_;
};


/** output stream that writes into a plain file
 */
class DcmOutputFileStream: public DcmOutputStream
{
public:
  /** constructor
   *  @param filename name of file to be created, must not be NULL or empty
   */
  DcmOutputFileStream(const char *filename);

  /// destructor
  virtual ~DcmOutputFileStream();

private:

  /// private unimplemented copy constructor
  DcmOutputFileStream(const DcmOutputFileStream&);

  /// private unimplemented copy assignment operator
  DcmOutputFileStream& operator=(const DcmOutputFileStream&);

  /// the final consumer of the filter chain
  DcmFileConsumer consumer_;

};


#endif
