// This is core/vil/file_formats/vil_nitf_message_header_v20.cxx
#include "vil_nitf_message_header_v20.h"
//================ GE Aerospace NITF support libraries =================
//:
// \file
// \brief Implentation for NITF version 2.0 message header.
//
//  \date: 2003/12/26
//  \author: mlaymon
//
// Written by:       Lynn Bigelow
// Date:             July, 1993
//
// Modification by:  Paul Max Payton (PMP)
// Date:             August 14, 1997
// Why?              Added code to parse rational polynomial stuff in NITF.
//
// Modification by:  Paul Max Payton (PMP)
// Date:             March 16, 1999
// Why?              Added code to handle anamorphic ratio (MPD26A) in NITF.
//
//=====================lkbjfcbtdddhtargbskmtaps=======================
//
// Copyright (C) 1998, Lockheed Martin Corporation
//
// This software is intellectual property of Lockheed Martin
// Corporation and may not be copied or redistributed except
// as specified in the FOCUS Software License.
//

#include <sys/types.h>
#ifndef WIN32
#include <sys/time.h>
#endif
#include <vcl_cstring.h>

#include "vil_nitf_macro_defs.h"
#include "vil_nitf_version_v20.h"
#include "vil_nitf_header_v20.h"

char const * const HEADER_VERSION_STR = "NITF02.00";
static int debug_level = 0;

vil_nitf_message_header_v20::vil_nitf_message_header_v20()
{
  Init();
}

vil_nitf_message_header_v20::vil_nitf_message_header_v20(const vil_nitf_message_header& h)
{
    Init();
    Copy(&h);
}

vil_nitf_message_header_v20::vil_nitf_message_header_v20(vil_nitf_message_header_v20 const& h)
 : vil_nitf_message_header(h)
{
    setVersion(vil_nitf_version_v20::get_version_str());
    FilledCopy(MDT, GetVersion20Date());
}

vil_nitf_message_header_v20::~vil_nitf_message_header_v20()
{
}

//: Read version from input.
//
// \return true if version is correct, otherwise false.

bool vil_nitf_message_header_v20::read_version(vil_stream* file)
{
    bool valid = false;

    if (!file || !file->ok()) return valid;

    valid = true;
    if (file->read(MHDR, 9) <  9 ||
        vcl_strcmp(MHDR, HEADER_VERSION_STR))
    {
        valid = false;
    }
    return valid;
}

