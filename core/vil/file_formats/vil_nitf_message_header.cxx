// This is core/vil/file_formats/vil_nitf_message_header.cxx
#include "vil_nitf_message_header.h"
//:
// \file

#include <vcl_iostream.h>
#include "vil_nitf_util.h"

//====================================================================
// Implementation for the NITF Message header.
//====================================================================

vil_nitf_message_header::vil_nitf_message_header()
    : OSTAID (ID), MDT (DT), MTITLE (TITLE), MSCLAS (CLAS),
      MSCODE (CODE), MSCTLH (CTLH), MSREL (REL),
      MSCAUT (CAUT), MSCTLN (CTLN), MSDWNG (DWNG),
      MSDEVT (DEVT)
{
    MHDR   = (char*) NULL;
    STRCPY (MHDR,   "");

    CLEVEL = 0;

    STYPE  = 0;
    STRCPY(STYPE,  "");

    MSCOP = MSCPYS = 0;

    ONAME = OPHONE = 0;

    STRCPY (ONAME,  "");
    STRCPY (OPHONE, "");

    ML = HL = 0;

    NUMDES = NUMI = NUML = NUMRES = NUMS = NUMT = 0;

    DESSH    = (DES_SH**) NULL;
    IMAGESH  = 0;
    LABELSH  = 0;
    RESSH    = (RES_SH**) NULL;
    SYMBOLSH = 0;
    TEXTSH   = 0;

    UDHDL = 0;
    UDHD  = 0;
    XHD   = new vil_nitf_extended_header(0);
}

//====================================================================
//: Copy constructor for vil_nitf_message_header.
//====================================================================
vil_nitf_message_header::vil_nitf_message_header(const vil_nitf_message_header& header)
    : vil_nitf_header(header),OSTAID(ID), MDT(DT), MTITLE(TITLE), MSCLAS(CLAS),
      MSCODE(CODE), MSCTLH(CTLH), MSREL(REL), MSCAUT(CAUT), MSCTLN(CTLN),
      MSDWNG(DWNG), MSDEVT(DEVT)
{
    MHDR   = new_strdup (header.MHDR);
    CLEVEL = 0;
    STYPE  = new_strdup (header.STYPE);
    MSCOP  = header.MSCOP;
    MSCPYS = header.MSCPYS;
    ONAME  = new_strdup (header.ONAME);
    OPHONE = new_strdup (header.OPHONE);

    ML = HL = 0;

    NUMDES = NUMI = NUML = NUMRES = NUMS = NUMT = 0;

    DESSH    = (DES_SH**) NULL;
    IMAGESH  = 0;
    LABELSH  = 0;
    RESSH    = (RES_SH**)NULL;
    SYMBOLSH = 0;
    TEXTSH   = 0;

    UDHDL = header.UDHDL;
    UDHD  = new char[UDHDL+1];

    UDHD[UDHDL] = (char)NULL;
    vcl_memcpy(UDHD, header.UDHD, UDHDL);

    XHD   = new vil_nitf_extended_header(*header.XHD);
}

vil_nitf_message_header::~vil_nitf_message_header()
{
    delete [] MHDR;
    delete [] STYPE;
    delete [] ONAME;
    delete [] OPHONE;

    int n;
    for (n = 0; n < NUMDES; n++) delete [] DESSH[n];
    for (n = 0; n < NUMI; n++)   delete  IMAGESH[n];
    for (n = 0; n < NUML; n++)   delete [] LABELSH[n];
    for (n = 0; n < NUMRES; n++) delete [] RESSH[n];
    for (n = 0; n < NUMS; n++)   delete [] SYMBOLSH[n];
    for (n = 0; n < NUMT; n++)   delete  TEXTSH[n];

    delete [] DESSH;
    delete [] IMAGESH;
    delete [] LABELSH;
    delete [] RESSH;
    delete [] SYMBOLSH;
    delete [] TEXTSH;

    delete [] UDHD;
    delete XHD;
}

//====================================================================
//: Method to return a copy of the vil_nitf_header.
//  The copy returned *must* be deleted by the caller.
//====================================================================
vil_nitf_header* vil_nitf_message_header::Copy()
{
    vil_nitf_header* rval = new vil_nitf_message_header (*this);
    return rval;
}

void vil_nitf_message_header::Copy (const vil_nitf_message_header* h)
{
    vil_nitf_header::Copy (h);

    FilledCopy (MHDR,h->MHDR);
    FilledCopy (STYPE,h->STYPE);;

    MSCOP = h->MSCOP;
    MSCPYS = h->MSCPYS;

    FilledCopy (ONAME,h->ONAME);
    FilledCopy (OPHONE,h->OPHONE);
}

// FIGURE OUT HOW TO PASS vcl_out.
//void vil_nitf_message_header::display_header_info (vcl_ostream out)
void vil_nitf_message_header::display_header_info (vcl_string caller) const
{
    static vcl_string method_name = "vil_nitf_message_header::display_header_info: ";

    vcl_cout << method_name;
    if (method_name.length() > 0) {
      vcl_cout << " from " << caller;
    }
    vcl_cout << vcl_endl;

    vcl_cout << "message version from file = " << this->MHDR << vcl_endl
             << "message header length  = " << this->GetHeaderLength() << vcl_endl
             << "message length = " << this->ML << vcl_endl
             << "station ID = " << this->OSTAID << vcl_endl
             << "message date and time = " << this->MDT << vcl_endl

             << "number of image segments = " << this->NUMI << vcl_endl;
    if (this->NUMI > 0) {
      for (int i = 0; i < this->NUMI; i++) {
        vcl_cout << "\timage[" << i << "] header length = "
                 << this->IMAGESH[i]->LISH
                 << "  image length = " << this->IMAGESH[i]->LI
                 << vcl_endl;
      }
    }
    vcl_cout << "datalength  = " << this->get_data_length() << vcl_endl;

    vcl_cout << "number of symbol (graphic) segments = " << this->NUMS << vcl_endl
             << "number of label segments = " << this->NUML << vcl_endl
             << "number of text segments = " << this->NUMT << vcl_endl
             << "number of DES segments = " << this->NUMDES << vcl_endl
             << "number of RES segments = " << this->NUMRES << vcl_endl;

}  // end method display_header_info

unsigned long vil_nitf_message_header::get_image_header_length (int image_num) const
{
    unsigned long header_length = 0;

    if (this->NUMI > 0) {
      if (image_num < this->NUMI
          && (this->IMAGESH[image_num] != 0)) {
          header_length = this->IMAGESH[image_num]->LISH ;
      }
    }
    return header_length ;
}

unsigned long vil_nitf_message_header::get_image_data_length (int image_num) const
{
    unsigned long data_length = 0;

    if (this->NUMI > 0) {
      if (image_num < this->NUMI
          && (this->IMAGESH[image_num] != 0)) {
          data_length = this->IMAGESH[image_num]->LI;
      }
    }
    return data_length;
}
