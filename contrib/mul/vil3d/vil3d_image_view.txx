// This is mul/vil3d/vil3d_image_view.txx
#ifndef vil3d_image_view_txx_
#define vil3d_image_view_txx_
//:
// \file
// \brief Represent images of one or more planes of Ts.
// \author Tim Cootes, Ian Scott
//
// Note: To keep down size of vil3d_image_view
// Please think carefully before adding any new methods.
// In particular any methods that provide new views (e.g. vil3d_slice)
// will be more usefully provided as external functions. - IMS.
// In that case, use the "relates" keyword of Doxygen to link the documentation
// of that function to the vil3d_image_view class.

#include "vil3d_image_view.h"
#include <vcl_string.h>
#include <vcl_cassert.h>
#include <vcl_ostream.h>
#include <vil/vil_pixel_format.h>

//=======================================================================

template<class T>
vil3d_image_view<T>::vil3d_image_view()
: top_left_(0),istep_(0),jstep_(0),kstep_(0),planestep_(0)
{}

template<class T>
vil3d_image_view<T>::vil3d_image_view(unsigned ni, unsigned nj,
                                      unsigned nk, unsigned n_planes)
: top_left_(0),istep_(1),jstep_(0),kstep_(0)
{
  set_size(ni,nj,nk,n_planes);
}

//: Set this view to look at someone else's memory data.
template<class T>
vil3d_image_view<T>::vil3d_image_view(const T* top_left,
                                      unsigned n_i, unsigned n_j, unsigned n_k, unsigned n_planes,
                                      vcl_ptrdiff_t i_step, vcl_ptrdiff_t j_step,
                                      vcl_ptrdiff_t k_step, vcl_ptrdiff_t plane_step)
{
  set_to_memory(top_left,n_i,n_j,n_k,n_planes,i_step,j_step,k_step,plane_step);
}

//: Set this view to look at another view's data
//  Need to pass the memory chunk to set up the internal smart ptr appropriately
template<class T>
vil3d_image_view<T>::vil3d_image_view(const vil_memory_chunk_sptr& mem_chunk,
                                      const T* top_left, unsigned n_i, unsigned n_j,
                                      unsigned n_k, unsigned n_planes,
                                      vcl_ptrdiff_t i_step, vcl_ptrdiff_t j_step,
                                      vcl_ptrdiff_t k_step, vcl_ptrdiff_t plane_step)
 : vil3d_image_view_base(n_i, n_j, n_k, n_planes)
 , top_left_(const_cast<T*>(top_left))
 , istep_(i_step), jstep_(j_step), kstep_(k_step)
 , planestep_(plane_step)
 , ptr_(mem_chunk)
{
  if (mem_chunk) // if we are doing a view transform on a non-owned image, then mem_chunk will be 0.
  {
    // check view and chunk are in rough agreement
    assert(mem_chunk->size() >= n_planes*n_i*n_j*n_k*sizeof(T));
    if (top_left  < reinterpret_cast<const T*>(mem_chunk->data()) ||
        top_left >= reinterpret_cast<const T*>(reinterpret_cast<char*>(mem_chunk->data()) + mem_chunk->size()))
      vcl_cerr << "top_left at " << static_cast<const void*>(top_left) << ", memory_chunk at "
               << reinterpret_cast<const void*>(mem_chunk->data()) << ", size " << mem_chunk->size()
               << ", size of data type " << sizeof(T) << '\n';
    assert(top_left >= reinterpret_cast<const T*>(mem_chunk->data()) &&
           top_left  < reinterpret_cast<const T*>(reinterpret_cast<char*>(mem_chunk->data()) + mem_chunk->size()));
  }
}

//: Create shallow copy of image with given base reference
//  Sets to empty image if target is of different pixel type
template<class T>
vil3d_image_view<T>::vil3d_image_view(const vil3d_image_view_base& base_ref)
  : top_left_(0),istep_(0),jstep_(0),kstep_(0),planestep_(0)
{
  operator=(base_ref);
}

//: Create shallow copy of image with given base reference
//  Sets to empty image if target is of different pixel type
template<class T>
vil3d_image_view<T>::vil3d_image_view(const vil3d_image_view_base_sptr& base_sptr)
  : top_left_(0),istep_(0),jstep_(0),kstep_(0),planestep_(0)
{
  operator=(base_sptr);
}

//: Create shallow copy of image with same type image
template<class T>
const vil3d_image_view<T>& vil3d_image_view<T>::operator=(const vil3d_image_view<T>& rhs)
{
  return operator=( static_cast<vil3d_image_view_base const&>(rhs) );
}

