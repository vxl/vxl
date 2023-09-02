// This is brl/bseg/bsgm/bsgm_disparity_estimator.cxx

//#include <iomanip>
//#include <algorithm>
#include <cmath>
#include <tuple>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>
//#include "vil/vil_save.h"
//#include "vil/vil_convert.h"
//#include <vil/algo/vil_structuring_element.h>
//#include <vil/algo/vil_sobel_3x3.h>
//#include <vil/algo/vil_median.h>
//#include <vil/algo/vil_binary_erode.h>
//#include <vil/algo/vil_gauss_reduce.h>

#include "bsgm_error_checking.h"


//-----------------------------------------------------------------------------
// implement non static memory for digital line generation
bool bsgm_generate_line(bool& init, float xs, float ys,
                        float xe, float ye,
                        float& x, float& y,
                        // internal memory 
                        float& dx, float& dy, float& mag,
                        float& xinc, float& yinc,
                        int& x1, int& y1)
{
  //static float dx, dy, mag, xinc, yinc;
  //static int x1, y1;
  if (init)
  {
    dx = xe-xs;
    dy = ye-ys;
    mag = (float)std::sqrt(dx*dx + dy*dy);
    if (mag<1)//Can't reach the next pixel under any circumstances
      return false;
    xinc = dx/mag;
    yinc = dy/mag;
    x1 = int(xe);
    y1 = int(ye);
    x = xs;
    y = ys;
    init = false;
//  done = false;
    return true;
  }

  //Previous pixel location
  int xp = int(x);
  int yp = int(y);
  //Increment along the line until the motion is greater than one pixel
  for (int i = 0; i<25; i++)
  {
    x += (float)0.5*xinc;
    y += (float)0.5*yinc;
    //Check to see if we have finished the span
    int xc = int(x), yc = int(y);
    bool passed_xe = ((xinc>=0)&&(xc>x1))||((xinc<0)&&(xc<x1));
    bool passed_ye = ((yinc>=0)&&(yc>y1))||((yinc<0)&&(yc<y1));
    if (passed_xe||passed_ye)
      return false;
    //Check if we have advanced by at least 1 pixel
    if (std::fabs(static_cast<float>(xc-xp))>=1||std::fabs(static_cast<float>(yc-yp))>=1)
      return true;
  }
  std::cout << "In bsgm_generate_line: - shouldn't happen - "
           << "xs="<<xs<<" ys="<<ys<<" xe="<<xe<<" ye="<< ye << std::endl;
  return false;
}


//-----------------------------------------------------------------
void bsgm_check_leftright(
  const vil_image_view<float>& disp1,
  const vil_image_view<float>& disp2,
  const vil_image_view<bool>& invalid1,
  const vil_image_view<bool>& invalid2,
  vil_image_view<bool>& error1)
{
  float dist_thresh = 1.0f;

  // Initialize error map
  error1.set_size(disp1.ni(), disp1.nj());
  error1.fill(true);

  // Iterate through first image
  for (int y = 0; y < static_cast<int>(disp1.nj()); y++) {
    for (int x = 0; x < static_cast<int>(disp1.ni()); x++) {
      if (invalid1(x, y)) continue;

      // Project each pixel into the second image
      int x2 = x + static_cast<int>(std::round(disp1(x, y) ));
      if (x2 < 0 || x2 >= static_cast<int>(disp2.ni())) continue;
      if (invalid2(x2, y)) continue;

      // Project back into first image and compute distance
      float x1 = x2 + disp2(x2, y);
      float dist = x1 - (float)x;
      if (fabs(dist) <= dist_thresh) error1(x, y) = false;
    }
  }
}


//-----------------------------------------------------------------------


//-----------------------------------------------------------------------
void
bsgm_invert_disparities(
  vil_image_view<float>& disp_img,
  int old_invalid,
  int new_invalid )
{
  for( int y = 0; y < static_cast<int>(disp_img.nj()); y++ )
    for( int x = 0; x < static_cast<int>(disp_img.ni()); x++ )
      disp_img(x,y) = disp_img(x,y) == old_invalid ? new_invalid : -disp_img(x,y);
}

