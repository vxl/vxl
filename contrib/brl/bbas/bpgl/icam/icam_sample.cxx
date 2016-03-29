#include "icam_sample.h"
//:
// \file
#include <vil/vil_bilin_interp.h>

void icam_sample::sample( unsigned int ni_dest,  unsigned int nj_dest,
                          vil_image_view<float> const& source,
                          icam_depth_transform const& dt,
                          vnl_vector<double>& samples,
                          vnl_vector<double>& mask,
                          unsigned& n_samples)
{
  unsigned ni_source = source.ni(), nj_source = source.nj();
  unsigned max_samples = (ni_dest-2)*(nj_dest-2);
  samples.set_size(max_samples);
  mask.set_size(max_samples);
  samples.fill(0.0);
  mask.fill(0.0);
  double to_u, to_v;
  unsigned index = 0;
  n_samples = 0;

  unsigned dest_li = ni_dest-1, dest_lj = nj_dest-1;
  unsigned src_li = ni_source-1, src_lj = nj_source-1;
  unsigned src_istep = source.istep(), src_jstep = source.jstep();
  const float* src_ptr = &source(0,0);
  // scan the set of destination pixel locations (i, j) and determine
  // their location in the source image (to_u, to_v). The mapping is
  // provided by the depth transform, dt.
  for (unsigned j = 1; j<dest_lj; j++)
    for (unsigned i = 1; i<dest_li; i++)
    {
      if (!dt.transform(i,j,to_u, to_v))
        continue;

      //check to see if the source is being accessed out of bounds
      //then non bounds-checking bilinear interpolation can be called
      if (to_u<0||to_v<0||to_u>=src_li||to_v>=src_lj) {
        mask[index++] = 0.0f;
        continue;
      }

      //assume grey scale, plane = 0
      //note that there is no bounds checking in this version
      double v = vil_bilin_interp(to_u, to_v, src_ptr,
                                  ni_source, nj_source,
                                  src_istep, src_jstep);

      mask[index] = 1.0f;
      samples[index++] = v;
      n_samples++;
    }
}

void icam_sample::resample(unsigned int ni_dest,  unsigned int nj_dest,
                           vil_image_view<float> const& source,
                           icam_depth_transform const& dt,
                           vil_image_view<float>& dest,
                           vil_image_view<float>& mask,
                           unsigned& n_samples)
{
  unsigned ni_source = source.ni(), nj_source = source.nj();
  dest.set_size(ni_dest, nj_dest);
  mask.set_size(ni_dest, nj_dest);
  dest.fill(0.0f);
  mask.fill(1.0f);
  // scan the set of destination pixel locations (i, j) and determine
  // their location in the source image (to_u, to_v). The mapping is
  // provided by the depth transform, dt.
  n_samples = 0;
  double to_u, to_v;
  for (unsigned j = 1; j<nj_dest-1; ++j)
    for (unsigned i = 1; i<ni_dest-1; ++i) {
      if (!dt.transform(i,j,to_u, to_v)) {
        mask(i,j) = 0.0f;
        continue;
      }
      if (to_u<0||to_v<0||to_u>=ni_source||to_v>=nj_source) {
        mask(i,j) = 0.0f;
        continue;
      }
      //assume grey scale, plane = 0
      double v = vil_bilin_interp_safe(source, to_u, to_v);
#if 0
      int iu = std::floor(to_u), iv = std::floor(to_v);
      double v = source(iu, iv);
#endif
      dest(i,j) = static_cast<float>(v);

      n_samples++;
    }
  //take care of borders
  for (unsigned j = 0; j<nj_dest; ++j) {
    mask(0,j) = 0.0f;
    dest(0,j) = 0.0f;
    mask(ni_dest-1,j) = 0.0f;
    dest(ni_dest-1,j) = 0.0f;
  }
  for (unsigned i = 0; i<ni_dest; ++i) {
    mask(i,0) = 0.0f;
    dest(i,0) = 0.0f;
    mask(i,nj_dest-1) = 0.0f;
    dest(i,nj_dest-1) = 0.0f;
  }
}


void icam_sample::sample(vil_image_view<float> const& map_dest,
                         vil_image_view<float> const& map_mask,
                         vnl_vector<double>& samples,
                         vnl_vector<double>& mask,
                         unsigned& n_samples)
{
  unsigned ni_dest = map_dest.ni(), nj_dest = map_dest.nj();
  unsigned max_samples = (ni_dest-2)*(nj_dest-2);
  samples.set_size(max_samples);
  mask.set_size(max_samples);
  samples.fill(0.0);
  mask.fill(0.0);
  unsigned index = 0;
  n_samples = 0;
  for (unsigned j = 1; j<nj_dest-1; ++j)
    for (unsigned i = 1; i<ni_dest-1; ++i) {
      if (map_mask(i,j) == 0.0f) {
        mask[index++]=0.0f;
        continue;
      }
      mask[index]=1.0f;
      samples[index++] = map_dest(i,j);
      n_samples++;
    }
}
