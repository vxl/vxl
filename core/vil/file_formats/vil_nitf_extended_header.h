// This is core/vil/file_formats/vil_nitf_extended_header.h
#ifndef vil_nitf_extended_header_h_
#define vil_nitf_extended_header_h_
//================ GE Aerospace NITF support libraries =================
//:
// \file
// \date: 2003/12/26
// \author: mlaymon
// \brief This file defines the class for extended headers included in an NITF message.
//
//  Ported from TargetJr by M. Laymon.

#include "vil_nitf_header.h"

class vil_nitf_extended_header : public vil_nitf_header
{
 public:
  vil_nitf_extended_header(unsigned long s) : XHDL(data_length_) {XHDL=s; XHD=0;}
  vil_nitf_extended_header(const vil_nitf_extended_header& header);
  virtual ~vil_nitf_extended_header() { delete[] XHD; }

  virtual int Read(vil_stream*);
  virtual int Write(vil_stream*);

  //NITF test-specific
  virtual int AsciiRead(vil_stream*);
  virtual int AsciiWrite(vil_stream*);

  virtual unsigned long GetHeaderLength() const { return XHDL; }

  unsigned long& XHDL; // Extended header data length.  Must be unsigned long
                       // because reference is set to data_length_
                       // attribute in super-class vil_nitf_header
                       // and we want data_length_ to be unsigned long
                       // for large images.
  char* XHD;

  virtual vil_nitf_header* Copy();
  void    Copy(const vil_nitf_extended_header*);
  const   vil_nitf_extended_header& operator=(const vil_nitf_extended_header& header)
  {
    Copy(&header);
    return *this;
  }
};

typedef vil_nitf_extended_header vil_nitf_extended_subheader;

#endif  // vil_nitf_extended_header_h_
