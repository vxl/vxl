// This is mul/mil/mil_image_2d_of.txx
#ifndef mil_image_2d_of_txx_
#define mil_image_2d_of_txx_
//:
// \file
// \brief Represent images of one or more planes of Ts.
// \author Tim Cootes

#include "mil_image_2d_of.h"

#include <vcl_cstdlib.h>
#include <vcl_string.h>

#include <vcl_cassert.h>
#include <vsl/vsl_indent.h>
#include <vsl/vsl_vector_io.h>
#include <vbl/io/vbl_io_smart_ptr.h>

//=======================================================================

template<class T>
mil_image_2d_of<T>::mil_image_2d_of()
: data_(0),nx_(0),ny_(0),xstep_(1),ystep_(0)
{
  planes_.resize(1);
  planes_[0] = 0;

  format_ = vcl_string("GreyByte");
}

template<class T>
mil_image_2d_of<T>::mil_image_2d_of(int nx, int ny, int n_planes)
: data_(0),nx_(0),ny_(0),xstep_(1),ystep_(0)
{
  set_n_planes(n_planes);
  resize(nx,ny);
}

//: Perform deep copy of this into image
template<class T>
void mil_image_2d_of<T>::deepCopy(const mil_image_2d_of& src)
{
  set_n_planes(src.n_planes());
  resize(src.nx(),src.ny());
  world2im_     = src.world2im_;

  int s_xstep = src.xstep();
  int s_ystep = src.ystep();

  // Do a deep copy
  // This is potentially inefficient
  for (unsigned int i=0;i<planes_.size();++i)
  {
    T* row = planes_[i];
    const T* i_row = src.plane(i);
    for (int y=0;y<ny_;++y)
    {
      const T* ip = i_row;
      for (int x=0;x<nx_;++x)
      {
        row[x] = *ip;
        ip+=s_xstep;
      }
      i_row += s_ystep;
      row += ystep_;
    }
  }
}

//=======================================================================

template<class T>
void mil_image_2d_of<T>::release_data()
{
  data_=0;
}

template<class T> mil_image_2d_of<T>::~mil_image_2d_of()
{
  // release_data();
}

//=======================================================================

template<class T>
void mil_image_2d_of<T>::resize2(int nx, int ny)
{
  if (nx==nx_ && ny==ny_  || n_planes()==0) return;

  release_data();

  data_ = new mil_image_data<T>;
  data_->resize(n_planes()*nx*ny);

  planes_[0]= (T*) data_->data();
  for (unsigned int i=1;i<planes_.size();++i)
  {
    planes_[i] = planes_[i-1] + (nx*ny);
  }

  nx_ = nx;
  ny_ = ny;
  xstep_ = 1;
  ystep_ = nx;
}

//=======================================================================

template<class T>
void mil_image_2d_of<T>::resize3(int nx, int ny, int n_planes)
{
  set_n_planes(n_planes);
  resize2(nx,ny);
}

//=======================================================================
//: Define number of planes.
//  Each plane will be resized to (0,0)
//  Default number of planes is 1
template<class T>
void mil_image_2d_of<T>::set_n_planes(int n)
{
  assert(n>0);
  if (planes_.size()!=(unsigned)n)
  {
    release_data();
    planes_.resize(n);
    for (int i=0;i<n;++i) planes_[i]=0;
  }

  nx_ = 0;
  ny_ = 0;
  xstep_ = 0;
  ystep_ = 0;
}

//=======================================================================
//: Define valid data region (including transform).
//  Resizes and sets the transformation so that
//  worldToIm(x,y) is valid for all points in range
//  Specifically, resize(1+xhi-xlo,1+yhi-ylo);
//  worldToIm() translates by (-xlo,-ylo)
template<class T>
void mil_image_2d_of<T>::setValidRegion(int xlo, int xhi, int ylo, int yhi)
{
  resize(1+xhi-xlo,1+yhi-ylo);
  world2im_.set_translation(-xlo,-ylo);
}


