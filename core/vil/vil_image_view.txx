// This is core/vil/vil_image_view.txx
#ifndef vil_image_view_txx_
#define vil_image_view_txx_
//:
// \file
// \brief Represent images of one or more planes of Ts.
// \author Ian Scott
//
// Note: To keep down size of vil_image_view
// Please think carefully before adding any new methods.
// In particular any methods that provide new views (e.g. vil_plane)
// will be more usefully provided as external functions. - IMS.
// In that case, use the "relates" keyword of Doxygen to link the documentation
// of that function to the vil_image_view class.
//
// \verbatim
//  Modifications
//   23 Oct.2003 - Peter Vanroose - Added support for 64-bit int pixels
// \endvarbatim

#include "vil_image_view.h"
#include <vcl_string.h>
#include <vcl_cassert.h>
#include <vcl_cstdlib.h>
#include <vcl_cmath.h>
#include <vcl_ostream.h>
#include <vil/vil_pixel_format.h>
#include <vcl_cstring.h>

//=======================================================================


template<class T>
vil_image_view<T>::vil_image_view(unsigned n_i, unsigned n_j, unsigned n_planes,
                                  unsigned n_interleaved_planes)
: top_left_(0), istep_(n_interleaved_planes)
{
  assert(n_planes==1 || n_interleaved_planes==1);
  assert(n_planes * n_interleaved_planes == 1 ||
         vil_pixel_format_num_components(vil_pixel_format_of(T())) == 1);
  set_size(n_i,n_j,n_planes*n_interleaved_planes);
}

//: Set this view to look at someone else's memory data.
template<class T>
vil_image_view<T>::vil_image_view(const T* top_left, unsigned n_i, unsigned n_j, unsigned n_planes,
                                  vcl_ptrdiff_t i_step, vcl_ptrdiff_t j_step, vcl_ptrdiff_t plane_step)
{
  set_to_memory(top_left,n_i,n_j,n_planes,i_step,j_step,plane_step);
}

//: Set this view to look at another view's data
//  Need to pass the memory chunk to set up the internal smart ptr appropriately
template<class T>
vil_image_view<T>::vil_image_view(vil_memory_chunk_sptr const& mem_chunk,
                                  const T* top_left, unsigned n_i, unsigned n_j, unsigned n_planes,
                                  vcl_ptrdiff_t i_step, vcl_ptrdiff_t j_step, vcl_ptrdiff_t plane_step)
 : vil_image_view_base(n_i, n_j, n_planes)
 , top_left_(const_cast<T*>(top_left))
 , istep_(i_step), jstep_(j_step)
 , planestep_(plane_step)
 , ptr_(mem_chunk)
{
#ifndef NDEBUG
  // check view and chunk are in rough agreement
  if (mem_chunk) // if we are doing a view transform on a non-owned image, then mem_chunk will be 0.
  {
    assert(mem_chunk->size() >= n_planes*n_i*n_j*sizeof(T));
    if (top_left  < reinterpret_cast<const T*>(mem_chunk->data()) ||
        top_left >= reinterpret_cast<const T*>(reinterpret_cast<const char*>(mem_chunk->data()) + mem_chunk->size()))
      vcl_cerr << "top_left at " << static_cast<const void*>(top_left) << ", memory_chunk at "
               << reinterpret_cast<const void*>(mem_chunk->data()) << ", size " << mem_chunk->size()
               << ", size of data type " << sizeof(T) << '\n';
    assert(top_left >= reinterpret_cast<const T*>(mem_chunk->data()) &&
           top_left  < reinterpret_cast<const T*>(reinterpret_cast<const char*>(mem_chunk->data()) + mem_chunk->size()));
  }
#endif
}

//: Copy constructor
// If this view cannot set itself to view the other data (e.g. because the
// types are incompatible) it will set itself to empty.
template<class T>
vil_image_view<T>::vil_image_view(const vil_image_view<T>& that)
: vil_image_view_base(that.ni(),that.nj(),that.nplanes()),
  top_left_(0), istep_(0), jstep_(0), planestep_(0), ptr_(0)
{
  operator=( static_cast<vil_image_view_base const&>(that) );
}

