// This is brcv/seg/bbgm/bbgm_image_of.txx
#ifndef bbgm_image_of_txx_
#define bbgm_image_of_txx_

//:
// \file

#include "bbgm_image_of.h"
#include <vbl/io/vbl_io_array_2d.h>


//===========================================================================
// Binary I/O Methods


//: Return a string name
// \note this is probably not portable
template<class _dist>
vcl_string
bbgm_image_of<_dist>::is_a() const
{
  return "bbgm_image_of<"+vcl_string(typeid(_dist).name())+">";
}


template<class _dist>
bbgm_image_base*
bbgm_image_of<_dist>::clone() const
{
  return new bbgm_image_of<_dist>(*this);
}


//: Return IO version number;
template<class _dist>
short
bbgm_image_of<_dist>::version() const
{
  return 1;
}


//: Binary save self to stream.
template<class _dist>
void
bbgm_image_of<_dist>::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  vsl_b_write(os, data_);
}


//: Binary load self from stream.
template<class _dist>
void
bbgm_image_of<_dist>::b_read(vsl_b_istream &is)
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
      vcl_cerr << "bbgm_image: unknown I/O version " << ver << '\n';
  }
}


#include <vbl/io/vbl_io_array_2d.txx>

#define BBGM_IMAGE_INSTANTIATE(T) \
template class bbgm_image_of<T >; \
VBL_IO_ARRAY_2D_INSTANTIATE(T)


#endif // bbgm_image_of_txx_