//=======================================================================
//: Fills all planes with b
template<class T>
void mil_image_2d_of<T>::fill(T b)
{
  for (unsigned int i=0;i<planes_.size();++i)
  {
    T* row = planes_[i];
    for (int y=0;y<ny_;++y)
    {
      if (xstep_==1)
        for (int x=0;x<nx_;++x) row[x]=b;
        else
        {
          int x2 = 0;
          for (int x=0;x<nx_;++x)
          {
            row[x2]=b;
            x2+=xstep_;
          }
        }

        row += ystep_;
    }
  }
}


//=======================================================================
//: Set vcl_string defining format
template<class T>
void mil_image_2d_of<T>::setFormat(const char* f)
{
  format_ = f;
}

//=======================================================================
//: Define parameters.
//  planes[i] is pointer to i'th plane of nx x ny image data
//  i should be valid in range [0,n_planes-1]
//  Copies of pointers recorded (i.e. a shallow copy)
template<class T>
void mil_image_2d_of<T>::set(vcl_vector<T*>& planes,
                             int nx, int ny, int xstep, int ystep,
                             const char* format)
{
  release_data();
  planes_ = planes;

  nx_ = nx;
  ny_ = ny;
  xstep_ = xstep;
  ystep_ = ystep;

  format_ = format;
}

//=======================================================================
//: Define parameters.
//  planes[i] is pointer to i'th plane of nx x ny image data
//  i should be valid in range [0,n_planes-1]
//  Copies of pointers recorded (i.e. a shallow copy)
template<class T>
void mil_image_2d_of<T>::set(T** planes, int n_planes,
                             int nx, int ny, int xstep, int ystep,
                             const char* format)
{
  release_data();
  planes_.resize(n_planes);
  for (int i=0; i<n_planes; ++i)
    planes_[i] = planes[i];

  nx_ = nx;
  ny_ = ny;
  xstep_ = xstep;
  ystep_ = ystep;

  format_ = format;
}

//=======================================================================
//: Define parameters for grey scale images (single plane)
template<class T>
void mil_image_2d_of<T>::setGrey(T* grey_data, int nx, int ny, int ystep)
{
  release_data();
  planes_.resize(1);
  planes_[0] = grey_data;

  nx_ = nx;
  ny_ = ny;
  ystep_ = ystep;

  format_ = vcl_string("GreyByte");
}

//=======================================================================
//: Define parameters for 3 plane (RGB) T images.
//  Sets up a 3 plane image with plane(0) = r, plane(1) = g etc
//  General declaration. Can be used to set a BGR image.
template<class T>
void mil_image_2d_of<T>::setRGB(T* r, T* g, T* b,
                                int nx, int ny, int ystep)
{
  release_data();
  planes_.resize(3);
  planes_[0] = r;
  planes_[1] = g;
  planes_[2] = b;
  nx_ = nx;
  ny_ = ny;
  ystep_ = ystep;

  format_ = vcl_string("RGBPlaneByte");
}

//=======================================================================
//: Define parameters for 3 plane (RGB) T images.
//  Sets up a 3 plane image with plane(0) = r, plane(1) = g etc
template<class T>
void mil_image_2d_of<T>::setRGB(T* r, T* g, T* b,
                                int nx, int ny, int xstep, int ystep)
{
  release_data();
  planes_.resize(3);
  planes_[0] = r;
  planes_[1] = g;
  planes_[2] = b;
  nx_ = nx;
  ny_ = ny;
  xstep_ = xstep;
  ystep_ = ystep;

  format_ = vcl_string("RGBPlaneByte");
}

