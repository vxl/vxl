#ifndef vil_math_h_
#define vil_math_h_
//:
// \file
// \brief Various mathematical manipulations of 2D images
// \author Tim Cootes

#include <vector>
#include <cmath>
#include <algorithm>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include "vil_image_view.h"
#include "vil_image_resource.h"
#include "vil_view_as.h"
#include "vil_plane.h"
#include "vil_transform.h"
#include <vil/vil_config.h>

#ifdef VXL_HAS_SSE2_HARDWARE_SUPPORT
#include "vil_math_sse.h"
#endif

//: Compute minimum and maximum values over view
template<class T>
inline void vil_math_value_range(const vil_image_view<T>& view, T& min_value, T& max_value)
{
  if (view.size()==0)
  {
    min_value = 0;
    max_value = 0;
    return;
  }

  min_value = *(view.top_left_ptr());
  max_value = min_value;

  unsigned ni = view.ni();
  unsigned nj = view.nj();
  unsigned np = view.nplanes();

  for (unsigned p=0;p<np;++p)
    for (unsigned j=0;j<nj;++j)
      for (unsigned i=0;i<ni;++i)
      {
        const T pixel = view(i,j,p);
        if (pixel<min_value)
          min_value=pixel;
        else if (pixel>max_value)
          max_value=pixel;
      }
}

//: Compute minimum and maximum values over view
template <>
inline void vil_math_value_range(const vil_image_view<vil_rgb<vxl_byte> >& rgb_view,
                                 vil_rgb<vxl_byte>& min_value, vil_rgb<vxl_byte>& max_value)
{
  vil_image_view<vxl_byte> plane_view = vil_view_as_planes(rgb_view);
  // Get range for each plane in turn
  vil_math_value_range(vil_plane(plane_view,0),min_value.r,max_value.r);
  vil_math_value_range(vil_plane(plane_view,1),min_value.g,max_value.g);
  vil_math_value_range(vil_plane(plane_view,2),min_value.b,max_value.b);
}

//: Compute minimum and maximum values over view
template <>
inline void vil_math_value_range(const vil_image_view<vil_rgb<float> >& rgb_view,
                                 vil_rgb<float>& min_value, vil_rgb<float>& max_value)
{
  vil_image_view<float> plane_view = vil_view_as_planes(rgb_view);
  // Get range for each plane in turn
  vil_math_value_range(vil_plane(plane_view,0),min_value.r,max_value.r);
  vil_math_value_range(vil_plane(plane_view,1),min_value.g,max_value.g);
  vil_math_value_range(vil_plane(plane_view,2),min_value.b,max_value.b);
}


//: Compute the values corresponding to several percentiles of the range of im.
// Percentiles are expressed as fraction, e.g. 0.05, or 0.95.
// \param im The image to examine.
// \param fraction The fractions of the data range (from the lower end).
// \retval value The image data values corresponding to the specified percentiles.
// \relatesalso vil_image_view
// \note This function requires the sorting of large parts of the image data
// and can be very expensive in terms of both processing and memory.
template <class T>
inline void vil_math_value_range_percentiles(const vil_image_view<T>& im,
                                             const std::vector<double>& fraction,
                                             std::vector<T>& value)
{
  value.clear();

  // Test for invalid inputs
  if (im.size()==0)
  {
    return;
  }
  const std::size_t nfrac = fraction.size();
  for (std::size_t f=0; f<nfrac; ++f)
  {
    if (fraction[f]<0.0 || fraction[f]>1.0)
      return;
  }

  // Copy the pixel values into a list.
  unsigned ni = im.ni();
  unsigned nj = im.nj();
  unsigned np = im.nplanes();
  std::ptrdiff_t istep = im.istep();
  std::ptrdiff_t jstep = im.jstep();
  std::ptrdiff_t pstep = im.planestep();
  std::vector<T> data(ni*nj*np);

  typename std::vector<T>::iterator it = data.begin();
  const T* plane = im.top_left_ptr();
  for (unsigned int p=0; p<np; ++p, plane+=pstep)
  {
    const T* row = plane;
    for (unsigned int j=0; j<nj; ++j, row+=jstep)
    {
      const T* pixel = row;
      for (unsigned int i=0; i<ni; ++i, pixel+=istep)
      {
        *it = *pixel;
        it++;
      }
    }
  }
  const std::size_t npix = data.size();

  // Get the nth_element corresponding to the specified fractions
  value.resize(nfrac);
  for (unsigned f=0; f<nfrac; ++f)
  {
    unsigned index = static_cast<unsigned>(fraction[f]*npix - 0.5);
    typename std::vector<T>::iterator index_it = data.begin() + index;
    std::nth_element(data.begin(), index_it, data.end());
    value[f] = *index_it;
  }
}


//: Compute the value corresponding to a percentile of the range of im.
// Percentile is expressed as fraction, e.g. 0.05, or 0.95.
// \param im The image to examine.
// \param fraction The fraction of the data range (from the lower end).
// \retval value The image data value corresponding to the specified percentile.
// \relatesalso vil_image_view
// \note This function requires the sorting of large parts of the image data
// and can be very expensive in terms of both processing and memory.
template <class T>
inline void vil_math_value_range_percentile(const vil_image_view<T>& im,
                                            const double fraction,
                                            T& value)
{
  std::vector<double> fractions(1, fraction);
  std::vector<T> values;
  vil_math_value_range_percentiles(im, fractions, values);
  if (!values.empty())
    value = values[0]; // Bounds-checked access in case previous line failed.
}


//: Sum of squared differences between two images
// \relatesalso vil_image_view
template <class imT, class sumT>
inline sumT vil_math_ssd(const vil_image_view<imT>& imA, const vil_image_view<imT>& imB, sumT /*dummy*/)
{
  assert(imA.ni() == imB.ni() && imB.nj() == imB.nj() && imA.nplanes() == imB.nplanes());
  sumT ssd=0;
  for (unsigned p=0;p<imA.nplanes();++p)
    for (unsigned j=0;j<imA.nj();++j)
      for (unsigned i=0;i<imA.ni();++i)
      {
        const sumT v = ((sumT)imA(i,j,p) - (sumT)imB(i,j,p));
        ssd += v*v;
      }
  return ssd;
}

