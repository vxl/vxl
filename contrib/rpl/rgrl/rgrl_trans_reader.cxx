//:
// \file
// \brief Smart reader function for reading in ASCII transformation file.
// \author Gehua yang, Chia-ling Tsai
// \date Aug 04 2004

#include "rgrl_trans_reader.h"
#include <rgrl/rgrl_trans_translation.h>
#include <rgrl/rgrl_trans_similarity.h>
#include <rgrl/rgrl_trans_affine.h>
#include <rgrl/rgrl_trans_reduced_quad.h>
#include <rgrl/rgrl_trans_quadratic.h>
#include <rgrl/rgrl_trans_homography2d.h>
#include <rgrl/rgrl_trans_rad_dis_homo2d.h>
#include <rgrl/rgrl_trans_rigid.h>
#include <rgrl/rgrl_trans_spline.h>
#include <rgrl/rgrl_util.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_compiler.h>

#undef READ_THIS_TRANSFORMATION
#define READ_THIS_TRANSFORMATION(tag, trans) \
  if ( tag_str.find(tag) == 0 ){ \
    trans* trans_ptr = new trans(); \
    trans_ptr->read(is); \
    return trans_ptr; \
  }


//: Read a transformation from input stream
//  The type of transformation depends on the content of the input stream.
//  NULL smart ptr is returned if reading fails.
//  Please check the validity of the return smart ptr
rgrl_transformation_sptr
rgrl_trans_reader( vcl_istream& is )
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

  // default, should never reach here
  vcl_cout<< "WARNING: " << RGRL_HERE << " ( line "
          << __LINE__ << " )\n"
          << "       " << "Tag " << tag_str
          << " cannot match with any existing transformations.\n"
          << "         Try to open istream in BINARY mode!" << vcl_endl;
  return 0;
}

//: stream operator for reading transformation
vcl_istream&
operator>> (vcl_istream& is, rgrl_transformation_sptr& trans_sptr)
{
  trans_sptr = rgrl_trans_reader( is );
  return is;
}

