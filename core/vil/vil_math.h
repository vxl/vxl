// This is core/vil/vil_math.h
#ifndef vil_math_h_
#define vil_math_h_
//:
// \file
// \brief Various mathematical manipulations of 2D images
// \author Tim Cootes

#include <vcl_cassert.h>
#include <vcl_vector.h>
#include <vcl_cmath.h>
#include <vcl_algorithm.h>
#include <vil/vil_image_view.h>
#include <vil/vil_view_as.h>
#include <vil/vil_plane.h>
#include <vil/vil_transform.h>


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
VCL_DEFINE_SPECIALIZATION
inline void vil_math_value_range(const vil_image_view<vil_rgb<vxl_byte> >& rgb_view,
                                 vil_rgb<vxl_byte>& min_value, vil_rgb<vxl_byte>& max_value)
{
  vil_image_view<vxl_byte> plane_view = vil_view_as_planes(rgb_view);
  // Get range for each plane in turn
  vil_math_value_range(vil_plane(plane_view,0),min_value.r,max_value.r);
  vil_math_value_range(vil_plane(plane_view,0),min_value.g,max_value.g);
  vil_math_value_range(vil_plane(plane_view,0),min_value.b,max_value.b);
}

//: Compute minimum and maximum values over view
VCL_DEFINE_SPECIALIZATION
inline void vil_math_value_range(const vil_image_view<vil_rgb<float> >& rgb_view,
                                 vil_rgb<float>& min_value, vil_rgb<float>& max_value)
{
  vil_image_view<float> plane_view = vil_view_as_planes(rgb_view);
  // Get range for each plane in turn
  vil_math_value_range(vil_plane(plane_view,0),min_value.r,max_value.r);
  vil_math_value_range(vil_plane(plane_view,0),min_value.g,max_value.g);
  vil_math_value_range(vil_plane(plane_view,0),min_value.b,max_value.b);
}


//: Compute value corresponding to a percentile of the range of im.
// Percentiles expressed as fraction, e.g. 0.05, or 0.95.
// \param im The image to examine.
// \param fraction The fraction of the data range (from the lower end).
// \retval value The image data value corresponding to the specified percentile.
// \relates vil_image_view
// \note This function requires the sorting of large parts of the image data
// and can be very expensive in terms of both processing and memory.
template <class T>
inline void vil_math_value_range_percentile(const vil_image_view<T>& im,
                                            const double fraction,
                                            T& value)
{
  // Test for invalid inputs
  if (im.size()==0 || fraction<0.0 || fraction>=1.0)
  {
    value = 0;
    return;
  }
  
  // Accumulate the pixel values into a list.
  unsigned ni = im.ni();
  unsigned nj = im.nj();
  unsigned np = im.nplanes();
  vcl_ptrdiff_t istep = im.istep(); 
  vcl_ptrdiff_t jstep = im.jstep();
  vcl_ptrdiff_t pstep = im.planestep();
  vcl_vector<T> data(ni*nj*np);
  
  typename vcl_vector<T>::iterator it = data.begin();
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
  unsigned npix = data.size();
  
  // Get the nth_element corresponding to the specified fraction
  int index = int (fraction*npix - 0.5);
  typename vcl_vector<T>::iterator index_it = data.begin() + index;
  vcl_nth_element(data.begin(), index_it, data.end());
  value = *index_it;
}


//: Sum of squared differences between two images
// \relates vil_image_view
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
// \relates vil_image_view
template <class imT, class sumT>
inline sumT
vil_math_ssd_complex(const vil_image_view<vcl_complex<imT> >& imA,
                     const vil_image_view<vcl_complex<imT> >& imB,
                     sumT /*dummy*/)
{
  assert(imA.ni() == imB.ni() && imB.nj() == imB.nj() && imA.nplanes() == imB.nplanes());
  sumT ssd=0;
  for (unsigned p=0;p<imA.nplanes();++p)
    for (unsigned j=0;j<imA.nj();++j)
      for (unsigned i=0;i<imA.ni();++i)
      {
        const vcl_complex<imT> d = imA(i,j,p) - imB(i,j,p);
        ssd += sumT( d.real()*d.real() + d.imag()*d.imag() );
      }
  return ssd;
}

//: Calc the mean of each pixel over all the planes.
// \relates vil_image_view
template<class aT, class sumT>
inline void vil_math_mean_over_planes(const vil_image_view<aT>& src,
                                      vil_image_view<sumT>& dest)
{
  dest.set_size(src.ni(), src.nj(), 1);
  for (unsigned j=0;j<src.nj();++j)
    for (unsigned i=0;i<src.ni();++i)
    {
      sumT sum=0;
      for (unsigned p=0;p<src.nplanes();++p)
        sum += (sumT) src(i,j,p);
      dest(i,j) = sum / src.nplanes();
    }
}