//: Sort of copy constructor
// If this view cannot set itself to view the other data (e.g. because the
// types are incompatible) it will set itself to empty.
template<class T>
vil_image_view<T>::vil_image_view(const vil_image_view_base& that):
top_left_(0), istep_(0), jstep_(0), planestep_(0), ptr_(0)
{
  operator=(that);
}

//: Sort of copy constructor
// If this view cannot set itself to view the other data (e.g. because the
// types are incompatible) it will set itself to empty.
template <class T>
vil_image_view<T>::vil_image_view(const vil_image_view_base_sptr& that):
top_left_(0), istep_(0), jstep_(0), planestep_(0), ptr_(0)
{
  operator=(that);
}

//: Perform deep copy of the src image, placing in this image
template<class T>
void vil_image_view<T>::deep_copy(const vil_image_view<T>& src)
{
  set_size(src.ni(),src.nj(),src.nplanes());

  if (src.is_contiguous())
  {
    istep_=src.istep_; jstep_= src.jstep_; planestep_ = src.planestep_;
    if (src.istep()>0 && src.jstep()>0 && src.planestep()>=0)
    {
      vcl_memcpy(top_left_,src.top_left_ptr(),src.size()*sizeof(T));
      return;
    }
    const_iterator s_it = src.begin();
    iterator d_it = begin();
    const_iterator end_it = src.end();
    while (s_it!=end_it) {*d_it = *s_it; ++s_it; ++d_it; }
    return;
  }

  const vcl_ptrdiff_t s_planestep = src.planestep();
  const vcl_ptrdiff_t s_istep = src.istep();
  const vcl_ptrdiff_t s_jstep = src.jstep();

  // Do a deep copy
  // This is potentially inefficient
  const T* src_data = src.top_left_ptr();
  T* data = top_left_;
  for (unsigned int p=0;p<nplanes_;++p,src_data += s_planestep,data += planestep_)
  {
    T* row = data;
    const T* src_row = src_data;
    for (unsigned int j=0;j<nj_;++j,row += jstep_,src_row += s_jstep)
    {
      T* p = row;
      const T* sp = src_row;
      for (unsigned int i=0;i<ni_;++i,p+=istep_,sp+=s_istep) *p = *sp;
    }
  }
}

// Notes on convert_components_from_planes() and convert_planes_from_components()
// These are used by the operator= to provide the appropriate smart conversion
// behaviour for the various types.
// I don't think that C++ templates support full pattern matching,
// so we have to provide one template instantiation to cover the general
// compound pixel case (the range of which is possibly infinite)
// We then specialise for all the scalar pixel cases (there are only so
// many scalar types).
// I guess someone could merge all the scalar specialisations using
// macros and substantially reduce the length of this code.


//: Convert planes to components from planes, or do nothing if types are wrong.
template <class T>
inline bool convert_components_from_planes(vil_image_view<T> &lhs,
                                           const vil_image_view_base &rhs_base)
{
  typedef typename T::value_type comp_type;

  const int ncomp =
    vil_pixel_format_num_components(vil_pixel_format_of(T()));

  vil_pixel_format fmt = vil_pixel_format_of(T());
  if (// both sides have equal component types and rhs has scalar pixels and
      rhs_base.pixel_format() == vil_pixel_format_component_format(fmt) &&
      // lhs has number of components equal to rhs's number of planes.
      ncomp == (int)rhs_base.nplanes() )
  {
    const vil_image_view<comp_type> &rhs = static_cast<const vil_image_view<comp_type>&>(rhs_base);
    // Check that the steps are suitable for viewing as components
    if (rhs.planestep() != 1 || vcl_abs((int)rhs.istep())<ncomp || vcl_abs((int)rhs.jstep())<ncomp ) return false;
    lhs = vil_image_view<T >(rhs.memory_chunk(),
                             reinterpret_cast<T const*>(rhs.top_left_ptr()),
                             rhs.ni(),rhs.nj(),1,
                             rhs.istep()/ncomp,rhs.jstep()/ncomp,1);
    return true;
  }
  else
    return false;
}


