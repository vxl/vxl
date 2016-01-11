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
 *  Purpose: base classes for output streams
 *
 */

#ifndef DCOSTRMA_H
#define DCOSTRMA_H

#include "osconfig.h"
#include "oftypes.h"  /* for OFBool */
#include "ofcond.h"   /* for OFCondition */
#include "dcxfer.h"   /* for E_StreamCompression */


/** pure virtual abstract base class for consumers, i.e. the final node
 *  of a filter chain in an output stream.
 */
class DcmConsumer
{
public:

  /// destructor
  virtual ~DcmConsumer()
  {
  }

  /** returns the status of the consumer. Unless the status is good,
   *  the consumer will not permit any operation.
   *  @return status, true if good
   */
  virtual OFBool good() const = 0;

  /** returns the status of the consumer as an OFCondition object.
   *  Unless the status is good, the consumer will not permit any operation.
   *  @return status, EC_Normal if good
   */
  virtual OFCondition status() const = 0;

  /** returns true if the consumer is flushed, i.e. has no more data
   *  pending in it's internal state that needs to be flushed before
   *  the stream is closed.
   *  @return true if consumer is flushed, false otherwise
   */
  virtual OFBool isFlushed() const = 0;

  /** returns the minimum number of bytes that can be written with the
   *  next call to write(). The DcmObject write methods rely on avail
   *  to return a value > 0 if there is no I/O suspension since certain
   *  data such as tag and length are only written "en bloc", i.e. all
   *  or nothing.
   *  @return minimum of space available in consumer
   */
  virtual Uint32 avail() const = 0;

  /** processes as many bytes as possible from the given input block.
   *  @param buf pointer to memory block, must not be NULL
   *  @param buflen length of memory block
   *  @return number of bytes actually processed.
   */
  virtual Uint32 write(const void *buf, Uint32 buflen) = 0;

  /** instructs the consumer to flush its internal content until
   *  either the consumer becomes "flushed" or I/O suspension occurs.
   *  After a call to flush(), a call to write() will produce undefined
   *  behaviour.
   */
  virtual void flush() = 0;
};


/** pure virtual abstract base class for output filters, i.e.
 *  intermediate nodes of a filter chain in an output stream.
 */
class DcmOutputFilter: public DcmConsumer
{
public:

  /// destructor
  virtual ~DcmOutputFilter()
  {
  }

  /** determines the consumer to which the filter is supposed
   *  to write it's output.  Once a consumer for the output filter has
   *  been defined, it cannot be changed anymore during the lifetime
   *  of the object.
   *  @param consumer reference to consumer, must not be circular chain
   */
  virtual void append(DcmConsumer& consumer) = 0;
};


/** base class for output streams.
 *  This class cannot be instantiated since the constructor is protected.
 */
class DcmOutputStream
{
public:

  /// destructor
  virtual ~DcmOutputStream();

  /** returns the status of the stream. Unless the status is good,
   *  the stream will not permit any operation.
   *  @return status, true if good
   */
  virtual OFBool good() const;

  /** returns the status of the consumer as an OFCondition object.
   *  Unless the status is good, the consumer will not permit any operation.
   *  @return status, EC_Normal if good
   */
  virtual OFCondition status() const;

  /** returns true if the stream is flushed, i.e. has no more data
   *  pending in it's internal state that needs to be flushed before
   *  the stream is closed.
   *  @return true if stream is flushed, false otherwise
   */
  virtual OFBool isFlushed() const;

  /** returns the minimum number of bytes that can be written with the
   *  next call to write(). The DcmObject write methods rely on avail
   *  to return a value > 0 if there is no I/O suspension since certain
   *  data such as tag and length are only written "en bloc", i.e. all
   *  or nothing.
   *  @return minimum of space available in stream
   */
  virtual Uint32 avail() const;

  /** processes as many bytes as possible from the given input block.
   *  @param buf pointer to memory block, must not be NULL
   *  @param buflen length of memory block
   *  @return number of bytes actually processed.
   */
  virtual Uint32 write(const void *buf, Uint32 buflen);

  /** instructs the stream to flush its internal content until
   *  either the stream becomes "flushed" or I/O suspension occurs.
   *  After a call to flush(), a call to write() will produce undefined
   *  behaviour.
   */
  virtual void flush();

  /** returns the total number of bytes written to the stream so far
   *  @return total number of bytes written to the stream
   */
  virtual Uint32 tell() const;

  /** installs a compression filter for the given stream compression type,
   *  which should be neither ESC_none nor ESC_unsupported. Once a compression
   *  filter is active, it cannot be deactivated or replaced during the
   *  lifetime of the stream.
   *  @param filterType type of compression filter
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition installCompressionFilter(E_StreamCompression filterType);

protected:

  /** protected constructor, to be called from derived class constructor
   *  @param initial initial pointer to first node in filter chain
   *     The pointer is not dereferenced in the constructor, so the
   *     object pointed to may be initialized later in the subclass
   *     constructor.
   */
  DcmOutputStream(DcmConsumer *initial);

private:

  /// private unimplemented copy constructor
  DcmOutputStream(const DcmOutputStream&);

  /// private unimplemented copy assignment operator
  DcmOutputStream& operator=(const DcmOutputStream&);

  /// pointer to first node in filter chain
  DcmConsumer *current_;

  /// pointer to compression filter, NULL if no compression
  DcmOutputFilter *compressionFilter_;

  /// counter for number of bytes written so far
  Uint32 tell_;
};



#endif