//: Calc the mean of each pixel over all the planes.
// \relates vil_image_view
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
// \relates vil_image_view
template<class imT, class sumT>
inline void vil_math_sum(sumT& sum, const vil_image_view<imT>& im, unsigned p)
{
  const imT* row = im.top_left_ptr()+p*im.planestep();
  vcl_ptrdiff_t istep = im.istep(),jstep=im.jstep();
  const imT* row_end = row + im.nj()*jstep;
  int row_len = im.ni()*im.istep();
  sum = 0;
  for (;row!=row_end;row+=jstep)
  {
    const imT* v_end = row + row_len;
    for (const imT* v = row;v!=v_end;v+=istep) sum+=*v;
  }
}

//: Mean of elements in plane p of image
// \relates vil_image_view
template<class imT, class sumT>
inline void vil_math_mean(sumT& mean, const vil_image_view<imT>& im, unsigned p)
{
  if (im.size()==0) { mean=0; return; }
  vil_math_sum(mean,im,p);
  mean/=(im.ni()*im.nj());
}

//: Sum of squares of elements in plane p of image
// \relates vil_image_view
template<class imT, class sumT>
inline void vil_math_sum_squares(sumT& sum, sumT& sum_sq, const vil_image_view<imT>& im, unsigned p)
{
  const imT* row = im.top_left_ptr()+p*im.planestep();
  vcl_ptrdiff_t istep = im.istep(),jstep=im.jstep();
  const imT* row_end = row + im.nj()*jstep;
  int row_len = im.ni()*im.istep();
  sum = 0; sum_sq = 0;
  for (;row!=row_end;row+=jstep)
  {
    const imT* v_end = row + row_len;
    for (const imT* v = row;v!=v_end;v+=istep) { sum+=*v; sum_sq+=sumT(*v)*sumT(*v); }
  }
}

//: Mean and variance of elements in plane p of image
// \relates vil_image_view
template<class imT, class sumT>
inline void vil_math_mean_and_variance(sumT& mean, sumT& var, const vil_image_view<imT>& im, unsigned p)
{
  if (im.size()==0) { mean=0; var=0; return; }
  sumT sum,sum_sq;
  vil_math_sum_squares(sum,sum_sq,im,p);
  mean = sum/(im.ni()*im.nj());
  var = sum_sq/(im.ni()*im.nj()) - mean*mean;
}

//: Functor class to compute square roots (returns zero if x<0)
class vil_math_sqrt_functor
{
 public:
  vxl_byte operator()(vxl_byte x) const { return vxl_byte(0.5+vcl_sqrt(float(x))); }
  unsigned operator()(unsigned x) const { return unsigned(0.5+vcl_sqrt(float(x))); }
  int operator()(int x)           const { return x>0?int(0.5+vcl_sqrt(float(x))):0; }
  short operator()(short x)       const { return x>0?short(0.5+vcl_sqrt(float(x))):0; }
  float operator()(float x)       const { return x>0?vcl_sqrt(x):0.0f; }
  double operator()(double x)     const { return x>0?vcl_sqrt(x):0.0; }
};

//: Compute square-root of each pixel element (or zero if negative)
// \relates vil_image_view
template<class T>
inline void vil_math_sqrt(vil_image_view<T>& image)
{
  vil_transform(image,vil_math_sqrt_functor());
}


//: Truncate each pixel value so it fits into range [min_v,max_v]
//  If value < min_v value=min_v
//  If value > max_v value=max_v
// \relates vil_image_view
template<class T>
inline void vil_math_truncate_range(vil_image_view<T>& image, T min_v, T max_v)
{
  unsigned ni = image.ni(),nj = image.nj(),np = image.nplanes();
  vcl_ptrdiff_t istep=image.istep(),jstep=image.jstep(),pstep = image.planestep();
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
 private:
  double s_;
 public:
  vil_math_scale_functor(double s) : s_(s) {}
  vxl_byte operator()(vxl_byte x) const { return vxl_byte(0.5+s_*x); }
  unsigned operator()(unsigned x) const { return unsigned(0.5+s_*x); }
  short operator()(short x)   const { double r=s_*x; return short(r<0?r-0.5:r+0.5); }
  int operator()(int x)       const { double r=s_*x; return int(r<0?r-0.5:r+0.5); }
  float operator()(float x)       const { return float(s_*x); }
  double operator()(double x)     const { return s_*x; }
  vcl_complex<double> operator()(vcl_complex<double> x) const { return s_*x; }
};

