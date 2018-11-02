// This is brl/bseg/bbgm/bbgm_feature_image.hxx
#ifndef bbgm_feature_image_hxx_
#define bbgm_feature_image_hxx_
//:
// \file

#include <iostream>
#include <typeinfo>
#include "bbgm_feature_image.h"
#include <vbl/io/vbl_io_array_2d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//===========================================================================
// Binary I/O Methods


//: Return a string name
// \note this is probably not portable
template<class f_type_>
std::string
bbgm_feature_image<f_type_>::is_a() const
{
  return "bbgm_feature_image<"+std::string(typeid(f_type_).name())+">";
}


template<class f_type_>
bbgm_feature_image_base*
bbgm_feature_image<f_type_>::clone() const
{
  return new bbgm_feature_image<f_type_>(*this);
}


//: Return IO version number;
template<class f_type_>
short
bbgm_feature_image<f_type_>::version() const
{
  return 1;
}


//: Binary save self to stream.
template<class f_type_>
void
bbgm_feature_image<f_type_>::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  vsl_b_write(os, data_);
}


//: Binary load self from stream.
template<class f_type_>
void
bbgm_feature_image<f_type_>::b_read(vsl_b_istream &is)
{
  if (!is)
    return;
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
  {
    case 1:
      vsl_b_read(is, data_);
      break;
    default:
      std::cerr << "bbgm_image: unknown I/O version " << ver << '\n';
  }
}


#include <vbl/io/vbl_io_array_2d.hxx>
#include <vsl/vsl_vector_io.hxx>
#define BBGM_FEATURE_IMAGE_INSTANTIATE(T) \
template class bbgm_feature_image<T >; \
VBL_IO_ARRAY_2D_INSTANTIATE(T)


#endif // bbgm_feature_image_hxx_
