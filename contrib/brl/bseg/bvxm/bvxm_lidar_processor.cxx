#include "bvxm_lidar_processor.h"
//:
// \file
#include "grid/bvxm_voxel_slab.h"
#include "grid/bvxm_voxel_slab_iterator.h"
#ifdef OPTION2 // currently FALSE
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#endif

#include <vgl/vgl_box_2d.h>
#include <vil/vil_image_view.h>

//: Return probability density of observing pixel values
bvxm_voxel_slab<float>
bvxm_lidar_processor::prob_density(float z_dim,  bvxm_voxel_slab<float> const& obs, float  /*voxel_width*/ )
{
  //the output
  bvxm_voxel_slab<float> probabilities(obs.nx(), obs.ny(), obs.nz());

  // Option 1: No LIDAR registration error.
  bvxm_voxel_slab<float>::const_iterator obs_it;
  bvxm_voxel_slab<float>::iterator prob_it = probabilities.begin();
  for (obs_it = obs.begin(); obs_it!= obs.end(); ++obs_it, ++prob_it)
  {
    float d;

    d = *obs_it - z_dim;

    // Calculate threshold such that the given number of pixels corresponds to
    // 2.5 standard deviations away in a standard normal gaussian
    float thresh = 2.5f / float(vox_thresh_);
    d *= thresh;

    *prob_it = gauss_.prob_density(d);
  }

#ifdef OPTION2 // currently FALSE
  // Option 2: As in thesis.
  assert( obs.nz() == 1 ); // Code assumes this.
  int search_rad = 4;
  for ( unsigned i = 0; i < obs.nx(); i++ ) {
    for ( unsigned j = 0; j < obs.ny(); j++ ) {
      int min_i = i-search_rad; if ( min_i < 0 ) min_i = 0;
      int min_j = j-search_rad; if ( min_j < 0 ) min_j = 0;
      int max_i = i+search_rad; if ( max_i >= (int)obs.nx() ) max_i = obs.nx()-1;
      int max_j = j+search_rad; if ( max_j >= (int)obs.ny() ) max_j = obs.ny()-1;
      float min_d;
      for ( int ni = min_i; ni <= max_i; ni++ ) {
        for ( int nj = min_j; nj <= max_j; nj++ ) {
          float d = obs(ni,nj,0)-z_dim;
          d = std::sqrt( d*d + (ni-min_i)*(ni-min_i)*voxel_width*voxel_width + (nj-min_j)*(nj-min_j)*voxel_width*voxel_width );
          if ( ni == min_i || nj == min_j ) min_d = d;
          if ( d < min_d ) min_d = d;
        }
      }

      float thresh = 2.5f / float(vox_thresh_);
      min_d *= thresh;
      probabilities(i,j,0) = gauss_.prob_density(min_d);
    }
  }
#endif // OPTION2
  return probabilities;
}

float bvxm_lidar_processor::prob_density(const vil_image_view_base_sptr& lidar,
                                         float z_dim,
                                         vnl_vector_fixed<float,3>& covar,  // sigma vals
                                         vgl_box_2d<double> lidar_roi,
                                         float voxel_width)
{
  vnl_vector_fixed<float,3> min_vec(float(lidar_roi.min_x()),
                                    float(lidar_roi.min_y()),
                                    z_dim-0.5f*voxel_width);//half of voxel size!!!
  vnl_vector_fixed<float,3> max_vec(float(lidar_roi.max_x()),
                                    float(lidar_roi.max_y()),
                                    z_dim+0.5f*voxel_width);

  int min_i = (int)std::floor(lidar_roi.min_x()); if ( min_i < 0 ) min_i = 0;
  int min_j = (int)std::floor(lidar_roi.min_y()); if ( min_j < 0 ) min_j = 0;
  int max_i = (int) std::ceil(lidar_roi.max_x()); if ( max_i >= (int)lidar->ni() ) max_i = lidar->ni()-1;
  int max_j = (int) std::ceil(lidar_roi.max_y()); if ( max_j >= (int)lidar->nj() ) max_j = lidar->nj()-1;

  float p = 1.0f;
  for ( int ni = min_i; ni < max_i; ni++ ) {
    for ( int nj = min_j; nj < max_j; nj++ ) {
      float d = -255.f; // dummy initialisation, to avoid compiler warning
      if (lidar->pixel_format() == VIL_PIXEL_FORMAT_BYTE) {
        if (auto *img_view = dynamic_cast<vil_image_view<unsigned char>*>(lidar.ptr()))
          d = (*img_view)(ni, nj);
      }
      else if (lidar->pixel_format() == VIL_PIXEL_FORMAT_FLOAT) {
        if (auto *img_view = dynamic_cast<vil_image_view<float>*>(lidar.ptr()))
          d = (*img_view)(ni, nj);
      }
      vnl_vector_fixed<float,3> m((float)ni+0.5f, (float)nj+0.5f, d-2.f);
      bsta_gauss_if3 gauss(m, covar);
      float p1 = gauss.probability(min_vec,max_vec);
      p *= 1.0f - p1;
   }
  }
  return 1.0f-p;
}
