// This is core/vil/file_formats/vil_nitf_message_header_v20.h
#ifndef vil_nitf_message_header_v20_h_
#define vil_nitf_message_header_v20_h_
//================ GE Aerospace NITF support libraries =================
//:
// \file
// \brief This file defines the NITF headers for NITF v1.1.
//
// For a given header 'head', head->Read(file) is called when the
// appropriate place in the file is reached, and it will return a
// StatusCode indicating success or failure.
//
//  \date: 2003/12/26
//  \author: mlaymon
//
// Written by:       Burt Smith
// Date:             August 26, 1992
//
//=====================lkbjfcbtdddhtargbskmtaps=======================

#include "vil_nitf_typeinfo.h"
#include "vil_nitf_header.h"
#include "vil_nitf_message_header.h"
#include "vil_nitf_util.h"

//====================================================================
// The NITF Message header.
//====================================================================

class vil_nitf_message_header_v20 : public vil_nitf_message_header
{
 public:
  vil_nitf_message_header_v20();
  vil_nitf_message_header_v20 (const vil_nitf_message_header&);
  vil_nitf_message_header_v20 (const vil_nitf_message_header_v20&);
  virtual ~vil_nitf_message_header_v20();

  virtual StatusCode Read (vil_stream*);
  virtual StatusCode Write (vil_stream*);
  virtual bool read_version (vil_stream* file);

  //NITF test-specific
  // NITF v2.0 Message headers do not have AsciiRead/Write() methods.
  // Default implementation returns STATUS_BAD.
  virtual StatusCode AsciiRead (vil_stream*);
  virtual StatusCode AsciiWrite (vil_stream*);

  virtual unsigned long GetHeaderLength() const;

 private:

  void Init();

  // Internal helper methods for method Read
  bool read_image_info (vil_stream* file);
//bool read_XXX_data (vil_stream* file);
  bool read_extended_header_data (vil_stream* file);
};

//====================================================================
// NITF v2.0 Message headers do not have AsciiRead/Write() methods.
//====================================================================
inline StatusCode vil_nitf_message_header_v20::AsciiRead (vil_stream* is)  {return STATUS_BAD;}
inline StatusCode vil_nitf_message_header_v20::AsciiWrite (vil_stream* is) {return STATUS_BAD;}

#endif
