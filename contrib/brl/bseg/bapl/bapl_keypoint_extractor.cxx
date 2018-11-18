// This is brl/bseg/bapl/bapl_keypoint_extractor.cxx
#include <iostream>
#include <cmath>
#include <cstdlib>
#include "bapl_keypoint_extractor.h"
//:
// \file

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <vil/vil_image_resource.h>
#include <bapl/bapl_lowe_keypoint.h>
#include <bapl/bapl_lowe_pyramid_set_sptr.h>
#include <bapl/bapl_lowe_pyramid_set.h>

#include <vgl/vgl_point_3d.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_math.h>
#include <vnl/algo/vnl_svd.h>


//: Extract the lowe keypoints from an image
bool bapl_keypoint_extractor( const vil_image_resource_sptr & image,
                              std::vector<bapl_keypoint_sptr> & keypoints,
                              float curve_ratio, bool verbose)
{
  // Create the group of image pyramids
  bapl_lowe_pyramid_set_sptr pyramid_set = new bapl_lowe_pyramid_set(image,3,6,verbose);

  if(verbose){
    std::cout << "Detecting Peaks" << std::endl;
  }

  // detect the peaks
  std::vector<vgl_point_3d<float> > peak_pts;
  bapl_dog_peaks(peak_pts, pyramid_set, curve_ratio);

  if(verbose){
    std::cout << "Peak count: " << peak_pts.size() <<std::endl;
  }

  bapl_lowe_orientation orientor(3.0, 36);
  for (auto & peak_pt : peak_pts)
  {
    float key_x = peak_pt.x();
    float key_y = peak_pt.y();
    float actual_scale;
    const vil_image_view<float> & orient_img = pyramid_set->grad_orient_at( peak_pt.z(), &actual_scale);
    const vil_image_view<float> & mag_img =  pyramid_set->grad_mag_at( peak_pt.z() );
    key_x /= actual_scale;  key_y /= actual_scale;

    // Add a keypoint for each possible orientation
    std::vector<float> orientations;
    orientor.orient_at(key_x, key_y, peak_pt.z(), orient_img, mag_img, orientations);
    for (float & orientation : orientations) {
      bapl_lowe_keypoint_sptr kp = bapl_lowe_keypoint_new( pyramid_set, peak_pt.x(), peak_pt.y(),
                                                           peak_pt.z(), orientation );
      keypoints.push_back( kp );
    }
  }
  // set the ids of the keypoints as their rank in the output vector
  for (unsigned i = 0; i < keypoints.size(); i++) {
    keypoints[i]->set_id(i);
  }

  if(verbose){
    std::cout<<"Found "<<keypoints.size()<<" keypoints."<<std::endl;
  }

  return true;
}


// compute (r+1)^2/r where r is the ratio of principal curvatures
inline float
curvature_ratio(const float *center, std::ptrdiff_t i_step, std::ptrdiff_t j_step)
{
  float n_m1_m1 = center[-i_step-j_step];
  float n_m1_0  = center[-i_step];
  float n_m1_1  = center[-i_step+j_step];
  float n_0_m1  = center[-j_step];
  float n_0_0   = center[0];
  float n_0_1   = center[j_step];
  float n_1_m1  = center[i_step-j_step];
  float n_1_0   = center[i_step];
  float n_1_1   = center[i_step+j_step];

  //Compute the 2nd order quadratic coefficients;
  //      1/3 * [ +1 -2 +1 ]
  // Dxx =      [ +1 -2 +1 ]
  //            [ +1 -2 +1 ]
  float Dxx = (   ( n_m1_m1+n_0_m1+n_1_m1
                 +n_m1_1 +n_0_1 +n_1_1 )
             -2.0f*(n_m1_0 +n_0_0 +n_1_0) )/3.0f;
  //      1/4 * [ +1  0 -1 ]
  // Dxy =      [  0  0  0 ]
  //            [ -1  0 +1 ]
  float Dxy = (n_m1_m1-n_m1_1-n_1_m1+n_1_1)/4.0f;
  //      1/3 * [ +1 +1 +1 ]
  // Dyy =      [ -2 -2 -2 ]
  //            [ +1 +1 +1 ]
  float Dyy = (   ( n_m1_m1+n_m1_0+n_m1_1
                 +n_1_m1 +n_1_0 +n_1_1 )
             -2.0f*(n_0_m1 +n_0_0 +n_0_1) )/3.0f;

  float TrH = Dxx + Dyy;
  float DetH = Dxx*Dyy - Dxy*Dxy;
  return TrH*TrH/DetH;
}


