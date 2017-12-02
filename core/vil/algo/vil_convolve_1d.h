// This is core/vil/algo/vil_convolve_1d.h
#ifndef vil_convolve_1d_h_
#define vil_convolve_1d_h_
//:
// \file
// \brief 1D Convolution with cunning boundary options
// \author Tim Cootes, Ian Scott (based on work by fsm)
//
// \note  The convolution operation is defined by
//    $(f*g)(x) = \int f(x-y) g(y) dy$
// i.e. the kernel g is reflected before the integration is performed.
// If you don't want this to happen, the behaviour you want is not
// called "convolution". So don't break the convolution routines in
// that particular way.

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vcl_compiler.h>
#include <vcl_compiler.h>
#include <vcl_cassert.h>
#include <vil/vil_image_view.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_property.h>


//: Available options for boundary behavior
// When convolving a finite signal the boundaries may be
// treated in various ways which can often be expressed in terms
// of ways to extend the signal outside its original range.
enum vil_convolve_boundary_option
{
  //: Do not fill destination edges at all.
  // i.e. leave them unchanged.
  vil_convolve_ignore_edge,

  //: Do not to extend the signal, but pad with zeros.
  // \verbatim
  //     |                               |
  // K                       ----*-------
  // in   ... ---------------------------
  // out  ... --------------------0000000
  // \endverbatim
  vil_convolve_no_extend,

  //: Zero-extend the input signal beyond the boundary.
  // \verbatim
  //     |                               |
  // K                              ----*--------
  // in   ... ---------------------------000000000000...
  // out  ... ---------------------------
  // \endverbatim
  vil_convolve_zero_extend,

  //: Extend the signal to be constant beyond the boundary.
  // \verbatim
  //     |                               |
  // K                              ----*--------
  // in   ... --------------------------aaaaaaaaaaaaa...
  // out  ... ---------------------------
  // \endverbatim
  vil_convolve_constant_extend,

  //: Extend the signal periodically beyond the boundary.
  // \verbatim
  //     |                               |
  // K                              ----*--------
  // in   abc...-------------------------abc...------..
  // out  ... ---------------------------
  // \endverbatim
  vil_convolve_periodic_extend,

  //: Extend the signal by reflection about the boundary.
  // \verbatim
  //     |                               |
  // K                              ----*--------
  // in   ... -------------------...edcbabcde...
  // out  ... ---------------------------
  // \endverbatim
  vil_convolve_reflect_extend,

  //: Kernel is trimmed and reweighed, to allow convolution up to boundary.
  // This one is slightly different. The input signal is not
  // extended in any way, but the kernel is trimmed to allow
  // convolution to proceed up to the boundary and reweighed
  // to keep the total area the same.
  // \note may not work with kernels which take negative values.
  vil_convolve_trim
};

