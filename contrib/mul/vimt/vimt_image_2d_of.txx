#ifndef vimt_image_2d_of_txx_
#define vimt_image_2d_of_txx_
//:
//  \file
//  \brief Container for vil2_image_view<T> + transform
//  \author Tim Cootes

#include <vcl_string.h>
#include <vcl_cassert.h>
#include <vsl/vsl_indent.h>
#include <vimt/vimt_image_2d_of.h>
#include <vil2/vil2_print.h>
#include <vil2/vil2_crop.h>
#include <vsl/vsl_vector_io.h>
#include <vil2/io/vil2_io_image_view.h>

//=======================================================================

template<class T>
vimt_image_2d_of<T>::vimt_image_2d_of()
{
}

template<class T>
vimt_image_2d_of<T>::vimt_image_2d_of(unsigned nx, unsigned ny, unsigned nplanes)
: image_(nx,ny,nplanes)
{
}

//: Perform deep copy of src into this image
template<class T>
void vimt_image_2d_of<T>::deep_copy(const vimt_image_2d_of& src)
{
  world2im_     = src.world2im_;
  image_.deep_copy(src.image_);
}

template<class T> vimt_image_2d_of<T>::~vimt_image_2d_of()
{
}


//: Define valid data region (including transform).
//  Resizes and sets the tranformation so that
//  worldToIm(x,y) is valid for all points in range
template<class T>
void vimt_image_2d_of<T>::set_valid_region(int x0, unsigned nx, int y0, unsigned ny)
{
  image_.resize(nx,ny);
  world2im_.set_translation(-x0,-y0);
}


//: Arrange that this is window on given image.
//  I.e. plane(i) points to im.plane(i) + offset
//  The world2im transform is set to match
//  so this appears identical to im when addressed
//  in world co-ords.
template<class T>
void vimt_image_2d_of<T>::set_to_window(const vimt_image_2d_of& im,
                                        unsigned x0, unsigned nx, unsigned y0, unsigned ny)
{
  assert(this!=&im);
  image_.set_to_window(im.image(),x0,nx,y0,ny);

  vimt_transform_2d trans;
  trans.set_translation(-double(x0),-double(y0));
  world2im_ = trans * im.world2im();
}

//: Create windowed view of given image
//  The parameters should be in image co-ords.
//  The world2im transform is set to match
//  so this appears identical to im when addressed
//  in world co-ords.
template<class T>
vimt_image_2d_of<T> vimt_image_2d_of<T>::window(unsigned x0, unsigned nx,
                                                unsigned y0, unsigned ny) const
{
  vimt_transform_2d trans;
  trans.set_translation(-double(x0),-double(y0));
  return vimt_image_2d_of<T>(vil2_crop(image_,x0,ny,y0,ny),trans*world2im_);
}


template<class T>
bool vimt_image_2d_of<T>::is_class(vcl_string const& s) const
{
  return s==vimt_image_2d_of<T>::is_a() || vimt_image_2d::is_class(s);
}

//=======================================================================

template<class T>
short vimt_image_2d_of<T>::version_no() const
{
  return 1;
}

//=======================================================================
template<class T>
vimt_image* vimt_image_2d_of<T>::clone() const
{
  return new vimt_image_2d_of(*this);
}

//=======================================================================

template<class T>
void vimt_image_2d_of<T>::print_summary(vcl_ostream& os) const
{
  os<<vsl_indent() << image_<<vcl_endl;
  os<<vsl_indent() << "Transform: "<<world2im_;
}

//=======================================================================
//: print all data to os
template<class T>
void vimt_image_2d_of<T>::print_all(vcl_ostream& os) const
{
  os<<vsl_indent();
  vil2_print_all(os,image_);
  os<<vcl_endl;
  os<<vsl_indent() << "Transform: "<<world2im_;
}


//=======================================================================

template<class T>
void vimt_image_2d_of<T>::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,image_);
  vsl_b_write(bfs,world2im_);
}

//=======================================================================

template<class T>
void vimt_image_2d_of<T>::b_read(vsl_b_istream& bfs)
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
    vcl_cerr << "I/O ERROR: vimt_image_2d_of<T>::b_read(vsl_b_istream&) \n";
    vcl_cerr << "           Unknown version number "<< version << "\n";
    bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

//: True if transforms, etc. are equal, and they share same image data.
//  This does not do a deep equality on image data. If the images point
//  to different image data objects that contain identical images, then
//  the result will still be false.
template<class T>
bool vimt_image_2d_of<T>::operator==(const vimt_image_2d_of<T> &other) const
{
  return image_ == other.image_ &&
    world2im_ == other.world2im_;
}

#define VIMT_IMAGE_2D_OF_INSTANTIATE(T) \
template class vimt_image_2d_of<T >

#endif // mil_image_2d_of_txx_