// refine the position based on a 3x3x3 image of neighbors
float
bapl_refine_peak( const vil_image_view<float> & neighbors, vnl_double_3& delta)
{
  vnl_double_3 D;

  // Dx
  D(0) = (neighbors(2,1,1) - neighbors(0,1,1))/2.0;

  // Dy
  D(1) = (neighbors(1,2,1) - neighbors(1,0,1))/2.0;

  // Dz
  D(2) = (neighbors(1,1,2) - neighbors(1,1,0))/2.0;

  // The Hessian
  vnl_double_3x3 H;

  // Dxx
  H(0,0) = (neighbors(2,1,1) - 2.0*neighbors(1,1,1) + neighbors(0,1,1));

  // Dyy
  H(1,1) = (neighbors(1,2,1) - 2.0*neighbors(1,1,1) + neighbors(1,0,1));;

  // Dzz
  H(2,2) = (neighbors(1,1,2) - 2.0*neighbors(1,1,1) + neighbors(1,1,0));;

  // Dxy
  H(0,1) = H(1,0) = (neighbors(2,2,1) + neighbors(0,0,1)
                    -neighbors(0,2,1) - neighbors(2,0,1))/4.0;

  // Dxz
  H(0,2) = H(2,0) =  (neighbors(2,1,2) + neighbors(0,1,0)
                     -neighbors(0,1,2) - neighbors(2,1,0))/4.0;

  // Dyz
  H(1,2) = H(2,1) =  (neighbors(1,2,2) + neighbors(1,0,0)
                     -neighbors(1,0,2) - neighbors(1,2,0))/4.0;

  delta = -vnl_svd<double>(H).inverse()*D;

  return neighbors(1,1,1) + (float)dot_product(D,delta)/2.0f;
}


//: DoG Peak finding helper function
inline bool
bapl_is_max_3x3(const float* im, std::ptrdiff_t i_step, std::ptrdiff_t j_step)
{
   if (*im <= im[i_step]) return false;
   if (*im <= im[-i_step]) return false;
   if (*im <= im[j_step]) return false;
   if (*im <= im[-j_step]) return false;
   if (*im <= im[i_step+j_step]) return false;
   if (*im <= im[i_step-j_step]) return false;
   if (*im <= im[j_step-i_step]) return false;
   if (*im <= im[-i_step-j_step]) return false;
   return true;
}


//: DoG Peak finding helper function
inline bool
bapl_is_min_3x3(const float* im, std::ptrdiff_t i_step, std::ptrdiff_t j_step)
{
   if (*im >= im[i_step]) return false;
   if (*im >= im[-i_step]) return false;
   if (*im >= im[j_step]) return false;
   if (*im >= im[-j_step]) return false;
   if (*im >= im[i_step+j_step]) return false;
   if (*im >= im[i_step-j_step]) return false;
   if (*im >= im[j_step-i_step]) return false;
   if (*im >= im[-i_step-j_step]) return false;
   return true;
}


//: DoG Peak finding helper function
inline bool
bapl_is_more_3x3(const float value, const float* im, std::ptrdiff_t i_step, std::ptrdiff_t j_step)
{
   if (value <= im[0]) return false;
   if (value <= im[i_step]) return false;
   if (value <= im[-i_step]) return false;
   if (value <= im[j_step]) return false;
   if (value <= im[-j_step]) return false;
   if (value <= im[i_step+j_step]) return false;
   if (value <= im[i_step-j_step]) return false;
   if (value <= im[j_step-i_step]) return false;
   if (value <= im[-i_step-j_step]) return false;
   return true;
}


