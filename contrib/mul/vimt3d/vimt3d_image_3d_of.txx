#ifndef vimt3d_image_3d_of_txx_
#define vimt3d_image_3d_of_txx_

//:
// \file
// \brief Container for vil_image_view<T> + transform
// \author Tim Cootes


#include "vimt3d_image_3d_of.h"
#include <vcl_string.h>
#include <vcl_cassert.h>
#include <vsl/vsl_indent.h>
#include <vsl/vsl_vector_io.h>
#include <vimt3d/vimt3d_image_3d_of.h>
#include <vil3d/vil3d_print.h>
#include <vil3d/io/vil3d_io_image_view.h>


//=======================================================================
//: Perform deep copy of src into this image
template<class T>
void vimt3d_image_3d_of<T>::deep_copy(const vimt3d_image_3d_of& src)
{
  world2im_ = src.world2im_;
  image_.deep_copy(src.image_);
}


//=======================================================================
template<class T> vimt3d_image_3d_of<T>::~vimt3d_image_3d_of()
{
}


//=======================================================================
//: Shallow equality tester.
//  The parameter must be identical type to this.
template<class T>
bool vimt3d_image_3d_of<T>::equals(const vimt_image &im) const
{
  assert(dynamic_cast<const vimt3d_image_3d_of<T> *>(&im));
  return operator==(static_cast<const vimt3d_image_3d_of<T> &>(im));
}


//=======================================================================
//: Define valid data region (including transform).
//  Resizes and sets the transformation so that
//  worldToIm(x,y) is valid for all points in range
template<class T>
void vimt3d_image_3d_of<T>::set_valid_region(int i0, unsigned ni,
                                             int j0, unsigned nj,
                                             int k0, unsigned nk)
{
  image_.set_size(ni,nj,nk);
  world2im_.set_translation(-i0,-j0,-k0);
}


//=======================================================================
template<class T>
bool vimt3d_image_3d_of<T>::is_class(vcl_string const& s) const
{
  return s==vimt3d_image_3d_of<T>::is_a() || vimt3d_image_3d::is_class(s);
}


//=======================================================================
template<class T>
short vimt3d_image_3d_of<T>::version_no() const
{
  return 1;
}


//=======================================================================
template<class T>
vimt_image* vimt3d_image_3d_of<T>::clone() const
{
  return new vimt3d_image_3d_of(*this);
}

//=======================================================================
template<class T>
vimt_image* vimt3d_image_3d_of<T>::deep_clone() const
{
  vimt3d_image_3d_of<T>* new_im = new vimt3d_image_3d_of<T>();
  new_im->deep_copy(*this);
  return new_im;
}


//=======================================================================
template<class T>
void vimt3d_image_3d_of<T>::print_summary(vcl_ostream& os) const
{
  os<<vsl_indent() << "Transform: "<<world2im_
    <<vsl_indent() << image_<<vcl_endl;
}


//=======================================================================
//: print all data to os
template<class T>
void vimt3d_image_3d_of<T>::print_all(vcl_ostream& os) const
{
  os<<vsl_indent();
  vil3d_print_all(os,image_);
  os<<'\n'<<vsl_indent() << "Transform: "<<world2im_<<'\n';
}


//=======================================================================
template<class T>
void vimt3d_image_3d_of<T>::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,image_);
  vsl_b_write(bfs,world2im_);
}


//=======================================================================
template<class T>
void vimt3d_image_3d_of<T>::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
  case (1):
    vsl_b_read(bfs,image_);
    vsl_b_read(bfs,world2im_);
    break;
  default:
    vcl_cerr << "I/O ERROR: vimt3d_image_3d_of<T>::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< version << '\n';
    bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}


//=======================================================================
//: True if transforms, etc. are equal, and they share same image data.
//  This does not do a deep equality on image data. If the images point
//  to different image data objects that contain identical images, then
//  the result will still be false.
template<class T>
bool vimt3d_image_3d_of<T>::operator==(const vimt3d_image_3d_of<T> &other) const
{
  return image_ == other.image_ &&
      world2im_ == other.world2im_;
}


//=======================================================================
//: True if the transforms and the actual image data are identical.
// The image pointers need not be identical,
// provided that the underlying image data are the same.
// \relates vimt3d_image_3d_of<T>
// \relates vil3d_image_view
template<class T>
bool vimt3d_image_3d_deep_equality(const vimt3d_image_3d_of<T>& lhs,
                                   const vimt3d_image_3d_of<T>& rhs)
{
  // First check the transforms are the same
  if (!(lhs.world2im() == rhs.world2im()))
    return false;

  // Now check that the underlying image data are identical
  return vil3d_image_view_deep_equality(lhs.image(), rhs.image());
}


//=======================================================================


#define VIMT3D_IMAGE_3D_OF_INSTANTIATE(T) \
VCL_DEFINE_SPECIALIZATION vcl_string vimt3d_image_3d_of<T >::is_a() const \
{ return vcl_string("vimt3d_image_3d_of<" #T ">"); } \
template class vimt3d_image_3d_of<T >; \
template bool vimt3d_image_3d_deep_equality(const vimt3d_image_3d_of<T >& lhs, \
                                            const vimt3d_image_3d_of<T >& rhs)

#endif // vimt3d_image_3d_of_txx_