VCL_DEFINE_SPECIALIZATION
inline bool convert_components_from_planes(vil_image_view<float> & /*lhs*/,
                                           const vil_image_view_base & /*rhs_base*/)
{return false;}  // when lhs has scalar pixels, don't attempt conversion

VCL_DEFINE_SPECIALIZATION
inline bool convert_components_from_planes(vil_image_view<double> & /*lhs*/,
                                           const vil_image_view_base & /*rhs_base*/)
{return false;}

VCL_DEFINE_SPECIALIZATION
inline bool convert_components_from_planes(vil_image_view<bool> & /*lhs*/,
                                           const vil_image_view_base & /*rhs_base*/)
{return false;}

VCL_DEFINE_SPECIALIZATION
inline bool convert_components_from_planes(vil_image_view<vxl_sbyte> & /*lhs*/,
                                           const vil_image_view_base & /*rhs_base*/)
{return false;}

VCL_DEFINE_SPECIALIZATION
inline bool convert_components_from_planes(vil_image_view<vxl_byte> & /*lhs*/,
                                           const vil_image_view_base & /*rhs_base*/)
{return false;}

VCL_DEFINE_SPECIALIZATION
inline bool convert_components_from_planes(vil_image_view<vxl_int_16> & /*lhs*/,
                                           const vil_image_view_base & /*rhs_base*/)
{return false;}

VCL_DEFINE_SPECIALIZATION
inline bool convert_components_from_planes(vil_image_view<vxl_uint_16> & /*lhs*/,
                                           const vil_image_view_base & /*rhs_base*/)
{return false;}

VCL_DEFINE_SPECIALIZATION
inline bool convert_components_from_planes(vil_image_view<vxl_int_32> & /*lhs*/,
                                           const vil_image_view_base & /*rhs_base*/)
{return false;}

VCL_DEFINE_SPECIALIZATION
inline bool convert_components_from_planes(vil_image_view<vxl_uint_32> & /*lhs*/,
                                           const vil_image_view_base & /*rhs_base*/)
{return false;}

#if VXL_HAS_INT_64

VCL_DEFINE_SPECIALIZATION
inline bool convert_components_from_planes(vil_image_view<vxl_int_64> & /*lhs*/,
                                           const vil_image_view_base & /*rhs_base*/)
{return false;}

VCL_DEFINE_SPECIALIZATION
inline bool convert_components_from_planes(vil_image_view<vxl_uint_64> & /*lhs*/,
                                           const vil_image_view_base & /*rhs_base*/)
{return false;}

#endif

//: Convert components to planes from planes, or do nothing if types are wrong.
template <class T>
inline bool convert_planes_from_components(vil_image_view<T> & /*lhs*/,
                                           const vil_image_view_base & /*rhs_base*/)
{ return false;} // when lhs has non-scalar pixels, don't attempt conversion

VCL_DEFINE_SPECIALIZATION
inline bool convert_planes_from_components(vil_image_view<vxl_byte> &lhs,
                                           const vil_image_view_base &rhs_base)
{
  const unsigned ncomp =
    vil_pixel_format_num_components(rhs_base.pixel_format());

  if (// rhs has just 1 plane
      rhs_base.nplanes() == 1 &&
      // both sides have equal component types
      vil_pixel_format_component_format(rhs_base.pixel_format()) == VIL_PIXEL_FORMAT_BYTE)
  {
    // cheat by casting to component type, not pixel type (because we don't know full pixel type at compile time.)
    const vil_image_view<vxl_byte> &rhs = static_cast<const vil_image_view<vxl_byte>&>(rhs_base);

    lhs = vil_image_view<vxl_byte>(rhs.memory_chunk(), rhs.top_left_ptr(),
                                   rhs.ni(),rhs.nj(),ncomp,
                                   rhs.istep()*ncomp,rhs.jstep()*ncomp,1);
    return true;
  }
  else
    return false;
}

