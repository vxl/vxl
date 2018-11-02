// This is core/vil/algo/vil_correlate_1d.h
#ifndef vil_correlate_1d_h_
#define vil_correlate_1d_h_
//:
// \file
// \brief 1D Convolution with cunning boundary options
// \author Tim Cootes (based on work by fsm)

#include <algorithm>
#include <cstring>
#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <vil/vil_image_view.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_property.h>
#include <vil/algo/vil_convolve_1d.h>

//: Correlate kernel[x] (x in [k_lo,k_hi]) with srcT
// Assumes dest and src same size (nx)
template <class srcT, class destT, class kernelT, class accumT>
inline void vil_correlate_1d(const srcT* src0, unsigned nx, std::ptrdiff_t s_step,
                             destT* dest0, std::ptrdiff_t d_step,
                             const kernelT* kernel,
                             std::ptrdiff_t k_lo, std::ptrdiff_t k_hi,
                             accumT ac,
                             vil_convolve_boundary_option start_option,
                             vil_convolve_boundary_option end_option)
{
  // Deal with start (fill elements 0..1-k_lo of dest)
  vil_convolve_edge_1d(src0,nx,s_step,dest0,d_step,kernel,-k_hi,-k_lo,-1,ac,start_option);

  const kernelT* k_begin = kernel+k_lo;
  const kernelT* k_end   = kernel+k_hi+1;
  const srcT* src = src0;

  destT* end_dest = dest0 + d_step*(int(nx)-k_hi);
  for (destT* dest = dest0-d_step*k_lo;dest!=end_dest;dest+=d_step,src+=s_step)
  {
    accumT sum = 0;
    const srcT* s= src;
    for (const kernelT *k = k_begin;k!=k_end; ++k,s+=s_step) sum+= (accumT)((*k)*(*s));
    *dest = destT(sum);
  }

  // Deal with end  (reflect data and kernel!)
  vil_convolve_edge_1d(src0+(nx-1)*s_step,nx,-s_step,
                       dest0+(nx-1)*d_step,-d_step,
                       kernel,k_lo,k_hi,1,ac,end_option);
}

//: correlate kernel[i] (i in [k_lo,k_hi]) with srcT in i-direction
// On exit dest_im(i,j) = sum src(i+x,j)*kernel(x)  (x=k_lo..k_hi)
// \note  This function does not reverse the kernel. If you want the
// kernel reversed, use vil_convolve_1d instead.
// \param kernel should point to tap 0.
// \param dest_im will be resized to size of src_im.
// \relatesalso vil_image_view
template <class srcT, class destT, class kernelT, class accumT>
inline void vil_correlate_1d(const vil_image_view<srcT>& src_im,
                             vil_image_view<destT>& dest_im,
                             const kernelT* kernel,
                             std::ptrdiff_t k_lo, std::ptrdiff_t k_hi,
                             accumT ac,
                             vil_convolve_boundary_option start_option,
                             vil_convolve_boundary_option end_option)
{
  unsigned ni = src_im.ni();
  unsigned nj = src_im.nj();
  std::ptrdiff_t s_istep = src_im.istep(), s_jstep = src_im.jstep();

  dest_im.set_size(ni,nj,src_im.nplanes());
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
        for (unsigned int j=0;j<nj;++j,src_row+=s_jstep,dest_row+=d_jstep)
          vil_correlate_1d(src_row,ni,1,  dest_row,1,
                           kernel,k_lo,k_hi,ac,start_option,end_option);
      else
        for (unsigned int j=0;j<nj;++j,src_row+=s_jstep,dest_row+=d_jstep)
          vil_correlate_1d(src_row,ni,1,  dest_row,d_istep,
                           kernel,k_lo,k_hi,ac,start_option,end_option);
    }
    else
    {
      if (d_istep == 1)
        for (unsigned int j=0;j<nj;++j,src_row+=s_jstep,dest_row+=d_jstep)
          vil_correlate_1d(src_row,ni,s_istep,  dest_row,1,
                           kernel,k_lo,k_hi,ac,start_option,end_option);
      else
        for (unsigned int j=0;j<nj;++j,src_row+=s_jstep,dest_row+=d_jstep)
          vil_correlate_1d(src_row,ni,s_istep,  dest_row,d_istep,
                           kernel,k_lo,k_hi,ac,start_option,end_option);
    }
  }
}

template <class destT, class kernelT, class accumT>
vil_image_resource_sptr vil_correlate_1d(
               const vil_image_resource_sptr& src_im,
               const destT dt,
               const kernelT* kernel, std::ptrdiff_t k_lo, std::ptrdiff_t k_hi,
               const accumT ac,
               vil_convolve_boundary_option start_option,
               vil_convolve_boundary_option end_option);