StatusCode vil_nitf_message_header_v20::Read(vil_stream* file)
{
    static vcl_string method_name = "vil_nitf_message_header_v20::Read: ";

    if (!file || !file->ok()) {
      return STATUS_BAD;
    }

    int curpos = file->tell();

    if (debug_level > 1) {
      vcl_cout << "\n#### enter " << method_name
               << "currpos = " << curpos << vcl_endl;
    }

    char buffer[16];

    vcl_string error_details = "";

    bool error = false;
    bool done  = false;

    while (!done)
    {
        if (!read_version(file)) {
            error = true;
            error_details = "read_version";
            break;
        }

        if ( !GetInt(buffer, &CLEVEL, 2, file) ||
             (CLEVEL > 6 && CLEVEL != 99 ))
        {
            error_details = "after read CLEVEL = " + CLEVEL;
            error = true;
            break;
        }

        if (file->read(STYPE,   4) <  4 ||
            file->read(OSTAID, 10) < 10 ||
            file->read(MDT,    14) < 14 ||
            file->read(MTITLE, 80) < 80)
        {
            error_details = "after read MTITLE";
            error = true;
            break;
        }

        buffer[1] = 0;  // For when MSCLAS is read in.

        if (file->read(buffer,  1) <  1 ||
            !ValidClassification(buffer))
        {
            error_details = "after read MSCLAS";
            error = true;
            break;
        }
        MSCLAS = ConvertClassification(buffer);

        buffer[1] = 0;                // For when ENCRYP is read in.

        if (file->read(MSCODE, 40) < 40 ||
            file->read(MSCTLH, 40) < 40 ||
            file->read(MSREL,  40) < 40 ||
            file->read(MSCAUT, 20) < 20 ||
            file->read(MSCTLN, 20) < 20 ||
            file->read(MSDWNG,  6) <  6 ||
            (!vcl_strcmp(MSDWNG, "999998") && file->read(MSDEVT,40)  < 40) ||
            !GetInt(buffer, &MSCOP, 5, file) ||
            !GetInt(buffer, &MSCPYS, 5, file) ||
            file->read(buffer,  1) < 1 ||
            !vcl_strpbrk(buffer, "01"))
        {
            error_details = "after read ENCRYP";
            error = true;
            break;
        }
         ENCRYP = buffer[0] == '0' ? NOTENCRYPTED : ENCRYPTED;

        if (file->read(ONAME,  27) < 27 ||
            file->read(OPHONE, 18) < 18 ||
            !GetInt(buffer, &ML, 12,  file) || ML < 388 ||
            !GetInt(buffer, &HL, 6, file) || HL < 388 || HL > 276380)
        {
            error_details = "after read HL";
            error = true;
            break;
        }

        // Now start reading the controls for each type
        // of sub-header.
        //

        // The Image Sub-Header information is first.
        //
        if (read_image_info(file) == false)
        {
            error_details = "after read_image_info";
            error = true;
            break;
        }

        if (error) break;

        int n;

        // The Symbol Sub-Header information is next.
        //
        for (n = 0; n < NUMS; n++) delete SYMBOLSH[n];
        delete SYMBOLSH;
        SYMBOLSH = 0;
        NUMS = 0;
        if (!GetInt( buffer, &NUMS, 3, file))
        {
            error_details = "after read NUMS";
            error = true;
            break;
        }
        if (NUMS > 0)
        {
            SYMBOLSH = new SymbolSH*[NUMS];
            for (n = 0; n < NUMS; n++) SYMBOLSH[n] = 0;
        }
        for (n = 0; n < NUMS; n++)
        {
            SYMBOLSH[n] = new SymbolSH;
            if (!GetInt(buffer, &SYMBOLSH[n]->LSSH, 4, file) ||
                SYMBOLSH[n]->LSSH > 7000                     ||
                !GetInt(buffer, &SYMBOLSH[n]->LS,   6, file))
            {
                error_details = "reading symbol header info";
                error = true;
                break;
            }
        }
        if (error) break;

        // The Label Sub-Header information is next.
        //
        for (n = 0; n < NUML; n++) delete LABELSH[n];
        delete LABELSH;
        LABELSH = 0;
        NUML = 0;
        if (!GetInt(buffer, &NUML, 3, file))
        {
            error_details = "after read NUML";
            error = true;
            break;
        }
        if (NUML > 0)
        {
            LABELSH = new LabelSH*[NUML];
            for (n=0; n<NUML; n++) LABELSH[n] = 0;
        }
        for (n = 0; n < NUML; n++)
        {
            LABELSH[n] = new LabelSH;
            if (!GetInt(buffer, &LABELSH[n]->LLSH, 4, file) ||
                !GetInt(buffer, &LABELSH[n]->LL,   3, file) ||
                LABELSH[n]->LL > 320)
            {
                error_details = "after reading label header info";
                error = true;
                break;
            }
        }
        if (error) break;

        // The Text Sub-Header information is next.
        //
        for (n = 0; n < NUMT; n++) delete TEXTSH[n];
        delete TEXTSH;
        TEXTSH = 0;
        NUMT = 0;
        if (!GetInt(buffer, &NUMT, 3, file))
        {
            error_details = "after read NUMT";
            error = true;
            break;
        }
        if (NUMT > 0)
        {
            TEXTSH = new TextSH*[NUMT];
            for (n = 0; n < NUMT; n++) TEXTSH[n] = 0;
        }
        for (n = 0; n < NUMT; n++)
        {
            TEXTSH[n] = new TextSH;
            if (!GetInt(buffer, &TEXTSH[n]->LTSH, 4, file) ||
                TEXTSH[n]->LTSH > 2000                     ||
                !GetInt(buffer, &TEXTSH[n]->LT,   5, file))
            {
                error_details = "after reading text header info";
                error = true;
                break;
            }
        }
        if (error) break;

        // The Data Extension Segment Sub-Header information is next.
        //
        for (n = 0; n < NUMDES; n++) delete DESSH[n];
        delete DESSH;
        DESSH = 0;
        NUMDES = 0;
        if (!GetInt(buffer, &NUMDES, 3, file))
        {
            error_details = "after read NUMDES";
            error = true;
            break;
        }
        if (NUMDES > 0)
        {
            DESSH = new DES_SH*[NUMDES];
            for (n = 0; n < NUMDES; n++) DESSH[n] = 0;
        }
        for (n = 0; n < NUMDES; n++)
        {
            DESSH[n] = new DES_SH;
            if (!GetInt(buffer, &DESSH[n]->LDSH, 4, file) ||
                !GetInt(buffer, &DESSH[n]->LD,   9, file))
            {
                error_details = "after read DES header info";
                error = true;
                break;
            }
        }
        if (error) break;

        // The Reserved Extension Segment Sub-Header information is first.
        //
        for (n = 0; n < NUMRES; n++) delete RESSH[n];
        delete RESSH;
        RESSH = 0;
        if (!GetInt(buffer, &NUMRES, 3, file))
        {
            error_details = "after read NUMRES";
            error = true;
            break;
        }
        if (NUMRES > 0)
        {
            RESSH = new RES_SH*[NUMRES];
            for (n = 0; n < NUMRES; n++) RESSH[n] = 0;
        }
        for (n = 0; n < NUMRES; n++)
        {
            RESSH[n] = new RES_SH;
            if (!GetInt(buffer, &RESSH[n]->LRSH, 4, file) ||
                !GetInt(buffer, &RESSH[n]->LR,   7, file))
            {
                error_details = "after read RES header info";
                error = true;
                break;
            }
        }
        if (error) break;

        // And finally, read any extended header information.
        //
        if (read_extended_header_data(file) == false)
        {
            error_details = "after read extended header info";
            error = true;
            break;
        }

        done = true;
    }  // end while (!done)

    if (error)
    {
        file->seek(curpos);
    }

    StatusCode status_code = (error ? STATUS_BAD : STATUS_GOOD);

    if (debug_level > 1) {
      vcl_cout << "\n#### exit " << method_name
               << "status code = " << status_code
               << "  file position = " << file->tell()
               << vcl_endl;

      if (error_details.length() > 0) {
        vcl_cout << " error details = <" << error_details << ">\n";
      }
    }

    return status_code;

}  // end method Read

