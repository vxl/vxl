// Source

//================ GE Aerospace NITF support libraries =================
//
// $Revision: 1.1 $ 
// $Date: 2003/12/26 00:19:51 $
// $Author: mlaymon $
//
// Description:	This file defines the class for extended headers included in an NITF message.
//
//  Ported from TargetJr by M. Laymon.
//

#ifndef	_vil_nitf_extended_header_h
#define _vil_nitf_extended_header_h

#include "vil_nitf_header.h"

class vil_nitf_extended_header : public vil_nitf_header
{
    public:
         vil_nitf_extended_header (unsigned long s) : XHDL (data_length_) {XHDL = s ; XHD = 0 ;}
         vil_nitf_extended_header (const vil_nitf_extended_header& header);
         virtual ~vil_nitf_extended_header() {delete [] XHD ; }

         virtual int Read (vil_stream*);
         virtual int Write (vil_stream*);

	 //NITF test-specific
         virtual int AsciiRead (vil_stream*);
         virtual int AsciiWrite (vil_stream*);

         virtual unsigned long GetHeaderLength() const {return XHDL ;}

         unsigned long&  XHDL ;  /// Extended header data length.  Must be unsigned long 
	                         /// because reference is set to data_length_
	                         /// attribute in super-class vil_nitf_header
	                         /// and we want data_length_ to be unsigned long
	                         /// for large images.
         char* XHD;

         virtual vil_nitf_header* Copy();
         void    Copy (const vil_nitf_extended_header*);
         const   vil_nitf_extended_header& operator= (const vil_nitf_extended_header&) ;
};

typedef vil_nitf_extended_header vil_nitf_extended_subheader ;

inline const vil_nitf_extended_header&
vil_nitf_extended_header::operator= (const vil_nitf_extended_header& header)
{
    Copy(&header);
    return(*this);
}

#endif  // _vil_nitf_header_h