//: DoG Peak finding helper function
inline bool
bapl_is_less_3x3(const float value, const float* im, std::ptrdiff_t i_step, std::ptrdiff_t j_step)
{
   if (value >= im[0]) return false;
   if (value >= im[i_step]) return false;
   if (value >= im[-i_step]) return false;
   if (value >= im[j_step]) return false;
   if (value >= im[-j_step]) return false;
   if (value >= im[i_step+j_step]) return false;
   if (value >= im[i_step-j_step]) return false;

   if (value >= im[j_step-i_step]) return false;
   if (value >= im[-i_step-j_step]) return false;
   return true;
}


//: Find the peaks in the DoG pyramid
void bapl_dog_peaks( std::vector<vgl_point_3d<float> >& peak_pts,
                     const bapl_lowe_pyramid_set_sptr& pyramid_set,
                     float curve_ratio )
{
  int num_oct = pyramid_set->num_octaves();
  int oct_size = pyramid_set->octave_size();
  float max_curve = (curve_ratio+1.0f)*(curve_ratio+1.0f)/curve_ratio;
  float min_curve = (-curve_ratio+1.0f)*(-curve_ratio+1.0f)/-curve_ratio;

  for (int index=1; index<(num_oct*oct_size)-1; ++index) {
    int a_scale = ((index+1)/oct_size == index/oct_size)?1:2;
    int b_scale = ((index-1)/oct_size == index/oct_size)?1:2;
    const vil_image_view<float> & above = pyramid_set->dog_pyramid((index+1)/oct_size, (index+1)%oct_size);
    const vil_image_view<float> & image = pyramid_set->dog_pyramid(index/oct_size, index%oct_size);
    const vil_image_view<float> & below = pyramid_set->dog_pyramid((index-1)/oct_size, (index-1)%oct_size);


    float ps = std::pow(2.0f,float((index/oct_size)-1));  // TODO: CHECK OUT CASTING

    unsigned ni = image.ni(), nj = image.nj();
    std::ptrdiff_t istep=image.istep(), jstep=image.jstep();
    const float* row = image.top_left_ptr() + 2*istep + 2*jstep;
    for (unsigned j=2;j<(nj/2-1)*2;++j,row += jstep)
    {
      const float* pixel = row;
      for (unsigned i=2;i<(ni/2-1)*2;++i,pixel+=istep)
      {
        int sign = 0;
        // check for maxima
        if ( bapl_is_max_3x3(pixel, istep, jstep) && //*pixel > 2.0f &&
             bapl_is_more_3x3(*pixel, &above(i/a_scale,j/a_scale), above.istep(), above.jstep()) &&
             bapl_is_more_3x3(*pixel, &below(i*b_scale,j*b_scale), below.istep(), below.jstep()) ) {
          sign = 1;
        }
        // check for minima
        else if ( bapl_is_min_3x3(pixel, istep, jstep) &&
                  bapl_is_less_3x3(*pixel, &above(i/a_scale,j/a_scale), above.istep(), above.jstep()) &&
                  bapl_is_less_3x3(*pixel, &below(i*b_scale,j*b_scale), below.istep(), below.jstep()) ) {
          sign = -1;
        }

        if ( sign == 0 ) continue; // this pixel is not a peak

        // refined indices
        int ri = i, rj = j, rindex = index;
        vnl_double_3 offset;
        int loc_scale = 1 << (rindex/oct_size);
        vil_image_view<float> neighbors = pyramid_set->dog_neighbors(rindex,ri*loc_scale,rj*loc_scale);
        float peak_val = bapl_refine_peak( neighbors, offset );

        // offset is more than one pixel away, reestimate
        bool peak_valid = true;
        if (std::fabs(offset(0)) >= 0.5 || std::fabs(offset(1)) >= 0.5 || std::fabs(offset(2)) >= 0.5) {
          ri = int(double(ri)+offset(0)+0.5);
          rj = int(double(rj)+offset(1)+0.5);
          rindex = int(double(rindex)+offset(2)+0.5);
          // verify that the new pixel is within bounds
          if ( rindex>=1 && rindex<(num_oct*oct_size)-1 ) {
            const vil_image_view<float> & rimage = pyramid_set->dog_pyramid(rindex/oct_size, rindex%oct_size);
            if (ri>=2 && ri<((int)rimage.ni()/2-1)*2 && rj>=2 && rj<((int)rimage.nj()/2-1)*2) {
              loc_scale = 1 << (rindex/oct_size);
              neighbors = pyramid_set->dog_neighbors(rindex,ri*loc_scale,rj*loc_scale);
              peak_val = bapl_refine_peak( neighbors, offset );
            }
            else
              peak_valid = false;
          }
          else
            peak_valid = false;

          peak_valid = peak_valid && std::fabs(offset(0)) < 0.5 &&
                                     std::fabs(offset(1)) < 0.5 &&
                                     std::fabs(offset(2)) < 0.5 ;
        }

        if ( !peak_valid ) continue;

        // ignore low contrast peaks
        if ( sign*peak_val < 0.015 /* was 0.03 */) continue;

        // ignore peaks with high principle curvature ratio
        const vil_image_view<float> & rimage = pyramid_set->dog_pyramid(rindex/oct_size, index%oct_size);
        float curv_rat = curvature_ratio(&rimage(ri,rj), rimage.istep(), rimage.jstep());
        if ( curv_rat > max_curve || curv_rat < min_curve ) continue;

        peak_pts.emplace_back((float)(ri+offset(0))*ps, (float)(rj+offset(1))*ps,
                                                (float)std::pow(2.0,((rindex+offset(2))/oct_size)-1) );
      }
    }
  }
}


