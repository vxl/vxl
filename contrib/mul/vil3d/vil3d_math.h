// This is mul/vil3d/vil3d_math.h
#ifndef vil3d_math_h_
#define vil3d_math_h_
//:
// \file
// \brief Various mathematical manipulations of 3D images
// \author Tim Cootes

#include <vcl_cassert.h>
#include <vcl_vector.h>
#include <vcl_functional.h> // for std::less<T>
#include <vil3d/vil3d_image_view.h>
#include <vil3d/vil3d_plane.h>
#include <vcl_algorithm.h>


//: Compute minimum and maximum values over im
// \relates vil3d_image_view
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
  vcl_ptrdiff_t istep = im.istep(), jstep=im.jstep(), kstep=im.kstep();

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
// \relates vil3d_image_view
// \note This function requires the sorting of large parts of the image data
// and can be very expensive in terms of both processing and memory.
// \sa vil3d_math_value_range_percentiles()
template <class T>
inline void vil3d_math_value_range_percentile(const vil3d_image_view<T>& im,
                                              const double fraction,
                                              T& value)
{
  vcl_vector<double> fractions(1, fraction);
  vcl_vector<T> values;
  vil3d_math_value_range_percentiles(im, fractions, values);
  if (values.size() > 0)
    value = values[0]; // Bounds-checked access in case previous line failed.
}


//: Compute value corresponding to several percentiles of the range of im.
// Percentiles expressed as fraction, e.g. 0.05, or 0.95.
// \param im The image to examine.
// \param fraction The fraction of the data range (from the lower end).
// \retval value The image data value corresponding to the specified percentiles.
// \relates vil3d_image_view
// \note This function requires the sorting of large parts of the image data
// and can be very expensive in terms of both processing and memory.
template <class T>
inline void vil3d_math_value_range_percentiles(const vil3d_image_view<T>& im,
                                               const vcl_vector<double> fraction,
                                               vcl_vector<T>& value)
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
  vcl_ptrdiff_t istep = im.istep();
  vcl_ptrdiff_t jstep=im.jstep();
  vcl_ptrdiff_t kstep=im.kstep();
  vcl_ptrdiff_t pstep = im.planestep();
  vcl_vector<T> data(ni*nj*nk*np);

  typename vcl_vector<T>::iterator it = data.begin();
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
    typename vcl_vector<T>::iterator index_it = data.begin() + index;
    vcl_nth_element(data.begin(), index_it, data.end());
    value[f] = *index_it;
  }
}


//: Calc the mean of each pixel over all the planes.
// \relates vil3d_image_view
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
// \relates vil3d_image_view
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


//: Compute sum of values in plane p
// \relates vil3d_image_view
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
  vcl_ptrdiff_t istep = im.istep(), jstep=im.jstep(), kstep=im.kstep();

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
// \relates vil3d_image_view
template <class imT, class sumT>
inline void vil3d_math_mean(sumT& mean, const vil3d_image_view<imT>& im,
                            unsigned p)
{
  if (im.size()==0) { mean=0; return; }
  vil3d_math_sum(mean,im,p);
  mean/=(im.ni()*im.nj()*im.nk());
}


//: Sum of squares of elements in plane p of image
// \relates vil3d_image_view
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
  vcl_ptrdiff_t istep = im.istep(), jstep=im.jstep(), kstep=im.kstep();

  const imT* slice = plane + p*im.planestep();
  for (unsigned int k=0;k<nk;++k, slice += kstep)
  {
    const imT* row = slice;
    for (unsigned int j=0;j<nj;++j, row += jstep)
    {
      const imT* p = row;
      for (unsigned int i=0;i<ni;++i, p+=istep)
      { sum += *p; sum_sq+=sumT(*p)*sumT(*p); }
    }
  }
}


//: Multiply values in-place in image view by scale and add offset
// \relates vil3d_image_view
template<class imT, class offsetT>
inline void vil3d_math_scale_and_offset_values(vil3d_image_view<imT>& image,
double scale, offsetT offset)
{
  unsigned ni = image.ni(), nj = image.nj(),
    nk = image.nk(), np = image.nplanes();
  vcl_ptrdiff_t istep=image.istep(), jstep=image.jstep(),
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
// \relates vil3d_image_view
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

#endif
