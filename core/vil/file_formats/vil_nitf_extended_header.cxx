// This is core/vil/file_formats/vil_nitf_extended_header.cxx

//================ GE Aerospace NITF support libraries =================
//:
// \file
// \brief Implementation of extended headers included in an NITF message.
// \date: 2003/12/26
// \author: mlaymon
//
//  Ported from TargetJr by M. Laymon.

#include <vcl_string.h>
#include <vcl_cstring.h>
#include <vcl_iostream.h>

#include "vil_nitf_extended_header.h"

static int debug_level = 0 ;
static const char* const CRLF = "\n";

//====================================================================
//: Copy constructor for vil_nitf_extended_header.
//====================================================================
vil_nitf_extended_header::vil_nitf_extended_header(const vil_nitf_extended_header& other_header)
  : XHDL(data_length_)
{
    XHDL = other_header.XHDL;
    XHD = new char[XHDL+1];
    XHD[XHDL] = '\0';

    vcl_memcpy(XHD, other_header.XHD, XHDL);
}

int vil_nitf_extended_header::Read(vil_stream* file)
{
    static vcl_string method_name = "vil_nitf_extended_header::Read: ";

    if (debug_level > 0) {
        vcl_cout << "##### enter " << method_name << "XHDL = " << XHDL
                 << "  file position = " << file->tell()
                 << vcl_endl;
    }
    if (!file || !file->ok()) {
      return STATUS_BAD;
    }

    delete [] XHD;
    XHD = 0;
    if (XHDL <= 0)
      return STATUS_GOOD;

    XHD = new char[XHDL + 1];
    XHD[XHDL] = 0;
    if (file->read(XHD, XHDL) < (long)XHDL)
      return STATUS_BAD;
    else
      return STATUS_GOOD;
}

int vil_nitf_extended_header::Write(vil_stream* file)
{
    if (!file || !file->ok()) return STATUS_BAD;
    if (!XHDL) return STATUS_GOOD;
    if (!XHD || file->write(XHD, XHDL) < (long)XHDL)
      return STATUS_BAD;
    else
      return STATUS_GOOD;
}

//====================================================================
//: Method to return a copy of the vil_nitf_header.
//  The copy returned *must* be deleted by the caller.
//====================================================================
vil_nitf_header* vil_nitf_extended_header::Copy()
{
    vil_nitf_header* rval = new vil_nitf_extended_header(*this);
    return rval;
}

void vil_nitf_extended_header::Copy(const vil_nitf_extended_header* h)
{
    //if (!h->XHDL) return;

    XHDL = h->XHDL;
    FilledCopy(XHD, h->XHD);
}

//====================================================================
// NITF test-specific
//====================================================================

int vil_nitf_extended_header::AsciiRead(vil_stream* f)
{
    return Read(f);
}

int vil_nitf_extended_header::AsciiWrite(vil_stream* f)
{
    if (Write(f) != STATUS_GOOD) return STATUS_BAD;
    if (!XHDL) return STATUS_GOOD;
    if (f->write(CRLF, vcl_strlen(CRLF)) < 1) return STATUS_BAD;

    return STATUS_GOOD;
}