//: Constructor
bapl_lowe_orientation::bapl_lowe_orientation(float sigma, unsigned num_bins)
 : sigma_(sigma), num_bins_(num_bins), bin_scale_((2*num_bins-1)/(6.28319f))
{
}


inline float gaussian( float x, float y, float sigma)
{
  return std::exp(-((x*x)+(y*y))/(2.0f*sigma*sigma));
}

//: Compute the orientation at (x,y) using the gradient orientation and magnitude images
void
bapl_lowe_orientation::orient_at( float x, float y, float scale,
                                  const vil_image_view<float> & grad_orient,
                                  const vil_image_view<float> & grad_mag,
                                  std::vector<float> & orientations )
{
  std::vector<float> histogram(num_bins_, 0.0);
  float log_scale = std::log(scale)/std::log(2.0f);
  float rel_scale = std::pow(2.0f, log_scale - std::floor(log_scale));
  float sigma = 3.0f * rel_scale;

  int size = int(3.0*sigma)+1;
  int x_int = int(x+0.5);
  int y_int = int(y+0.5);

  for (int i=-size; i<=size; ++i) {
    for (int j=-size; j<=size; ++j) {
      if (i+x_int>=0 && i+x_int<int(grad_orient.ni()) &&
          j+y_int>=0 && j+y_int<int(grad_orient.nj()) ) {
        float x_dist = i-(x-x_int);
        float y_dist = j-(y-y_int);
        if ( x_dist*x_dist + y_dist*y_dist <= 9.0*sigma*sigma ) {
          float weight = grad_mag(i+x_int,j+y_int)*gaussian(x_dist, y_dist, sigma);
          int bin = ((int((grad_orient(i+x_int,j+y_int)+vnl_math::pi)*bin_scale_)+1)/2)%num_bins_;
          histogram[bin] += weight;
        }
      }
    }
  }
  float max = 0.0;
  std::vector<int> peaks;
  // find the maximum peak
  for (unsigned int i=0; i<num_bins_; ++i) {
    if ( histogram[i] > histogram[(i-1)%num_bins_] &&
         histogram[i] > histogram[(i+1)%num_bins_] ) {
      if ( histogram[i] > max ) max = histogram[i];
      peaks.push_back(i);
    }
  }
  assert(!peaks.empty());
  // find all peaks within 80% of the max peak
  orientations.clear();
  max *= 0.8f;
  for (int peak : peaks) {
    if (histogram[ peak ] > max) {
      //check for zivide by zero condition
      if (num_bins_ == 0) {
        std::cerr << "ERROR: Division by 0" << std::endl;
        throw 0;
      }
      //parabolic interpolation
      float ypos = histogram[ (peak+1)%num_bins_ ];
      float yneg = histogram[ (peak-1)%num_bins_ ];
      float dy   = (ypos - yneg)/2.0f;
      float d2y  = 2.0f*histogram[ peak ] - ypos - yneg;
      float dx = 6.28319f/num_bins_;
      float angle = (float(peak)+dy/d2y)*dx;
      orientations.push_back(angle);
    }
  }
}
