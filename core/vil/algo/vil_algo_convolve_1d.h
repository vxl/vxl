// This is mul/vil2/algo/vil2_algo_convolve_1d.h
#ifndef vil2_algo_convolve_1d_h_
#define vil2_algo_convolve_1d_h_
//:
//  \file
//  \brief 1D Convolution with cunning boundary options
//  \author Tim Cootes (based on work by fsm)

#include <vcl_compiler.h>
#include <vcl_algorithm.h>
#include <vcl_cstdlib.h> // for vcl_abort()
#include <vcl_cstring.h>
#include <vcl_cassert.h>
#include <vil2/vil2_image_view.h>
#include <vil2/vil2_image_view_functions.h>
#include <vil2/vil2_image_resource.h>
#include <vil2/vil2_property.h>

//: Available options for boundary behavior
// When convolving a finite signal the boundaries may be
// treated in various ways which can often be expressed in terms
// of ways to extend the signal outside its original range.
enum vil2_convolve_boundary_option
{
  // Do not fill destination edges at all
  // ie leave them unchanged.
  vil2_convolve_ignore_edge, /**< Do not fill destination edges at all. */
  // Do not to extend the signal, but pad with zeros.
  //     |                               |
  // K                       ----*-------
  // in   ... ---------------------------
  // out  ... --------------------0000000
  vil2_convolve_no_extend, /**< Do not to extend the signal, but pad with zeros. */

  // Zero-extend the input signal beyond the boundary.
  //     |                               |
  // K                              ----*--------
  // in   ... ---------------------------000000000000...
  // out  ... ---------------------------
  vil2_convolve_zero_extend, /**< Zero-extend the input signal beyond the boundary. */

  // Extend the signal to be constant beyond the boundary
  //     |                               |
  // K                              ----*--------
  // in   ... --------------------------aaaaaaaaaaaaa...
  // out  ... ---------------------------
  vil2_convolve_constant_extend, /**< Extend the signal to be constant beyond the boundary. */

  // Extend the signal periodically beyond the boundary.
  //     |                               |
  // K                              ----*--------
  // in   abc...-------------------------abc...------..
  // out  ... ---------------------------
  vil2_convolve_periodic_extend, /**< Extend the signal periodically beyond the boundary. */

  // Extend the signal by reflection about the boundary.
  //     |                               |
  // K                               ----*--------
  // in   ... -------------------...edcbabcde...
  // out  ... ---------------------------
  vil2_convolve_reflect_extend, /**< Extend the signal by reflection about the boundary. */

  // This one is slightly different. The input signal is not
  // extended in any way, but the kernel is trimmed to allow
  // convolution to proceed up to the boundary and reweighted
  // to keep the total area the same.
  // *** may not work with kernels which take negative values.
  vil2_convolve_trim /**< Kernel is trimmed and reweighted, to allow convolution up to boundary. */
};

