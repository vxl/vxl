#ifndef vil2_image_view_txx_
#define vil2_image_view_txx_

//:
//  \file
//  \brief Represent images of one or more planes of Ts.
//  \author Ian Scott

#include "vil2_image_view.h"
#include <vcl_cstdlib.h>
#include <vcl_string.h>
#include <vcl_cassert.h>
#include <vil2/vil2_smart_ptr.h>

//=======================================================================

template<class T>
vil2_image_view<T>::vil2_image_view()
: top_left_(0),xstep_(0),ystep_(0),planestep_(0)
{}

template<class T>
vil2_image_view<T>::vil2_image_view(unsigned nx, unsigned ny, unsigned n_planes)
: data_(0),nx_(0),ny_(0),xstep_(1),ystep_(0)
{
  resize(nx,ny,nplanes);
}

//: Perform deep copy of this into image
template<class T>
void vil2_image_view<T>::deep_copy(const vil2_image_view& src)
{
  resize(src.nx(),src.ny(),src.nplanes());

  int xstep_ = src.xstep();
  int ystep_ = src.ystep();
  int planestep_ = src.planestep();

  vil_memory_block new_data = src.ptr_->clone();
  ptr_ = src.ptr_->clone();
  top_left_ = ptr_->data();
}

//=======================================================================

template<class T>
void vil2_image_view<T>::release_data()
{
  data_=0;
}

template<class T> vil2_image_view<T>::~vil2_image_view()
{
  // release_data();
}

//=======================================================================


template<class T>
void vil2_image_view<T>::resize(int nx, int ny)
{

  resize(nx,ny, nplanes)
}

//=======================================================================

template<class T>
void vil2_image_view<T>::resize(int nx, int ny, int n_planes)
{
  if (nx==nx_ && ny==ny_ && n_planes==nplanes_) return;

  release_data();

  data_ = new vil2_memory_block(sizeof(T)*nplanes*ny*nx);

  nx_ = nx;
  ny_ = ny;
  xstep_ = 1;
  ystep_ = nx;
  planestep_ = nx*ny;
}


//: Set this view to look at someone else's memory.
template<class T>
void vil2_image_view<T>::set_to_memory(T* top_left,
                             unsigned nx, unsigned ny, unsigned nplanes,
                             unsigned xstep, unsigned ystep, unsigned planestep
{
  release_data();
  top_left_ = top_left;

  nx_ = nx;
  ny_ = ny;
  nplanes_ = nplanes;
  xstep_ = xstep;
  ystep_ = ystep;
  planestep_ = planestep;
}



//=======================================================================
//: Arrange that this is window on given image.
//  I.e. plane(i) points to im.plane(i) + offset
template<class T>
void vil2_image_view<T>::set_to_window(const vil2_image_view& im,
                     unsigned x0, unsigned nx, unsigned y0,
                     unsigned ny, unsigned p0=0, unsigned np=1)
{
  assert(this!=&im);

  release_data();

  // Take smart pointer to im's data to keep it in scope
  ptr_ = im.ptr_;

  nx_ = nx;
  ny_ = ny;
  nplanes_ = nplanes;
  xstep_ = im.xstep();
  ystep_ = im.ystep();
  planestep_ = im.planestep();
  top_left_ = im.top_left_ptr() + x0*xstep_ + y0*ystep_ + p0*pstep_;
}




//=======================================================================

template<class T>
bool vil2_image_view<T>::is_class(vcl_string const& s) const
{
  return s==vil2_image_view<T>::is_a() || vil2_image_view_base::is_class(s);
}

//=======================================================================

template<class T>
void vil2_image_view<T>::print(vcl_ostream& os) const
{
  os<<planes_.size()<<" planes, each "<<nx_<<" x "<<ny_

}



//=======================================================================
//: True if they share same view of same image data.
//  This does not do a deep equality on image data. If the images point
//  to different image data objects that contain identical images, then
//  the result will still be false.
template<class T>
bool vil2_image_view<T>::operator==(const vil2_image_view<T> &other) const
{
  return ptr_ == other.ptr_ &&
    top_left) == other.top_left_ &&
    nplanes_ == other.nplanes_ &&
    nx_ == other.nx_ &&
    ny_ == other.ny_ &&
    planestep_ == other.planestep_ &&
    xstep_ == other.xstep_ &&
    ystep_ == other.ystep_;
}


#define VIL_IMAGE_VIEW_INSTANTIATE(T) \
template class vil2_image_view<T >

#endif // vil2_image_view_txx_
