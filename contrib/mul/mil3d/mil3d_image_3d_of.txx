// This is mul/mil3d/mil3d_image_3d_of.txx
#ifndef mil3d_image_3d_of_txx_
#define mil3d_image_3d_of_txx_
//:
// \file
// \brief Represent 3D images of one or more planes of Ts.
// \author Graham Vincent (extended from mil_image_2d_of by Tim Cootes)

#include "mil3d_image_3d_of.h"

#include <vcl_cstdlib.h>
#include <vcl_string.h>

#include <vcl_cassert.h>
#include <vsl/vsl_indent.h>
#include <vsl/vsl_vector_io.h>
#include <vbl/io/vbl_io_smart_ptr.h>

//=======================================================================
// Dflt ctor
//=======================================================================

template<class T>
mil3d_image_3d_of<T>::mil3d_image_3d_of()
    : data_(0),nx_(0),ny_(0), nz_(0), xstep_(1),ystep_(0), zstep_(0)
{
    planes_.resize(1);
    planes_[0] = 0;

    format_ = vcl_string("GreyByte");
}

template<class T>
mil3d_image_3d_of<T>::mil3d_image_3d_of(int nx, int ny, int nz,int n_planes)
    : data_(0),nx_(0),ny_(0),nz_(0), xstep_(1),ystep_(0), zstep_(0)
{
    set_n_planes(n_planes);
    resize(nx,ny,nz);
}

//: Perform deep copy of this into image
template<class T>
void mil3d_image_3d_of<T>::deepCopy(const mil3d_image_3d_of& src)
{
    set_n_planes(src.n_planes());
    resize(src.nx(),src.ny(),src.nz());
    world2im_     = src.world2im_;

    int s_xstep = src.xstep();
    int s_ystep = src.ystep();
    int s_zstep = src.zstep();

    // Do a deep copy - inefficient for now
    for (int i=0;i<n_planes();++i)
    {
        const T* sdata = src.plane(i);
        T* ddata = plane(i);
        for (int z=0;z<nz_;++z)
        {
            for (int y=0;y<ny_;++y)
            {
                for (int x=0;x<nx_;++x)
                {
                 int sindex = x*xstep_+ystep_*y+z*zstep_;
                 int dindex = x*s_xstep+s_ystep*y+z*s_zstep;
                 ddata[dindex]=sdata[sindex];
                }
            }
        }
    }
}

//=======================================================================
// Destructor
//=======================================================================

template<class T>
void mil3d_image_3d_of<T>::release_data()
{
    data_=0;
}

template<class T> mil3d_image_3d_of<T>::~mil3d_image_3d_of()
{
// release_data();
}


//=======================================================================
//: Resize all planes
//=======================================================================
template<class T>
void mil3d_image_3d_of<T>::resize2(int nx, int ny, int nz)
{
    if (nx==nx_ && ny==ny_ && nz==nz_ || n_planes()==0) return;

    release_data();

    data_ = new mil_image_data<T>;
    data_->resize(n_planes()*nx*ny*nz);

    planes_[0]= (T*) data_->data();
    for (unsigned int i=1;i<planes_.size();++i)
    {
        planes_[i] = planes_[i-1] + (nx*ny*nz);
    }

    nx_ = nx;
    ny_ = ny;
    nz_ = nz;
    xstep_ = 1;
    ystep_ = nx;
    zstep_ = nx*ny;
}

//=======================================================================
//: Resize to n_planes of [0..nx-1][0..ny-1][0..nz-1]
//=======================================================================
template<class T>
void mil3d_image_3d_of<T>::resize3(int nx, int ny, int nz, int n_planes)
{
  set_n_planes(n_planes);
  resize2(nx,ny,nz);
}

//=======================================================================
//: Define number of planes.
//  Each plane will be resized to (0,0)
//  Default number of planes is 1
//=======================================================================
template<class T>
void mil3d_image_3d_of<T>::set_n_planes(int n)
{
    assert(n > 0);
    if (planes_.size()!=(unsigned)n)
    {
        release_data();
        planes_.resize(n);
        for (int i=0;i<n;++i) planes_[i]=0;
    }

    nx_ = 0;
    ny_ = 0;
    nz_=0;
    xstep_ = 0;
    ystep_ = 0;
    zstep_ = 0;
}

//=======================================================================
//: Define valid data region (including transform).
//  Resizes and sets the transformation so that
//  worldToIm(x,y,z) is valid for all points in range
//  Specifically, resize(1+xhi-xlo,1+yhi-ylo,1+zhi-zlo);
//  worldToIm() translates by (-xlo,-ylo,-zlo)
//=======================================================================
template<class T>
void mil3d_image_3d_of<T>::setValidRegion(int xlo, int xhi, int ylo, int yhi, int zlo, int zhi)
{
    resize(xhi-xlo,yhi-ylo,zhi-zlo);
    world2im_.set_translation(-xlo,-ylo,-zlo);
}