//: Sum squared magnitude differences between two complex images
// \relatesalso vil_image_view
template <class imT, class sumT>
inline sumT
vil_math_ssd_complex(const vil_image_view<std::complex<imT> >& imA,
                     const vil_image_view<std::complex<imT> >& imB,
                     sumT /*dummy*/)
{
  assert(imA.ni() == imB.ni() && imB.nj() == imB.nj() && imA.nplanes() == imB.nplanes());
  sumT ssd=0;
  for (unsigned p=0;p<imA.nplanes();++p)
    for (unsigned j=0;j<imA.nj();++j)
      for (unsigned i=0;i<imA.ni();++i)
      {
        const std::complex<imT> d = imA(i,j,p) - imB(i,j,p);
        ssd += sumT( d.real()*d.real() + d.imag()*d.imag() );
      }
  return ssd;
}

//: Calc the mean of each pixel over all the planes.
// \relatesalso vil_image_view
template<class aT, class sumT>
inline void vil_math_mean_over_planes(const vil_image_view<aT>& src,
                                      vil_image_view<sumT>& dest)
{
  if (src.nplanes()==1 && src.is_a()==dest.is_a())
  {
    dest.deep_copy(src);
    return;
  }
  dest.set_size(src.ni(), src.nj(), 1);
  for (unsigned j=0;j<src.nj();++j)
    for (unsigned i=0;i<src.ni();++i)
    {
      sumT sum=0;
      for (unsigned p=0;p<src.nplanes();++p)
        sum += (sumT) src(i,j,p);
      dest(i,j) = (sumT)( sum / src.nplanes() );
    }
}

//: Calc the mean of each pixel over all the planes.
// \relatesalso vil_image_view
template<class inT, class outT, class sumT>
inline void vil_math_mean_over_planes(const vil_image_view<inT>& src,
                                      vil_image_view<outT>& dest,
                                      sumT /*dummy*/)
{
  dest.set_size(src.ni(), src.nj(), 1);
  for (unsigned j=0;j<src.nj();++j)
    for (unsigned i=0;i<src.ni();++i)
    {
      sumT sum=0;
      for (unsigned p=0;p<src.nplanes();++p)
        sum += static_cast<sumT>(src(i,j,p));
      dest(i,j) = static_cast<outT>(sum / src.nplanes());
    }
}

//: Sum of elements in plane p of image
// \relatesalso vil_image_view
template<class imT, class sumT>
inline void vil_math_sum(sumT& sum, const vil_image_view<imT>& im, unsigned p)
{
  const imT* row = im.top_left_ptr()+p*im.planestep();
  std::ptrdiff_t istep = im.istep(),jstep=im.jstep();
  const imT* row_end = row + im.nj()*jstep;
  std::ptrdiff_t row_len = im.ni()*im.istep();
  sum = 0;
  for (;row!=row_end;row+=jstep)
  {
    const imT* v_end = row + row_len;
    for (const imT* v = row;v!=v_end;v+=istep) sum+=(sumT)(*v);
  }
}

//: Mean of elements in plane p of image
// \relatesalso vil_image_view
template<class imT, class sumT>
inline void vil_math_mean(sumT& mean, const vil_image_view<imT>& im, unsigned p)
{
  if (im.size()==0) { mean=0; return; }
  vil_math_sum(mean,im,p);
  mean/=(sumT)(im.ni()*im.nj());
}

// helper function for reporting an error without cluttering the
// header with unnecessary includes.
void vil_math_median_unimplemented();

//: Median of elements in plane p of an image.
//
// For integral types, if the median is half way between two
// values, the result will be the floor of the average.
//
// \relatesalso vil_image_view
template <class imT>
inline void vil_math_median(imT & /*median*/,
                            const vil_image_view<imT> & /*im*/,
                            unsigned /*p*/) {
  vil_math_median_unimplemented();
}
// median is unimplemented in the general case (for now).

// Purposefully not documented via doxygen; let the general template's
// documentation be the documentation.
template <>
void vil_math_median(vxl_byte& median, const vil_image_view<vxl_byte>& im, unsigned p);


//: Sum of squares of elements in plane p of image
// \relatesalso vil_image_view
template<class imT, class sumT>
inline void vil_math_sum_squares(sumT& sum, sumT& sum_sq, const vil_image_view<imT>& im, unsigned p)
{
  const imT* row = im.top_left_ptr()+p*im.planestep();
  std::ptrdiff_t istep = im.istep(),jstep=im.jstep();
  const imT* row_end = row + im.nj()*jstep;
  std::ptrdiff_t row_len = im.ni()*im.istep();
  sum = 0; sum_sq = 0;
  for (;row!=row_end;row+=jstep)
  {
    const imT* v_end = row + row_len;
    for (const imT* v = row;v!=v_end;v+=istep) { sum+=*v; sum_sq+=sumT(*v)*sumT(*v); }
  }
}

//: Sum of squares of masked elements in plane p of image
// \relatesalso vil_image_view
template<class imT, class sumT>
inline bool vil_math_sum_squares(sumT& sum, sumT& sum_sq, unsigned int & count,
                                 const vil_image_view<imT>& im, const vil_image_view<bool> & mask, unsigned p)
{
  count = 0;
  if ( im.ni() != mask.ni() || im.nj() != mask.nj() || mask.nplanes() != 1)
  {
    return false;
  }
  const imT* row = im.top_left_ptr()+p*im.planestep();
  std::ptrdiff_t istep = im.istep(),jstep=im.jstep();
  const imT* row_end = row + im.nj()*jstep;
  std::ptrdiff_t row_len = im.ni()*im.istep();
  const bool* m_row = mask.top_left_ptr()+mask.planestep();
  std::ptrdiff_t m_istep = mask.istep(), m_jstep=mask.jstep();
  sum = 0; sum_sq = 0;
  for (;row!=row_end;row+=jstep, m_row+=m_jstep)
  {
    const imT* v_end = row + row_len;
    const bool* b = m_row;
    for (const imT* v = row; v!=v_end;v+=istep, b+=m_istep )
    {
      if(*b)
      {
        ++count;
        sum+=*v;
        sum_sq+=sumT(*v)*sumT(*v);
      }
    }
  }
  return true;
}


//: Mean and variance of elements in plane p of image
// \relatesalso vil_image_view
template<class imT, class sumT>
inline void vil_math_mean_and_variance(sumT& mean, sumT& var, const vil_image_view<imT>& im, unsigned p)
{
  if (im.size()==0) { mean=0; var=0; return; }
  sumT sum,sum_sq;
  vil_math_sum_squares(sum,sum_sq,im,p);
  mean = sum/float(im.ni()*im.nj());
  var = sum_sq/float(im.ni()*im.nj()) - mean*mean;
}

