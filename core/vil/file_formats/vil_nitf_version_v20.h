// Source

#ifndef _vil_nitf_version_v20_h
#define _vil_nitf_version_v20_h

//
// Description:	Minimal implementation of NITFVersion until decision made 
//     on whether full port should be done.  MAL 30sep2003
//
// $Revision: 1.1 $ 
// $Date: 2003/12/26 00:22:50 $
// $Author: mlaymon $
//

#include "vil_nitf_version.h"

class vil_nitf_version_v20 : public vil_nitf_version
{
    public:
         ~vil_nitf_version_v20() ;

         bool operator== (vil_nitf_version&) ;

         static vil_nitf_version_v20 * GetVersion() ;

         virtual vil_nitf_image_subheader_band * newImageHeaderBand (
             vil_nitf_image_subheader_band * ish_band = 0) ;

    protected:

         vil_nitf_version_v20 (const char* name) ;
} ;

#endif  // _vil_nitf_version_v20_h
