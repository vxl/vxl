// This is mul/vil2/algo/vil2_correlate_1d.h
#ifndef vil2_correlate_1d_h_
#define vil2_correlate_1d_h_
//:
// \file
// \brief 1D Convolution with cunning boundary options
// \author Tim Cootes (based on work by fsm)

#include <vcl_compiler.h>
#include <vcl_algorithm.h>
#include <vcl_cstring.h>
#include <vcl_cassert.h>
#include <vil2/vil2_image_view.h>
#include <vil2/vil2_image_resource.h>
#include <vil2/vil2_property.h>
#include <vil2/algo/vil2_convolve_1d.h>

//: Correlate kernel[x] (x in [k_lo,k_hi]) with srcT
// Assumes dest and src same size (nx)
template <class srcT, class destT, class kernelT, class accumT>
inline void vil2_correlate_1d(const srcT* src0, unsigned nx, vcl_ptrdiff_t s_step,
                              destT* dest0, vcl_ptrdiff_t d_step,
                              const kernelT* kernel,
                              vcl_ptrdiff_t k_lo, vcl_ptrdiff_t k_hi,
                              accumT ac,
                              vil2_convolve_boundary_option start_option,
                              vil2_convolve_boundary_option end_option)
{
  // Deal with start (fill elements 0..1-k_lo of dest)
  vil2_convolve_edge_1d(src0,nx,s_step,dest0,d_step,kernel,-k_hi,-k_lo,-1,ac,start_option);

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
  vil2_convolve_edge_1d(src0+(nx-1)*s_step,nx,-s_step,
                        dest0+(nx-1)*d_step,-d_step,
                        kernel,k_lo,k_hi,1,ac,end_option);
}

//: correlate kernel[i] (i in [k_lo,k_hi]) with srcT in i-direction
// On exit dest_im(i,j) = sum src(i+x,j)*kernel(x)  (x=k_lo..k_hi)
// \note  This function doen not reverse the kernel. If you want the
// kernel reversed, use vil2_convolve_1d instead.
// \param kernel should point to tap 0.
// \param dest_im will be resized to size of src_im.
// \relates vil2_image_view
template <class srcT, class destT, class kernelT, class accumT>
inline void vil2_correlate_1d(const vil2_image_view<srcT>& src_im,
                              vil2_image_view<destT>& dest_im,
                              const kernelT* kernel,
                              vcl_ptrdiff_t k_lo, vcl_ptrdiff_t k_hi,
                              accumT ac,
                              vil2_convolve_boundary_option start_option,
                              vil2_convolve_boundary_option end_option)
{
  unsigned ni = src_im.ni();
  unsigned nj = src_im.nj();
  vcl_ptrdiff_t s_istep = src_im.istep(), s_jstep = src_im.jstep();

  dest_im.set_size(ni,nj,src_im.nplanes());
  vcl_ptrdiff_t d_istep = dest_im.istep(),d_jstep = dest_im.jstep();

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
          vil2_correlate_1d(src_row,ni,1,  dest_row,1,
                           kernel,k_lo,k_hi,ac,start_option,end_option);
      else
        for (unsigned int j=0;j<nj;++j,src_row+=s_jstep,dest_row+=d_jstep)
          vil2_correlate_1d(src_row,ni,1,  dest_row,d_istep,
                           kernel,k_lo,k_hi,ac,start_option,end_option);
    }
    else
    {
      if (d_istep == 1)
        for (unsigned int j=0;j<nj;++j,src_row+=s_jstep,dest_row+=d_jstep)
          vil2_correlate_1d(src_row,ni,s_istep,  dest_row,1,
                           kernel,k_lo,k_hi,ac,start_option,end_option);
      else
        for (unsigned int j=0;j<nj;++j,src_row+=s_jstep,dest_row+=d_jstep)
          vil2_correlate_1d(src_row,ni,s_istep,  dest_row,d_istep,
                           kernel,k_lo,k_hi,ac,start_option,end_option);
    }
  }
}

template <class destT, class kernelT, class accumT>
inline vil2_image_resource_sptr vil2_correlate_1d(
               const vil2_image_resource_sptr& src_im,
               const destT dt,
               const kernelT* kernel, vcl_ptrdiff_t k_lo, vcl_ptrdiff_t k_hi,
               const accumT ac,
               vil2_convolve_boundary_option start_option,
               vil2_convolve_boundary_option end_option);

//: A resource adaptor that behaves like a correlated version of its input
template <class kernelT, class accumT, class destT>
class vil2_correlate_1d_resource : public vil2_image_resource
{
  //: Construct a correlate filter.
  // You can't create one of these directly, use vil2_correlate_1d instead
  vil2_correlate_1d_resource(const vil2_image_resource_sptr& src,
                             const kernelT* kernel, vcl_ptrdiff_t k_lo, vcl_ptrdiff_t k_hi,
                             vil2_convolve_boundary_option start_option,
                             vil2_convolve_boundary_option end_option)  :
      src_(src), kernel_(kernel), klo_(k_lo), khi_(k_hi),
      start_option_(start_option), end_option_(end_option)
    {
      // Can't do period extension yet.
      assert (start_option != vil2_convolve_periodic_extend ||
              end_option != vil2_convolve_periodic_extend);
    }

