//:
// \file
// \brief Smart reader for reading in features.
// \author Gehua yang
// \date Aug 04 2004

#include "rgrl_feature_reader.h"
#include <rgrl/rgrl_feature_point.h>
#include <rgrl/rgrl_feature_landmark.h>
#include <rgrl/rgrl_feature_face_pt.h>
#include <rgrl/rgrl_feature_trace_pt.h>

#include <rgrl/rgrl_util.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_compiler.h>

#undef READ_THIS_FEATURE
#define READ_THIS_FEATURE(tag, fea) \
  if ( tag_str.find(tag) == 0 ){ \
    fea* fea_ptr = new fea(); \
    fea_ptr->read(is); \
    return fea_ptr; \
  }


//: Read a feature from input stream
//  The type of feature depends on the content of the input stream.
//  NULL smart ptr is returned if reading fails.
//  Please check the validity of the return smart ptr
rgrl_feature_sptr
rgrl_feature_reader( vcl_istream& is )
{
  vcl_string tag_str;
  vcl_streampos pos;

  // skip any empty lines
  rgrl_util_skip_empty_lines( is );
  // store current reading position
  pos = is.tellg();
  vcl_getline( is, tag_str );

  // back to the beginning of the tag line
  is.seekg( pos );

  // use the following macro to read in each specific feature.
  // The first argument is a string to identify the feature.
  // The second is the corresponding feature class
  //
  READ_THIS_FEATURE("POINT",    rgrl_feature_point)
  READ_THIS_FEATURE("LANDMARK", rgrl_feature_landmark)
  READ_THIS_FEATURE("FACE",     rgrl_feature_face_pt)
  READ_THIS_FEATURE("TRACE",    rgrl_feature_trace_pt)
  // default, should never reach here
  vcl_cout<< "WARNING: " << RGRL_HERE << " ( line "
          << __LINE__ << " )\n"
          << "       " << "Tag " << tag_str
          << " cannot match with any existing features.\n"
          << "         Try to open istream in BINARY mode!" << vcl_endl;
  return 0;
}

//: stream operator for reading feature
vcl_istream&
operator>> (vcl_istream& is, rgrl_feature_sptr& fea_sptr)
{
  fea_sptr = rgrl_feature_reader( is );
  return is;
}