//: Convolve edge with kernel[x*kstep] x in [k_lo,k_hi] (k_hi>=0)
//  Fills only edge: dest[i], i=0..(k_hi-1)
template <class srcT, class destT, class kernelT, class accumT>
inline void vil_convolve_edge_1d(const srcT* src, unsigned n, std::ptrdiff_t s_step,
                                 destT* dest, std::ptrdiff_t d_step,
                                 const kernelT* kernel,
                                 std::ptrdiff_t k_lo, std::ptrdiff_t k_hi,
                                 std::ptrdiff_t kstep, accumT,
                                 vil_convolve_boundary_option option)
{
  switch (option)
  {
   case vil_convolve_ignore_edge:
    return;
   case vil_convolve_no_extend:
    // Initialise first elements of row to zero
    for (std::ptrdiff_t i=-k_hi;i<0;++i,dest+=d_step)
      *dest = 0;
    return;
   case vil_convolve_zero_extend:
    // Assume src[i]==0 for i<0
//    for (std::ptrdiff_t i=-k_hi+1;i<=0;++i,dest+=d_step,src+=s_step)
    for (std::ptrdiff_t i=0;i<k_hi;++i,dest+=d_step)
    {
      accumT sum = 0;
      const srcT* s = src;
      const kernelT* k = kernel+i*kstep;
      for (std::ptrdiff_t j=i;j>=k_lo;--j,s+=s_step,k-=kstep)
        sum+= (accumT)((*s)*(*k));
      *dest=(destT)sum;
    }
    return;
   case vil_convolve_constant_extend:
   {
    // Assume src[i]=src[0] for i<0
    std::ptrdiff_t i_max = k_hi-1;
    for (std::ptrdiff_t i=0;i<=i_max;++i)
    {
      accumT sum=0;
      for (std::ptrdiff_t j=-k_hi;j<=-k_lo;++j)
      {
        if ((i+j)<0) sum+=(accumT)(src[0]*kernel[j*(-kstep)]);
        else         sum+=(accumT)(src[(i+j)*s_step]*kernel[j*(-kstep)]);
      }
      dest[i*d_step]=(destT)sum;
    }
    return;
   }
   case vil_convolve_reflect_extend:
   {
    // Assume src[i]=src[0] for i<0
    std::ptrdiff_t i_max = k_hi-1;
    for (std::ptrdiff_t i=0;i<=i_max;++i)
    {
      accumT sum=0;
      for (std::ptrdiff_t j=-k_hi;j<=-k_lo;++j)
      {
        if ((i+j)<0) sum+=(accumT)(src[-(i+j)*s_step]*kernel[j*(-kstep)]);
        else         sum+=(accumT)(src[(i+j)*s_step]*kernel[j*(-kstep)]);
      }
      dest[i*d_step]=(destT)sum;
    }
    return;
   }
   case vil_convolve_periodic_extend:
   {
    // Assume src[i]=src[n+i] for i<0
    std::ptrdiff_t i_max = k_hi-1;
    for (int i=0;i<=i_max;++i)
    {
      accumT sum=0;
      for (std::ptrdiff_t j=k_hi;j>=k_lo;--j)
        sum+=(accumT)(src[((i-j+n)%n)*s_step]*kernel[j*kstep]);
      dest[i*d_step]=(destT)sum;
    }
    return;
   }
   case vil_convolve_trim:
   {
    // Truncate and reweight kernel
    accumT k_sum_all=0;
    for (std::ptrdiff_t j=-k_hi;j<=-k_lo;++j) k_sum_all+=(accumT)(kernel[j*(-kstep)]);

    std::ptrdiff_t i_max = k_hi-1;
    for (std::ptrdiff_t i=0;i<=i_max;++i)
    {
      accumT sum=0;
      accumT k_sum=0;
      // Sum elements which overlap src
      // ie i+j>=0  (so j starts at -i)
      for (std::ptrdiff_t j=-i;j<=-k_lo;++j)
      {
        sum+=(accumT)(src[(i+j)*s_step]*kernel[j*(-kstep)]);
        k_sum += (accumT)(kernel[j*(-kstep)]);
      }
      dest[i*d_step]=(destT)(sum*k_sum_all/k_sum);
    }
    return;
   }
   default:
    std::cout<<"ERROR: vil_convolve_edge_1d: "
            <<"Sorry, can't deal with supplied edge option.\n";
    std::abort();
  }
}

//: Convolve kernel[x] (x in [k_lo,k_hi]) with srcT
// Assumes dest and src same size (nx)
// Kernel must not be larger than nx;
template <class srcT, class destT, class kernelT, class accumT>
inline void vil_convolve_1d(const srcT* src0, unsigned nx, std::ptrdiff_t s_step,
                            destT* dest0, std::ptrdiff_t d_step,
                            const kernelT* kernel,
                            std::ptrdiff_t k_lo, std::ptrdiff_t k_hi,
                            accumT ac,
                            vil_convolve_boundary_option start_option,
                            vil_convolve_boundary_option end_option)
{
  assert(k_hi - k_lo < int(nx));

  // Deal with start (fill elements 0..1+k_hi of dest)
  vil_convolve_edge_1d(src0,nx,s_step,dest0,d_step,kernel,k_lo,k_hi,1,ac,start_option);

  const kernelT* k_rbegin = kernel+k_hi;
  const kernelT* k_rend   = kernel+k_lo-1;
  assert(k_rbegin >= k_rend);
  const srcT* src = src0;

  for (destT       * dest = dest0 + d_step*k_hi,
       * const   end_dest = dest0 + d_step*(int(nx)+k_lo);
       dest!=end_dest;
       dest+=d_step,src+=s_step)
  {
    accumT sum = 0;
    const srcT* s= src;
    for (const kernelT *k = k_rbegin;k!=k_rend;--k,s+=s_step)
      sum+= (accumT)((*k)*(*s));
    *dest = destT(sum);
  }

  // Deal with end  (reflect data and kernel!)
  vil_convolve_edge_1d(src0+(nx-1)*s_step,nx,-s_step,
                       dest0+(nx-1)*d_step,-d_step,
                       kernel,-k_hi,-k_lo,-1,ac,end_option);
}