//: Read image information.  Does NOT actually read image bytes.
//
// \param return false if error

bool vil_nitf_message_header_v20::read_image_info(vil_stream* file)
{
    bool success = true;

    int n;
    for (n = 0; n < NUMI; n++) {
        delete IMAGESH[n];
    }
    delete IMAGESH;
    IMAGESH = 0;
    NUMI = 0;

    char buffer[16];
    if (!GetInt(buffer, &NUMI, 3, file))
    {
        success = false;
    }
    if (success && NUMI > 0)
    {
        IMAGESH = new ImageSH*[NUMI];
        for (n = 0; n < NUMI; n++) {
            IMAGESH[n] = 0;
        }

        for (n = 0; n < NUMI; n++)
        {
            IMAGESH[n] = new ImageSH;

            success = GetInt(buffer, &IMAGESH[n]->LISH, 6, file);
            if (!success ||
                IMAGESH[n]->LISH > 111000 ||
                !GetInt(buffer, &IMAGESH[n]->LI, 10, file))
            {
                success = false;
                break;
            }
        }  // end for NUMI
    }

    return success;
}

//: Read image information for extended headers.
//
// \param return false if error

bool vil_nitf_message_header_v20::read_extended_header_data(vil_stream* file)
{
    static vcl_string method_name = "vil_nitf_message_header_v20::read_extended_header_data: ";

    bool success = true;
    unsigned read_count = 0;

    vil_streampos start_pos = file->tell();

    if (debug_level > 1) {
      vcl_cout << "\n#### enter " << method_name
               << "file position = " << start_pos << vcl_endl;
    }

    char buffer[16];

    if (!GetInt(buffer, &UDHDL, 5, file))
    {
        success = false;
    }

    if (debug_level > 1) {
      vcl_cout << method_name << "UDHDL = " << UDHDL
               << " (user defined header data length).\n";
    }

    delete UDHD;
    UDHD = 0;
    if (success && (UDHDL > 0))
    {
        read_count += 5;

        UDHD = new char[UDHDL + 1];
        UDHD[UDHDL] = 0;
        if (file->read(UDHD, UDHDL) < UDHDL)
        {
            success = false;
        }
    }

    if (success)
    {
        read_count += UDHDL;

        int tmp = 0;
        if (GetInt(buffer, &tmp, 5, file))
        {
          read_count += 5;
          XHD->XHDL = tmp;

          if (debug_level > 1) {
            vcl_cout << method_name << "XHD->XHDL = " << XHD->XHDL
                     << "  read_count = " << read_count << vcl_endl;
          }

          if (XHD->Read(file) != STATUS_GOOD) {
              success = false;
          }
          else {
            read_count += XHD->XHDL;
          }
        }
        else {
            success = false;
        }
    }

    if (debug_level > 1) {
      vcl_cout << "\n#### exit " << method_name
               << "success = " << success
               << "  file position = " << file->tell() << vcl_endl;

      unsigned pos_diff = file->tell() - start_pos;
      vcl_cout << "    read_count = " << read_count
               << "    position diff = " << pos_diff << vcl_endl;
    }

    return success;
}