//: Convolve edge with kernel[x*kstep] x in [k_lo,k_hi] (k_lo<=0)
//  Fills only edge: dest[i], i=0..(-k_lo-1)
template <class srcT, class destT, class kernelT, class accumT>
inline void vil2_algo_convolve_edge_1d(const srcT* src, unsigned n, int s_step,
                                       destT* dest, int d_step,
                                       const kernelT* kernel, int k_lo, int k_hi, int kstep,
                                       accumT,
                                       vil2_convolve_boundary_option option)
{
  if (option==vil2_convolve_ignore_edge) return;
  if (option==vil2_convolve_no_extend)
  {
    // Initialise first elements of row to zero
    for (int i=k_lo;i<0;++i,dest+=d_step)
      *dest = 0;
  }
  else if (option==vil2_convolve_zero_extend)
  {
    // Assume src[i]==0 for i<0
    for (int i=k_lo+1;i<=0;++i,dest+=d_step,src+=s_step)
    {
      accumT sum = 0;
      const srcT* s = src;
      const kernelT* k = kernel+i*kstep;
      for (int j=i;j<=k_hi;++j,s+=s_step,k+=kstep) sum+= (accumT)((*s)*(*k));
      *dest=sum;
    }
  }
  else if (option==vil2_convolve_constant_extend)
  {
    // Assume src[i]=src[0] for i<0
    int i_max = 1-k_lo;
    for (int i=0;i<=i_max;++i)
    {
      accumT sum=0;
      for (int j=k_lo;j<=k_hi;++j)
      {
        if ((i+j)<0) sum+=(accumT)(src[0]*kernel[j*kstep]);
        else         sum+=(accumT)(src[(i+j)*s_step]*kernel[j*kstep]);
      }
      dest[i*d_step]=sum;
    }
  }
  else if (option==vil2_convolve_periodic_extend)
  {
    // Assume src[i]=src[0] for i<0
    int i_max = -1-k_lo;
    for (int i=0;i<=i_max;++i)
    {
      accumT sum=0;
      for (int j=k_lo;j<=k_hi;++j)
      {
        if ((i+j)<0) sum+=(accumT)(src[0]*kernel[j*kstep]);
        else         sum+=(accumT)(src[(i+j)*s_step]*kernel[j*kstep]);
      }
      dest[i*d_step]=sum;
    }
  }
  else if (option==vil2_convolve_reflect_extend)
  {
    // Assume src[i]=src[n+i] for i<0
    int i_max = -1-k_lo;
    for (int i=0;i<=i_max;++i)
    {
      accumT sum=0;
      for (int j=k_lo;j<=k_hi;++j)
        sum+=(accumT)(src[((i+j+n)%n)*s_step]*kernel[j*kstep]);
      dest[i*d_step]=sum;
    }
  }
  else if (option==vil2_convolve_trim)
  {
    // Truncate and reweight kernel
    accumT k_sum_all=0;
    for (int j=k_lo;j<=k_hi;++j) k_sum_all+=(accumT)(kernel[j*kstep]);

    int i_max = -1-k_lo;
    for (int i=0;i<=i_max;++i)
    {
      accumT sum=0;
      accumT k_sum=0;
      // Sum elements which overlap src
      // ie i+j>=0  (so j starts at -i)
      for (int j=-i;j<=k_hi;++j)
      {
        sum+=(accumT)(src[(i+j)*s_step]*kernel[j*kstep]);
        k_sum += (accumT)(kernel[j*kstep]);
      }
      dest[i*d_step]=sum*k_sum_all/k_sum;
    }
  }
  else
  {
    vcl_cout<<"vil2_algo_convolve_edge_1d: ";
    vcl_cout<<"Sorry, can't deal with supplied edge option."<<vcl_endl;
    vcl_abort();
  }
}

//: Convolve kernel[x] (x in [k_lo,k_hi]) with srcT
// Assumes dest and src same size (nx)
// \relates vil2_image_view
template <class srcT, class destT, class kernelT, class accumT>
inline void vil2_algo_convolve_1d(const srcT* src0, unsigned nx, int s_step,
                                  destT* dest0, int d_step,
                                  const kernelT* kernel, int k_lo, int k_hi,
                                  accumT ac,
                                  vil2_convolve_boundary_option start_option,
                                  vil2_convolve_boundary_option end_option)
{
  // Deal with start (fill elements 0..1-k_lo of dest)
  vil2_algo_convolve_edge_1d(src0,nx,s_step,dest0,d_step,kernel,k_lo,k_hi,1,ac,start_option);

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
  vil2_algo_convolve_edge_1d(src0+(nx-1)*s_step,nx,-s_step,
                             dest0+(nx-1)*d_step,-d_step,
                             kernel,-k_hi,-k_lo,-1,ac,end_option);
}

//: Convolve kernel[i] (i in [k_lo,k_hi]) with srcT in i-direction
// On exit dest_im(i,j) = sum src(i+x,j)*kernel(x)  (x=k_lo..k_hi)
// \param kernel should point to tap 0.
// Assumes dest and src same size (nx)
template <class srcT, class destT, class kernelT, class accumT>
inline void vil2_algo_convolve_1d(const vil2_image_view<srcT>& src_im,
                                  vil2_image_view<destT>& dest_im,
                                  const kernelT* kernel, int k_lo, int k_hi,
                                  accumT ac,
                                  vil2_convolve_boundary_option start_option,
                                  vil2_convolve_boundary_option end_option)
{
  unsigned ni = src_im.ni();
  unsigned nj = src_im.nj();
  int s_istep = src_im.istep(), s_jstep = src_im.jstep();

  dest_im.resize(ni,nj,src_im.nplanes());
  int d_istep = dest_im.istep(),d_jstep = dest_im.jstep();

  for (int p=0;p<src_im.nplanes();++p)
  {
    // Select first row of p-th plane
    const srcT*  src_row  = src_im.top_left_ptr()+p*src_im.planestep();
    destT* dest_row = dest_im.top_left_ptr()+p*dest_im.planestep();

    // Apply convolution to each row in turn
    for (int j=0;j<nj;++j,src_row+=s_jstep,dest_row+=d_jstep)
      vil2_algo_convolve_1d(src_row,ni,s_istep,  dest_row,d_istep,
                            kernel,k_lo,k_hi,ac,start_option,end_option);
  }
}