VCL_DEFINE_SPECIALIZATION
inline bool convert_planes_from_components(vil_image_view<vxl_sbyte> &lhs,
                                           const vil_image_view_base &rhs_base)
{
  const unsigned ncomp =
    vil_pixel_format_num_components(rhs_base.pixel_format());

  if (// rhs has just 1 plane
      rhs_base.nplanes() == 1 &&
      // both sides have equal component types
      vil_pixel_format_component_format(rhs_base.pixel_format()) == VIL_PIXEL_FORMAT_SBYTE)
  {
    // cheat by casting to component type, not pixel type (because we don't know full pixel type at compile time.)
    const vil_image_view<vxl_sbyte> &rhs = static_cast<const vil_image_view<vxl_sbyte>&>(rhs_base);

    lhs = vil_image_view<vxl_sbyte>(rhs.memory_chunk(), rhs.top_left_ptr(),
                                    rhs.ni(),rhs.nj(),ncomp,
                                    rhs.istep()*ncomp,rhs.jstep()*ncomp,1);
    return true;
  }
  else
    return false;
}

VCL_DEFINE_SPECIALIZATION
inline bool convert_planes_from_components(vil_image_view<vxl_uint_16> &lhs,
                                           const vil_image_view_base &rhs_base)
{
  const unsigned ncomp =
    vil_pixel_format_num_components(rhs_base.pixel_format());

  if (// rhs has just 1 plane
      rhs_base.nplanes() == 1 &&
      // both sides have equal component types
      vil_pixel_format_component_format(rhs_base.pixel_format()) == VIL_PIXEL_FORMAT_UINT_16)
  {
    // cheat by casting to component type, not pixel type (because we don't know full pixel type at compile time.)
    const vil_image_view<vxl_uint_16> &rhs = static_cast<const vil_image_view<vxl_uint_16>&>(rhs_base);

    lhs = vil_image_view<vxl_uint_16>(rhs.memory_chunk(), rhs.top_left_ptr(),
                                      rhs.ni(),rhs.nj(),ncomp,
                                      rhs.istep()*ncomp,rhs.jstep()*ncomp,1);
    return true;
  }
  else
    return false;
}

VCL_DEFINE_SPECIALIZATION
inline bool convert_planes_from_components(vil_image_view<vxl_int_16> &lhs,
                                           const vil_image_view_base &rhs_base)
{
  const unsigned ncomp =
    vil_pixel_format_num_components(rhs_base.pixel_format());

  if (// rhs has just 1 plane
      rhs_base.nplanes() == 1 &&
      // both sides have equal component types
      vil_pixel_format_component_format(rhs_base.pixel_format()) == VIL_PIXEL_FORMAT_INT_16)
  {
    // cheat by casting to component type, not pixel type (because we don't know full pixel type at compile time.)
    const vil_image_view<vxl_int_16> &rhs = static_cast<const vil_image_view<vxl_int_16>&>(rhs_base);

    lhs = vil_image_view<vxl_int_16>(rhs.memory_chunk(), rhs.top_left_ptr(),
                                     rhs.ni(),rhs.nj(),ncomp,
                                     rhs.istep()*ncomp,rhs.jstep()*ncomp,1);
    return true;
  }
  else
    return false;
}

VCL_DEFINE_SPECIALIZATION
inline bool convert_planes_from_components(vil_image_view<vxl_uint_32> &lhs,
                                           const vil_image_view_base &rhs_base)
{
  const unsigned ncomp =
    vil_pixel_format_num_components(rhs_base.pixel_format());

  if (// rhs has just 1 plane
      rhs_base.nplanes() == 1 &&
      // both sides have equal component types
       vil_pixel_format_component_format(rhs_base.pixel_format()) == VIL_PIXEL_FORMAT_UINT_32)
  {
    // cheat by casting to component type, not pixel type (because we don't know full pixel type at compile time.)
    const vil_image_view<vxl_uint_32> &rhs = static_cast<const vil_image_view<vxl_uint_32>&>(rhs_base);

    lhs = vil_image_view<vxl_uint_32>(rhs.memory_chunk(), rhs.top_left_ptr(),
                                      rhs.ni(),rhs.nj(),ncomp,
                                      rhs.istep()*ncomp,rhs.jstep()*ncomp,1);
    return true;
  }
  else
    return false;
}

