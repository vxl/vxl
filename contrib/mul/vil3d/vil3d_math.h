// This is mul/vil3d/vil3d_math.h
#ifndef vil3d_math_h_
#define vil3d_math_h_
//:
// \file
// \brief Various mathematical manipulations of 3D images
// \author Tim Cootes

#include <vector>
#include <iostream>
#include <algorithm>
#include <cassert>
// not used? #include <functional> // for std::less<T>
#include <vil3d/vil3d_image_view.h>
#include <vil3d/vil3d_plane.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


//: Compute minimum and maximum values over im
// \relatesalso vil3d_image_view
template <class T>
inline void vil3d_math_value_range(const vil3d_image_view<T>& im,
                                   T& min_value, T& max_value)
{
  if (im.size()==0)
  {
    min_value = 0;
    max_value = 0;
    return;
  }

  const T* plane = im.origin_ptr();
  min_value = *plane;
  max_value = min_value;

  unsigned ni = im.ni();
  unsigned nj = im.nj();
  unsigned nk = im.nk();
  unsigned np = im.nplanes();
  std::ptrdiff_t istep = im.istep(), jstep=im.jstep(), kstep=im.kstep();

  for (unsigned int p=0;p<np;++p, plane += im.planestep())
  {
    const T* slice = plane;
    for (unsigned int k=0;k<nk;++k, slice += kstep)
    {
      const T* row = slice;
      for (unsigned int j=0;j<nj;++j, row += jstep)
      {
        const T* pixel = row;
        for (unsigned int i=0;i<ni;++i, pixel+=istep)
        {
          if (*pixel<min_value)      min_value = *pixel;
          else if (*pixel>max_value) max_value = *pixel;
        }
      }
    }
  }
}


//: Compute value corresponding to a percentile of the range of im.
// Percentiles expressed as fraction, e.g. 0.05, or 0.95.
// \param im The image to examine.
// \param fraction The fraction of the data range (from the lower end).
// \retval value The image data value corresponding to the specified percentile.
// \relatesalso vil3d_image_view
// \note This function requires the sorting of large parts of the image data
// and can be very expensive in terms of both processing and memory.
// \sa vil3d_math_value_range_percentiles()
template <class T>
inline void vil3d_math_value_range_percentile(const vil3d_image_view<T>& im,
                                              const double fraction,
                                              T& value)
{
  std::vector<double> fractions(1, fraction);
  std::vector<T> values;
  vil3d_math_value_range_percentiles(im, fractions, values);
  if (!values.empty())
    value = values[0]; // Bounds-checked access in case previous line failed.
}


