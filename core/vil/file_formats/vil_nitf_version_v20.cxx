// This is core/vil/file_formats/vil_nitf_version_v20.cxx
#include "vil_nitf_version_v20.h"
//:
// \file
// \brief Implementation of version class for NITF version 2.0
//
// \date: 2003/12/26
// \author: mlaymon

#include <vcl_string.h>

#include "vil_nitf_macro_defs.h"

char const * const V20_VERSION_STR = "NITF 2.0";
#define DEBUG 0

//====================================================================
// Constructor(s) and destructor.  Note: the constructor is protected.
//====================================================================
vil_nitf_version_v20::vil_nitf_version_v20 (const char* name)
  : vil_nitf_version (name)
{
}

vil_nitf_version_v20::~vil_nitf_version_v20()
{
  // DO NOTHING HERE.  HAVE DESTRUCTOR JUST SO DESTRUCTOR
  // IN PARENT CLASS WILL BE CALLED.
}

//====================================================================
// Operator== can be used to determine if two NITF objects are the
// same version.
//====================================================================
bool vil_nitf_version_v20::operator== (vil_nitf_version& otherVersion)
{
    return vil_nitf_version::operator== (otherVersion);
}


/*====================================================================*/
/**
 *  Method to return a vil_nitf_version_v20 object as a vil_nitf_version
 *  object.  This always returns the same object.  It can be used to
 *  prevent a proliferation of  vil_nitf_version objects in the system.
 */
/*====================================================================*/
vil_nitf_version_v20 * vil_nitf_version_v20::GetVersion()
{
    static vcl_string method_name = "vil_nitf_version_v20::GetVersion: ";
    static vil_nitf_version_v20 * nitfvXX = 0;
    if (nitfvXX == 0) {
// Since this is a singleton, I don't think we need to delete it.
// MAL 17oct2003
        nitfvXX = new vil_nitf_version_v20 (V20_VERSION_STR);
#if DEBUG
        vcl_cout << method_name << "create version object = "
                 << nitfvXX << " <" << nitfvXX->GetTitle() << ">" << vcl_endl ;
#endif
    }
    return nitfvXX ;
}

vil_nitf_image_subheader_band * vil_nitf_version_v20::newImageHeaderBand (
    vil_nitf_image_subheader_band * ish_band)
{
    static vcl_string method_name = "vil_nitf_version_v20::newImageHeaderBand: ";

    vil_nitf_image_subheader_band * rval = new vil_nitf_image_subheader_band;

    rval->ITYPE = rval->IFC = rval->IMFLT = 0;
    STRCPY (rval->ITYPE, "  /      "); // 2 char + / + 6 characters + NULL.
    STRCPY (rval->IFC,   "N");         // 1 character+NULL  (not used).
    STRCPY (rval->IMFLT, "   ");       // 3 characters+NULL (not used).
    rval->NLUTS = rval->NELUT = 0;
    rval->LUTD  = 0;

    if (ish_band != NULL) {
        if (ish_band->ITYPE) FilledCopy (rval->ITYPE, ish_band->ITYPE);
        if (ish_band->IFC)   FilledCopy (rval->IFC,   ish_band->IFC);
        if (ish_band->IMFLT) FilledCopy (rval->IMFLT, ish_band->IMFLT);
        if (ish_band->NLUTS > 0 && ish_band->NELUT > 0) {
            rval->NLUTS = ish_band->NLUTS;
            rval->NELUT = ish_band->NELUT;
            rval->LUTD  = new unsigned char*[rval->NLUTS];

            unsigned int j;
            for (j = 0; j < rval->NLUTS; j++) {
                rval->LUTD[j] = new unsigned char[rval->NELUT];
                vcl_memcpy(rval->LUTD[j], ish_band->LUTD[j], rval->NELUT);
            }
        }
    }

    return rval;
}