//: Multiply values in-place in image view by scale
// \relates vil_image_view
template<class T>
inline void vil_math_scale_values(vil_image_view<T>& image, double scale)
{
  vil_transform(image,vil_math_scale_functor(scale));
}

//: Multiply values in-place in image view by scale and add offset
// \relates vil_image_view
template<class imT, class offsetT>
inline void vil_math_scale_and_offset_values(vil_image_view<imT>& image, double scale, offsetT offset)
{
  unsigned ni = image.ni(),nj = image.nj(),np = image.nplanes();
  vcl_ptrdiff_t istep=image.istep(),jstep=image.jstep(),pstep = image.planestep();
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
  if (var>0) s = 1.0/vcl_sqrt(var);
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

  vcl_ptrdiff_t istepA=src.istep(),jstepA=src.jstep(),pstepA = src.planestep();
  vcl_ptrdiff_t istepB=dest.istep(),jstepB=dest.jstep();
  const srcT* rowA = src.top_left_ptr();
  destT* rowB = dest.top_left_ptr();
  for (unsigned j=0;j<nj;++j,rowA += jstepA,rowB += jstepB)
  {
      const srcT* pixelA = rowA;
      destT* pixelB = rowB;
      for (unsigned i=0;i<ni;++i,pixelA+=istepA,pixelB+=istepB)
      {
        destT sum2 = 0;
        const srcT* p=pixelA;
        for (unsigned k=0;k<np;++k,p+=pstepA) sum2 += destT(*p)*destT(*p);
        *pixelB = destT(vcl_sqrt(sum2/np));
      }
  }
}

//: Compute sum of two images (im_sum = imA+imB)
// \relates vil_image_view
template<class aT, class bT, class sumT>
inline void vil_math_image_sum(const vil_image_view<aT>& imA,
                               const vil_image_view<bT>& imB,
                               vil_image_view<sumT>& im_sum)
{
  unsigned ni = imA.ni(),nj = imA.nj(),np = imA.nplanes();
  assert(imB.ni()==ni && imB.nj()==nj && imB.nplanes()==np);
  im_sum.set_size(ni,nj,np);

  vcl_ptrdiff_t istepA=imA.istep(),jstepA=imA.jstep(),pstepA = imA.planestep();
  vcl_ptrdiff_t istepB=imB.istep(),jstepB=imB.jstep(),pstepB = imB.planestep();
  vcl_ptrdiff_t istepS=im_sum.istep(),jstepS=im_sum.jstep(),pstepS = im_sum.planestep();
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
// \relates vil_image_view
template<class aT, class bT, class sumT>
inline void vil_math_image_product(const vil_image_view<aT>& imA,
                                   const vil_image_view<bT>& imB,
                                   vil_image_view<sumT>& im_product)
{
  unsigned ni = imA.ni(),nj = imA.nj(),np = imA.nplanes();
  assert(imB.ni()==ni && imB.nj()==nj);
  assert(imB.nplanes()==1 || imB.nplanes()==np);
  im_product.set_size(ni,nj,np);

  vcl_ptrdiff_t istepA=imA.istep(),jstepA=imA.jstep(),pstepA = imA.planestep();
  vcl_ptrdiff_t istepB=imB.istep(),jstepB=imB.jstep(),pstepB = imB.planestep();
  vcl_ptrdiff_t istepP=im_product.istep(),jstepP=im_product.jstep(),
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

//: Compute pixel-wise ratio of two images : im_ratio(i,j) = imA(i,j)/imB(i,j)
//  Pixels cast to type sumT before calculation.
//  If imB(i,j,p)==0, im_ration(i,j,p)=0
//
//  If images have the same number of planes,
//  then im_ratio(i,j,p) = imA(i,j,p)/imB(i,j,p).
//  If imB only has one plane, then im_ratio(i,j,p) = imA(i,j,p)/imB(i,j,0).
// \relates vil_image_view
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

  vcl_ptrdiff_t istepA=imA.istep(),jstepA=imA.jstep(),pstepA = imA.planestep();
  vcl_ptrdiff_t istepB=imB.istep(),jstepB=imB.jstep(),pstepB = imB.planestep();
  vcl_ptrdiff_t istepR=im_ratio.istep(),jstepR=im_ratio.jstep(),
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
      if (*pixelB==0) *pixelR=0;
      else *pixelR = sumT(*pixelA)/sumT(*pixelB);
    }
  }
}