//: Mean and variance of masked elements in plane p of image
// \relatesalso vil_image_view
template<class imT, class sumT>
inline bool vil_math_mean_and_variance(sumT& mean, sumT& var, const vil_image_view<imT>& im,
                                       const vil_image_view<bool> & mask, unsigned p)
{
  if (im.size()==0) { mean=0; var=0; return true; }
  sumT sum,sum_sq;
  unsigned int count = 0;
  if(!vil_math_sum_squares(sum,sum_sq, count, im,mask,p))
  {
    return false;
  }
  mean = sum/float(count);
  var = sum_sq/float(count) - mean*mean;
  return true;
}

//: Mean and variance of masked elements in plane p for vil_image_resource
// \relatedalso vil_image_resource
template< class sumT >
inline bool vil_math_mean_and_variance(sumT& mean, sumT& var, const vil_image_resource_sptr im,
                                       const vil_image_view<bool> & mask, unsigned p)
{
  if(im == nullptr || im->get_view() == nullptr)
  {
    return false;
  }
  switch (im->pixel_format())
  {
#define macro( F , T )                                                  \
    case F :                                                            \
      return vil_math_mean_and_variance(mean, var,                      \
                                        static_cast<vil_image_view<T >&>(*(im->get_view())), \
                                        mask, p);                       \

    macro(VIL_PIXEL_FORMAT_BYTE , vxl_byte )
      macro(VIL_PIXEL_FORMAT_SBYTE , vxl_sbyte )
      macro(VIL_PIXEL_FORMAT_UINT_32 , vxl_uint_32 )
      macro(VIL_PIXEL_FORMAT_UINT_16 , vxl_uint_16 )
      macro(VIL_PIXEL_FORMAT_INT_32 , vxl_int_32 )
      macro(VIL_PIXEL_FORMAT_INT_16 , vxl_int_16 )
      macro(VIL_PIXEL_FORMAT_BOOL , bool )
      macro(VIL_PIXEL_FORMAT_FLOAT , float )
      macro(VIL_PIXEL_FORMAT_DOUBLE , double )
#undef macro
  default:
      return false;
  }
  return true;
}


//: Functor class to compute square roots (returns zero if x<0)
class vil_math_sqrt_functor
{
 public:
  vxl_byte operator()(vxl_byte x) const { return static_cast<vxl_byte>(0.5+std::sqrt(double(x))); }
  unsigned operator()(unsigned x) const { return static_cast<unsigned int>(0.5+std::sqrt(double(x))); }
  int operator()(int x)           const { return x>0?static_cast<int>(0.5+std::sqrt(double(x))):0; }
  short operator()(short x)       const { return x>0?static_cast<short>(0.5+std::sqrt(double(x))):0; }
  float operator()(float x)       const { return x>0?std::sqrt(x):0.0f; }
  double operator()(double x)     const { return x>0?std::sqrt(x):0.0; }
};

//: Compute square-root of each pixel element (or zero if negative)
// \relatesalso vil_image_view
template<class T>
inline void vil_math_sqrt(vil_image_view<T>& image)
{
  vil_transform(image,vil_math_sqrt_functor());
}


//: Truncate each pixel value so it fits into range [min_v,max_v]
//  If value < min_v value=min_v
//  If value > max_v value=max_v
// \relatesalso vil_image_view
template<class T>
inline void vil_math_truncate_range(vil_image_view<T>& image, T min_v, T max_v)
{
  unsigned ni = image.ni(),nj = image.nj(),np = image.nplanes();
  std::ptrdiff_t istep=image.istep(),jstep=image.jstep(),pstep = image.planestep();
  T* plane = image.top_left_ptr();
  for (unsigned p=0;p<np;++p,plane += pstep)
  {
    T* row = plane;
    for (unsigned j=0;j<nj;++j,row += jstep)
    {
      T* pixel = row;
      for (unsigned i=0;i<ni;++i,pixel+=istep)
      {
        if (*pixel<min_v) *pixel=min_v;
        else if (*pixel>max_v) *pixel=max_v;
      }
    }
  }
}

//: Functor class to scale by s
class vil_math_scale_functor
{
  double s_;
public:
  vil_math_scale_functor(double s) : s_(s) {}
  vxl_byte operator()(vxl_byte x) const { return vxl_byte(0.5+s_*x); }
  unsigned operator()(unsigned x) const { return unsigned(0.5+s_*x); }
  short operator()(short x)   const { double r=s_*x; return short(r<0?r-0.5:r+0.5); }
  int operator()(int x)       const { double r=s_*x; return int(r<0?r-0.5:r+0.5); }
  float operator()(float x)       const { return float(s_*x); }
  double operator()(double x)     const { return s_*x; }
  std::complex<double> operator()(std::complex<double> x) const { return s_*x; }
};


//: Functor class to scale by s and translate (offset) by t.
// \note Watch out for overflow, especially for smaller types.
// \sa vil_math_scale_and_offset_values()
class vil_math_scale_and_translate_functor
{
public:
  //: Constructor
  // \param s Scaling.
  // \param t Translation (offset).
  vil_math_scale_and_translate_functor(const double s, const double t)
    : s_(s), t_(t) {}

  vxl_byte operator()(vxl_byte x) const { return vxl_byte(0.5+s_*x+t_); }
  unsigned operator()(unsigned x) const { return unsigned(0.5+s_*x+t_); }
  short operator()(short x)       const { double r=s_*x+t_; return short(r<0?r-0.5:r+0.5); }
  int operator()(int x)           const { double r=s_*x+t_; return int(r<0?r-0.5:r+0.5); }
  float operator()(float x)       const { return float(s_*x+t_); }
  double operator()(double x)     const { return s_*x+t_; }
  std::complex<double> operator()(std::complex<double> x) const { return s_*x+t_; } // Not sure if this one makes sense

private:
  double s_;
  double t_;
};


//: Functor class to compute logarithms (returns zero if x<=0)
class vil_math_log_functor
{
 public:
  vxl_byte operator()(vxl_byte x) const { return static_cast<vxl_byte>(0.5+std::log(double(x))); }
  unsigned operator()(unsigned x) const { return static_cast<unsigned int>(0.5+std::log(double(x))); }
  int operator()(int x)           const { return x>0?static_cast<int>(0.5+std::log(double(x))):0; }
  short operator()(short x)       const { return x>0?static_cast<short>(0.5+std::log(double(x))):0; }
  float operator()(float x)       const { return x>0?std::log(x):0.0f; }
  double operator()(double x)     const { return x>0?std::log(x):0.0; }
};