VCL_DEFINE_SPECIALIZATION
inline bool convert_planes_from_components(vil_image_view<vxl_int_32> &lhs,
                                           const vil_image_view_base &rhs_base)
{
  const unsigned ncomp =
    vil_pixel_format_num_components(rhs_base.pixel_format());

  if (// rhs has just 1 plane
      rhs_base.nplanes() == 1 &&
      // both sides have equal component types
      vil_pixel_format_component_format(rhs_base.pixel_format()) == VIL_PIXEL_FORMAT_INT_32)
  {
    // cheat by casting to component type, not pixel type (because we don't know full pixel type at compile time.)
    const vil_image_view<vxl_int_32> &rhs = static_cast<const vil_image_view<vxl_int_32>&>(rhs_base);

    lhs = vil_image_view<vxl_int_32>(rhs.memory_chunk(), rhs.top_left_ptr(),
                                     rhs.ni(),rhs.nj(),ncomp,
                                     rhs.istep()*ncomp,rhs.jstep()*ncomp,1);
    return true;
  }
  else
    return false;
}

#if VXL_HAS_INT_64

VCL_DEFINE_SPECIALIZATION
inline bool convert_planes_from_components(vil_image_view<vxl_uint_64> &lhs,
                                           const vil_image_view_base &rhs_base)
{
  const unsigned ncomp =
    vil_pixel_format_num_components(rhs_base.pixel_format());

  if (// rhs has just 1 plane
      rhs_base.nplanes() == 1 &&
      // both sides have equal component types
       vil_pixel_format_component_format(rhs_base.pixel_format()) == VIL_PIXEL_FORMAT_UINT_64)
  {
    // cheat by casting to component type, not pixel type (because we don't know full pixel type at compile time.)
    const vil_image_view<vxl_uint_64> &rhs = static_cast<const vil_image_view<vxl_uint_64>&>(rhs_base);

    lhs = vil_image_view<vxl_uint_64>(rhs.memory_chunk(), rhs.top_left_ptr(),
                                      rhs.ni(),rhs.nj(),ncomp,
                                      rhs.istep()*ncomp,rhs.jstep()*ncomp,1);
    return true;
  }
  else
    return false;
}

VCL_DEFINE_SPECIALIZATION
inline bool convert_planes_from_components(vil_image_view<vxl_int_64> &lhs,
                                           const vil_image_view_base &rhs_base)
{
  const unsigned ncomp =
    vil_pixel_format_num_components(rhs_base.pixel_format());

  if (// rhs has just 1 plane
      rhs_base.nplanes() == 1 &&
      // both sides have equal component types
      vil_pixel_format_component_format(rhs_base.pixel_format()) == VIL_PIXEL_FORMAT_INT_64)
  {
    // cheat by casting to component type, not pixel type (because we don't know full pixel type at compile time.)
    const vil_image_view<vxl_int_64> &rhs = static_cast<const vil_image_view<vxl_int_64>&>(rhs_base);

    lhs = vil_image_view<vxl_int_64>(rhs.memory_chunk(), rhs.top_left_ptr(),
                                     rhs.ni(),rhs.nj(),ncomp,
                                     rhs.istep()*ncomp,rhs.jstep()*ncomp,1);
    return true;
  }
  else
    return false;
}

#endif // VXL_HAS_INT_64

VCL_DEFINE_SPECIALIZATION
inline bool convert_planes_from_components(vil_image_view<float> &lhs,
                                           const vil_image_view_base &rhs_base)
{
  const unsigned ncomp =
    vil_pixel_format_num_components(rhs_base.pixel_format());

  if (// rhs has just 1 plane
      rhs_base.nplanes() == 1 &&
      // both sides have equal component types
      vil_pixel_format_component_format(rhs_base.pixel_format()) == VIL_PIXEL_FORMAT_FLOAT)
  {
    // cheat by casting to component type, not pixel type (because we don't know full pixel type at compile time.)
    const vil_image_view<float> &rhs = static_cast<const vil_image_view<float>&>(rhs_base);

    lhs = vil_image_view<float>(rhs.memory_chunk(), rhs.top_left_ptr(),
                                rhs.ni(),rhs.nj(),ncomp,
                                rhs.istep()*ncomp,rhs.jstep()*ncomp,1);
    return true;
  }
  else
    return false;
}

