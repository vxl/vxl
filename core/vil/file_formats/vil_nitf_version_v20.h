// This is core/vil/file_formats/vil_nitf_version_v20.h
#ifndef vil_nitf_version_v20_h_
#define vil_nitf_version_v20_h_
//:
// \file
// \brief Minimal implementation of NITFVersion
//  until decision made on whether full port should be done.  MAL 30sep2003
//
// \date: 2003/12/26
// \author: mlaymon

#include "vil_nitf_version.h"

class vil_nitf_version_v20 : public vil_nitf_version
{
 public:
  ~vil_nitf_version_v20();

  bool operator==(vil_nitf_version&);

  static vil_nitf_version_v20 * GetVersion();

  virtual vil_nitf_image_subheader_band * newImageHeaderBand(
             vil_nitf_image_subheader_band * ish_band = 0);

 protected:

  vil_nitf_version_v20 (const char* name);
};

#endif  // vil_nitf_version_v20_h_