//=======================================================================
//: Define parameters for packed RGB T images.
//  Sets up a 3 plane image, assuming nx x ny image
//  of xstep T pixels, i.e. red(x,y) = data[x*xstep+y*step],
//  green(x,y) = data[1+x*xstep+y*step],
//  blue(x,y) = data[2+x*xstep+y*step]
template<class T>
void mil_image_2d_of<T>::setRGB(T* data, int nx, int ny, int xstep, int ystep)
{
  release_data();
  planes_.resize(3);
  planes_[0] = data;
  planes_[1] = data+1;
  planes_[2] = data+2;
  nx_ = nx;
  ny_ = ny;
  xstep_ = xstep;
  ystep_ = ystep;

  format_ = vcl_string("RGBPackedByte");
}

//=======================================================================
//: Arrange that this is window on given image.
//  I.e. plane(i) points to im.plane(i) + offset
//  The world2im transform is set to match
//  so this appears identical to im when addressed
//  in world coordinates.
template<class T>
void mil_image_2d_of<T>::setToWindow(const mil_image_2d_of& im,
                                     int xlo, int xhi, int ylo, int yhi)
{
  assert(this!=&im);

  int n_planes = im.n_planes();
  set_n_planes(n_planes);
  release_data();

  // Take smart pointer to im's data to keep it in scope
  data_ = im.data_;

  nx_ = 1+xhi-xlo;
  ny_ = 1+yhi-ylo;
  xstep_ = im.xstep();
  ystep_ = im.ystep();
  int offset = xlo * im.xstep() + ylo * im.ystep();

  // const problem: planes_ isn't const but im.plane(i) is.
  // without having separate pointers for const/non-const
  // we can't get over this easily
  for (int i=0;i<n_planes;++i)
    planes_[i] = const_cast<T*>(im.plane(i))+offset;

  mil_transform_2d trans;
  trans.set_translation(-xlo,-ylo);
  world2im_ = trans * im.world2im();
  format_ = im.format();
}

//=======================================================================
//: Get range of values in plane p
template<class T>
void mil_image_2d_of<T>::getRange(T& min_f, T& max_f, int p) const
{
  const T* row = planes_[p];
  min_f = row[0];
  max_f = min_f;
  for (int y=0;y<ny_;++y)
  {
    int x=0;
    for (int j=0;j<nx_;++j)
    {
      if (row[x]<min_f) min_f=row[x];
      else
        if (row[x]>max_f) max_f=row[x];
        x+=xstep_;
    }
    row+=ystep_;
  }
}

//=======================================================================
//: Get range of values over all planes
template<class T>
void mil_image_2d_of<T>::getRange(T& min_f, T& max_f) const
{
  if (planes_.size()==0)
  {
    min_f = 0;
    max_f = 0;
    return;
  }

  getRange(min_f,max_f,0);

  for (unsigned int i=1;i<planes_.size();++i)
  {
    T min_fi,max_fi;
    getRange(min_fi,max_fi,i);
    if (min_fi<min_f) min_f=min_fi;
    if (max_fi>max_f) max_f=max_fi;
  }
}

//=======================================================================

template<class T>
bool mil_image_2d_of<T>::is_class(vcl_string const& s) const
{
  return s==mil_image_2d_of<T>::is_a() || mil_image_2d::is_class(s);
}

//=======================================================================

template<class T>
short mil_image_2d_of<T>::version_no() const
{
  return 1;
}

//=======================================================================
template<class T>
mil_image* mil_image_2d_of<T>::clone() const
{
  return new mil_image_2d_of(*this);
}

//=======================================================================

template<class T>
void mil_image_2d_of<T>::print_summary(vcl_ostream& os) const
{
  os<<"Format: "<<format_<<"  "
    <<planes_.size()<<" planes, each "<<nx_<<" x "<<ny_<<vcl_endl
    <<vsl_indent() << "Transform: "<<world2im_;
}