VCL_DEFINE_SPECIALIZATION
inline bool convert_planes_from_components(vil_image_view<double> &lhs,
                                           const vil_image_view_base &rhs_base)
{
  const unsigned ncomp =
    vil_pixel_format_num_components(rhs_base.pixel_format());

  if (// rhs has just 1 plane
      rhs_base.nplanes() == 1 &&
      // both sides have equal component types
      vil_pixel_format_component_format(rhs_base.pixel_format()) == VIL_PIXEL_FORMAT_DOUBLE)
  {
    // cheat by casting to component type, not pixel type (because we don't know full pixel type at compile time.)
    const vil_image_view<double> &rhs = static_cast<const vil_image_view<double>&>(rhs_base);

    lhs = vil_image_view<double>(rhs.memory_chunk(), rhs.top_left_ptr(),
                                 rhs.ni(),rhs.nj(),ncomp,
                                 rhs.istep()*ncomp,rhs.jstep()*ncomp,1);
    return true;
  }
  else
    return false;
}


template<class T>
const vil_image_view<T> & vil_image_view<T>::operator= (const vil_image_view<T> & rhs)
{
  return operator=( static_cast<vil_image_view_base const&>(rhs) );
}


template<class T>
const vil_image_view<T> & vil_image_view<T>::operator= (const vil_image_view_base & rhs)
{
  if (static_cast<const vil_image_view_base*>(this) == &rhs)
    return *this;

  if (rhs.pixel_format() == pixel_format())
  {
    const vil_image_view<T> &that = static_cast<const vil_image_view<T>&>(rhs);
    ni_=that.ni_;
    nj_=that.nj_;
    nplanes_=that.nplanes_;
    istep_=that.istep_;
    jstep_=that.jstep_;
    planestep_=that.planestep_;
    top_left_=that.top_left_;
    ptr_=that.ptr_;
    return *this;
  }

  if (convert_components_from_planes(*this, rhs))
    return *this;

  if (convert_planes_from_components(*this, rhs))
    return *this;

  set_to_memory(0, 0, 0, 0, 0, 0, 0);
  return *this;
}


//=======================================================================


template<class T>
void vil_image_view<T>::set_size(unsigned n_i, unsigned n_j)
{
  set_size(n_i,n_j, nplanes_);
}

//: True if data all in one unbroken block and top_left_ptr() is lowest data address
template<class T>
bool vil_image_view<T>::is_contiguous() const
{
  // For a contiguous image, the smallest step size should be 1, the
  // next step size should be the width of corresponding to the
  // smallest step size, and so on. So, sort the step sizes and check
  // if this is the case.

  // Sort the step sizes in ascending order, and keep the
  // corresponding widths.

  vcl_ptrdiff_t s1, s2, s3;
  unsigned n1, n2;
  if ( istep_ < jstep_ )
    if ( jstep_ < planestep_ )
    {
      s1 = istep_; s2 = jstep_; s3 = planestep_;
      n1 = ni_;    n2 = nj_;  //  n3 = nplanes_;
    }
    else // planestep_ < jstep_
      if ( istep_ < planestep_ )
      {
        s1 = istep_; s2 = planestep_; s3 = jstep_;
        n1 = ni_;    n2 = nplanes_; //  n3 = nj_;
      }
      else // planestep_ < istep_
      {
        s1 = planestep_; s2 = istep_; s3 = jstep_;
        n1 = nplanes_;   n2 = ni_;  //  n3 = nj_;
      }
  else // jstep < istep_
    if ( jstep_ < planestep_ )
      if ( istep_ < planestep_ )
      {
        s1 = jstep_; s2 = istep_; s3 = planestep_;
        n1 = nj_;    n2 = ni_;  //  n3 = nplanes_;
      }
      else // planestep_ < istep_
      {
        s1 = jstep_; s2 = planestep_; s3 = istep_;
        n1 = nj_;    n2 = nplanes_;  // n3 = ni_;
      }
    else // planestep_ < jstep_
    {
      s1 = planestep_; s2 = jstep_; s3 = istep_;
      n1 = nplanes_;   n2 = nj_;  //  n3 = ni_;
    }

  return s1 == 1 &&
         s2 > 0 && unsigned(s2) == n1 &&
         s3 > 0 && unsigned(s3) == n1*n2;
}