//: Multiply values in-place in image view by scale
// \relatesalso vil_image_view
template<class T>
inline void vil_math_scale_values(vil_image_view<T>& image, double scale)
{
  vil_transform(image,vil_math_scale_functor(scale));
}

//: Multiply values in-place in image view by scale and add offset
// \relatesalso vil_image_view
template<class imT, class offsetT>
inline void vil_math_scale_and_offset_values(vil_image_view<imT>& image, double scale, offsetT offset)
{
  unsigned ni = image.ni(),nj = image.nj(),np = image.nplanes();
  std::ptrdiff_t istep=image.istep(),jstep=image.jstep(),pstep = image.planestep();
  imT* plane = image.top_left_ptr();
  for (unsigned p=0;p<np;++p,plane += pstep)
  {
    imT* row = plane;
    for (unsigned j=0;j<nj;++j,row += jstep)
    {
      imT* pixel = row;
      for (unsigned i=0;i<ni;++i,pixel+=istep) *pixel = imT(scale*(*pixel)+offset);
    }
  }
}

//: Scale and offset values so their mean is zero and their variance is one.
//  Only works on signed types!
template<class imT>
inline void vil_math_normalise(vil_image_view<imT>& image)
{
  assert(image.nplanes()==1);
  double mean,var;
  vil_math_mean_and_variance(mean,var,image,0);
  double s=0;
  if (var>0) s = 1.0/std::sqrt(var);
  vil_math_scale_and_offset_values(image,s,-s*mean);
}

//: Computes RMS of each pixel over the planes of src image
// Dest is a single plane image, $dest(i,j)^2 = 1/np sum_p src(i,j,p)^2$
// Summation is performed using type destT
template<class srcT, class destT>
inline
void vil_math_rms(const vil_image_view<srcT>& src,
                  vil_image_view<destT>& dest)
{
  unsigned ni = src.ni(),nj = src.nj(),np = src.nplanes();
  dest.set_size(ni,nj,1);

  std::ptrdiff_t istepA=src.istep(),jstepA=src.jstep(),pstepA = src.planestep();
  std::ptrdiff_t istepB=dest.istep(),jstepB=dest.jstep();
  const srcT* rowA = src.top_left_ptr();
  destT* rowB = dest.top_left_ptr();
  for (unsigned j=0;j<nj;++j,rowA += jstepA,rowB += jstepB)
  {
    const srcT* pixelA = rowA;
    const srcT* end_pixelA = rowA+ni*istepA;
    destT* pixelB = rowB;

    if (np==1)
    {
      for (;pixelA!=end_pixelA; pixelA+=istepA,pixelB+=istepB)
        *pixelB = std::fabs(destT(*pixelA));
    }
    else if (np==2)
    {
      for (;pixelA!=end_pixelA; pixelA+=istepA,pixelB+=istepB)
      {
        destT sum2 = destT(*pixelA)*(*pixelA)
          + destT(pixelA[pstepA])*(pixelA[pstepA]);
        *pixelB = destT(std::sqrt(sum2/2));
      }
    }
    else
    {
      for (;pixelA!=end_pixelA; pixelA+=istepA,pixelB+=istepB)
      {
        *pixelB = destT(*pixelA)*destT(*pixelA);
        const srcT* p=pixelA+pstepA;
        const srcT* end_p=pixelA+np*pstepA;
        for (;p!=end_p;p+=pstepA) *pixelB += destT(*p)*destT(*p);
        *pixelB = destT(std::sqrt(*pixelB/np));
      }
    }
  }
}

//: Computes Root Sum of Squares of each pixel over the planes of src image
// Dest is a single plane image, $dest(i,j) = sqrt(sum_p src(i,j,p)^2)$
// Differs from RMS by the scaling factor sqrt(nplanes)
// Summation is performed using type destT
template<class srcT, class destT>
inline
void vil_math_rss(const vil_image_view<srcT>& src,
                  vil_image_view<destT>& dest)
{
  unsigned ni = src.ni(),nj = src.nj(),np = src.nplanes();
  dest.set_size(ni,nj,1);

  std::ptrdiff_t istepA=src.istep(),jstepA=src.jstep(),pstepA = src.planestep();
  std::ptrdiff_t istepB=dest.istep(),jstepB=dest.jstep();
  const srcT* rowA = src.top_left_ptr();
  destT* rowB = dest.top_left_ptr();
  for (unsigned j=0;j<nj;++j,rowA += jstepA,rowB += jstepB)
  {
    const srcT* pixelA = rowA;
    const srcT* end_pixelA = rowA+ni*istepA;
    destT* pixelB = rowB;

    if (np==1)
    {
      for (;pixelA!=end_pixelA; pixelA+=istepA,pixelB+=istepB)
        *pixelB = std::fabs(destT(*pixelA));
    }
    else if (np==2)
    {
      for (;pixelA!=end_pixelA; pixelA+=istepA,pixelB+=istepB)
      {
        destT sum2 = destT(*pixelA)*(*pixelA)
          + destT(pixelA[pstepA])*(pixelA[pstepA]);
        *pixelB = destT(std::sqrt(sum2));
      }
    }
    else
    {
      for (;pixelA!=end_pixelA; pixelA+=istepA,pixelB+=istepB)
      {
        *pixelB = destT(*pixelA)*destT(*pixelA);
        const srcT* p=pixelA+pstepA;
        const srcT* end_p=pixelA+np*pstepA;
        for (;p!=end_p;p+=pstepA) *pixelB += destT(*p)*destT(*p);
        *pixelB = destT(std::sqrt(*pixelB));
      }
    }
  }
}