//: Create shallow copy of image with given base reference
//  Sets to empty image if target is of different pixel type
template<class T>
const vil3d_image_view<T>& vil3d_image_view<T>::operator=(const vil3d_image_view_base& base_ref)
{
  if (static_cast<const vil3d_image_view_base*>(this) == &base_ref)
    return *this;

  if (base_ref.pixel_format() == pixel_format())
  {
    const vil3d_image_view<T> &that = static_cast<const vil3d_image_view<T>&>(base_ref);
    ni_=that.ni_;
    nj_=that.nj_;
    nk_=that.nk_;
    nplanes_=that.nplanes_;
    istep_=that.istep_;
    jstep_=that.jstep_;
    kstep_=that.kstep_;
    planestep_=that.planestep_;
    top_left_=that.top_left_;
    ptr_=that.ptr_;
    return *this;
  }

  clear();
  return *this;
}

//: Perform deep copy of the src image, placing in this image
template<class T>
void vil3d_image_view<T>::deep_copy(const vil3d_image_view<T>& src)
{
  set_size(src.ni(),src.nj(),src.nk(),src.nplanes());

  vcl_ptrdiff_t s_planestep = src.planestep();
  vcl_ptrdiff_t s_istep = src.istep();
  vcl_ptrdiff_t s_jstep = src.jstep();
  vcl_ptrdiff_t s_kstep = src.kstep();

  // Do a deep copy
  // This is potentially inefficient
  const T* src_data = src.origin_ptr();
  T* data = top_left_;
  for (unsigned int p=0;p<nplanes_;++p,src_data+=s_planestep,data+=planestep_)
  {
    T* slice = data;
    const T* src_slice = src_data;
    for (unsigned int k=0;k<nk_;++k,slice+=kstep_,src_slice+=s_kstep)
    {
      T* row = slice;
      const T* src_row = src_slice;
      for (unsigned int j=0;j<nj_;++j,row += jstep_,src_row += s_jstep)
      {
        T* p = row;
        const T* sp = src_row;
        for (unsigned int i=0;i<ni_;++i,p+=istep_,sp+=s_istep) *p = *sp;
      }
    }
  }
}


template<class T> vil3d_image_view<T>::~vil3d_image_view()
{
  // release_data();
}

//=======================================================================

template<class T>
void vil3d_image_view<T>::set_size(unsigned n_i, unsigned n_j, unsigned n_k)
{
  set_size(n_i,n_j,n_k, nplanes_);
}

//: True if data all in one unbroken block and origin_ptr() is lowest data address
template<class T>
bool vil3d_image_view<T>::is_contiguous() const
{
  // RRR GGG BBB
  if (planestep_==int(ni_*nj_*nk_))
  {
    if (istep_==1 && jstep_==int(ni_) && kstep_==int(ni_*nj_) ) return true;
    if (istep_==1 && kstep_==int(ni_) && jstep_==int(ni_*nk_) ) return true;
    if (jstep_==1 && istep_==int(nj_) && kstep_==int(ni_*nj_) ) return true;
    if (jstep_==1 && kstep_==int(nj_) && istep_==int(nj_*nk_) ) return true;
    if (kstep_==1 && istep_==int(nk_) && jstep_==int(ni_*nk_) ) return true;
    if (kstep_==1 && jstep_==int(nk_) && istep_==int(nj_*nk_) ) return true;
  }

  int np = nplanes_;
  // RGBRGBRGB
  if (planestep_==1)
  {
    if (istep_==np && jstep_==int(ni_*np) && kstep_==int(ni_*nj_*np) ) return true;
    if (istep_==np && kstep_==int(ni_*np) && jstep_==int(ni_*nk_*np) ) return true;
    if (jstep_==np && istep_==int(nj_*np) && kstep_==int(ni_*nj_*np) ) return true;
    if (jstep_==np && kstep_==int(nj_*np) && istep_==int(nj_*nk_*np) ) return true;
    if (kstep_==np && istep_==int(nk_*np) && jstep_==int(ni_*nk_*np) ) return true;
    if (kstep_==np && jstep_==int(nk_*np) && istep_==int(nj_*nk_*np) ) return true;
  }

  // Note that there may be other weird combinations
  return false;
}

//=======================================================================

template<class T>
void vil3d_image_view<T>::set_size(unsigned n_i, unsigned n_j, unsigned n_k, unsigned n_planes)
{
  if (n_i==ni_ && n_j==nj_ && n_k==nk_ && n_planes==nplanes_) return;

  release_memory();

  vil_pixel_format fmt = vil_pixel_format_of(T());
  ptr_ = new vil_memory_chunk(sizeof(T)*n_planes*n_k*n_j*n_i,
    vil_pixel_format_component_format(fmt));

  ni_ = n_i;
  nj_ = n_j;
  nk_ = n_k;
  nplanes_ = n_planes;
  istep_ = 1;
  jstep_ = n_i;
  kstep_ = n_i*n_j;
  planestep_ = n_i*n_j*n_k;

  top_left_ = reinterpret_cast<T*>(ptr_->data());
}