static void adaptive_shadow_prob(size_t i, size_t j, vil_image_view<unsigned short> const& rect_image,
                                 vil_image_view<float> const& shadow_step_prob,
                                 vgl_vector_2d<float> sun_dir, vil_image_view<float>& shadow_prob,
                                 float scan_length, float default_shadow_thresh, float shad_stp_thresh){
  int ni = rect_image.ni(), nj = rect_image.nj();
  int min_sha_step_interval = 1;
  float x_start = i, y_start = j;
  vgl_point_2d<float> ps(x_start, y_start);
  vgl_point_2d<float> pe = ps + scan_length*sun_dir;
  float xe = pe.x(), ye = pe.y();
  // scan along the sun vector and determine the shadow intensity profile
  // starting at a shadow step filter probability above shad_stp_thresh
  // profile data
  //                     ss_prob rect  xi   yi
  std::vector<std::tuple<float, float, int, int, bool> > shstp_vals;
  // scan violated outside image bounds
  bool broke_in_shadow = false;
  // line scan
  float x, y;
  bool init = true;
  // internal memory for generate line since static memory not allowed
  // as is the case in the brip_line_generator implementation
  //=======
  float dx, dy, mag, xinc,  yinc;
  int x1,  y1;
  //=======
  while(bsgm_generate_line(init, float(x_start), float(y_start), xe, ye, x, y,
                           // internal memory 
                           dx, dy, mag, xinc, yinc, x1, y1))
    {
      int xi = (int)x, yi = (int)y; //convert the pixel location to integer
      if (xi < 0 || xi >= ni || yi < 0 || yi >= nj) {
        broke_in_shadow = true;
        break;
      }
      float ss_prob = shadow_step_prob(xi, yi);
      float rect = rect_image(xi, yi);
      shstp_vals.emplace_back(ss_prob, rect, xi, yi, false);
    }//end while

  // analyze profile for threshold at the start of the shadow region
  size_t n = shstp_vals.size();
  bool start = false;
  float thresh = default_shadow_thresh;
  bool done = false;
  for(size_t i = 0; i<n&&!done; ++i){
    bool ss = std::get<0>(shstp_vals[i])> shad_stp_thresh;
    int ii = std::get<2>(shstp_vals[i]), jj = std::get<3>(shstp_vals[i]);
    // account for holes in the shadow step region
    // pixel x is active if ss is true
    // or any of sl, s+ , sr
    //          |  scan
    //          v
    //          x
    //       sl s+ sr
    bool ss_plus = false;
    bool ss_left = false;
    bool ss_right = false;
    if(i<(n-1)){
      ss_plus = std::get<0>(shstp_vals[i+1])> shad_stp_thresh;
      // for debug purposes
      int i_plus = std::get<2>(shstp_vals[i+1]);
      int j_plus = std::get<3>(shstp_vals[i+1]);
      if(i_plus < ni-1){
        int i_right = i_plus + 1;
        ss_right = shadow_step_prob(i_right, j_plus)>shad_stp_thresh;
      }
      if(i_plus > 0){
        int i_left = i_plus - 1;
        ss_left = shadow_step_prob(i_left, j_plus) > shad_stp_thresh;
      }
    }
    std::get<4>(shstp_vals[i]) = ss||ss_plus||ss_left||ss_right;
    float rec = std::get<1>(shstp_vals[i]);
    if(ss&&!start){
      start = true;
    }
    if(start&&std::get<4>(shstp_vals[i])){
      thresh = rec;
      if(thresh < default_shadow_thresh)
        thresh = default_shadow_thresh;
      if(i == (n-1)){
        done = true;
        start = false;
      }
      int xi = std::get<2>(shstp_vals[i]), yi = std::get<3>(shstp_vals[i]);
      shadow_prob(xi, yi) = 1.0;
      continue;
    }
    done = true;
    start = false;
  }
  // thresh is set at end of shad step line scan

  // finally scan and classify shadow
  start = false;
  bool sprint = false;
  done = false;
  for(size_t i = 0; i<n&&!done; ++i){
    int xi = std::get<2>(shstp_vals[i]), yi = std::get<3>(shstp_vals[i]);
    float rect = std::get<1>(shstp_vals[i]);
    bool shadow = rect<thresh;
    bool ss = std::get<4>(shstp_vals[i]);
    if(!start&&(shadow||ss)){
      start = true;
    }
    if(start && (ss || shadow)){
      shadow_prob(xi, yi) = 1.0;
      if(i == (n-1)){
        done = true;
        start = false;
      }
    }
  }
}
void bsgm_shadow_prob(vil_image_view<unsigned short> const& rect_img,
                 vgl_vector_2d<float> const& sun_dir,
                 float default_shadow_thresh, 
                 vil_image_view<float> const& shadow_step_prob,
                 vil_image_view<float>& shadow_prob,
                 float scan_length, float ss_thresh){
  size_t ni = rect_img.ni(), nj = rect_img.nj();
  shadow_prob.set_size(ni, nj);
  shadow_prob.fill(0.0f);
  for (size_t j = 0; j < nj; ++j)
    for (size_t i = 0; i < ni; ++i) 
      if(float(shadow_step_prob(i, j)) >= ss_thresh)
        adaptive_shadow_prob(i, j, rect_img, shadow_step_prob,
                             sun_dir, shadow_prob,scan_length,
                             default_shadow_thresh, ss_thresh);
}