//=======================================================================

template<class T>
void vil_image_view<T>::set_size(unsigned n_i, unsigned n_j, unsigned n_planes)
{
  if (n_i==ni_ && n_j==nj_ && n_planes==nplanes_) return;

  release_memory();

  vil_pixel_format fmt = vil_pixel_format_of(T());
  ptr_ = new vil_memory_chunk(sizeof(T)*n_planes*n_j*n_i,
                              vil_pixel_format_component_format(fmt));

  ni_ = n_i;
  nj_ = n_j;
  nplanes_ = n_planes;
  if (istep_==0) istep_ = 1;
  jstep_ = n_i*istep_;
  planestep_ = istep_==1 ? n_i*n_j : 1;

  top_left_ = reinterpret_cast<T*>(ptr_->data());
}


//: Set this view to look at someone else's memory.
template<class T>
void vil_image_view<T>::set_to_memory(const T* top_left,
                                      unsigned n_i, unsigned n_j, unsigned n_planes,
                                      vcl_ptrdiff_t i_step, vcl_ptrdiff_t j_step, vcl_ptrdiff_t plane_step)
{
  release_memory();
  top_left_ = const_cast<T*>(top_left);  // Remove const, as view may end up manipulating data

  ni_ = n_i;
  nj_ = n_j;
  nplanes_ = n_planes;
  istep_ = i_step;
  jstep_ = j_step;
  planestep_ = plane_step;
}


//=======================================================================
//: Fill view with given value
template<class T>
void vil_image_view<T>::fill(T value)
{
  T* plane = top_left_;

  if (is_contiguous())
  {
    iterator it = begin();
    const_iterator end_it = end();
    while (it!=end_it) {*it = value; ++it; }
    return;
  }

  if (istep_==1)
  {
    for (unsigned int p=0;p<nplanes_;++p,plane += planestep_)
    {
      T* row = plane;
      for (unsigned int j=0;j<nj_;++j,row += jstep_)
      {
        int i = ni_;
        while (i!=0) { row[--i]=value; }
      }
    }
    return;
  }

  if (jstep_==1)
  {
    for (unsigned int p=0;p<nplanes_;++p,plane += planestep_)
    {
      T* col = plane;
      for (unsigned int i=0;i<ni_;++i,col += istep_)
      {
        int j = nj_;
        while (j!=0) { col[--j]=value; }
      }
    }
    return;
  }

  for (unsigned int p=0;p<nplanes_;++p,plane += planestep_)
  {
    T* row = plane;
    for (unsigned int j=0;j<nj_;++j,row += jstep_)
    {
      T* p = row;
      for (unsigned int i=0;i<ni_;++i,p+=istep_) *p = value;
    }
  }
}

//=======================================================================

template<class T>
bool vil_image_view<T>::is_class(vcl_string const& s) const
{
  return s==vil_image_view<T>::is_a() || vil_image_view_base::is_class(s);
}

//=======================================================================

template<class T>
void vil_image_view<T>::print(vcl_ostream& os) const
{
  os<<nplanes_<<" planes, each "<<ni_<<" x "<<nj_;
}

//=======================================================================
//: True if they share same view of same image data.
//  This does not do a deep equality on image data. If the images point
//  to different image data objects that contain identical images, then
//  the result will still be false.
template<class T>
bool vil_image_view<T>::operator==(const vil_image_view_base &rhs) const
{
  if (rhs.pixel_format() != pixel_format()) return false;

  const vil_image_view<T> & other = static_cast<const vil_image_view<T> &>(rhs);

  if (this == &other) return true;

  if (!(bool) *this && !(bool)other) return true;
  return ptr_  == other.ptr_ &&
    top_left_  == other.top_left_ &&
    nplanes_   == other.nplanes_ &&
    ni_        == other.ni_ &&
    nj_        == other.nj_ &&
    (nplanes_ <= 1 || planestep_ == other.planestep_) &&
    istep_     == other.istep_ &&
    jstep_     == other.jstep_;
}