//: Convolve kernel[i] (i in [k_lo,k_hi]) with srcT in i-direction
// On exit dest_im(i,j) = sum src(i-x,j)*kernel(x)  (x=k_lo..k_hi)
// \note  This function reverses the kernel. If you don't want the
// kernel reversed, use vil_correlate_1d instead. The kernel must
// not be larger than src_im.ni()
// \param kernel should point to tap 0.
// \param dest_im will be resized to size of src_im.
// \relatesalso vil_image_view
template <class srcT, class destT, class kernelT, class accumT>
inline void vil_convolve_1d(const vil_image_view<srcT>& src_im,
                            vil_image_view<destT>& dest_im,
                            const kernelT* kernel,
                            std::ptrdiff_t k_lo, std::ptrdiff_t k_hi,
                            accumT ac,
                            vil_convolve_boundary_option start_option,
                            vil_convolve_boundary_option end_option)
{
  unsigned n_i = src_im.ni();
  unsigned n_j = src_im.nj();
  assert(k_hi - k_lo +1 <= (int) n_i);
  std::ptrdiff_t s_istep = src_im.istep(), s_jstep = src_im.jstep();

  dest_im.set_size(n_i,n_j,src_im.nplanes());
  std::ptrdiff_t d_istep = dest_im.istep(),d_jstep = dest_im.jstep();

  for (unsigned int p=0;p<src_im.nplanes();++p)
  {
    // Select first row of p-th plane
    const srcT*  src_row  = src_im.top_left_ptr()+p*src_im.planestep();
    destT* dest_row = dest_im.top_left_ptr()+p*dest_im.planestep();

    // Apply convolution to each row in turn
    // First check if either istep is 1 for speed optimisation.

    if (s_istep == 1)
    {
      if (d_istep == 1)
        for (unsigned int j=0;j<n_j;++j,src_row+=s_jstep,dest_row+=d_jstep)
          vil_convolve_1d(src_row,n_i,1,  dest_row,1,
                          kernel,k_lo,k_hi,ac,start_option,end_option);
      else
        for (unsigned int j=0;j<n_j;++j,src_row+=s_jstep,dest_row+=d_jstep)
          vil_convolve_1d(src_row,n_i,1,  dest_row,d_istep,
                          kernel,k_lo,k_hi,ac,start_option,end_option);
    }
    else
    {
      if (d_istep == 1)
        for (unsigned int j=0;j<n_j;++j,src_row+=s_jstep,dest_row+=d_jstep)
          vil_convolve_1d(src_row,n_i,s_istep,  dest_row,1,
                          kernel,k_lo,k_hi,ac,start_option,end_option);
      else
        for (unsigned int j=0;j<n_j;++j,src_row+=s_jstep,dest_row+=d_jstep)
          vil_convolve_1d(src_row,n_i,s_istep,  dest_row,d_istep,
                          kernel,k_lo,k_hi,ac,start_option,end_option);
    }
  }
}

template <class destT, class kernelT, class accumT>
vil_image_resource_sptr vil_convolve_1d(
               const vil_image_resource_sptr& src_im,
               const destT dt,
               const kernelT* kernel, int k_lo, int k_hi,
               const accumT ac,
               vil_convolve_boundary_option start_option,
               vil_convolve_boundary_option end_option);

//: A resource adaptor that behaves like a convolved version of its input
template <class kernelT, class accumT, class destT>
class vil_convolve_1d_resource : public vil_image_resource
{
  //: Construct a convolve filter.
  // You can't create one of these directly, use vil_convolve_1d instead
  vil_convolve_1d_resource(const vil_image_resource_sptr& src,
                           const kernelT* kernel, int k_lo, int k_hi,
                           vil_convolve_boundary_option start_option,
                           vil_convolve_boundary_option end_option)  :
      src_(src), kernel_(kernel), klo_(k_lo), khi_(k_hi),
      start_option_(start_option), end_option_(end_option)
    {
      // Can't do period extension yet.
      assert (start_option != vil_convolve_periodic_extend ||
              end_option != vil_convolve_periodic_extend);
    }

