// This is core/vil/file_formats/vil_nitf_version.cxx
#include "vil_nitf_version.h"
//:
// \file
// \brief Implementation of for class vil_nitf_version.
//
// \date: 2003/12/26
// \author: mlaymon

#include <vcl_cstring.h>

#include "vil_nitf_macro_defs.h"

//====================================================================
// Constructor(s) and destructor.  Note: the constructor is protected.
//====================================================================
vil_nitf_version::vil_nitf_version (const char* name)
{
    uvers = 0;
    cvers = 0;
    STRCPY (cvers, name ? name : "");
}

vil_nitf_version::~vil_nitf_version()
{
    delete cvers;
}

//====================================================================
// Operator== can be used to determine if two NITF objects are the
// same version.
//====================================================================
bool vil_nitf_version::operator== (const vil_nitf_version& otherVersion)
{
    bool rval = false;

    if (cvers != 0 && otherVersion.cvers != 0) {
      if ((cvers == otherVersion.cvers) ||
          vcl_strcmp(cvers, otherVersion.cvers) == 0) {
        rval = true;
      }
    }
    return rval;
}