//: A resource adaptor that behaves like a correlated version of its input
template <class kernelT, class accumT, class destT>
class vil_correlate_1d_resource : public vil_image_resource
{
 public:
  //: Construct a correlate filter.
  // You can't create one of these directly, use vil_correlate_1d instead
  vil_correlate_1d_resource(const vil_image_resource_sptr& src,
                            const kernelT* kernel, std::ptrdiff_t k_lo, std::ptrdiff_t k_hi,
                            vil_convolve_boundary_option start_option,
                            vil_convolve_boundary_option end_option)  :
      src_(src), kernel_(kernel), klo_(k_lo), khi_(k_hi),
      start_option_(start_option), end_option_(end_option)
    {
      // Can't do period extension yet.
      assert (start_option != vil_convolve_periodic_extend ||
              end_option != vil_convolve_periodic_extend);
    }

  friend vil_image_resource_sptr vil_correlate_1d <> (
    const vil_image_resource_sptr& src_im, const destT dt, const kernelT* kernel,
    std::ptrdiff_t k_lo, std::ptrdiff_t k_hi, const accumT ac,
    vil_convolve_boundary_option start_option,
    vil_convolve_boundary_option end_option);

 public:
  vil_image_view_base_sptr get_copy_view(unsigned i0, unsigned ni,
                                                 unsigned j0, unsigned nj) const override
  {
    if (i0 + ni > src_->ni() || j0 + nj > src_->nj())  return nullptr;
    const unsigned lsrc = (unsigned)std::max(0,int(i0+klo_)); // lhs of input window
    const unsigned hsrc = std::min(src_->ni(),(unsigned int)(i0+ni-klo_+khi_)); // 1+rhs of input window.
    const unsigned lboundary = std::min((unsigned) -klo_, i0); // width of lhs boundary area.
    assert (hsrc > lsrc);
    vil_image_view_base_sptr vs = src_->get_view(lsrc, hsrc-lsrc, j0, nj);
    vil_image_view<destT> dest(vs->ni(), vs->nj(), vs->nplanes());
    switch (vs->pixel_format())
    {
#define macro( F , T ) \
      case F : \
        vil_correlate_1d(static_cast<vil_image_view<T >&>(*vs),dest, \
                         kernel_, klo_, khi_, accumT(), start_option_, end_option_); \
        return new vil_image_view<destT>(vil_crop(dest, lboundary, ni, 0, nj));

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
        return nullptr;
    }
  }

  unsigned nplanes() const override { return src_->nplanes(); }
  unsigned ni() const override { return src_->ni(); }
  unsigned nj() const override { return src_->nj(); }

  enum vil_pixel_format pixel_format() const override
  { return vil_pixel_format_of(accumT()); }


  //: Put the data in this view back into the image source.
  bool put_view(const vil_image_view_base&  /*im*/, unsigned  /*i0*/, unsigned  /*j0*/) override
  {
    std::cerr << "WARNING: vil_correlate_1d_resource::put_back\n"
             << "\tYou can't push data back into a correlate filter.\n";
    return false;
  }

  //: Extra property information
  bool get_property(char const* tag, void* property_value = nullptr) const override
  {
    if (0==std::strcmp(tag, vil_property_read_only))
      return property_value ? (*static_cast<bool*>(property_value)) = true : true;

    return src_->get_property(tag, property_value);
  }

 protected:
  vil_image_resource_sptr src_;
  const kernelT* kernel_;
  std::ptrdiff_t klo_, khi_;
  vil_convolve_boundary_option start_option_, end_option_;
};

//: Create an image_resource object which correlate kernel[x] x in [k_lo,k_hi] with srcT
// \note  This function does not reverse the kernel. If you want the
// kernel reversed, use vil_convolve_1d instead.
// \param kernel should point to tap 0.
// \relatesalso vil_image_resource
template <class destT, class kernelT, class accumT>
vil_image_resource_sptr vil_correlate_1d(
                         const vil_image_resource_sptr& src_im,
                         const destT  /*dt*/,
                         const kernelT* kernel, std::ptrdiff_t k_lo, std::ptrdiff_t k_hi,
                         const accumT,
                         vil_convolve_boundary_option start_option,
                         vil_convolve_boundary_option end_option)
{
  return new vil_correlate_1d_resource<kernelT, accumT, destT>(src_im, kernel, k_lo, k_hi, start_option, end_option);
}

#endif // vil_correlate_1d_h_