StatusCode vil_nitf_message_header_v20::Write(vil_stream* file)
{
    if (!file || !file->ok()) return STATUS_BAD;
    // WAS IOFile IN TARGETJR.  NO CORRESPONDING METHOD ON vil_stream ?
//    if (file->GetMode() == 'r')   return STATUS_BAD;

    bool error = false;
    bool done  = false;
    char buffer[13];

    int curpos = file->tell();
    while (!done)
    {
        buffer[0] = *ConvertClassification(MSCLAS);
        if (file->write(MHDR,   vcl_strlen(MHDR))    <  9 ||
            file->write(STYPE,  vcl_strlen(STYPE))   <  6 ||
            file->write(OSTAID, vcl_strlen(OSTAID))  < 10 ||
            file->write(MDT,    vcl_strlen(MDT))     < 14 ||
            file->write(MTITLE, vcl_strlen(MTITLE))  < 80 ||
            file->write(buffer, 1)                   <  1 || // MSCLAS
            file->write(MSCODE, vcl_strlen(MSCODE))  < 40 ||
            file->write(MSCTLH, vcl_strlen(MSCTLH))  < 40 ||
            file->write(MSREL,  vcl_strlen(MSREL))   < 40 ||
            file->write(MSCAUT, vcl_strlen(MSCAUT))  < 20 ||
            file->write(MSCTLN, vcl_strlen(MSCTLN))  < 20 ||
            file->write(MSDWNG, vcl_strlen(MSDWNG))  <  6 ||
            (!vcl_strcmp(MSDWNG, "999998") &&
             file->write(MSDEVT, vcl_strlen(MSDEVT))  < 40) ||
            !PutInt(buffer, MSCOP,   5, file)             ||
            !PutInt(buffer, MSCPYS,  5, file)             ||
            !((ENCRYP==NOTENCRYPTED && file->write((const char*)"0", 1)==1)  ||
              (ENCRYP==ENCRYPTED    && file->write((const char*)"1", 1)==1)) ||
            file->write(ONAME,  vcl_strlen(ONAME))   < 27 ||
            file->write(OPHONE, vcl_strlen(OPHONE))  < 18 ||
            !PutInt(buffer, ML,    12, file)              ||
            !PutInt(buffer, HL,     6, file)              ||
            !PutInt(buffer, NUMI,   3, file))
        {
            error = true;
            break;
        }

        int i;
        for (i=0; i<NUMI; i++)
        {
            if (!PutInt(buffer, IMAGESH[i]->LISH,  6, file) ||
                !PutInt(buffer, IMAGESH[i]->LI,   10, file))
            {
                error = true;
                break;
            }
        }
        if (error) break;

        if (!PutInt(buffer, NUMS, 3, file)) { error = true; break; }
        for (i=0; i<NUMS; i++)
        {
            if (!PutInt(buffer, SYMBOLSH[i]->LSSH,  4, file) ||
                !PutInt(buffer, SYMBOLSH[i]->LS,    6, file))
            {
                error = true;
                break;
            }
        }
        if (error) break;

        if (!PutInt(buffer, NUML, 3, file)) { error = true; break; }
        for (i=0; i<NUML; i++)
        {
            if (!PutInt(buffer, LABELSH[i]->LLSH,  4, file) ||
                !PutInt(buffer, LABELSH[i]->LL,    3, file))
            {
                error = true;
                break;
            }
        }
        if (error) break;

        if (!PutInt(buffer, NUMT, 3, file)) { error = true; break; }
        for (i=0; i<NUMT; i++)
        {
            if (!PutInt(buffer, TEXTSH[i]->LTSH,  4, file) ||
                !PutInt(buffer, TEXTSH[i]->LT,    5, file))
            {
                error = true;
                break;
            }
        }
        if (error) break;

        if (!PutInt(buffer, NUMDES, 3, file)) { error = true; break; }
        for (i=0; i<NUMDES; i++)
        {
            if (!PutInt(buffer, DESSH[i]->LDSH,  4, file) ||
                !PutInt(buffer, DESSH[i]->LD,    9, file))
            {
                error = true;
                break;
            }
        }
        if (error) break;

        if (!PutInt(buffer, NUMRES, 3, file)) { error = true; break; }
        for (i=0; i<NUMRES; i++)
        {
            if (!PutInt(buffer, RESSH[i]->LRSH,  4, file) ||
                !PutInt(buffer, RESSH[i]->LR,    7, file))
            {
                error = true;
                break;
            }
        }
        if (error) break;

        //UDHD will be nil of UDHDL=0
        if (!PutInt(buffer, UDHDL, 5, file) ||
            (UDHDL>0 && (int)vcl_strlen(UDHD) < UDHDL) ||
            (UDHDL>0 && file->write(UDHD, UDHDL) < UDHDL))
        {
            error = true;
            break;
        }

        if (!PutInt(buffer, XHD->XHDL, 5, file))
        {
            error = true;
            break;
        }
        if (XHD->Write(file) != STATUS_GOOD)
        {
            error = true;
            break;
        }

        done=true;
    }

    if (error) file->seek(curpos);

    return error ? STATUS_BAD : STATUS_GOOD;
}