//=======================================================================
//: print all data to os
template<class T>
void mil_image_2d_of<T>::print_all(vcl_ostream& os) const
{
  os<<vsl_indent();
  print_summary(os);
  os<<vcl_endl;

  for (int i=0;i<n_planes();++i)
  {
    if (n_planes()>1) os<<vsl_indent()<<"Plane "<<i<<":\n";
    const T* im_data = plane(i);
    for (int y=0;y<ny_;++y)
    {
      os<<vsl_indent();
      for (int x=0;x<nx_;++x)
      {
        int v = int(im_data[ystep_*y+x*xstep_]);
        if (v<10)  os<<' ';
        if (v<100) os<<' ';
        os<<v<<' ';
      }
      os<<vcl_endl;
    }
  }
}


//=======================================================================
//: print all data to os
template<class T>
void mil_image_2d_of<T>::print_messy_all(vcl_ostream& os) const
{
  os<<vsl_indent();
  print_summary(os);
  os<<vcl_endl;

  for (int i=0;i<n_planes();++i)
  {
    if (n_planes()>1) os<<vsl_indent()<<"Plane "<<i<<":\n";
    const T* im_data = plane(i);
    for (int y=0;y<ny_;++y)
    {
      os<<vsl_indent();
      for (int x=0;x<nx_;++x)
      {
        float v = float ( im_data[ystep_*y+x*xstep_] );
        os<<v<<' ';
      }
      os<<vcl_endl;
    }
  }
}

//=======================================================================

template<class T>
void mil_image_2d_of<T>::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  if (!data_)
  {
    vcl_cerr << "template<class T> mil_image_2d_of<T>::b_write() :\n"
             << "  This image refers to external data and "
             << "cannot be restored correctly if saved like this.\n";
    vcl_abort();
  }
  vsl_b_write(bfs,data_);

  vsl_b_write(bfs,nx_);
  vsl_b_write(bfs,ny_);
  vsl_b_write(bfs,xstep_);
  vsl_b_write(bfs,ystep_);
  vsl_b_write(bfs,format_);
  vsl_b_write(bfs,world2im_);

  int n = planes_.size();
  vcl_vector<int> plane_offsets(n);
  for (int i=0;i<n;++i)
    plane_offsets[i]=int(planes_[i]-(T*)data_->data());
  vsl_b_write(bfs,plane_offsets);
}

//=======================================================================

template<class T>
void mil_image_2d_of<T>::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  release_data();

  vcl_vector<int> plane_offsets;;

  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
  case (1):
    vsl_b_read(bfs,data_);
    vsl_b_read(bfs,nx_);
    vsl_b_read(bfs,ny_);
    vsl_b_read(bfs,xstep_);
    vsl_b_read(bfs,ystep_);
    vsl_b_read(bfs,format_);
    vsl_b_read(bfs,world2im_);
    vsl_b_read(bfs,plane_offsets);
    break;
  default:
    vcl_cerr << "I/O ERROR: mil_image_2d_of<T>::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< version << '\n';
    bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }

  int n = plane_offsets.size();
  planes_.resize(n);
  for (int i=0;i<n;++i)
    planes_[i]=(T*)data_->data() + plane_offsets[i];
}

//=======================================================================
//: True if transforms, etc. are equal, and they share same image data.
//  This does not do a deep equality on image data. If the images point
//  to different image data objects that contain identical images, then
//  the result will still be false.
template<class T>
bool mil_image_2d_of<T>::operator==(const mil_image_2d_of<T> &other) const
{
  return data_ == other.data_ &&
    planes_ == other.planes_ &&
    nx_ == other.nx_ &&
    ny_ == other.ny_ &&
    xstep_ == other.xstep_ &&
    ystep_ == other.ystep_ &&
    format_ == other.format_ &&
    world2im_ == other.world2im_;
}


#undef MIL_IMAGE_2D_OF_INSTANTIATE
#define MIL_IMAGE_2D_OF_INSTANTIATE(T) \
VCL_DEFINE_SPECIALIZATION vcl_string mil_image_2d_of<T >::is_a() const \
{ return vcl_string("mil_image_2d_of<" #T ">"); } \
template class mil_image_2d_of<T >


#endif // mil_image_2d_of_txx_
