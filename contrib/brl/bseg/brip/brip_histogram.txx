// This is brl/bseg/brip/brip_histogram.txx
#ifndef brip_histogram_txx_
#define brip_histogram_txx_
//:
// \file
// \brief Construct histogram from pixels in given image.
// \author Matt Leotta
//

#include <brip/brip_histogram.h>
#include <vcl_cassert.h>
#include <vcl_algorithm.h>

//: Construct histogram from pixels in the given image.
template<class T>
double brip_histogram(const vil_image_view<T>& image,
                      vcl_vector<double>& histo,
                      double min, double max, unsigned n_bins)
{
  histo.resize(n_bins);
  vcl_fill(histo.begin(),histo.end(),0.0);
  double x0 = double(min);
  double s = double(n_bins-1)/(double(max)-x0);
  double total_weight = 0.0;

  unsigned ni=image.ni(), nj=image.nj(), np=image.nplanes();
  vcl_ptrdiff_t istep=image.istep(), jstep=image.jstep(), pstep=image.planestep();

  const T* plane = image.top_left_ptr();
  for (unsigned p=0; p<np; ++p,plane+=pstep)
  {
    const T* row = plane;
    for (unsigned j=0; j<nj; ++j,row+=jstep)
    {
      const T* pixel = row;
      for (unsigned i=0; i<ni; ++i,pixel+=istep)
      {
        int index = int(0.5+s*(double(*pixel) - x0));
        if (index>=0 && (unsigned)index<n_bins){
          histo[index]+= 1.0;
          total_weight += 1.0;
        }
      }
    }
  }
  return total_weight;
}


//: Construct weighted histogram from pixels in the given image using values in an image of weights.
template<class T>
double brip_weighted_histogram(const vil_image_view<T>& image,
                               const vil_image_view<double>& weights,
                               vcl_vector<double>& histo,
                               double min, double max, unsigned n_bins)
{
  assert( (image.ni() == weights.ni()) &&
          (image.nj() == weights.nj()) &&
          (image.nplanes() == weights.nplanes()) );

  histo.resize(n_bins);
  vcl_fill(histo.begin(),histo.end(),0.0);
  double x0 = double(min);
  double s = double(n_bins-1)/(double(max)-x0);
  double total_weight = 0.0;

  unsigned ni=image.ni(), nj=image.nj(), np=image.nplanes();
  vcl_ptrdiff_t i_istep=image.istep(), i_jstep=image.jstep(), i_pstep=image.planestep();
  vcl_ptrdiff_t w_istep=weights.istep(), w_jstep=weights.jstep(), w_pstep=weights.planestep();

  const T* i_plane = image.top_left_ptr();
  const double* w_plane = weights.top_left_ptr();
  for (unsigned p=0; p<np; ++p,i_plane+=i_pstep,w_plane+=w_pstep)
  {
    const T* i_row = i_plane;
    const double* w_row = w_plane;
    for (unsigned j=0; j<nj; ++j,i_row+=i_jstep,w_row+=w_jstep)
    {
      const T* i_pixel = i_row;
      const double* w_pixel = w_row;
      for (unsigned i=0; i<ni; ++i,i_pixel+=i_istep,w_pixel+=w_istep)
      {
        int index = int(0.5+s*(double(*i_pixel) - x0));
        if (index>=0 && (unsigned)index<n_bins){
          histo[index]+= *w_pixel;
          total_weight += *w_pixel;
        }
      }
    }
  }
  return total_weight;
}


//: Construct the joint histogram between image1 and image2
template<class T>
double brip_joint_histogram(const vil_image_view<T>& image1,
                            const vil_image_view<T>& image2,
                            vcl_vector<vcl_vector<double> >& histo,
                            double min, double max, unsigned n_bins)
{
  assert( (image1.ni() == image2.ni()) &&
          (image1.nj() == image2.nj()) &&
          (image1.nplanes() == image2.nplanes()) );

  vcl_vector<double> empty(n_bins, 0.0);
  histo.clear();
  for (unsigned i=0; i<n_bins; ++i)
    histo.push_back(empty);

  double x0 = double(min);
  double s = double(n_bins-1)/(double(max)-x0);
  double total_weight = 0.0;

  unsigned ni=image1.ni(), nj=image1.nj(), np=image1.nplanes();
  vcl_ptrdiff_t istep1=image1.istep(), jstep1=image1.jstep(), pstep1=image1.planestep();
  vcl_ptrdiff_t istep2=image2.istep(), jstep2=image2.jstep(), pstep2=image2.planestep();

  const T* plane1 = image1.top_left_ptr();
  const T* plane2 = image2.top_left_ptr();
  for (unsigned p=0; p<np; ++p,plane1+=pstep1,plane2+=pstep2)
  {
    const T* row1 = plane1;
    const T* row2 = plane2;
    for (unsigned j=0; j<nj; ++j,row1+=jstep1,row2+=jstep2)
    {
      const T* pixel1 = row1;
      const T* pixel2 = row2;
      for (unsigned i=0; i<ni; ++i,pixel1+=istep1,pixel2+=istep2)
      {
        int index1 = int(0.5+s*(double(*pixel1) - x0));
        int index2 = int(0.5+s*(double(*pixel2) - x0));
        if (index1>=0 && (unsigned)index1<n_bins &&
            index2>=0 && (unsigned)index2<n_bins){
          histo[index1][index2] += 1;
          total_weight += 1;
        }
      }
    }
  }
  return total_weight;
}


// Macro to perform manual instantiations
#define BRIP_HISTOGRAM_INSTANTIATE(T) \
  template \
  double brip_histogram(const vil_image_view<T >& image, \
                        vcl_vector<double>& histo, \
                        double min, double max, unsigned n_bins); \
  template \
  double brip_weighted_histogram(const vil_image_view<T >& image, \
                                 const vil_image_view<double>& weights, \
                                 vcl_vector<double>& histo, \
                                 double min, double max, unsigned n_bins); \
  template \
  double brip_joint_histogram(const vil_image_view<T >& image1, \
                              const vil_image_view<T >& image2, \
                              vcl_vector<vcl_vector<double> >& histo, \
                              double min, double max, unsigned n_bins)

#endif // brip_histogram_txx_