//=======================================================================
//: Fills all planes with b
//=======================================================================
template<class T>
void mil3d_image_3d_of<T>::fill(T b)
{
    for (unsigned int p=0;p<planes_.size();++p) {
     T* data = planes_[p];
     // inefficient for now
    for (int z=0;z<nz_;++z)
    {
        for (int y=0;y<ny_;++y)
        {
            for (int x=0;x<nx_;++x)
            {
                  int index = x*xstep_+ystep_*y+z*zstep_;
                 data[index]=b;
            }
        }
     }
    }
}


//=======================================================================
//: Set vcl_string defining format
//=======================================================================
template<class T>
void mil3d_image_3d_of<T>::setFormat(const char* f)
{
    format_ = f;
}


//=======================================================================
//: Define parameters
//  planes[i] is pointer to i'th plane of nx x ny image data
//  i should be valid in range [0,n_planes-1]
//  Copies of pointers recorded (i.e. a shallow copy)
//=======================================================================
template<class T>
void mil3d_image_3d_of<T>::set(vcl_vector<T*>& planes,
                               int nx, int ny, int nz,
                               int xstep, int ystep, int zstep,
                               const char* format)
{
    release_data();
    planes_ = planes;

    nx_ = nx;
    ny_ = ny;
    nz_ = nz;
    xstep_ = xstep;
    ystep_ = ystep;
    zstep_ = zstep;

    format_ = format;
}

//=======================================================================
//: Arrange that this is window on given image.
//  I.e. plane(i) points to im.plane(i) + offset
//  The world2im transform is set to match
//  so this appears identical to im when addressed
//  in world coordinates.
//=======================================================================
template<class T>
void mil3d_image_3d_of<T>::setToWindow(const mil3d_image_3d_of& im,
                                       int xlo, int xhi,
                                       int ylo, int yhi,
                                       int zlo, int zhi)
{
    assert(this!=&im);

    int n_planes = im.n_planes();
    set_n_planes(n_planes);
    release_data();

    // Take smart pointer to im's data to keep it in scope
    data_ = im.data_;

    nx_ = 1+xhi-xlo;
    ny_ = 1+yhi-ylo;
    nz_ = 1+zhi-zlo;
    xstep_ = im.xstep();
    ystep_ = im.ystep();
    zstep_ = im.zstep();
    int offset = xlo * im.xstep() + ylo * im.ystep()+ zlo*im.zstep();

        // const problem: planes_ isn't const but im.plane(i) is.
        // without having separate pointers for const/non-const
        // we can't get over this easily
    for (int i=0;i<n_planes;++i)
        planes_[i] = const_cast<T*>(im.plane(i))+offset;

    mil3d_transform_3d trans;
    trans.set_translation(-xlo,-ylo,-zlo);
    world2im_ = trans * im.world2im();
    format_ = im.format();
}


//=======================================================================
//: Get range of values in plane p
//=======================================================================
template<class T>
void mil3d_image_3d_of<T>::getRange(T& min_f, T& max_f, int p) const
{
    const T* data = planes_[p];
    min_f = data[0];
    max_f = min_f;
    // inefficient for now
    for (int z=0;z<nz_;++z)
    {
        for (int y=0;y<ny_;++y)
        {
            for (int x=0;x<nx_;++x)
            {
                  int index = x*xstep_+ystep_*y+z*zstep_;
                 T val=data[index];
                 if (val<min_f) min_f=val;
                 else
                 if (val>max_f) max_f=val;
            }
        }
    }
}

//=======================================================================
//: Get range of values over all planes
//=======================================================================
template<class T>
void mil3d_image_3d_of<T>::getRange(T& min_f, T& max_f) const
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
// Method: is_class
//=======================================================================

template<class T>
bool mil3d_image_3d_of<T>::is_class(vcl_string const& s) const
{
  return s==mil3d_image_3d_of<T>::is_a() || mil3d_image_3d::is_class(s);
}

//=======================================================================
// Method: version_no
//=======================================================================

template<class T>
short mil3d_image_3d_of<T>::version_no() const
{
    return 1;
}

//=======================================================================
// Method: clone
//=======================================================================
template<class T>
mil_image* mil3d_image_3d_of<T>::clone() const
{
    return new mil3d_image_3d_of(*this);
}

//=======================================================================
// Method: print
//=======================================================================

template<class T>
void mil3d_image_3d_of<T>::print_summary(vcl_ostream& os) const
{
    os<<"Format: "<<format_<<"  "
      <<planes_.size()<<" planes, each "<<nx_<<" x "<<ny_<<" x "<<nz_<<'\n'
      <<vsl_indent() << "Transform: "<<world2im_;
}