//: Computes sum of squares of each pixel over the planes of src image
// Dest is a single plane image, $dest(i,j) = sum_p src(i,j,p)^2$
// Summation is performed using type destT
template<class srcT, class destT>
inline
void vil_math_sum_sqr(const vil_image_view<srcT>& src,
                      vil_image_view<destT>& dest)
{
  unsigned ni = src.ni(),nj = src.nj(),np = src.nplanes();
  dest.set_size(ni,nj,1);

  std::ptrdiff_t istepA=src.istep(),jstepA=src.jstep(),pstepA = src.planestep();
  std::ptrdiff_t istepB=dest.istep(),jstepB=dest.jstep();
  const srcT* rowA = src.top_left_ptr();
  destT* rowB = dest.top_left_ptr();
  for (unsigned j=0;j<nj;++j,rowA += jstepA,rowB += jstepB)
  {
    const srcT* pixelA = rowA;
    const srcT* end_pixelA = rowA+ni*istepA;
    destT* pixelB = rowB;
    if (np==1)
    {
      for (;pixelA!=end_pixelA; pixelA+=istepA,pixelB+=istepB)
        *pixelB = destT(*pixelA)*(*pixelA);
    }
    else if (np==2)
    {
      for (;pixelA!=end_pixelA; pixelA+=istepA,pixelB+=istepB)
        *pixelB =   destT(*pixelA)*(*pixelA)
          + destT(pixelA[pstepA])*(pixelA[pstepA]);
    }
    else
    {
      for (;pixelA!=end_pixelA; pixelA+=istepA,pixelB+=istepB)
      {
        *pixelB = destT(*pixelA)*destT(*pixelA);
        const srcT* p=pixelA+pstepA;
        const srcT* end_p=pixelA+np*pstepA;
        for (;p!=end_p;p+=pstepA) *pixelB += destT(*p)*destT(*p);
      }
    }
  }
}

//: Compute sum of two images (im_sum = imA+imB)
// \relatesalso vil_image_view
template<class aT, class bT, class sumT>
inline void vil_math_image_sum(const vil_image_view<aT>& imA,
                               const vil_image_view<bT>& imB,
                               vil_image_view<sumT>& im_sum)
{
  unsigned ni = imA.ni(),nj = imA.nj(),np = imA.nplanes();
  assert(imB.ni()==ni && imB.nj()==nj && imB.nplanes()==np);
  im_sum.set_size(ni,nj,np);

  std::ptrdiff_t istepA=imA.istep(),jstepA=imA.jstep(),pstepA = imA.planestep();
  std::ptrdiff_t istepB=imB.istep(),jstepB=imB.jstep(),pstepB = imB.planestep();
  std::ptrdiff_t istepS=im_sum.istep(),jstepS=im_sum.jstep(),pstepS = im_sum.planestep();
  const aT* planeA = imA.top_left_ptr();
  const bT* planeB = imB.top_left_ptr();
  sumT* planeS     = im_sum.top_left_ptr();
  for (unsigned p=0;p<np;++p,planeA += pstepA,planeB += pstepB,planeS += pstepS)
  {
    const aT* rowA   = planeA;
    const bT* rowB   = planeB;
    sumT* rowS = planeS;
    for (unsigned j=0;j<nj;++j,rowA += jstepA,rowB += jstepB,rowS += jstepS)
    {
      const aT* pixelA = rowA;
      const bT* pixelB = rowB;
      sumT* pixelS = rowS;
      for (unsigned i=0;i<ni;++i,pixelA+=istepA,pixelB+=istepB,pixelS+=istepS)
        *pixelS = sumT(*pixelA)+sumT(*pixelB);
    }
  }
}

//: Compute pixel-wise product of two images (im_prod(i,j) = imA(i,j)*imB(i,j)
//  If images have the same number of planes,
//  then im_prod(i,j,p) = imA(i,j,p)*imB(i,j,p).
//  If imB only has one plane, then im_prod(i,j,p) = imA(i,j,p)*imB(i,j,0).
// \relatesalso vil_image_view
template<class aT, class bT, class sumT>
inline void vil_math_image_product(const vil_image_view<aT>& imA,
                                   const vil_image_view<bT>& imB,
                                   vil_image_view<sumT>& im_product)
{
  unsigned ni = imA.ni(),nj = imA.nj(),np = imA.nplanes();
  assert(imB.ni()==ni && imB.nj()==nj);
  assert(imB.nplanes()==1 || imB.nplanes()==np);
  im_product.set_size(ni,nj,np);

  std::ptrdiff_t istepA=imA.istep(),jstepA=imA.jstep(),pstepA = imA.planestep();
  std::ptrdiff_t istepB=imB.istep(),jstepB=imB.jstep(),pstepB = imB.planestep();
  std::ptrdiff_t istepP=im_product.istep(),jstepP=im_product.jstep(),
    pstepP = im_product.planestep();

  // For one plane case, arrange that im_prod(i,j,p) = imA(i,j,p)*imB(i,j,0)
  if (imB.nplanes()==1) pstepB=0;

  const aT* planeA = imA.top_left_ptr();
  const bT* planeB = imB.top_left_ptr();
  sumT* planeP     = im_product.top_left_ptr();
  for (unsigned p=0;p<np;++p,planeA += pstepA,planeB += pstepB,planeP += pstepP)
  {
    const aT* rowA   = planeA;
    const bT* rowB   = planeB;
    sumT* rowP = planeP;
    for (unsigned j=0;j<nj;++j,rowA += jstepA,rowB += jstepB,rowP += jstepP)
    {
      const aT* pixelA = rowA;
      const bT* pixelB = rowB;
      sumT* pixelP = rowP;
      for (unsigned i=0;i<ni;++i,pixelA+=istepA,pixelB+=istepB,pixelP+=istepP)
        *pixelP = sumT(*pixelA)*sumT(*pixelB);
    }
  }
}

//: Compute the max of two images (im_max = max(imA, imB))
// \relatesalso vil_image_view
template<class aT, class bT, class maxT>
inline void vil_math_image_max(const vil_image_view<aT>& imA,
                               const vil_image_view<bT>& imB,
                               vil_image_view<maxT>& im_max)
{
  unsigned ni = imA.ni(),nj = imA.nj(),np = imA.nplanes();
  assert(imB.ni()==ni && imB.nj()==nj && imB.nplanes()==np);
  im_max.set_size(ni,nj,np);

  std::ptrdiff_t istepA=imA.istep(),jstepA=imA.jstep(),pstepA = imA.planestep();
  std::ptrdiff_t istepB=imB.istep(),jstepB=imB.jstep(),pstepB = imB.planestep();
  std::ptrdiff_t istepS=im_max.istep(),jstepS=im_max.jstep(),pstepS = im_max.planestep();
  const aT* planeA = imA.top_left_ptr();
  const bT* planeB = imB.top_left_ptr();
  maxT* planeS     = im_max.top_left_ptr();
  for (unsigned p=0;p<np;++p,planeA += pstepA,planeB += pstepB,planeS += pstepS)
  {
    const aT* rowA   = planeA;
    const bT* rowB   = planeB;
    maxT* rowS = planeS;
    for (unsigned j=0;j<nj;++j,rowA += jstepA,rowB += jstepB,rowS += jstepS)
    {
      const aT* pixelA = rowA;
      const bT* pixelB = rowB;
      maxT* pixelS = rowS;
      for (unsigned i=0;i<ni;++i,pixelA+=istepA,pixelB+=istepB,pixelS+=istepS)
        *pixelS = maxT(std::max(*pixelA, *pixelB));
    }
  }
}

