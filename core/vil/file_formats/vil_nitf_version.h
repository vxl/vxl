// This is core/vil/file_formats/vil_nitf_version.h
#ifndef vil_nitf_version_h_
#define vil_nitf_version_h_
//:
// \file
// \brief Minimal implementation of NITFVersion from TargetJr
//
// \date: 2003/12/26
// \author: mlaymon

#include "vil_nitf_image_subheader.h"

class vil_nitf_version
{
 public:
  virtual ~vil_nitf_version();

  const char*  GetTitle();
  unsigned int GetVersion();

  bool operator== (const vil_nitf_version&);

  virtual vil_nitf_image_subheader_band * newImageHeaderBand (
      vil_nitf_image_subheader_band * ish_band = 0) {return 0; }

 protected:

  vil_nitf_version (const char* name);

 private:

#if 0  // COMMENTED OUT
  struct nitfVersion
  {
      char*        name;
      unsigned int id;
      vil_nitf_version* version;
  };
#endif
  char * cvers;
  unsigned int uvers;
};

inline const char * vil_nitf_version::GetTitle() {return cvers; }
inline unsigned int vil_nitf_version::GetVersion() {return uvers; }

#endif  // vil_nitf_version_h_