//=======================================================================
//: Provides an ordering.
//  Useful for ordered containers.
//  There is no guaranteed meaning to the less than operator, except that
//  (a<b && b<a)  is false and  !(a<b) && !(b<a)  is equivalent to  a==b
template<class T>
bool vil_image_view<T>::operator<(const vil_image_view_base& rhs) const
{
  if (rhs.pixel_format() != pixel_format()) return pixel_format() < rhs.pixel_format();

  const vil_image_view<T> & other = static_cast<const vil_image_view<T> &>(rhs);
  if (ptr_ != other.ptr_) return ptr_<other.ptr_;
  if (!(bool) *this && !(bool)other) return false;
  if (nplanes_ != other.nplanes_) return nplanes_ < other.nplanes_;
  if (ni_ != other.ni_) return ni_ < other.ni_;
  if (nj_ != other.nj_) return nj_ < other.nj_;
  if (planestep_ != other.planestep_) return planestep_ < other.planestep_;
  if (istep_ != other.istep_) return istep_ < other.istep_;
  return jstep_ < other.jstep_;
}


//=======================================================================
//: Provides an ordering.
//  Useful for ordered containers.
//  There is no guaranteed meaning to the less than operator, except that
//  (a>b) is equivalent to (b<a)
template<class T>
bool vil_image_view<T>::operator>(const vil_image_view_base& rhs) const
{
  if (rhs.pixel_format() != pixel_format()) return pixel_format() > rhs.pixel_format();

  const vil_image_view<T> & other = static_cast<const vil_image_view<T> &>(rhs);

  if (this == &other) return false;

  if (ptr_ != other.ptr_) return ptr_>other.ptr_;
  if (!(bool) *this && !(bool)other) return false;
  if (nplanes_ != other.nplanes_) return nplanes_ > other.nplanes_;
  if (ni_ != other.ni_) return ni_ > other.ni_;
  if (nj_ != other.nj_) return nj_ > other.nj_;
  if (planestep_ != other.planestep_) return planestep_ > other.planestep_;
  if (istep_ != other.istep_) return istep_ > other.istep_;
  return jstep_ > other.jstep_;
}


//=======================================================================
//: True if the actual images are identical.
// $\bigwedge_{i,j,p} {\textstyle src}(i,j,p) == {\textstyle dest}(i,j,p)$
// The data may be formatted differently in each memory chunk.
//  O(size).
// \relates vil_image_view
template<class T>
bool vil_image_view_deep_equality(const vil_image_view<T> &lhs,
                                  const vil_image_view<T> &rhs)
{
  if (lhs.nplanes() != rhs.nplanes() ||
      lhs.nj() != rhs.nj() ||
      lhs.ni() != rhs.ni())
    return false;

  for (unsigned p = 0; p < rhs.nplanes(); ++p)
    for (unsigned j = 0; j < rhs.nj(); ++j)
      for (unsigned i = 0; i < rhs.ni(); ++i)
        if (!(rhs(i,j,p) == lhs(i,j,p)))
          return false;
  return true;
}

//=======================================================================

// Specializations must be declared in all translation units where
// they are used.  Since we do not know what instantiations will be
// defined, and each requires a specialization, we define the primary
// template of is_a to call a function that will be declared and
// specialized only in the instantiation translation units.
template <class T> vcl_string vil_image_view_type_name(T*);

template <class T>
vcl_string vil_image_view<T>::is_a() const
{
  return vil_image_view_type_name(static_cast<T*>(0));
}

#define VIL_IMAGE_VIEW_INSTANTIATE(T) \
VCL_DEFINE_SPECIALIZATION vcl_string vil_image_view_type_name(T*) \
{ return vcl_string("vil_image_view<" #T ">"); } \
template class vil_image_view<T >; \
template bool vil_image_view_deep_equality(const vil_image_view<T >&, \
                                           const vil_image_view<T >&)

#endif // vil_image_view_txx_