//: Set this view to look at someone else's memory.
template<class T>
void vil3d_image_view<T>::set_to_memory(const T* top_left,
                                        unsigned n_i, unsigned n_j,
                                        unsigned n_k, unsigned n_planes,
                                        vcl_ptrdiff_t i_step, vcl_ptrdiff_t j_step,
                                        vcl_ptrdiff_t k_step, vcl_ptrdiff_t plane_step)
{
  release_memory();
  top_left_ = const_cast<T*>(top_left);  // Remove const, as view may end up manipulating data

  ni_ = n_i;
  nj_ = n_j;
  nk_ = n_k;
  nplanes_ = n_planes;
  istep_ = i_step;
  jstep_ = j_step;
  kstep_ = k_step;
  planestep_ = plane_step;
}

//=======================================================================
//: Fill view with given value
template<class T>
void vil3d_image_view<T>::fill(T value)
{
  T* plane = top_left_;
  for (unsigned int p=0;p<nplanes_;++p,plane += planestep_)
  {
    T* slice = plane;
    for (unsigned int k=0;k<nk_;++k,slice += kstep_)
    {
      T* row = slice;
      for (unsigned int j=0;j<nj_;++j,row += jstep_)
      {
        T* p = row;
        for (unsigned int i=0;i<ni_;++i,p+=istep_) *p = value;
      }
    }
  }
}

//=======================================================================

template<class T>
bool vil3d_image_view<T>::is_class(vcl_string const& s) const
{
  return s==vil3d_image_view<T>::is_a() || vil3d_image_view_base::is_class(s);
}

//=======================================================================

template<class T>
void vil3d_image_view<T>::print(vcl_ostream& os) const
{
  os<<nplanes_<<" planes, each "<<ni_<<" x "<<nj_<<" x "<<nk_;
}

//=======================================================================
//: True if they share same view of same image data.
//  This does not do a deep equality on image data. If the images point
//  to different image data objects that contain identical images, then
//  the result will still be false.
template<class T>
bool vil3d_image_view<T>::operator==(const vil3d_image_view<T> &other) const
{
  if (!(bool) *this && !(bool)other) return true;
  return ptr_  == other.ptr_ &&
    top_left_  == other.top_left_ &&
    nplanes_   == other.nplanes_ &&
    ni_        == other.ni_ &&
    nj_        == other.nj_ &&
    nk_        == other.nk_ &&
    planestep_ == other.planestep_ &&
    istep_     == other.istep_ &&
    jstep_     == other.jstep_ &&
    kstep_     == other.kstep_;
}

//=======================================================================
//: Provides an ordering.
//  Useful for ordered containers.
//  There is no guaranteed meaning to the less than operator, except that
//  (a<b && b<a)  is false and  !(a<b) && !(b<a)  is equivalent to  a==b
template<class T>
bool vil3d_image_view<T>::operator<(const vil3d_image_view<T>& other) const
{
  if (ptr_ != other.ptr_) return ptr_<other.ptr_;
  if ((bool) *this && (bool)other) return false;
  if (nplanes_ != other.nplanes_) return nplanes_ < other.nplanes_;
  if (ni_ != other.ni_) return ni_ < other.ni_;
  if (nj_ != other.nj_) return nj_ < other.nj_;
  if (nk_ != other.nk_) return nk_ < other.nk_;
  if (planestep_ != other.planestep_) return planestep_ < other.planestep_;
  if (istep_ != other.istep_) return istep_ < other.istep_;
  if (jstep_ != other.jstep_) return jstep_ < other.jstep_;
  return kstep_ < other.kstep_;
}


//=======================================================================
//: True if the actual images are identical.
// $\bigwedge_{i,j,k,p} {\textstyle src}(i,j,k,p) == {\textstyle dest}(i,j,k,p)$
// The data may be formatted differently in each memory chunk.
//  O(size).
// \relates vil3d_image_view
template<class T>
bool vil3d_image_view_deep_equality(const vil3d_image_view<T> &lhs,
                                    const vil3d_image_view<T> &rhs)
{
  if (lhs.nplanes() != rhs.nplanes() ||
      lhs.nk() != rhs.nk() ||
      lhs.nj() != rhs.nj() ||
      lhs.ni() != rhs.ni())
    return false;

  for (unsigned p = 0; p < rhs.nplanes(); ++p)
   for (unsigned k = 0; k < rhs.nk(); ++k)
    for (unsigned j = 0; j < rhs.nj(); ++j)
      for (unsigned i = 0; i < rhs.ni(); ++i)
        if (!(rhs(i,j,k,p) == lhs(i,j,k,p)))
          return false;
  return true;
}

#define VIL3D_IMAGE_VIEW_INSTANTIATE(T) \
VCL_DEFINE_SPECIALIZATION vcl_string vil3d_image_view<T >::is_a() const \
{ return vcl_string("vil3d_image_view<" #T ">"); } \
template class vil3d_image_view<T >; \
template bool vil3d_image_view_deep_equality(const vil3d_image_view<T >&, \
                                             const vil3d_image_view<T >&)

#endif // vil3d_image_view_txx_