//: Compute value corresponding to several percentiles of the range of im.
// Percentiles expressed as fraction, e.g. 0.05, or 0.95.
// \param im The image to examine.
// \param fraction The fraction of the data range (from the lower end).
// \retval value The image data value corresponding to the specified percentiles.
// \relatesalso vil3d_image_view
// \note This function requires the sorting of large parts of the image data
// and can be very expensive in terms of both processing and memory.
template <class T>
inline void vil3d_math_value_range_percentiles(const vil3d_image_view<T>& im,
                                               const std::vector<double> fraction,
                                               std::vector<T>& value)
{
  value.clear();

  // Test for invalid inputs
  if (im.size()==0)
  {
    return;
  }
  unsigned nfrac = fraction.size();
  for (unsigned f=0; f<nfrac; ++f)
  {
    if (fraction[f]<0.0 || fraction[f]>1.0)
      return;
  }

  // Copy the pixel values into a local list.
  unsigned ni = im.ni();
  unsigned nj = im.nj();
  unsigned nk = im.nk();
  unsigned np = im.nplanes();
  std::ptrdiff_t istep = im.istep();
  std::ptrdiff_t jstep=im.jstep();
  std::ptrdiff_t kstep=im.kstep();
  std::ptrdiff_t pstep = im.planestep();
  std::vector<T> data(ni*nj*nk*np);

  typename std::vector<T>::iterator it = data.begin();
  const T* plane = im.origin_ptr();
  for (unsigned int p=0;p<np;++p, plane += pstep)
  {
    const T* slice = plane;
    for (unsigned int k=0;k<nk;++k, slice += kstep)
    {
      const T* row = slice;
      for (unsigned int j=0;j<nj;++j, row += jstep)
      {
        const T* pixel = row;
        for (unsigned int i=0;i<ni;++i, pixel+=istep)
        {
          *it = *pixel;
          it++;
        }
      }
    }
  }
  unsigned npix = data.size();

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


//: Calc the mean of each pixel over all the planes.
// \relatesalso vil3d_image_view
template<class aT, class sumT>
inline void vil3d_math_mean_over_planes(const vil3d_image_view<aT>& src,
                                        vil3d_image_view<sumT>& dest)
{
  dest.set_size(src.ni(), src.nj(), src.nk(), 1);
  for (unsigned k=0;k<src.nk();++k)
    for (unsigned j=0;j<src.nj();++j)
      for (unsigned i=0;i<src.ni();++i)
      {
        sumT sum=0;
        for (unsigned p=0;p<src.nplanes();++p)
          sum += (sumT) src(i,j,k,p);
        dest(i,j,k) = sum / src.nplanes();
      }
}

//: Calc the mean of each pixel over all the planes.
// \relatesalso vil3d_image_view
template<class inT, class outT, class sumT>
inline void vil3d_math_mean_over_planes(const vil3d_image_view<inT>& src,
                                        vil3d_image_view<outT>& dest,
                                        sumT /*dummy*/)
{
  dest.set_size(src.ni(), src.nj(), src.nk(), 1);
  for (unsigned k=0;k<src.nk();++k)
    for (unsigned j=0;j<src.nj();++j)
      for (unsigned i=0;i<src.ni();++i)
      {
        sumT sum=0;
        for (unsigned p=0;p<src.nplanes();++p)
          sum += static_cast<sumT>(src(i,j,k,p));
        dest(i,j,k) = static_cast<outT>(sum / src.nplanes());
      }
}

//: Calculate the rms of each pixel over all the planes.
// \relatesalso vil3d_image_view
template<class inT, class outT, class sumT>
inline void vil3d_math_rms(const vil3d_image_view<inT>& src,
                           vil3d_image_view<outT>& dest,
                           sumT /*dummy*/)
{
  dest.set_size(src.ni(), src.nj(), src.nk(), 1);
  for (unsigned k=0;k<src.nk();++k)
    for (unsigned j=0;j<src.nj();++j)
      for (unsigned i=0;i<src.ni();++i)
      {
        sumT sum_sqr=0;
        for (unsigned p=0;p<src.nplanes();++p)
          sum_sqr += static_cast<sumT>(src(i,j,k,p))*static_cast<sumT>(src(i,j,k,p));
        dest(i,j,k) = static_cast<outT>(std::sqrt(sum_sqr / src.nplanes()));
      }
}


//: Compute sum of values in plane p
// \relatesalso vil3d_image_view
template <class imT, class sumT>
inline void vil3d_math_sum(sumT& sum, const vil3d_image_view<imT>& im,
                           unsigned p)
{
  assert(p<im.nplanes());
  sum=0;
  if (im.size()==0)
  {
    return;
  }

  const imT* plane = im.origin_ptr();
  unsigned ni = im.ni();
  unsigned nj = im.nj();
  unsigned nk = im.nk();
  std::ptrdiff_t istep = im.istep(), jstep=im.jstep(), kstep=im.kstep();

  const imT* slice = plane + p*im.planestep();
  for (unsigned int k=0;k<nk;++k, slice += kstep)
  {
    const imT* row = slice;
    for (unsigned int j=0;j<nj;++j, row += jstep)
    {
      const imT* pixel = row;
      for (unsigned int i=0;i<ni;++i, pixel+=istep) sum += sumT(*pixel);
    }
  }
}

//: Mean of elements in plane p of image
// \relatesalso vil3d_image_view
template <class imT, class sumT>
inline void vil3d_math_mean(sumT& mean, const vil3d_image_view<imT>& im,
                            unsigned p)
{
  if (im.size()==0) { mean=0; return; }
  vil3d_math_sum(mean,im,p);
  mean/=(im.ni()*im.nj()*im.nk());
}


//: Sum of squares of elements in plane p of image
// \relatesalso vil3d_image_view
template <class imT, class sumT>
inline void vil3d_math_sum_squares(sumT& sum, sumT& sum_sq,
                                   const vil3d_image_view<imT>& im, unsigned p)
{
  assert(p<im.nplanes());
  sum = 0; sum_sq=0;
  if (im.size()==0)
  {
    return;
  }

  const imT* plane = im.origin_ptr();
  unsigned ni = im.ni();
  unsigned nj = im.nj();
  unsigned nk = im.nk();
  std::ptrdiff_t istep = im.istep(), jstep=im.jstep(), kstep=im.kstep();

  const imT* slice = plane + p*im.planestep();
  for (unsigned int k=0;k<nk;++k, slice += kstep)
  {
    const imT* row = slice;
    for (unsigned int j=0;j<nj;++j, row += jstep)
    {
      const imT* r = row;
      for (unsigned int i=0;i<ni;++i, r+=istep)
      { sum += *r; sum_sq+=sumT(*r)*sumT(*r); }
    }
  }
}


//: Sum of squared differences between two images
// \relatesalso vil_image_view
template <class imT, class sumT>
inline sumT vil3d_math_ssd(const vil3d_image_view<imT>& imA,
                           const vil3d_image_view<imT>& imB, sumT /*dummy*/)
{
  assert(imA.ni() == imB.ni() && imB.nj() == imB.nj() &&
         imB.nk() == imB.nk() && imA.nplanes() == imB.nplanes());
  sumT ssd=0;
  for (unsigned p=0;p<imA.nplanes();++p)
    for (unsigned k=0;k<imA.nk();++k)
      for (unsigned j=0;j<imA.nj();++j)
        for (unsigned i=0;i<imA.ni();++i)
        {
          const sumT v = ((sumT)imA(i,j,k,p) - (sumT)imB(i,j,k,p));
          ssd += v*v;
        }
  return ssd;
}


//: Multiply values in-place in image view by scale and add offset
// \relatesalso vil3d_image_view
template<class imT, class offsetT>
inline void vil3d_math_scale_and_offset_values(vil3d_image_view<imT>& image,
                                               double scale, offsetT offset)
{
  unsigned ni = image.ni(), nj = image.nj(),
    nk = image.nk(), np = image.nplanes();
  std::ptrdiff_t istep=image.istep(), jstep=image.jstep(),
    kstep = image.kstep(), pstep = image.planestep();
  imT* plane = image.origin_ptr();
  for (unsigned p=0;p<np;++p,plane += pstep)
  {
    imT* slice = plane;
    for (unsigned k=0;k<nk;++k,slice += kstep)
    {
      imT* row = slice;
      for (unsigned j=0;j<nj;++j,row += jstep)
      {
        imT* pixel = row;
        for (unsigned i=0;i<ni;++i,pixel+=istep)
          *pixel = imT(scale*(*pixel)+offset);
      }
    }
  }
}


//: Mean and variance of elements in plane p of image
// \relatesalso vil3d_image_view
template <class imT, class sumT>
inline void vil3d_math_mean_and_variance(sumT& mean, sumT& var,
                                         const vil3d_image_view<imT>& im,
                                         unsigned p)
{
  if (im.size()==0) { mean=0; var=0; return; }
  sumT sum, sum_sq;
  vil3d_math_sum_squares(sum,sum_sq,im,p);
  mean = sum/(im.ni()*im.nj()*im.nk());
  var = sum_sq/(im.ni()*im.nj()*im.nk()) - mean*mean;
}


//: Mean and variance of elements in plane p of image
// \relatesalso vil3d_image_view
template <class imT, class sumT>
inline sumT vil3d_math_dot_product(const vil3d_image_view<imT>& imA,
                                   const vil3d_image_view<imT>& imB, sumT)
{
  assert(imA.ni() == imB.ni() && imB.nj() == imB.nj() &&
         imB.nk() == imB.nk() && imA.nplanes() == imB.nplanes());
  sumT dp=0;
  for (unsigned p=0;p<imA.nplanes();++p)
    for (unsigned k=0;k<imA.nk();++k)
      for (unsigned j=0;j<imA.nj();++j)
        for (unsigned i=0;i<imA.ni();++i)
          dp += (sumT)imA(i,j,k,p) * (sumT)imB(i,j,k,p);
  return dp;
}


//: Compute difference of two images (im_sum = imA-imB)
// \relatesalso vil_image_view
template<class aT, class bT, class sumT>
inline void vil3d_math_image_difference(const vil3d_image_view<aT>& imA,
                                        const vil3d_image_view<bT>& imB,
                                        vil3d_image_view<sumT>& im_sum)
{
  unsigned ni = imA.ni(),nj = imA.nj(),nk = imA.nk(),np = imA.nplanes();
  assert(imB.ni()==ni && imB.nj()==nj && imB.nk()==nk && imB.nplanes()==np);
  im_sum.set_size(ni,nj,nk,np);

  std::ptrdiff_t istepA=imA.istep(),jstepA=imA.jstep(),kstepA=imA.kstep(),pstepA = imA.planestep();
  std::ptrdiff_t istepB=imB.istep(),jstepB=imB.jstep(),kstepB=imB.kstep(),pstepB = imB.planestep();
  std::ptrdiff_t istepS=im_sum.istep(),jstepS=im_sum.jstep(),kstepS=im_sum.kstep(),pstepS = im_sum.planestep();
  const aT* planeA = imA.origin_ptr();
  const bT* planeB = imB.origin_ptr();
  sumT* planeS     = im_sum.origin_ptr();
  for (unsigned p=0;p<np;++p,planeA += pstepA,planeB += pstepB,planeS += pstepS)
  {
    const aT* sliceA   = planeA;
    const bT* sliceB   = planeB;
    sumT* sliceS = planeS;
    for (unsigned k=0;k<nk;++k,sliceA += kstepA,sliceB += kstepB,sliceS += kstepS)
    {
      const aT* rowA = sliceA;
      const bT* rowB = sliceB;
      sumT* rowS = sliceS;
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
}

//: Compute sum of two images (im_sum = imA+imB)
// \relatesalso vil_image_view
template<class aT, class bT, class sumT>
inline void vil3d_math_image_sum(const vil3d_image_view<aT>& imA,
                                 const vil3d_image_view<bT>& imB,
                                 vil3d_image_view<sumT>& im_sum)
{
  unsigned ni = imA.ni(),nj = imA.nj(),nk = imA.nk(),np = imA.nplanes();
  assert(imB.ni()==ni && imB.nj()==nj && imB.nk()==nk && imB.nplanes()==np);
  im_sum.set_size(ni,nj,nk,np);

  std::ptrdiff_t istepA=imA.istep(),jstepA=imA.jstep(),kstepA=imA.kstep(),pstepA = imA.planestep();
  std::ptrdiff_t istepB=imB.istep(),jstepB=imB.jstep(),kstepB=imB.kstep(),pstepB = imB.planestep();
  std::ptrdiff_t istepS=im_sum.istep(),jstepS=im_sum.jstep(),kstepS=im_sum.kstep(),pstepS = im_sum.planestep();
  const aT* planeA = imA.origin_ptr();
  const bT* planeB = imB.origin_ptr();
  sumT* planeS     = im_sum.origin_ptr();
  for (unsigned p=0;p<np;++p,planeA += pstepA,planeB += pstepB,planeS += pstepS)
  {
    const aT* sliceA   = planeA;
    const bT* sliceB   = planeB;
    sumT* sliceS = planeS;
    for (unsigned k=0;k<nk;++k,sliceA += kstepA,sliceB += kstepB,sliceS += kstepS)
    {
      const aT* rowA = sliceA;
      const bT* rowB = sliceB;
      sumT* rowS = sliceS;
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
}

//: Compute pixel-wise product of two images (im_prod = imA*imB)
// \relatesalso vil_image_view
template<class aT, class bT, class prodT>
inline void vil3d_math_image_product(const vil3d_image_view<aT>& imA,
                                     const vil3d_image_view<bT>& imB,
                                     vil3d_image_view<prodT>& im_prod)
{
  unsigned ni = imA.ni(),nj = imA.nj(),nk = imA.nk(),np = imA.nplanes();
  assert(imB.ni()==ni && imB.nj()==nj && imB.nk()==nk && imB.nplanes()==np);
  im_prod.set_size(ni,nj,nk,np);

  std::ptrdiff_t istepA=imA.istep(),jstepA=imA.jstep(),kstepA=imA.kstep(),pstepA = imA.planestep();
  std::ptrdiff_t istepB=imB.istep(),jstepB=imB.jstep(),kstepB=imB.kstep(),pstepB = imB.planestep();
  std::ptrdiff_t istepS=im_prod.istep(),jstepS=im_prod.jstep(),kstepS=im_prod.kstep(),pstepS = im_prod.planestep();
  const aT* planeA = imA.origin_ptr();
  const bT* planeB = imB.origin_ptr();
  prodT* planeS     = im_prod.origin_ptr();
  for (unsigned p=0;p<np;++p,planeA += pstepA,planeB += pstepB,planeS += pstepS)
  {
    const aT* sliceA   = planeA;
    const bT* sliceB   = planeB;
    prodT* sliceS = planeS;
    for (unsigned k=0;k<nk;++k,sliceA += kstepA,sliceB += kstepB,sliceS += kstepS)
    {
      const aT* rowA = sliceA;
      const bT* rowB = sliceB;
      prodT* rowS = sliceS;
      for (unsigned j=0;j<nj;++j,rowA += jstepA,rowB += jstepB,rowS += jstepS)
      {
        const aT* pixelA = rowA;
        const bT* pixelB = rowB;
        prodT* pixelS = rowS;
        for (unsigned i=0;i<ni;++i,pixelA+=istepA,pixelB+=istepB,pixelS+=istepS)
          *pixelS = prodT(*pixelA)*prodT(*pixelB);
      }
    }
  }
}


//: imA = fa*imA + fb*imB  (Useful for moving averages!)
// Can do running sum using vil_add_image_fraction(running_mean,1-f,new_im,f)
// to update current mean by a fraction f of new_im
// \relatesalso vil_image_view
template<class aT, class bT, class scaleT>
inline void vil3d_math_add_image_fraction(vil3d_image_view<aT>& imA, scaleT fa,
                                          const vil3d_image_view<bT>& imB, scaleT fb)
{
  unsigned ni = imA.ni(),nj = imA.nj(),nk = imA.nk(),np = imA.nplanes();
  assert(imB.ni()==ni && imB.nj()==nj && imB.nk()==nk && imB.nplanes()==np);

  std::ptrdiff_t istepA=imA.istep(),jstepA=imA.jstep(),kstepA=imA.kstep(),pstepA = imA.planestep();
  std::ptrdiff_t istepB=imB.istep(),jstepB=imB.jstep(),kstepB=imB.kstep(),pstepB = imB.planestep();
  aT* planeA = imA.origin_ptr();
  const bT* planeB = imB.origin_ptr();
  for (unsigned p=0;p<np;++p,planeA += pstepA,planeB += pstepB)
  {
    aT* sliceA   = planeA;
    const bT* sliceB   = planeB;
    for (unsigned k=0;k<nk;++k,sliceA += kstepA,sliceB += kstepB)
    {
      aT* rowA = sliceA;
      const bT* rowB = sliceB;
      for (unsigned j=0;j<nj;++j,rowA += jstepA,rowB += jstepB)
      {
        aT* pixelA = rowA;
        const bT* pixelB = rowB;
        for (unsigned i=0;i<ni;++i,pixelA+=istepA,pixelB+=istepB)
          *pixelA = aT(fa*(*pixelA)+fb*(*pixelB));
      }
    }
  }
}

//: Truncate each pixel value so it fits into range [min_v,max_v]
//  If value < min_v value=min_v
//  If value > max_v value=max_v
// \relatesalso vil3d_image_view
template<class T>
inline void vil3d_math_truncate_range(vil3d_image_view<T>& image,
                                      T min_v, T max_v)
{
  unsigned ni=image.ni(), nj=image.nj(), nk=image.nk(), np=image.nplanes();
  std::ptrdiff_t istep=image.istep(), jstep=image.jstep(),
                kstep=image.kstep(), pstep=image.planestep();
  T* plane = image.origin_ptr();
  for (unsigned p=0; p<np; ++p, plane+=pstep)
  {
    T* slice = plane;
    for (unsigned k=0; k<nk; ++k, slice+=kstep)
    {
      T* row = slice;
      for (unsigned j=0; j<nj; ++j, row+=jstep)
      {
        T* pixel = row;
        for (unsigned i=0; i<ni; ++i, pixel+=istep)
        {
          if (*pixel<min_v) *pixel=min_v;
          else if (*pixel>max_v) *pixel=max_v;
        }
      }
    }
  }
}

//: Calc integral image im_sum(i+1,j+1,k+1)= sum (x<=i,y<=j,z<=k) imA(x,y,z)
//  Useful thing for quickly computing mean over large regions,
//  as demonstrated in Viola and Jones (CVPR01).
// The sum of elements in the ni x nj x nk volume with corner (i,j,k)
// is given by
// im_sum(i,j,k+nk)+im_sum(i+ni,j+nj,j+nk)+im_sum(i,j+nj,k)+im_sum(i+ni,j,k)
// -im_sum(i,j,k)-im_sum(i+ni,j+nj,k)-im_sum(i+ni,n,k_nk)-im_sum(i,j+nj,k+nk
// \relatesalso vil_image_view
template<class aT, class sumT>
inline void vil3d_math_integral_image(const vil3d_image_view<aT>& imA,
                                    vil3d_image_view<sumT>& im_sum)
{
  assert(imA.nplanes()==1);
  unsigned ni = imA.ni(),nj = imA.nj(),nk = imA.nk();
  unsigned ni1=ni+1;
  unsigned nj1=nj+1;
  unsigned nk1=nk+1;
  im_sum.set_size(ni1,nj1,nk1,1);

  // Put zeros along first plane of im_sum
  std::ptrdiff_t istepS=im_sum.istep();
  std::ptrdiff_t jstepS=im_sum.jstep();
  std::ptrdiff_t kstepS=im_sum.kstep();
  sumT* planeS = im_sum.origin_ptr();
  sumT* rowS = planeS;
  sumT* voxelS;
  for (unsigned j=0;j<nj1;++j,rowS += jstepS)
  {
    voxelS = rowS;
    for (unsigned i=0;i<ni1;++i,voxelS+=istepS)
      *voxelS=0;
  }

  // Now sum from original image (imA)
  std::ptrdiff_t istepA=imA.istep();
  std::ptrdiff_t jstepA=imA.jstep();
  std::ptrdiff_t kstepA=imA.kstep();

  const aT* planeA = imA.origin_ptr();

  sumT sum;
  std::ptrdiff_t prev_j = -jstepS;
  std::ptrdiff_t prev_k = -kstepS;
  planeS += kstepS;

  // for each plane, do a 2D integral image first
  for (unsigned k=0;k<nk;++k,planeA += kstepA,planeS += kstepS)
  {
    const aT* rowA = planeA;
    rowS = planeS;

    // Put zeros along first row in plane
    voxelS = rowS;
    for (unsigned i=0;i<ni1;++i,voxelS += istepS)
      *voxelS = 0;
    rowS += jstepS;

    // Compute integral sums for rest of plane
    for (unsigned j=0;j<nj;++j,rowA += jstepA,rowS += jstepS)
    {
      const aT* voxelA = rowA;
      voxelS = rowS;
      sum = 0;

      // set value at first column of each row to zero!
      *voxelS = 0;
      voxelS += istepS;

      // compute the 2D integral image
      for (unsigned i=1; i<ni1;++i,voxelA+=istepA,voxelS+=istepS)
      {
        sum += *voxelA;
        *voxelS = sum + voxelS[prev_j];
      }
    }
  }

  // Go through from plane 2 to end and add values of voxels in plane
  // above to get the 3D integral image
  planeS = im_sum.origin_ptr() + 2*kstepS;
  for (unsigned k=2;k<nk1;k++,planeS += kstepS)
  {
    // Skip first row in each plane
    rowS = planeS+jstepS;
    for (unsigned j=1;j<nj1;j++,rowS += jstepS)
    {
      // Skip first col in each row
      voxelS = rowS+istepS;
      for (unsigned i=1;i<ni1;i++,voxelS += istepS)
        *voxelS += voxelS[prev_k];
    }
  }

}


//: Calc integral image im_sum_sq(i+1,j+1,k+1) = sum (x<=i,y<=j,z<=k) imA^2
//  Also calcs integral image, im_sum(i+1,j+1,k+1 = sum (x=i,y<=j,z<=kk) imA
//  Useful thing for quickly computing mean and variance over large regions,
//  as demonstrated in Viola and Jones (CVPR01).
// The sum of elements in the ni x nj x nk volume with corner (i,j,k)
// is given by:
// im_sum(i,j,k+nk)+im_sum(i+ni,j+nj,j+nk)+im_sum(i,j+nj,k)+im_sum(i+ni,j,k)
// -im_sum(i,j,k)-im_sum(i+ni,j+nj,k)-im_sum(i+ni,n,k_nk)-im_sum(i,j+nj,k+nk
// \relatesalso vil_image_view
template<class aT, class sumT>
inline void vil3d_math_integral_sqr_image(const vil3d_image_view<aT>& imA,
                                    vil3d_image_view<sumT>& im_sum,
                                    vil3d_image_view<sumT>& im_sum_sq)
{
  assert(imA.nplanes()==1);
  unsigned ni = imA.ni(),nj = imA.nj(),nk = imA.nk();
  unsigned ni1=ni+1;
  unsigned nj1=nj+1;
  unsigned nk1=nk+1;
  im_sum.set_size(ni1,nj1,nk1,1);
  im_sum_sq.set_size(ni1,nj1,nk1,1);

  // Put zeros along first plane of im_sum & im_sum_sq
  std::ptrdiff_t istepS=im_sum.istep();
  std::ptrdiff_t istepS2=im_sum_sq.istep();
  std::ptrdiff_t jstepS=im_sum.jstep();
  std::ptrdiff_t jstepS2=im_sum_sq.jstep();
  std::ptrdiff_t kstepS=im_sum.kstep();
  std::ptrdiff_t kstepS2=im_sum_sq.kstep();
  sumT* planeS = im_sum.origin_ptr();
  sumT* planeS2 = im_sum_sq.origin_ptr();
  sumT* rowS = planeS;
  sumT* rowS2 = planeS2;
  sumT *voxelS, *voxelS2;

  // im_sum
  for (unsigned j=0;j<nj1;++j,rowS += jstepS)
  {
    voxelS = rowS;
    for (unsigned i=0;i<ni1;++i,voxelS+=istepS)
      *voxelS=0;
  }

  // im_sum_sq
  for (unsigned j=0;j<nj1;++j,rowS2 += jstepS2)
  {
    voxelS2 = rowS2;
    for (unsigned i=0;i<ni1;++i,voxelS2+=istepS2)
      *voxelS2=0;
  }

  // Now sum from original image (imA)
  std::ptrdiff_t istepA=imA.istep();
  std::ptrdiff_t jstepA=imA.jstep();
  std::ptrdiff_t kstepA=imA.kstep();

  const aT* planeA = imA.origin_ptr();

  sumT sum, sum2;
  std::ptrdiff_t prev_j = -jstepS;
  std::ptrdiff_t prev_k = -kstepS;
  std::ptrdiff_t prev_j2 = -jstepS2;
  std::ptrdiff_t prev_k2 = -kstepS2;
  planeS += kstepS;
  planeS2 += kstepS2;

  // for each plane, do a 2D integral image first
  for (unsigned k=0;k<nk;++k,planeA+=kstepA,planeS+=kstepS,planeS2+=kstepS2)
  {
    const aT* rowA = planeA;
    rowS = planeS;
    rowS2 = planeS2;

    // Put zeros along first row in plane
    voxelS = rowS;
    voxelS2 = rowS2;
    for (unsigned i=0;i<ni1;++i,voxelS += istepS, voxelS2 += istepS2)
    {
      *voxelS = 0;
      *voxelS2 = 0;
    }
    rowS += jstepS;
    rowS2 += jstepS2;

    // Compute integral sums for rest of plane
    for (unsigned j=0;j<nj;++j,rowA += jstepA,rowS += jstepS,rowS2+=jstepS2)
    {
      const aT* voxelA = rowA;
      voxelS = rowS;
      voxelS2 = rowS2;
      sum = 0;
      sum2 = 0;

      // set value at first column of each row to zero!
      *voxelS = 0;
      *voxelS2 = 0;
      voxelS += istepS;
      voxelS2 += istepS2;

      // compute the 2D integral image
      for (unsigned i=1;i<ni1;++i,voxelA+=istepA,
                                  voxelS+=istepS,voxelS2+=istepS2)
      {
        sum += *voxelA;
        *voxelS = sum + voxelS[prev_j];
        sum2 += sumT(*voxelA)*sumT(*voxelA);
        *voxelS2 = sum2 + voxelS2[prev_j2];
      }
    }
  }

  // Go through from plane 2 to end and add values of voxels in plane
  // above to get the 3D integral image
  planeS = im_sum.origin_ptr() + 2*kstepS;
  planeS2 = im_sum_sq.origin_ptr() + 2*kstepS2;
  for (unsigned k=2;k<nk1;k++,planeS += kstepS, planeS2 += kstepS2)
  {
    // Skip first row in each plane
    rowS = planeS+jstepS;
    rowS2 = planeS2+jstepS2;
    for (unsigned j=1;j<nj1;j++,rowS += jstepS,rowS2 += jstepS2)
    {
      // Skip first col in each row
      voxelS = rowS+istepS;
      voxelS2 = rowS2+istepS2;
      for (unsigned i=1;i<ni1;i++,voxelS += istepS,voxelS2 += istepS2)
      {
        *voxelS += voxelS[prev_k];
        *voxelS2 += voxelS2[prev_k2];
      }
    }
  }

}

// Scale and offset each voxel value so each plane has zero mean and unit variance.
template<class T>
inline void vil3d_math_normalise(vil3d_image_view<T>& im)
{
  double mean,var;
  for (unsigned p=0;p<im.nplanes();++p)
  {
    vil3d_math_mean_and_variance(mean,var,im,p);
    double sum_sq=var*im.ni()*im.nj()*im.nk();
    double s=1.0/std::max(1e-6,std::sqrt(sum_sq));
    vil3d_image_view<float> imp=vil3d_plane(im,p);
    vil3d_math_scale_and_offset_values(imp,s,-mean*s);
  }
}

#endif