template <class destT, class kernelT, class accumT>
inline vil2_image_resource_sptr vil2_algo_convolve_1d(
  const vil2_image_resource_sptr& src_im,
  const destT dt,
  const kernelT* kernel, int k_lo, int k_hi,
  const accumT ac,
  vil2_convolve_boundary_option start_option,
  vil2_convolve_boundary_option end_option);

//: A resource adaptor that behaves like a algo_convolve_1d'ed version of its input
template <class kernelT, class accumT, class destT>
class vil2_algo_convolve_1d_resource : public vil2_image_resource
{
  //: Construct a convolve filter.
  // You can't create one of these directly, use vil2_algo_convolve_1d instead
  vil2_algo_convolve_1d_resource(const vil2_image_resource_sptr& src,
                                  const kernelT* kernel, int k_lo, int k_hi,
                                  vil2_convolve_boundary_option start_option,
                                  vil2_convolve_boundary_option end_option):
    src_(src), kernel_(kernel), klo_(k_lo), khi_(k_hi),
    start_option_(start_option), end_option_(end_option)
    {
      // Can't do period extension yet.
      assert (start_option != vil2_convolve_periodic_extend ||
              end_option != vil2_convolve_periodic_extend);
    }

  friend vil2_image_resource_sptr vil2_algo_convolve_1d VCL_NULL_TMPL_ARGS (
    const vil2_image_resource_sptr& src_im, const destT dt, const kernelT* kernel,
    int k_lo, int k_hi, const accumT ac,
    vil2_convolve_boundary_option start_option,
    vil2_convolve_boundary_option end_option);

 public:
  virtual vil2_image_view_base_sptr get_copy_view(unsigned i0, unsigned ni,
                                                  unsigned j0, unsigned nj) const
  {
    if (i0 + ni > src_->ni() || j0 + nj > src_->nj())  return 0;
    const unsigned lsrc = (unsigned) vcl_max(0,(int)i0 + klo_); // lhs of input window
    const unsigned hsrc = vcl_min(src_->ni(),i0 + ni - klo_ + khi_); // 1+rhs of input window.
    const unsigned lboundary = vcl_min((unsigned) -klo_, i0); // width of lhs boundary area.
    assert (hsrc > lsrc);
    vil2_image_view_base_sptr vs = src_->get_view(lsrc, hsrc-lsrc, j0, nj);
    vil2_image_view<destT> dest(vs->ni(), vs->nj(), vs->nplanes());
    switch (vs->pixel_format())
    {
#define macro( F , T ) \
      case F : \
        vil2_algo_convolve_1d(static_cast<vil2_image_view<T >&>(*vs),dest, \
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
  virtual bool put_view(const vil2_image_view_base& im, unsigned i0,
                        unsigned j0) {
    vcl_cerr << "WARNING: vil2_algo_convolve_1d_resource::put_back\n"
      "\t You can't push data back into a convolve filter." << vcl_endl;
    return false;
  }

  //: Extra property information
  virtual bool get_property(char const* tag, void* property_value = 0) const {
      if (0==vcl_strcmp(tag, vil2_property_read_only))
    return property_value ? (*(bool*)property_value) = true : true;

    return src_->get_property(tag, property_value);
  }

  //: Return the name of the class;
  virtual vcl_string is_a() const
  {
    static const vcl_string class_name_="vil2_algo_convolve_1d_resource";
    return class_name_;
  }

  //: Return true if the name of the class matches the argument
  virtual bool is_class(vcl_string const&s) const
  {
    return s==vil2_algo_convolve_1d_resource::is_a() || vil2_image_resource::is_class(s);
  }

 protected:
  vil2_image_resource_sptr src_;
  const kernelT* kernel_;
  int klo_, khi_;
  vil2_convolve_boundary_option start_option_, end_option_;
};

//: Create an image_resource object which convolve kernel[x] x in [k_lo,k_hi] with srcT
// \param kernel should point to tap 0.
template <class destT, class kernelT, class accumT>
inline vil2_image_resource_sptr vil2_algo_convolve_1d(
  const vil2_image_resource_sptr& src_im,
  const destT dt,
  const kernelT* kernel, int k_lo, int k_hi,
  const accumT ac,
  vil2_convolve_boundary_option start_option,
  vil2_convolve_boundary_option end_option)
{
  return new vil2_algo_convolve_1d_resource<kernelT, accumT, destT>(src_im,
    kernel, k_lo, k_hi, start_option, end_option);
}

#endif // vil2_algo_convolve_1d_h_