  friend vil2_image_resource_sptr vil2_correlate_1d VCL_NULL_TMPL_ARGS (
    const vil2_image_resource_sptr& src_im, const destT dt, const kernelT* kernel,
    vcl_ptrdiff_t k_lo, vcl_ptrdiff_t k_hi, const accumT ac,
    vil2_convolve_boundary_option start_option,
    vil2_convolve_boundary_option end_option);

 public:
  virtual vil2_image_view_base_sptr get_copy_view(unsigned i0, unsigned ni,
                                                  unsigned j0, unsigned nj) const
  {
    if (i0 + ni > src_->ni() || j0 + nj > src_->nj())  return 0;
    const unsigned lsrc = (unsigned) vcl_max(0L,(vcl_ptrdiff_t)i0 + klo_); // lhs of input window
    const unsigned hsrc = vcl_min(src_->ni(),i0 + ni - klo_ + khi_); // 1+rhs of input window.
    const unsigned lboundary = vcl_min((unsigned) -klo_, i0); // width of lhs boundary area.
    assert (hsrc > lsrc);
    vil2_image_view_base_sptr vs = src_->get_view(lsrc, hsrc-lsrc, j0, nj);
    vil2_image_view<destT> dest(vs->ni(), vs->nj(), vs->nplanes());
    switch (vs->pixel_format())
    {
#define macro( F , T ) \
      case F : \
        vil2_correlate_1d(static_cast<vil2_image_view<T >&>(*vs),dest, \
          kernel_, klo_, khi_, accumT(), start_option_, end_option_); \
        return new vil2_image_view<destT>(vil2_crop(dest, lboundary, ni, 0, nj));

      macro(VIL2_PIXEL_FORMAT_BYTE , vxl_byte )
      macro(VIL2_PIXEL_FORMAT_SBYTE , vxl_sbyte )
      macro(VIL2_PIXEL_FORMAT_UINT_32 , vxl_uint_32 )
      macro(VIL2_PIXEL_FORMAT_UINT_16 , vxl_uint_16 )
      macro(VIL2_PIXEL_FORMAT_INT_32 , vxl_int_32 )
      macro(VIL2_PIXEL_FORMAT_INT_16 , vxl_int_16 )
      macro(VIL2_PIXEL_FORMAT_BOOL , bool )
      macro(VIL2_PIXEL_FORMAT_FLOAT , float )
      macro(VIL2_PIXEL_FORMAT_DOUBLE , double )
#undef macro
      default:
        return 0;
    }
  }

  virtual unsigned nplanes() const { return src_->nplanes(); }
  virtual unsigned ni() const { return src_->ni(); }
  virtual unsigned nj() const { return src_->nj(); }

  virtual enum vil2_pixel_format pixel_format() const
  { return vil2_pixel_format_of(accumT()); }


  //: Put the data in this view back into the image source.
  virtual bool put_view(const vil2_image_view_base& im, unsigned i0, unsigned j0)
  {
    vcl_cerr << "WARNING: vil2_correlate_1d_resource::put_back\n"
             << "\tYou can't push data back into a correlate filter.\n";
    return false;
  }

  //: Extra property information
  virtual bool get_property(char const* tag, void* property_value = 0) const
  {
    if (0==vcl_strcmp(tag, vil2_property_read_only))
      return property_value ? (*static_cast<bool*>(property_value)) = true : true;

    return src_->get_property(tag, property_value);
  }

 protected:
  vil2_image_resource_sptr src_;
  const kernelT* kernel_;
  int klo_, khi_;
  vil2_convolve_boundary_option start_option_, end_option_;
};

//: Create an image_resource object which correlate kernel[x] x in [k_lo,k_hi] with srcT
// \note  This function doen not reverse the kernel. If you want the
// kernel reversed, use vil2_convolve_1d instead.
// \param kernel should point to tap 0.
// \relates vil2_image_resource
template <class destT, class kernelT, class accumT>
inline vil2_image_resource_sptr vil2_correlate_1d(
                         const vil2_image_resource_sptr& src_im,
                         const destT dt,
                         const kernelT* kernel, vcl_ptrdiff_t k_lo, vcl_ptrdiff_t k_hi,
                         const accumT,
                         vil2_convolve_boundary_option start_option,
                         vil2_convolve_boundary_option end_option)
{
  return new vil2_correlate_1d_resource<kernelT, accumT, destT>(src_im,
                              kernel, k_lo, k_hi, start_option, end_option);
}

#endif // vil2_correlate_1d_h_
