// This is core/vil/file_formats/vil_nitf_header.h
#ifndef vil_nitf_header_h_
#define vil_nitf_header_h_
//================ GE Aerospace NITF support libraries =================
//:
// \file
// \date: 2003/12/26
// \author: mlaymon
//
// \brief This file defines the base class for headers included in an NITF message.
//
//  Ported from TargetJr by M. Laymon.
//
//     The NITF Header
//     Known sub-classes:
//       - The Image Sub-Header
//       - The Symbol Sub-Header - Symbols are referred to as Graphics in NITF specification
//       - The DES Sub-header version 2.0 and higher only
//       - The Label Sub-Header
//       - The Text Sub-Header
//
//   Following classes were in TargetJr implementation, but only apply to version 1.1 and lower.
//   Port of NITF capability to VXL is only for NITF version 2.0 and above.
//     - The Audio Sub-Header
//     - The NPI Sub-Header
//
// Written by:       Burt Smith
// Date:             August 24, 1992 (Allison's 9 month birthday)
//
// Modification by:  Paul Max Payton (PMP)
// Date:             August 14, 1997
// Why?              Added rational polynomial stuff in NITF's image subheader.
//
// Modification by:  Glen W. Brooksby (GWB)
// Date:             February 18, 1999
// Why?:             Added support for ICHIPB Support Data Extension
//
//=====================lkbjfcbtdddhtargbskmtaps=======================
//
// Copyright (C) 1998, Lockheed Martin Corporation
//
// This software is intellectual property of Lockheed Martin
// Corporation and may not be copied or redistributed except
// as specified in the FOCUS Software License.
//

#include <vcl_cassert.h>
#include <vcl_cstring.h>

#include <vil/vil_smart_ptr.h>
#include <vil/vil_stream.h>

#include "vil_nitf_typeinfo.h"

class vil_nitf_version;
class vil_nitf_image_subheader;
class vil_nitf_message_header;

enum   NITFClass { UNCLASSIFIED, RESTRICTED, CONFIDENTIAL, SECRET, TOPSECRET };
extern NITFClass DefaultClassification;
enum   NITFEncryp { ENCRYPTED, NOTENCRYPTED };

NITFClass   ConvertClassification (const char*);
const char* ConvertClassification (NITFClass);
bool     ValidClassification (const char*);

//====================================================================
// Generic NITF header.
//====================================================================

class vil_nitf_header
{
 public:
  vil_nitf_header (unsigned long len = 0);
  vil_nitf_header (const vil_nitf_header& header);
  virtual ~vil_nitf_header ();

  virtual int Read (vil_stream*)  { return STATUS_BAD; }
  virtual int Write (vil_stream*) { return STATUS_BAD; }

  //NITF test-specific
  virtual int AsciiRead (vil_stream*)  { return STATUS_BAD; }
  virtual int AsciiWrite (vil_stream*) { return STATUS_BAD; }

  virtual unsigned long GetHeaderLength() const {return 0;} /// DEFAULT IMPLEMENTATION

  // Fields common to all (or nearly all) NITF objects.

  virtual unsigned long get_data_length() const {return data_length_;}  /// Default implementaton
  void set_data_length (unsigned long new_val) {data_length_ = new_val;}

  char*      ID;
  char*      DT;  /// Date and Time stored in format DDhhmmssTMMMYY where T = time zone.
  char*      TITLE;  /// Name in some objects.
  NITFClass  CLAS;
  char*      CODE;
  char*      CTLH;
  char*      REL;
  char*      CAUT;
  char*      CTLN;
  char*      DWNG;
  char*      DEVT;
  NITFEncryp ENCRYP;

  int        DLVL;
  int        ALVL;
  int        LOCrow;
  int        LOCcolumn;

  vil_nitf_version * get_version();  // SHOULD THIS BE CONST ??  MAL 2oct2003
  void    setVersion (vil_nitf_version*);

  virtual vil_nitf_header* Copy();
  void    Copy (const vil_nitf_header*);
  const   vil_nitf_header& operator= (const vil_nitf_header&);

  void set_title (char * new_val);

 protected:
  vil_nitf_version* version_;
  bool verbose_;
  unsigned long data_length_;  /// Want largest integer value possible for large images.

 private:
  // vil cannot have dependency on vbl, so cannot sub-class vbl_ref_count.
  // Do own reference counting here.
  friend class vil_smart_ptr<vil_nitf_header>;
  friend class vil_smart_ptr<vil_nitf_image_subheader>;
  friend class vil_smart_ptr<vil_nitf_message_header>;
  void ref() { ++reference_count_;}
  void unref() {
    assert(reference_count_>0);
    if (--reference_count_<=0) delete this;
  }
  int reference_count_;
};


//====================================================================
// The FilledCopy() function copies the src string to the dst string
// with the restriction that the dst string may not change length.
// if strlen (src) < strlen(dst) it is right padded with 'fill', and if
// strlen (src) > strlen (dst) it is truncated.
//====================================================================
inline char* FilledCopy (char* dst, const char* src, char fill = ' ')
{
  int d = vcl_strlen (dst);
  int s = vcl_strlen (src);
   if (s < d)
    vcl_memset(dst+s, fill, d-s);
  else
    s = d;
  return vcl_strncpy(dst, src, s);
}

inline const vil_nitf_header&
vil_nitf_header::operator=(const vil_nitf_header& header)
{
  Copy (&header);
  return *this;
}

typedef vil_smart_ptr<vil_nitf_header> vil_nitf_header_sptr;

#endif  // vil_nitf_header_h_
