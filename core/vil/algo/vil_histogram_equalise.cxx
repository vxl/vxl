#include "vil_histogram_equalise.h"
//:
//  \file
//  \brief Apply histogram equalisation to given image
//  \author Tim Cootes

#include <vil/algo/vil_histogram.h>
#include <vil/vil_math.h>


//: Apply histogram equalisation to given byte image
void vil_histogram_equalise(vil_image_view<vxl_byte>& image)
{
  std::vector<double> histo(256);
  vil_histogram_byte(image,histo);

  // Create cumulative frequency curve
  double sum=0.0;
  for (unsigned i=0;i<256;++i) { sum+=histo[i]; histo[i]=sum; }

  // Parameters of mapping
  int lo = 0;
  // Find smallest value in image
  while (histo[lo]==0) lo++;
  double x0 = histo[lo];
  double s =255.1/(sum-x0);  // Smallest values get mapped to zero

  std::vector<vxl_byte> lookup(256);
  vxl_byte* lup = &lookup[0];
  for (unsigned i=0;i<256;++i) { lup[i]= vxl_byte(s*(histo[i]-x0)); }

  unsigned ni = image.ni(),nj = image.nj(),np = image.nplanes();
  std::ptrdiff_t istep=image.istep(),jstep=image.jstep(),pstep = image.planestep();
  vxl_byte* plane = image.top_left_ptr();
  for (unsigned p=0;p<np;++p,plane += pstep)
  {
    vxl_byte* row = plane;
    for (unsigned j=0;j<nj;++j,row += jstep)
    {
      vxl_byte* pixel = row;
      for (unsigned i=0;i<ni;++i,pixel+=istep) *pixel = lup[*pixel];
    }
  }
}

//: Apply histogram equalisation to given float image
void vil_histogram_equalise(vil_image_view<float>& image)
{
  unsigned n_bins = 4000;
  std::vector<double> histo(n_bins);

  // Find smallest and largest values in image
  float min_v, max_v;
  vil_math_value_range(image, min_v, max_v);

  // Generate histogram
  vil_histogram(image, histo, min_v, max_v, n_bins);

  // Create cumulative frequency curve
  double sum = 0.0;
  for (unsigned i=0; i<n_bins; ++i) { sum+=histo[i]; histo[i]=sum; }

  // Get mapping parameters and generate lookup tables
  int lo = 0;
  while (histo[lo]==0) lo++;
  double x0 = histo[lo];
  // To map bins to 256 output value range (smallest values gets mapped to zero)
  double s_b2o =255.1/(sum-x0);
  std::vector<unsigned> lookup_b2o(n_bins);
  unsigned* lup_b2o = &lookup_b2o[0];
  for (unsigned i=0;i<n_bins;++i) { lup_b2o[i]=unsigned(s_b2o*(histo[i]-x0)); }
  // To map input values to bins
  double s_i2b = double(n_bins-1)/(double(max_v-min_v));
  std::vector<unsigned> lookup_i2b(max_v-min_v+1);
  unsigned* lup_i2b = &lookup_i2b[0];
  for (unsigned i=0;i<=(max_v-min_v);++i) { lup_i2b[i]=unsigned(s_i2b*i); }

  // Update image values
  unsigned ni = image.ni(), nj = image.nj(), np = image.nplanes();
  std::ptrdiff_t istep=image.istep(), jstep=image.jstep(), pstep = image.planestep();
  float* plane = image.top_left_ptr();
  for (unsigned p=0; p<np; ++p, plane+=pstep)
  {
    float* row = plane;
    for (unsigned j=0;j<nj;++j,row += jstep)
    {
      float* pixel = row;
      for (unsigned i=0; i<ni; ++i,pixel+=istep)
        *pixel = lup_b2o[lup_i2b[(unsigned)(*pixel-min_v)]];
    }
  }
}