  friend vil_image_resource_sptr vil_convolve_1d VCL_NULL_TMPL_ARGS (
    const vil_image_resource_sptr& src_im, const destT dt, const kernelT* kernel,
    int k_lo, int k_hi, const accumT ac,
    vil_convolve_boundary_option start_option,
    vil_convolve_boundary_option end_option);

 public:
  virtual vil_image_view_base_sptr get_copy_view(unsigned i0, unsigned n_i,
                                                 unsigned j0, unsigned n_j) const
  {
    if (i0 + n_i > src_->ni() || j0 + n_j > src_->nj())  return 0;
    const unsigned lsrc = (unsigned) std::max(0,(int)i0 + klo_); // lhs of input window
    const unsigned hsrc = std::min(src_->ni(),i0 + n_i - klo_ + khi_); // 1+rhs of input window.
    const unsigned lboundary = std::min((unsigned) -klo_, i0); // width of lhs boundary area.
    assert (hsrc > lsrc);
    vil_image_view_base_sptr vs = src_->get_view(lsrc, hsrc-lsrc, j0, n_j);
    vil_image_view<destT> dest(vs->ni(), vs->nj(), vs->nplanes());
    switch (vs->pixel_format())
    {
#define macro( F , T ) \
     case F : \
      vil_convolve_1d(static_cast<vil_image_view<T >&>(*vs),dest, \
                      kernel_, klo_, khi_, accumT(), start_option_, end_option_); \
      return new vil_image_view<destT>(vil_crop(dest, lboundary, n_i, 0, n_j));

      macro(VIL_PIXEL_FORMAT_BYTE , vxl_byte )
      macro(VIL_PIXEL_FORMAT_SBYTE , vxl_sbyte )
      macro(VIL_PIXEL_FORMAT_UINT_32 , vxl_uint_32 )
      macro(VIL_PIXEL_FORMAT_UINT_16 , vxl_uint_16 )
      macro(VIL_PIXEL_FORMAT_INT_32 , vxl_int_32 )
      macro(VIL_PIXEL_FORMAT_INT_16 , vxl_int_16 )
      macro(VIL_PIXEL_FORMAT_BOOL , bool )
      macro(VIL_PIXEL_FORMAT_FLOAT , float )
      macro(VIL_PIXEL_FORMAT_DOUBLE , double )
// complex<float> should work - but causes all manner of compiler template errors.
// maybe need a better compiler, maybe there is a code fix - IMS
#undef macro
     default:
      return 0;
    }
  }

  virtual unsigned nplanes() const { return src_->nplanes(); }
  virtual unsigned ni() const { return src_->ni(); }
  virtual unsigned nj() const { return src_->nj(); }

  virtual enum vil_pixel_format pixel_format() const
  { return vil_pixel_format_of(accumT()); }


  //: Put the data in this view back into the image source.
  virtual bool put_view(const vil_image_view_base&  /*im*/, unsigned  /*i0*/, unsigned  /*j0*/)
  {
    std::cerr << "WARNING: vil_convolve_1d_resource::put_back\n"
             << "\tYou can't push data back into a convolve filter.\n";
    return false;
  }

  //: Extra property information
  virtual bool get_property(char const* tag, void* property_value = 0) const
  {
    if (0==std::strcmp(tag, vil_property_read_only))
      return property_value ? (*static_cast<bool*>(property_value)) = true : true;

    return src_->get_property(tag, property_value);
  }

 protected:
  vil_image_resource_sptr src_;
  const kernelT* kernel_;
  int klo_, khi_;
  vil_convolve_boundary_option start_option_, end_option_;
};

//: Create an image_resource object which convolve kernel[x] x in [k_lo,k_hi] with srcT
// \note  This function reverses the kernel. If you don't want the
// kernel reversed, use vil_correlate_1d instead.
// \param kernel should point to tap 0.
// \relatesalso vil_image_resource
template <class destT, class kernelT, class accumT>
vil_image_resource_sptr vil_convolve_1d(
                         const vil_image_resource_sptr& src_im,
                         const destT  /*dt*/,
                         const kernelT* kernel, int k_lo, int k_hi,
                         const accumT,
                         vil_convolve_boundary_option start_option,
                         vil_convolve_boundary_option end_option)
{
  return new vil_convolve_1d_resource<kernelT, accumT, destT>(src_im, kernel, k_lo, k_hi, start_option, end_option);
}

#endif // vil_convolve_1d_h_

