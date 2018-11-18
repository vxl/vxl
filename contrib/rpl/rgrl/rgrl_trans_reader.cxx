//:
// \file
// \brief Smart reader function for reading in ASCII transformation file.
// \author Gehua yang, Chia-ling Tsai
// \date Aug 04 2004

#include <iostream>
#include <fstream>
#include <string>
#include "rgrl_trans_reader.h"
#include <rgrl/rgrl_trans_translation.h>
#include <rgrl/rgrl_trans_similarity.h>
#include <rgrl/rgrl_trans_affine.h>
#include <rgrl/rgrl_trans_reduced_quad.h>
#include <rgrl/rgrl_trans_quadratic.h>
#include <rgrl/rgrl_trans_homography2d.h>
#include <rgrl/rgrl_trans_rad_dis_homo2d.h>
#include <rgrl/rgrl_trans_rigid.h>
#include <rgrl/rgrl_trans_couple.h>
#include <rgrl/rgrl_trans_spline.h>
#include <rgrl/rgrl_util.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

// initialize the static variables
std::vector< rgrl_transformation_sptr >  rgrl_trans_reader::xform_candidates_;

rgrl_transformation_sptr
rgrl_trans_reader::
read( char const* fn )
{
  std::ifstream ifs( fn, std::ios::in|std::ios::binary );
  if( ifs.good() )
    return read( ifs );
  else
    return nullptr;
}

#undef READ_THIS_TRANSFORMATION
#define READ_THIS_TRANSFORMATION(tag, trans) \
  if ( tag_str.find(tag) == 0 ){    \
    trans* trans_ptr = new trans(); \
    if( trans_ptr->read(is) )       \
      return trans_ptr;             \
    else                            \
      return 0;                     \
  }

//: Read a transformation from input stream
//  The type of transformation depends on the content of the input stream.
//  NULL smart ptr is returned if reading fails.
//  Please check the validity of the return smart ptr
rgrl_transformation_sptr
rgrl_trans_reader::
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

  typedef std::vector< rgrl_transformation_sptr >::const_iterator iter;
  for( iter i=xform_candidates_.begin(); i!=xform_candidates_.end(); ++i ) {

    // make a copy of the transformation
    rgrl_transformation_sptr candidate = (*i)->clone();
    if( candidate->read( is) )
      return candidate;

    // else reset the pos
    is.seekg( pos );
  }

  // 3. built-in classes are handled in a different way
  // use the following macro to read in each specific transformation.
  // The first argument is a string to identify the transformation.
  // The second is the corresponding transformation class
  //
  // NOTE: due to the use of find function, the order of the following is important
  //       If one tag is a subset of the other tag, then it must be after the other one.
  //       For instance, AFFINE_NEW must be in front of AFFINE. Otherwise,
  //       it is read in as AFFINE
  //
  READ_THIS_TRANSFORMATION("TRANSLATION", rgrl_trans_translation)
  READ_THIS_TRANSFORMATION("SIMILARITY", rgrl_trans_similarity)
  READ_THIS_TRANSFORMATION("AFFINE", rgrl_trans_affine)
  READ_THIS_TRANSFORMATION("REDUCED_QUADRATIC", rgrl_trans_reduced_quad)
  READ_THIS_TRANSFORMATION("RIGID", rgrl_trans_rigid)
  READ_THIS_TRANSFORMATION("QUADRATIC", rgrl_trans_quadratic)
  READ_THIS_TRANSFORMATION("BSPLINE", rgrl_trans_spline)
  READ_THIS_TRANSFORMATION("HOMOGRAPHY2D_WITH_RADIAL_DISTORTION", rgrl_trans_rad_dis_homo2d)
  READ_THIS_TRANSFORMATION("HOMOGRAPHY2D", rgrl_trans_homography2d)
  READ_THIS_TRANSFORMATION("COUPLE_TRANS", rgrl_trans_couple)

  // default, should never reach here
  std::cout<< "WARNING: " << RGRL_HERE << " ( line "
          << __LINE__ << " )\n"
          << "       " << "Tag [" << tag_str
          << "] cannot match with any existing transformations.\n"
          << "         Try to open istream in BINARY mode!" << std::endl;
  return nullptr;
}

void
rgrl_trans_reader::
add_xform( const rgrl_transformation_sptr& xform )
{
  xform_candidates_.push_back( xform );
}

//: stream operator for reading transformation
std::istream&
operator>> (std::istream& is, rgrl_transformation_sptr& trans_sptr)
{
  trans_sptr = rgrl_trans_reader::read( is );
  return is;
}
