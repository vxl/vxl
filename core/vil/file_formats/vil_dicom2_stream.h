#ifndef vil_dicom2_stream_h_
#define vil_dicom2_stream_h_

//:
// \file
// \author Amitha Perera

#include <dcistrma.h>

class vil_stream;

class vil_dicom2_stream_producer
  : public DcmProducer
{
public:
  vil_dicom2_stream_producer( vil_stream* vs );

  virtual ~vil_dicom2_stream_producer();

  virtual OFBool good() const;
  virtual OFCondition status() const;
  virtual OFBool eos() const;
  virtual Uint32 avail() const;
  virtual Uint32 read(void *buf, Uint32 buflen);
  virtual Uint32 skip(Uint32 skiplen);
  virtual void putback(Uint32 num);

private:
  vil_stream* vs_;
};


class vil_dicom2_stream_factory
  : public DcmInputStreamFactory
{
public:
  vil_dicom2_stream_factory( vil_stream* vs );

  virtual ~vil_dicom2_stream_factory();

  virtual DcmInputStream* create() const;

  virtual DcmInputStreamFactory* clone() const
    {
      return new vil_dicom2_stream_factory(*this);
    }

private:
  vil_stream* vs_;
};

class vil_dicom2_stream_input
  : public DcmInputStream
{
public:
  vil_dicom2_stream_input( vil_stream* vs );
  virtual ~vil_dicom2_stream_input();
  virtual DcmInputStreamFactory* newFactory() const;

};

#endif // vil_dicom2_stream_h_
