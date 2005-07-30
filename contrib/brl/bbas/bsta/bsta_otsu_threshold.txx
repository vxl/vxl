#ifndef bsta_otsu_threshold_txx_
#define bsta_otsu_threshold_txx_

#include <bsta/bsta_otsu_threshold.h>
#include <vcl_cmath.h>

template <class T>
bsta_otsu_threshold<T>::bsta_otsu_threshold(bsta_histogram<T> const& hist)
: distribution_1d_(hist)
{
}

template <class T>
bsta_otsu_threshold<T>::bsta_otsu_threshold(vcl_vector<T> data, T low , T high)
:distribution_1d_(low,high,data)
{
}

template <class T>
bsta_otsu_threshold<T>::~bsta_otsu_threshold()
{
}

template <class T>
unsigned bsta_otsu_threshold<T>::bin_threshold()
{
  T no_of_pixels_in_class1=0;
  T no_of_pixels_in_class2=0;

  int threshold =0;
  T between_class_variance = 0;
  T max_between_class_variance=0;
  no_of_pixels_in_class2 = distribution_1d_.area();

  for (unsigned t=0; t<distribution_1d_.nbins(); ++t)
  {
    T mu1 = distribution_1d_.mean(0, t);
    T mu2 = distribution_1d_.mean(t, distribution_1d_.nbins()-1);

    between_class_variance=no_of_pixels_in_class1*no_of_pixels_in_class2*
                           (mu1-mu2)*(mu1-mu2);

    if (between_class_variance>max_between_class_variance)
    {
      max_between_class_variance=between_class_variance;
      threshold=t;
    }
    no_of_pixels_in_class1+=distribution_1d_.counts(t);
    no_of_pixels_in_class2-=distribution_1d_.counts(t);
  }

  return threshold;
}

#undef BSTA_OTSU_THRESHOLD_INSTANTIATE
#define BSTA_OTSU_THRESHOLD_INSTANTIATE(T) \
template class bsta_otsu_threshold<T >

#endif // bsta_otsu_threshold_txx_