//: Return length of header for this message file.

unsigned long vil_nitf_message_header_v20::GetHeaderLength() const
{
    unsigned long length = 388 +
                 NUMDES*13 + NUMI*16 + NUML*7 + NUMRES*11 + NUMS*10 + NUMT*9 +
                 UDHDL + XHD->GetHeaderLength();
    if (!vcl_strcmp(MSDWNG, "999998")) {
      length += 40;
    }

    // JUST FOR DEBUGGING.
    static vcl_string method_name = " vil_nitf_message_header_v20::GetHeaderLength: ";

    if (length != vil_nitf_message_header::GetHeaderLength()) {
      vcl_cout << method_name << "WARNING: calculated header length = " << length
               << vcl_endl
               << "    does not equal header length from file  = "
               << vil_nitf_message_header::GetHeaderLength() << vcl_endl;
    }
    return length;
}

//====================================================================
// Protected methods for vil_nitf_message_header_v20.
//====================================================================

void vil_nitf_message_header_v20::Init()
{
    setVersion(vil_nitf_version_v20::get_version_str());

    char initstr[80];                   // Will be filled with spaces.

    vcl_memset(initstr,' ',80);

    STRNCPY(MHDR,   HEADER_VERSION_STR,  9);
    STRNCPY(STYPE,  initstr,      6);
    STRNCPY(OSTAID, initstr,     10);

    // We must fool SCCS, otherwise it will try to
    // be smart and interpret the percents in the string.
    //
    vcl_time_t clock = vcl_time(NULL);
    vcl_tm *t_m = vcl_localtime(&clock);
    // char* format = "%d%H%M%SZ%h%y";

    STRNCPY(MDT, initstr, 14);
    vcl_strftime(MDT, 15, date_format, t_m);

    STRNCPY(MTITLE, initstr,     80);
    MSCLAS = DefaultClassification;
    STRNCPY(MSCODE, initstr,     40);
    STRNCPY(MSCTLH, initstr,     40);
    STRNCPY(MSREL,  initstr,     40);
    STRNCPY(MSCAUT, initstr,     20);
    STRNCPY(MSCTLN, initstr,     20);
    STRNCPY(MSDWNG, initstr,      6);
    STRNCPY(MSDEVT, initstr,     40);
    MSCOP = MSCPYS = 0;
    ENCRYP = NOTENCRYPTED;
    STRNCPY(ONAME,  initstr,     27);
    STRNCPY(OPHONE, initstr,     18);
    ML = HL = 0;

    int n;
    for (n=0; n < NUMDES; n++) delete DESSH[n];
    for (n=0; n < NUMI; n++)   delete IMAGESH[n];
    for (n=0; n < NUML; n++)   delete LABELSH[n];
    for (n=0; n < NUMRES; n++) delete RESSH[n];
    for (n=0; n < NUMS; n++)   delete SYMBOLSH[n];
    for (n=0; n < NUMT; n++)   delete TEXTSH[n];

    NUMDES = NUMI = NUML = NUMRES = NUMS = NUMT = 0;

    delete DESSH;
    delete IMAGESH;
    delete LABELSH;
    delete RESSH;
    delete SYMBOLSH;
    delete TEXTSH;

    DESSH = (DES_SH**)NULL;
    IMAGESH = 0;
    LABELSH = 0;
    RESSH = (RES_SH**) NULL;
    SYMBOLSH = 0;
    TEXTSH = 0;

    UDHDL = 0;
    delete UDHD;
    UDHD = 0;
    delete XHD;
    XHD = new vil_nitf_extended_header(0);
}