//: Compute difference of two images (im_sum = imA-imB)
// \relates vil_image_view
template<class aT, class bT, class sumT>
inline void vil_math_image_difference(const vil_image_view<aT>& imA,
                                      const vil_image_view<bT>& imB,
                                      vil_image_view<sumT>& im_sum)
{
  unsigned ni = imA.ni(),nj = imA.nj(),np = imA.nplanes();
  assert(imB.ni()==ni && imB.nj()==nj && imB.nplanes()==np);
  im_sum.set_size(ni,nj,np);

  vcl_ptrdiff_t istepA=imA.istep(),jstepA=imA.jstep(),pstepA = imA.planestep();
  vcl_ptrdiff_t istepB=imB.istep(),jstepB=imB.jstep(),pstepB = imB.planestep();
  vcl_ptrdiff_t istepS=im_sum.istep(),jstepS=im_sum.jstep(),pstepS = im_sum.planestep();
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

//: Compute absolute difference of two images (im_sum = |imA-imB|)
// \relates vil_image_view
template<class aT, class bT, class sumT>
inline void vil_math_image_abs_difference(const vil_image_view<aT>& imA,
                                          const vil_image_view<bT>& imB,
                                          vil_image_view<sumT>& im_sum)
{
  unsigned ni = imA.ni(),nj = imA.nj(),np = imA.nplanes();
  assert(imB.ni()==ni && imB.nj()==nj && imB.nplanes()==np);
  im_sum.set_size(ni,nj,np);

  vcl_ptrdiff_t istepA=imA.istep(),jstepA=imA.jstep(),pstepA = imA.planestep();
  vcl_ptrdiff_t istepB=imB.istep(),jstepB=imB.jstep(),pstepB = imB.planestep();
  vcl_ptrdiff_t istepS=im_sum.istep(),jstepS=im_sum.jstep(),pstepS = im_sum.planestep();
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
      {
        // The following construction works for all types, including unsigned
        *pixelS = (sumT)(*pixelA>*pixelB?(*pixelA-*pixelB):(*pixelB-*pixelA));
      }
    }
  }
}

//: imA = fa*imA + fb*imB  (Useful for moving averages!)
// Can do running sum using vil_add_image_fraction(running_mean,1-f,new_im,f)
// to update current mean by a fraction f of new_im
// \relates vil_image_view
template<class aT, class bT, class scaleT>
inline void vil_math_add_image_fraction(vil_image_view<aT>& imA, scaleT fa,
                                        const vil_image_view<bT>& imB, scaleT fb)
{
  unsigned ni = imA.ni(),nj = imA.nj(),np = imA.nplanes();
  assert(imB.ni()==ni && imB.nj()==nj && imB.nplanes()==np);

  vcl_ptrdiff_t istepA=imA.istep(),jstepA=imA.jstep(),pstepA = imA.planestep();
  vcl_ptrdiff_t istepB=imB.istep(),jstepB=imB.jstep(),pstepB = imB.planestep();
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
// \relates vil_image_view
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
  vcl_ptrdiff_t istepS=im_sum.istep(),jstepS=im_sum.jstep();
  sumT* rowS     = im_sum.top_left_ptr();
  sumT* pixelS = rowS;
  for (unsigned i=0;i<ni1;++i,pixelS+=istepS)
    *pixelS=0;

  // Now sum from original image (imA)
  vcl_ptrdiff_t istepA=imA.istep(),jstepA=imA.jstep();
  const aT* rowA = imA.top_left_ptr();

  sumT sum;
  vcl_ptrdiff_t prev_j = -jstepS;
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
    { sum+= *pixelA; *pixelS=sum + pixelS[prev_j];}
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
// \relates vil_image_view
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
  vcl_ptrdiff_t istepS=im_sum.istep(),jstepS=im_sum.jstep();
  vcl_ptrdiff_t istepS2=im_sum_sq.istep(),jstepS2=im_sum_sq.jstep();
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
  vcl_ptrdiff_t istepA=imA.istep(),jstepA=imA.jstep();
  const aT* rowA = imA.top_left_ptr();

  sumT sum,sum2;
  vcl_ptrdiff_t prev_j = -jstepS;
  vcl_ptrdiff_t prev_j2 = -jstepS2;
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
      sum+= *pixelA;
      *pixelS=sum + pixelS[prev_j];
      sum2+=sumT(*pixelA)*sumT(*pixelA);
      *pixelS2 = sum2 + pixelS2[prev_j2];
    }
  }
}

#endif // vil_math_h_