//: Compute the min of two images (im_min = min(imA, imB))
// \relatesalso vil_image_view
template<class aT, class bT, class minT>
inline void vil_math_image_min(const vil_image_view<aT>& imA,
                               const vil_image_view<bT>& imB,
                               vil_image_view<minT>& im_min)
{
  unsigned ni = imA.ni(),nj = imA.nj(),np = imA.nplanes();
  assert(imB.ni()==ni && imB.nj()==nj && imB.nplanes()==np);
  im_min.set_size(ni,nj,np);

  std::ptrdiff_t istepA=imA.istep(),jstepA=imA.jstep(),pstepA = imA.planestep();
  std::ptrdiff_t istepB=imB.istep(),jstepB=imB.jstep(),pstepB = imB.planestep();
  std::ptrdiff_t istepS=im_min.istep(),jstepS=im_min.jstep(),pstepS = im_min.planestep();
  const aT* planeA = imA.top_left_ptr();
  const bT* planeB = imB.top_left_ptr();
  minT* planeS     = im_min.top_left_ptr();
  for (unsigned p=0;p<np;++p,planeA += pstepA,planeB += pstepB,planeS += pstepS)
  {
    const aT* rowA   = planeA;
    const bT* rowB   = planeB;
    minT* rowS = planeS;
    for (unsigned j=0;j<nj;++j,rowA += jstepA,rowB += jstepB,rowS += jstepS)
    {
      const aT* pixelA = rowA;
      const bT* pixelB = rowB;
      minT* pixelS = rowS;
      for (unsigned i=0;i<ni;++i,pixelA+=istepA,pixelB+=istepB,pixelS+=istepS)
        *pixelS = minT(std::min(*pixelA, *pixelB));
    }
  }
}

//: Compute pixel-wise ratio of two images : im_ratio(i,j) = imA(i,j)/imB(i,j)
//  Pixels cast to type sumT before calculation.
//  If imB(i,j,p)==0, im_ration(i,j,p)=0
//
//  If images have the same number of planes,
//  then im_ratio(i,j,p) = imA(i,j,p)/imB(i,j,p).
//  If imB only has one plane, then im_ratio(i,j,p) = imA(i,j,p)/imB(i,j,0).
// \relatesalso vil_image_view
template<class aT, class bT, class sumT>
inline void vil_math_image_ratio(const vil_image_view<aT>& imA,
                                 const vil_image_view<bT>& imB,
                                 vil_image_view<sumT>& im_ratio)
{
  unsigned ni = imA.ni(),nj = imA.nj(),np = imA.nplanes();
  assert(imB.ni()==ni && imB.nj()==nj);
  assert(imB.ni()==ni && imB.nj()==nj);
  assert(imB.nplanes()==1 || imB.nplanes()==np);
  im_ratio.set_size(ni,nj,np);

  std::ptrdiff_t istepA=imA.istep(),jstepA=imA.jstep(),pstepA = imA.planestep();
  std::ptrdiff_t istepB=imB.istep(),jstepB=imB.jstep(),pstepB = imB.planestep();
  std::ptrdiff_t istepR=im_ratio.istep(),jstepR=im_ratio.jstep(),
    pstepR = im_ratio.planestep();

  // For one plane case, arrange that im_ratio(i,j,p) = imA(i,j,p)/imB(i,j,0)
  if (imB.nplanes()==1) pstepB=0;

  const aT* planeA = imA.top_left_ptr();
  const bT* planeB = imB.top_left_ptr();
  sumT* planeR     = im_ratio.top_left_ptr();
  for (unsigned p=0;p<np;++p,planeA += pstepA,planeB += pstepB,planeR += pstepR)
  {
    const aT* rowA   = planeA;
    const bT* rowB   = planeB;
    sumT* rowR = planeR;
    for (unsigned j=0;j<nj;++j,rowA += jstepA,rowB += jstepB,rowR += jstepR)
    {
      const aT* pixelA = rowA;
      const bT* pixelB = rowB;
      sumT* pixelR = rowR;
      for (unsigned i=0;i<ni;++i,pixelA+=istepA,pixelB+=istepB,pixelR+=istepR)
      {
        if (*pixelB==0) *pixelR=0;
        else *pixelR = sumT(*pixelA)/sumT(*pixelB);
      }
    }
  }
}

//: Compute difference of two images (im_sum = imA-imB)
// \relatesalso vil_image_view
template<class aT, class bT, class sumT>
inline void vil_math_image_difference(const vil_image_view<aT>& imA,
                                      const vil_image_view<bT>& imB,
                                      vil_image_view<sumT>& im_sum)
{
  unsigned ni = imA.ni(),nj = imA.nj(),np = imA.nplanes();
  assert(imB.ni()==ni && imB.nj()==nj && imB.nplanes()==np);
  im_sum.set_size(ni,nj,np);

  std::ptrdiff_t istepA=imA.istep(),jstepA=imA.jstep(),pstepA = imA.planestep();
  std::ptrdiff_t istepB=imB.istep(),jstepB=imB.jstep(),pstepB = imB.planestep();
  std::ptrdiff_t istepS=im_sum.istep(),jstepS=im_sum.jstep(),pstepS = im_sum.planestep();
  const aT* planeA = imA.top_left_ptr();
  const bT* planeB = imB.top_left_ptr();
  sumT* planeS     = im_sum.top_left_ptr();
  for (unsigned p=0;p<np;++p,planeA += pstepA,planeB += pstepB,planeS += pstepS)
  {
    const aT* rowA   = planeA;
    const bT* rowB   = planeB;
    sumT* rowS = planeS;
    for (unsigned j=0;j<nj;++j,rowA += jstepA,rowB += jstepB,rowS += jstepS)
    {
      const aT* pixelA = rowA;
      const bT* pixelB = rowB;
      sumT* pixelS = rowS;
      for (unsigned i=0;i<ni;++i,pixelA+=istepA,pixelB+=istepB,pixelS+=istepS)
        *pixelS = sumT(*pixelA)-sumT(*pixelB);
    }
  }
}