//=======================================================================
//: Print all data to os
//=======================================================================
template<class T>
void mil3d_image_3d_of<T>::print_all(vcl_ostream& os) const
{
    os<<vsl_indent();
    print_summary(os);
    os<<'\n';

    for (int i=0;i<n_planes();++i)
    {
      if (n_planes()>1) os<<vsl_indent()<<"Plane "<<i<<":\n";
      const T* im_data = plane(i);
      for (int z=0;z<nz_;++z)
      {
        os<<vsl_indent()<<"z= "<<z<<":\n"
          <<vsl_indent();

        for (int y=ny_-1;y>=0;--y)
        {
          os<<vsl_indent();
          for (int x=0;x<nx_;++x)
          {
            int v = int(im_data[ystep_*y+x*xstep_+z*zstep_]);
            if (v<10)  os<<' ';
            if (v<100) os<<' ';
            os<<v<<' ';
          }
          os<<'\n';
        }
        os<<'\n';
      }
    }
}

//=======================================================================
// Method: save
//=======================================================================

template<class T>
void mil3d_image_3d_of<T>::b_write(vsl_b_ostream& bfs) const
{
    vsl_b_write(bfs,version_no());
    if (!data_)
    {
      vcl_cerr << "template<class T> mil3d_image_3d_of<T>::b_write() :\n"
               << "  This image refers to external data and"
               << " cannot be restored correctly if saved like this.\n";
      vcl_abort();
    }
    vsl_b_write(bfs,data_);

    vsl_b_write(bfs,nx_);
    vsl_b_write(bfs,ny_);
    vsl_b_write(bfs,nz_);
    vsl_b_write(bfs,xstep_);
    vsl_b_write(bfs,ystep_);
    vsl_b_write(bfs,zstep_);
    vsl_b_write(bfs,format_);
    vsl_b_write(bfs,world2im_);

    int n = planes_.size();
    vcl_vector<int> plane_offsets(n);
    for (int i=0;i<n;++i)
        plane_offsets[i]=int(planes_[i]-(T*)data_->data());
    vsl_b_write(bfs,plane_offsets);
}

//=======================================================================
// Method: load
//=======================================================================

template<class T>
void mil3d_image_3d_of<T>::b_read(vsl_b_istream& bfs)
{
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
            vsl_b_read(bfs,nz_);
            vsl_b_read(bfs,xstep_);
            vsl_b_read(bfs,ystep_);
            vsl_b_read(bfs,zstep_);
            vsl_b_read(bfs,format_);
            vsl_b_read(bfs,world2im_);
            vsl_b_read(bfs,plane_offsets);
            break;
        default:
            vcl_cerr << "template<class T> mil3d_image_3d_of<T>::b_read() :\n"
                     << "  Unexpected version number " << version << '\n';
            vcl_abort();
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
//=======================================================================
template<class T>
bool mil3d_image_3d_of<T>::operator==(const mil3d_image_3d_of<T> &other) const
{
  return data_ == other.data_ &&
    planes_ == other.planes_ &&
    nx_ == other.nx_ &&
    ny_ == other.ny_ &&
    nz_ == other.nz_ &&
    xstep_ == other.xstep_ &&
    ystep_ == other.ystep_ &&
    zstep_ == other.zstep_ &&
    format_ == other.format_ &&
    world2im_ == other.world2im_;
}

//=======================================================================
//: Creates deep copy of slice of data from 3d
//=======================================================================
template<class T>
bool mil3d_image_3d_of<T>::deepSlice(Axis axis,int slice_number, mil_image_2d_of<T> &image_slice) const
{
    assert(world2im().isIdentity() || world2im().form()==mil3d_transform_3d::ZoomOnly);
    bool result = true;
    mil_transform_2d t;
    switch(axis)
    {
        case XAXIS:
            image_slice.resize(ny(),nz());
            for (int z=0;z<nz();++z)
                for (int y=0;y<ny();++y)
                    image_slice(y,z)=(*this)(slice_number,y,z);
            t.set_zoom_only(world2im_.matrix()(1,1),world2im_.matrix()(2,2),0,0);
            image_slice.setWorld2im(t);
            break;
        case YAXIS:
            image_slice.resize(nx(),nz());
            for (int z=0;z<nz();++z)
                for (int x=0;x<nx();++x)
                    image_slice(x,z)=(*this)(x,slice_number,z);
            t.set_zoom_only(world2im_.matrix()(0,0),world2im_.matrix()(2,2),0,0);
            image_slice.setWorld2im(t);
            break;
        case ZAXIS:
            image_slice.resize(nx(),ny());
            for (int y=0;y<ny();++y)
                for (int x=0;x<nx();++x)
                    image_slice(x,y)=(*this)(x,y,slice_number);
            t.set_zoom_only(world2im_.matrix()(0,0),world2im_.matrix()(1,1),0,0);
            image_slice.setWorld2im(t);
            break;
        default:
            result=false;
    }

    return result;
}

#undef MIL3D_IMAGE_3D_OF_INSTANTIATE
#define MIL3D_IMAGE_3D_OF_INSTANTIATE(T) \
VCL_DEFINE_SPECIALIZATION vcl_string mil3d_image_3d_of<T >::is_a() const \
{ return vcl_string("mil3d_image_3d_of<" #T ">"); } \
template class mil3d_image_3d_of<T >

#endif // mil3d_image_3d_of_txx_
