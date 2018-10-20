#ifndef vil_pyramid_image_view_hxx_
#define vil_pyramid_image_view_hxx_

#include "vil_pyramid_image_view.h"
#include <vil/vil_image_view.h>
#include <vil/vil_resample_bilin.h>

template <class T>
vil_pyramid_image_view<T>::vil_pyramid_image_view(unsigned levels, unsigned ni,
                                                  unsigned nj, unsigned n_planes)
  : nlevels_(levels), max_levels_(256)
{
  images_.resize(levels);
  scales_.resize(levels);
  unsigned i=ni, j=nj;
  double scale=1.0;
  for (unsigned l=0; l<levels && !limit_reached(i,j); l++) {
    images_[l] = new vil_image_view<T>(i, j, n_planes);
    scales_[l] = scale;
    i/=2;j/=2;
    scale/=2.0;
  }
}

template <class T>
vil_pyramid_image_view<T>::vil_pyramid_image_view(vil_image_view_base_sptr image, unsigned levels)
: nlevels_(levels), max_levels_(256)
{
  vil_image_view<T>* img=dynamic_cast<vil_image_view<T>*>(image.ptr());
  if (!img)
    return;

  // it works with grey scale images
  if (img->nplanes() != 1)
    return;

  images_.resize(levels);
  scales_.resize(levels);
  unsigned i=image->ni()/2, j=image->nj()/2;
  double scale=1.0;
  images_[0] = image;
  scales_[0] = 1.0;

  for (unsigned l=1; l<levels && !limit_reached(i,j); l++) {
    // scale down the image
    scale/=2.0;
    img = static_cast<vil_image_view<T>*>(images_[l-1].ptr());
    vil_image_view_base_sptr new_img=nullptr;
    scale_down(*img, new_img);
    images_[l] = new_img;
    scales_[l] = scale;
    i/=2;j/=2;
  }
}

template <class T>
vil_pyramid_image_view<T>::vil_pyramid_image_view(std::vector<vil_image_view_base_sptr> const& images,
                                                  std::vector<double> const& scales)
{
  nlevels_=(unsigned int)(images.size());
  images_.resize(nlevels_);
  scales_.resize(nlevels_);
  for (unsigned l=0; l<nlevels_; l++) {
    images_[l] = images[l];
    scales_[l] = scales[l];
  }
}

template <class T>
vil_pyramid_image_view<T>::vil_pyramid_image_view(const vil_pyramid_image_view<T>& rhs)
{
  this->images_.resize(rhs.nlevels());
  this->nlevels_ = rhs.nlevels();
  this->images_ = rhs.images_;
  this->scales_ = rhs.scales_;
}

template <class T>
void vil_pyramid_image_view<T>::add_view(vil_image_view_base_sptr &image, double scale)
{
  // find the place of the image based on the ordered scales
  unsigned i=0;
  std::vector<vil_image_view_base_sptr>::iterator image_iter=images_.begin();
  std::vector<double>::iterator scale_iter= scales_.begin();
  while (i<nlevels_ && scale<scales_[i]) {
    i++;
    image_iter++;
    scale_iter++;
  }

  // insert into the vector
  images_.insert(image_iter,image);
  scales_.insert(scale_iter,scale);
  nlevels_++;
}

template <class T>
const vil_pyramid_image_view<T>&
vil_pyramid_image_view<T>::operator=(const vil_pyramid_image_view<T>& rhs)
{
  this->images_.resize(rhs.nlevels());
  this->max_levels_=rhs.max_levels();
  this->nlevels_ = rhs.nlevels();
  this->images_ = rhs.images_;
  this->scales_ = rhs.scales_;
  return *this;
}

template <class T>
void vil_pyramid_image_view<T>::scale_down(const vil_image_view<T>& image_in,
                                           vil_image_view_base_sptr& image_out)
{
  unsigned ni=image_in.ni();
  unsigned nj=image_in.nj();
  unsigned ni2=ni/2;
  unsigned nj2=nj/2;
  vil_image_view<T>* half_size = new vil_image_view<T>(ni2, nj2);
  vil_resample_bilin(image_in, *half_size, ni2, nj2);
  image_out = half_size;
}


#define VIL_PYRAMID_IMAGE_VIEW_INSTANTIATE(T) \
template class vil_pyramid_image_view<T >

#endif