//: Compute absolute difference of two 1D images (im_sum = |imA-imB|)
// \relatesalso vil_image_view
template<class aT, class bT, class dT>
inline void vil_math_image_abs_difference_1d_generic(
  const aT* pxA, std::ptrdiff_t isA,
  const bT* pxB, std::ptrdiff_t isB,
  dT* pxD, std::ptrdiff_t isD,
  unsigned len)
{
  for (unsigned i =0; i < len; ++i, pxA += isA, pxB += isB, pxD += isD)
  {
    // The following construction works for all types, including unsigned
    *pxD = (*pxA > *pxB) ? (dT)(*pxA - *pxB) : (dT)(*pxB - *pxA);
  }
}


//: Compute absolute difference of two 1D images (im_sum = |imA-imB|)
// Specialize this function for an optimized implementation
template<class aT, class bT, class dT>
inline void vil_math_image_abs_difference_1d(
  const aT* pxA, std::ptrdiff_t isA,
  const bT* pxB, std::ptrdiff_t isB,
  dT* pxD, std::ptrdiff_t isD,
  unsigned len)
{
  vil_math_image_abs_difference_1d_generic<aT, bT, dT>(
    pxA, isA, pxB, isB, pxD, isD, len);
}


//: Compute absolute difference of two images (im_sum = |imA-imB|)
// \relatesalso vil_image_view
template<class aT, class bT, class dT>
inline void vil_math_image_abs_difference(const vil_image_view<aT>& imA,
                                          const vil_image_view<bT>& imB,
                                          vil_image_view<dT>& imD)
{
  unsigned ni = imA.ni(), nj = imA.nj(), np = imA.nplanes();
  assert(imB.ni() == ni && imB.nj() == nj && imB.nplanes() == np);
  imD.set_size(ni, nj, np);

  std::ptrdiff_t isA=imA.istep(), jsA=imA.jstep(), psA = imA.planestep();
  std::ptrdiff_t isB=imB.istep(), jsB=imB.jstep(), psB = imB.planestep();
  std::ptrdiff_t isD=imD.istep(), jsD=imD.jstep(), psD = imD.planestep();
  const aT* planeA = imA.top_left_ptr();
  const bT* planeB = imB.top_left_ptr();
  dT* planeD = imD.top_left_ptr();

  for (unsigned p = 0; p < np ;++p, planeA += psA, planeB += psB, planeD += psD)
  {
    const aT* rowA = planeA;
    const bT* rowB = planeB;

    dT* rowD = planeD;
    for (unsigned j = 0; j < nj; ++j, rowA += jsA, rowB += jsB, rowD += jsD)
    {
      vil_math_image_abs_difference_1d<aT,bT,dT>(
        rowA, isA, rowB, isB, rowD, isD, ni);
    }
  }
}


//: Compute  sum of absolute difference between two images (|imA-imB|)
// \relatesalso vil_image_view
template<class aT, class bT>
inline double vil_math_image_abs_difference(const vil_image_view<aT>& imA,
                                            const vil_image_view<bT>& imB)
{
  double sum=0.0;
  unsigned ni = imA.ni(),nj = imA.nj(),np = imA.nplanes();
  assert(imB.ni()==ni && imB.nj()==nj && imB.nplanes()==np);

  std::ptrdiff_t istepA=imA.istep(),jstepA=imA.jstep(),pstepA = imA.planestep();
  std::ptrdiff_t istepB=imB.istep(),jstepB=imB.jstep(),pstepB = imB.planestep();
  const aT* planeA = imA.top_left_ptr();
  const bT* planeB = imB.top_left_ptr();
  for (unsigned p=0;p<np;++p,planeA += pstepA,planeB += pstepB)
  {
    const aT* rowA   = planeA;
    const bT* rowB   = planeB;
    for (unsigned j=0;j<nj;++j,rowA += jstepA,rowB += jstepB)
    {
      const aT* pixelA = rowA;
      const bT* pixelB = rowB;
      for (unsigned i=0;i<ni;++i,pixelA+=istepA,pixelB+=istepB)
      {
        // The following construction works for all types, including unsigned
        sum += (*pixelA>*pixelB?(*pixelA-*pixelB):(*pixelB-*pixelA));
      }
    }
  }
  return sum;
}

//: Compute magnitude of two images taken as vector components, sqrt(A^2 + B^2)
// \relatesalso vil_image_view
template<class aT, class bT, class magT>
inline void vil_math_image_vector_mag(const vil_image_view<aT>& imA,
                                      const vil_image_view<bT>& imB,
                                      vil_image_view<magT>& im_mag)
{
  unsigned ni = imA.ni(),nj = imA.nj(),np = imA.nplanes();
  assert(imB.ni()==ni && imB.nj()==nj && imB.nplanes()==np);
  im_mag.set_size(ni,nj,np);

  std::ptrdiff_t istepA=imA.istep(),jstepA=imA.jstep(),pstepA = imA.planestep();
  std::ptrdiff_t istepB=imB.istep(),jstepB=imB.jstep(),pstepB = imB.planestep();
  std::ptrdiff_t istepM=im_mag.istep(),jstepM=im_mag.jstep(),pstepM = im_mag.planestep();
  const aT* planeA = imA.top_left_ptr();
  const bT* planeB = imB.top_left_ptr();
  magT* planeM     = im_mag.top_left_ptr();
  for (unsigned p=0;p<np;++p,planeA += pstepA,planeB += pstepB,planeM += pstepM)
  {
    const aT* rowA   = planeA;
    const bT* rowB   = planeB;
    magT* rowM = planeM;
    for (unsigned j=0;j<nj;++j,rowA += jstepA,rowB += jstepB,rowM += jstepM)
    {
      const aT* pixelA = rowA;
      const bT* pixelB = rowB;
      magT* pixelM = rowM;
      for (unsigned i=0;i<ni;++i,pixelA+=istepA,pixelB+=istepB,pixelM+=istepM)
      {
        // The following construction works for all types, including unsigned
        magT mag_sqr = static_cast<magT>((*pixelA)*(*pixelA) + (*pixelB)*(*pixelB));
        magT mag = vil_math_sqrt_functor()(mag_sqr);
        *pixelM = mag;
      }
    }
  }
}

