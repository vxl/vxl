// This is core/vil/file_formats/vil_nitf_message_header.h
#ifndef vil_nitf_message_header_h_
#define vil_nitf_message_header_h_
//:
// \file
// \date: 2003/12/26
// \author: mlaymon
//
// \brief This file defines the message header for an NITF message.
//  Extracted from NITFHeader.h
//
// NOTE: In the official NITF specification, this information
//     is referred to as the NITF file header, because its fields
//     referred to the structure of the entire file.
//
//  Ported from TargetJr by M. Laymon.

#include "vil_nitf_header.h"
#include "vil_nitf_extended_header.h"

#include <vcl_string.h>

/// Struct containing header and data lengths for each image segment
struct ImageSH
{
  int LISH;   //<: Image subheader length - 6 digits - range = 000439 - 999999
  int LI;     //<: Image length - 10 digits - range = 0000000001 - 9999999999
};

/// Struct containing header and data lengths for each symbol (== grapics) segment
struct SymbolSH
{
  int LSSH;   //<: Symbol subheader length - 4 digits - range = 0258 - 9999
  int LS;     //<: Symbol data length - 6 digits - range = 000001 - 999999
};

struct DES_SH     { int LDSH; int LD; };
struct LabelSH    { int LLSH; int LL; };
struct RES_SH     { int LRSH; int LR; };
struct TextSH     { int LTSH; int LT; };

class vil_nitf_message_header : public vil_nitf_header
{
 public:
  vil_nitf_message_header();
  vil_nitf_message_header (const vil_nitf_message_header& header);
  virtual ~vil_nitf_message_header();

  // A * indicates that the item is not used or superseded in NITF2.1
  char*  MHDR;                   // Message Type & Version
  int    CLEVEL;                 // Compliance Level
  char*  STYPE;                  // System Type
  char*& OSTAID;                 // Originating Station ID
  char*& MDT;  // Message Date & Time stored in format DDhhmmssTMMMYY where T = time zone.
  char*& MTITLE;                 // Message Title
  NITFClass& MSCLAS;             // Message Security Classification
  char*& MSCODE;                 // Message Codewords
  char*& MSCTLH;                 // Message Control and Handling
  char*& MSREL;                  // Message Releasing Instructions
  char*& MSCAUT;                 // Message Classification Authority
  char*& MSCTLN;                 // Message Security Control Number
  char*& MSDWNG;                 // Message Security Downgrade *
  char*& MSDEVT;                 // Message Downgrading Event *
  int    MSCOP;                  // Message Copy Number
  int    MSCPYS;                 // Message Number of Copies

  char*  ONAME;                  // Originator's Name
  char*  OPHONE;                 // Originator's Phone Number
  int    ML;                     // Message Length
  int    HL;                     // NITF Header Length

  // attributes NUMA, AUDIOSH, NUMF, NPISH removed because they do not
  // apply to version 2.0 and above

  int    NUMDES;                 // Number of Data Extension Segments (DES)
  DES_SH** DESSH;                // Array of sizes of DES Sub-Headers
  int    NUMI;                   // Number of image segments
  ImageSH** IMAGESH;             // Array of ImageSH structs for each image segment.
  int    NUML;                   // Number of label segments
  LabelSH** LABELSH;             // Array of sizes of label subheaders for each segment.
  int    NUMRES;                 // Number of Reserved Extension Segments (RES)
  RES_SH** RESSH;                // Array of sizes of RES subheaders for each segment.
  int    NUMS;                   // Number of symbol segments.
                                 // NOTE: reference to NUMS in NITF spec talks about graphic segments, not symbol segments.
  SymbolSH** SYMBOLSH;           // Array of sizes of symbol subheaders for each segment.
  int    NUMT;                   // Number of Text Sub-Headers
  TextSH** TEXTSH;               // Array of sizes of Text subheaders for each segment.

  int    UDHDL;                  // User Defined Header Data Length
  char*  UDHD;                   // User Defined Header Data
  vil_nitf_extended_header* XHD; // Extended Header Data

  virtual vil_nitf_header* Copy();
  void    Copy (const vil_nitf_message_header*);
  const   vil_nitf_message_header& operator=(const vil_nitf_message_header&);

  virtual unsigned long GetHeaderLength() const {return HL;}  // Return length of message header.
  virtual unsigned long get_data_length() const {return ML;}  // Return length of message.
                                                              // (== File length - message header length ?  MAL 23oct2003)
  unsigned long get_image_header_length (int image_num = 0) const; // get length of header for Nth image. Default is for 1st image.
  unsigned long get_image_data_length (int image_num = 0) const; // get data length for Nth image. Default is for 1st image.

  // FIGURE OUT HOW TO PASS vcl_out.
  //void display_header_info (vcl_ostream out) const;
  void display_header_info (vcl_string method_name) const;
};

inline const vil_nitf_message_header& vil_nitf_message_header::operator= (
    const vil_nitf_message_header& header)
{
  Copy (&header);
  return *this;
}

typedef vil_smart_ptr<vil_nitf_message_header> vil_nitf_message_header_sptr;

#endif  // end vil_nitf_message_header_h_
