//:
// \file
// \brief Smart reader for reading in features.
// \author Gehua yang
// \date Aug 04 2004

#include <iostream>
#include <string>
#include "rgrl_feature_reader.h"
#include <rgrl/rgrl_feature_point.h>
#include <rgrl/rgrl_feature_landmark.h>
#include <rgrl/rgrl_feature_face_pt.h>
#include <rgrl/rgrl_feature_trace_pt.h>

#include <rgrl/rgrl_util.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

// initialize the static variables
std::vector< rgrl_feature_sptr >  rgrl_feature_reader::feature_candidates_;


#undef READ_THIS_FEATURE
#define READ_THIS_FEATURE(tag, fea) \
  if ( tag_str.find(tag) == 0 ){ \
    fea* fea_ptr = new fea(); \
    fea_ptr->read(is); \
    return fea_ptr; \
  }

void rgrl_feature_reader::add_feature( const rgrl_feature_sptr& feat )
{
  feature_candidates_.push_back(feat);
}

rgrl_feature_sptr
rgrl_feature_reader::
read( std::istream& is )
{
  std::string tag_str;
  std::streampos pos;

  // 1. get to the tag line and save the position
  //
  // skip any empty lines
  rgrl_util_skip_empty_lines( is );
  // store current reading position
  pos = is.tellg();
  std::getline( is, tag_str );

  // 2. try classes stored in the vector
  //
  // back to the beginning of the tag line
  is.seekg( pos );

  typedef std::vector< rgrl_feature_sptr >::const_iterator iter;
  for( iter i=feature_candidates_.begin(); i!=feature_candidates_.end(); ++i ) {

    // make a copy of the transformation
    rgrl_feature_sptr candidate = (*i)->clone();
    if( candidate->read( is ) )
      return candidate;

    // else reset the pos
    is.seekg( pos );
  }

  // 3. built-in classes are handled in a different way
  // use the following macro to read in each specific feature.
  // The first argument is a string to identify the feature.
  // The second is the corresponding feature class
  //
  READ_THIS_FEATURE("POINT",    rgrl_feature_point)
  READ_THIS_FEATURE("LANDMARK", rgrl_feature_landmark)
  READ_THIS_FEATURE("FACE",     rgrl_feature_face_pt)
  READ_THIS_FEATURE("TRACE",    rgrl_feature_trace_pt)
  // default, should never reach here
  std::cout<< "WARNING: " << RGRL_HERE << " ( line "
      << __LINE__ << " )\n"
      << "       " << "Tag " << tag_str
      << " cannot match with any existing features.\n"
      << "         Try to open istream in BINARY mode!" << std::endl;
  return nullptr;
}

//: Read a feature from input stream
//  The type of feature depends on the content of the input stream.
//  NULL smart ptr is returned if reading fails.
//  Please check the validity of the return smart ptr
rgrl_feature_sptr
rgrl_feature_reader( std::istream& is )
{
  return rgrl_feature_reader::read(is);
}

//: stream operator for reading feature
std::istream&
operator>> (std::istream& is, rgrl_feature_sptr& fea_sptr)
{
  fea_sptr = rgrl_feature_reader( is );
  return is;
}