//: imA = fa*imA + fb*imB  (Useful for moving averages!)
// Can do running sum using vil_add_image_fraction(running_mean,1-f,new_im,f)
// to update current mean by a fraction f of new_im
// \relatesalso vil_image_view
template<class aT, class bT, class scaleT>
inline void vil_math_add_image_fraction(vil_image_view<aT>& imA, scaleT fa,
                                        const vil_image_view<bT>& imB, scaleT fb)
{
  unsigned ni = imA.ni(),nj = imA.nj(),np = imA.nplanes();
  assert(imB.ni()==ni && imB.nj()==nj && imB.nplanes()==np);

  std::ptrdiff_t istepA=imA.istep(),jstepA=imA.jstep(),pstepA = imA.planestep();
  std::ptrdiff_t istepB=imB.istep(),jstepB=imB.jstep(),pstepB = imB.planestep();
  aT* planeA = imA.top_left_ptr();
  const bT* planeB = imB.top_left_ptr();
  for (unsigned p=0;p<np;++p,planeA += pstepA,planeB += pstepB)
  {
    aT* rowA   = planeA;
    const bT* rowB   = planeB;
    for (unsigned j=0;j<nj;++j,rowA += jstepA,rowB += jstepB)
    {
      aT* pixelA = rowA;
      const bT* pixelB = rowB;
      for (unsigned i=0;i<ni;++i,pixelA+=istepA,pixelB+=istepB)
        *pixelA = aT(fa*(*pixelA)+fb*(*pixelB));
    }
  }
}

//: Compute integral image im_sum(i+1,j+1) = sum (x<=i,y<=j) imA(x,y)
//  Useful thing for quickly computing mean over large regions,
//  as demonstrated in Viola and Jones (CVPR01).
// The sum of elements in the ni x nj square with corner (i,j)
// is given by im_sum(i,j)+im_sum(i+ni,j+nj)-im_sum(i+ni,j)-im_sum(i,j+nj)
// \relatesalso vil_image_view
template<class aT, class sumT>
inline void vil_math_integral_image(const vil_image_view<aT>& imA,
                                    vil_image_view<sumT>& im_sum)
{
  assert(imA.nplanes()==1);
  unsigned ni = imA.ni(),nj = imA.nj();
  unsigned ni1=ni+1;
  unsigned nj1=nj+1;
  im_sum.set_size(ni1,nj1,1);

  // Put zeros along first row of im_sum
  std::ptrdiff_t istepS=im_sum.istep(),jstepS=im_sum.jstep();
  sumT* rowS     = im_sum.top_left_ptr();
  sumT* pixelS = rowS;
  for (unsigned i=0;i<ni1;++i,pixelS+=istepS)
    *pixelS=0;

  // Now sum from original image (imA)
  std::ptrdiff_t istepA=imA.istep(),jstepA=imA.jstep();
  const aT* rowA = imA.top_left_ptr();

  sumT sum;
  std::ptrdiff_t prev_j = -jstepS;
  rowS += jstepS;

  for (unsigned j=0;j<nj;++j,rowA += jstepA,rowS += jstepS)
  {
    const aT* pixelA = rowA;
    pixelS = rowS;
    sum = 0;
    // set first value at start of each row to zero!
    *pixelS = 0;
    pixelS+=istepS;
    for (unsigned i=1;i<ni1;++i,pixelA+=istepA,pixelS+=istepS)
    { sum+= (sumT)(*pixelA); *pixelS=sum + pixelS[prev_j];}
  }
}

//: Compute integral image im_sum_sq(i+1,j+1) = sum (x<=i,y<=j) imA(x,y)^2
// Also computes sum im_sum(i+1,j+1) = sum (x<=i,y<=j) imA(x,y)
//
//  Useful thing for quickly computing mean and variance over large regions,
//  as demonstrated in Viola and Jones (CVPR01).
//
// The sum of elements in the ni x nj square with corner (i,j)
// is given by im_sum(i,j)+im_sum(i+ni,j+nj)-im_sum(i+ni,j)-im_sum(i,j+nj)
//
// Similar result holds for sum of squares, allowing rapid calculation of variance etc.
// \relatesalso vil_image_view
template<class aT, class sumT>
inline void vil_math_integral_sqr_image(const vil_image_view<aT>& imA,
                                        vil_image_view<sumT>& im_sum,
                                        vil_image_view<sumT>& im_sum_sq)
{
  assert(imA.nplanes()==1);
  unsigned ni = imA.ni(),nj = imA.nj();
  unsigned ni1=ni+1;
  unsigned nj1=nj+1;
  im_sum.set_size(ni1,nj1,1);
  im_sum_sq.set_size(ni1,nj1,1);

  // Put zeros along first row of im_sum & im_sum_sq
  std::ptrdiff_t istepS=im_sum.istep(),jstepS=im_sum.jstep();
  std::ptrdiff_t istepS2=im_sum_sq.istep(),jstepS2=im_sum_sq.jstep();
  sumT* rowS     = im_sum.top_left_ptr();
  sumT* rowS2     = im_sum_sq.top_left_ptr();
  // im_sum
  sumT* pixelS = rowS;
  for (unsigned i=0;i<ni1;++i,pixelS+=istepS)
    *pixelS=0;

  // im_sum_sq
  sumT* pixelS2 = rowS2;
  for (unsigned i=0;i<ni1;++i,pixelS2+=istepS2)
    *pixelS2=0;

  // Now sum from original image (imA)
  std::ptrdiff_t istepA=imA.istep(),jstepA=imA.jstep();
  const aT* rowA = imA.top_left_ptr();

  sumT sum,sum2;
  std::ptrdiff_t prev_j = -jstepS;
  std::ptrdiff_t prev_j2 = -jstepS2;
  rowS += jstepS;
  rowS2 += jstepS2;

  for (unsigned j=0;j<nj;++j,rowA += jstepA,rowS += jstepS,rowS2 += jstepS2)
  {
    const aT* pixelA = rowA;
    pixelS = rowS;
    pixelS2 = rowS2;
    sum = 0;
    sum2 = 0;
    // set first value at start of each row to zero!
    *pixelS = 0;
    *pixelS2 = 0;
    pixelS+=istepS;
    pixelS2+=istepS2;
    for (unsigned i=1;i<ni1;++i,pixelA+=istepA,pixelS+=istepS,pixelS2+=istepS2)
    {
      sum+= (sumT)(*pixelA);
      *pixelS=sum + pixelS[prev_j];
      sum2+=sumT(*pixelA)*sumT(*pixelA);
      *pixelS2 = sum2 + pixelS2[prev_j2];
    }
  }
}


#ifdef VXL_HAS_SSE2_HARDWARE_SUPPORT
#include "vil_math_sse.hxx"
#endif

#endif // vil_math_h_
