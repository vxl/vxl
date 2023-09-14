// This is brl/bseg/bsgm/bsgm_error_checking.hxx
#ifndef bsgm_error_checking_hxx_
#define bsgm_error_checking_hxx_

//#include <string>
//#include <vector>
//#include <set>
//#include <iostream>
//#include <sstream>
//#include <utility>
#include <algorithm>
#include <map>
#include <tuple>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_save.h>
#include <vil/algo/vil_gauss_filter.h>
#include <vil/algo/vil_sobel_3x3.h>
#include <vil/algo/vil_blob.h>
#include "bsgm_error_checking.h"
#include <brip/brip_line_generator.h>
#include <bsta/bsta_histogram.h>

static bool pair_greater(std::pair<int, int> const& a, std::pair<int, int> const& b){
  return a.second > b.second;
}


template <class T>
void bsgm_check_shadows(
  vil_image_view<float>& disp_img,
  const vil_image_view<T>& img,
  float invalid_disparity,
  unsigned short shadow_thresh,
  const vgl_box_2d<int>& img_window)
{
  // image window
  int img_start_x, img_start_y;
  if (img_window.is_empty()) {
    img_start_x = 0;
    img_start_y = 0;
  }
  else {
    img_start_x = img_window.min_x();
    img_start_y = img_window.min_y();
  }

  // check shadow threshold
  int w = disp_img.ni(), h = disp_img.nj();
  for (int y = 0, img_y = img_start_y; y < h; y++, img_y++) {
    for (int x = 0, img_x = img_start_x; x < w; x++, img_x++) {
      if (std::isnan(invalid_disparity) && std::isnan(disp_img(x, y))) {
        continue;
      } else if (disp_img(x, y) == invalid_disparity) {
        continue;
      } else if (img(img_x, img_y) < shadow_thresh) {
        disp_img(x, y) = invalid_disparity;
      }
    }
  }
}


// find a pixel offset to reconcile the response distribution at the maximum value position
static float max_pix_offset(float fm, float f0, float fp){
  float ratio_minus = fm / f0, ratio_plus = fp / f0;
  if ((ratio_minus > ratio_plus) && ratio_minus > 0.75)
    return -1;
  if ((ratio_plus > ratio_minus) && ratio_plus > 0.75)
    return +1;
  return 0;
}


/*
                             *
                          *  |
                       *     |
       radius       *    +   |
    ------------ * ------------ k  the filter kernel is a 1st derivative operator
    |  -      *      radius
    |      *
    |   *
    |*

  The filter is applied in the opposite sense to the sun dir unit vector
*/
static std::vector<std::tuple<int, int, int> > step_filter(int radius, vgl_vector_2d<float> const& sun_dir, int& sum_coef){
  vgl_point_2d<float> p0(0.0f, 0.0f), pp, pm;
  float margin = 3.0f;
  float mag = margin*radius;
  // generate a pixel quantized vector opposite to sun_dir
  pm = p0 + mag * sun_dir;
  pp = p0 - mag * sun_dir;
  float xs = pm.x(), ys = pm.y(); // start point
  float xe = pp.x(), ye = pp.y(); // end point
  bool init = true;
  int start = -int(mag);
  float x, y;
  std::vector<std::tuple<int, int, int> > ret;
  sum_coef = 0;
  while (brip_line_generator::generate(init, xs, ys, xe, ye, x, y))
  {
    if (start>=(-radius) && start<=(radius)){
      ret.push_back(std::tuple<int, int, int>(int(x), int(y), start));
      sum_coef+=abs(start);
    }
    start++;
  }
  return ret;
}
static std::vector<std::vector<std::tuple<int, int> > > step_mask(int radius, vgl_vector_2d<float> const& sun_dir){
  std::vector<std::vector<std::tuple<int, int> > > ret;
  //vector orthogonal to the sun direction
  vgl_vector_2d<float> orth(-sun_dir.y(), sun_dir.x());
  vgl_point_2d<float> p0(0.0f, 0.0f), pp, pm;
  float margin = 3.0f;
  float mag = margin*radius;
  // generate a pixel quantized vector opposite to sun_dir
  pm = p0 + mag * sun_dir;
  pp = p0 - mag * sun_dir;
  float xs = pm.x(), ys = pm.y(); // start point
  float xe = pp.x(), ye = pp.y(); // end point
  for(float del = -1.0f; del<=1.0f; del+=1.0f){
    std::vector<std::tuple<int, int> > scan;
    bool init = true;
    int start = -int(mag);
    float x, y;
    float dx = del*orth.x(), dy = del*orth.y();
    while (brip_line_generator::generate(init, xs+dx, ys+dy, xe+dx, ye+dy, x, y))
      {
        if (start>=(-radius) && start<=(radius)){
          scan.push_back(std::tuple<int, int>(int(x), int(y)));
        }
        start++;
      }
    ret.push_back(scan);
  }
  return ret;
}


template <class T>
void
bsgm_shadow_step_filter(const vil_image_view<T> & img,
                        const vil_image_view<bool> & invalid,
                        vil_image_view<float> & step_prob_img,
                        const vgl_vector_2d<float> & sun_dir,
                        int radius,
                        int response_low,
                        int shadow_high)
{
  int sum_coef;
  std::vector<std::tuple<int, int, int>> deriv_pix_offset = step_filter(radius, sun_dir, sum_coef);
  int ni = img.ni(), nj = img.nj(), ns = deriv_pix_offset.size();
  vil_image_view<float> resp_img(ni, nj);
  resp_img.fill(0.0f);
  vil_image_view<T> min_img(ni, nj);
  min_img.fill(0);
  resp_img.fill(0);
  vil_image_view<float> debug_resp_img(ni, nj);
  debug_resp_img.fill(0.0f);
  vil_image_view<float> center(ni, nj);
  center.fill(0.0f);
  step_prob_img.set_size(ni, nj);
  step_prob_img.fill(0.0f);
  // border
  int itstart = int(radius) + 1;
  for (int j = itstart; j < (nj - itstart); ++j)
    for (int i = itstart; i < (ni - itstart); ++i)
    {
      bool any_invalid = false;
      float resp = 0;
      T vmin = std::numeric_limits<T>::max();
      for (int k = 0; (k < ns) && !any_invalid; ++k)
      {
        int di = std::get<0>(deriv_pix_offset[k]);
        int dj = std::get<1>(deriv_pix_offset[k]);
        int wd = std::get<2>(deriv_pix_offset[k]);
        int off_i = i + di, off_j = j + dj;
        if (off_i < 0 || off_i >= ni)
          continue;
        if (off_j < 0 || off_j >= nj)
          continue;
        if (invalid(off_i, off_j))
        {
          any_invalid = true;
          continue;
        }

        T v = img(off_i, off_j);
        resp += float(wd) * v;
        if (k == 0)
          vmin = v;
      }
      if (!any_invalid)
        {
        resp /= float(sum_coef);
        resp = float(resp) / (float(vmin)+1.0f);
        resp_img(i, j) = resp;
        debug_resp_img(i, j) = resp;
      }
      else
      {
        resp = 0.0f;
        resp_img(i, j) = resp;
        debug_resp_img(i, j) = resp;
      }
    }
  // compute peak response location
  T max_pix = std::numeric_limits<T>::max();

  for (int j = itstart; j < (nj - itstart); ++j)
    for (int i = itstart; i < (ni - itstart); ++i)
    {
      float v = resp_img(i, j);
      if (v <= 1.0f)
        continue;
      int k_max = 0;
      float fm1 = 0.0f, fm = 0.0f, fp = 0.0f, fp1 = 0.0f, f0 = 0.0f, v_max = 0;
      for (int k = 0; k < ns; ++k)
      {
        int di = std::get<0>(deriv_pix_offset[k]);
        int dj = std::get<1>(deriv_pix_offset[k]);
        int off_i = i + di, off_j = j + dj;
        if (off_i < 0 || off_i >= ni)
          continue;
        if (off_j < 0 || off_j >= nj)
          continue;
        float v = resp_img(off_i, off_j);
        if (v > v_max)
        {
          v_max = v;
          k_max = k;
        }
      }
      if (k_max > 1 && k_max < (ns - 1))
      {
        int di = std::get<0>(deriv_pix_offset[k_max - 1]);
        int dj = std::get<1>(deriv_pix_offset[k_max - 1]);
        fm = resp_img(i + di, j + dj);
        f0 = v_max;
        di = std::get<0>(deriv_pix_offset[k_max + 1]);
        dj = std::get<1>(deriv_pix_offset[k_max + 1]);
        fp = resp_img(i + di, j + dj);
      }else
        continue;
      float ratio_minus = (f0 - fp) / f0;
      float ratio_plus = (f0 - fm) / f0;
      if (k_max == radius||( (k_max == radius-1 )&&ratio_plus<0.05 )||( (k_max == radius+1)&&ratio_minus<0.05 ) )
        {
          center(i, j) = v_max;
        }
     }
  int invalid_r = 2*radius;
  std::vector<std::vector<std::tuple<int, int> > > invalid_offset = step_mask(invalid_r, sun_dir);
  int invalid_itstart = int(invalid_r) + 1;
  for (int j = invalid_itstart; j < (nj - invalid_itstart); ++j)
    for (int i = invalid_itstart; i < (ni - invalid_itstart); ++i)
    {
      float v = center(i, j);
      if (v <= 0.0f)
        continue;
      float p = v / (1.0f + v);
      for (size_t w = 0; w < invalid_offset.size(); ++w)
      {
        int invalid_ns = invalid_offset[w].size();
        for (int k = 0; k < invalid_ns; ++k)
        {
          int di = std::get<0>(invalid_offset[w][k]);
          int dj = std::get<1>(invalid_offset[w][k]);
          int off_i = i+di, off_j = j+dj;
          if(off_i<0 || off_i >= ni)
            continue;
          if(off_j<0 || off_j >= nj)
            continue;
          if (k <= invalid_r) // stop at roof edge
            step_prob_img(off_i, off_j) = p;
        }
        step_prob_img(i, j) = p;
      }
    }
}
static void adaptive_shadow_prob(size_t i, size_t j, vil_image_view<bool> const& invalid,
                                 vil_image_view<unsigned short> const& rect_image,
                                 vil_image_view<float> const& shadow_step_prob,
                                 vgl_vector_2d<float> sun_dir, vil_image_view<float>& shadow_prob,
                                 float scan_length, float default_shadow_thresh, float shad_stp_thresh){
  int ni = rect_image.ni(), nj = rect_image.nj();
  // could be starting from default shadow
  bool initial_ss = shadow_step_prob(i, j) > shad_stp_thresh;
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

  size_t n = shstp_vals.size();
  bool start = false;
  bool done = false;
  // scan initialized by default shadow not shadow step
  if(!initial_ss){
    for(size_t i = 0; i<n&&!done; ++i){
      int xi = std::get<2>(shstp_vals[i]), yi = std::get<3>(shstp_vals[i]);
      bool inv = invalid(xi, yi);
      float rect = std::get<1>(shstp_vals[i]);
      bool shadow = rect<default_shadow_thresh;
      if(!start&& shadow){
        start = true;
      }
      if(start && shadow){
        if(!inv && rect < default_shadow_thresh)
          shadow_prob(xi, yi) = 1.0;
        if(i == (n-1)){
          done = true;
          start = false;
        }
      }
    }
    return;
  }
  // analyze profile for threshold at the start of the shadow region
  float thresh = default_shadow_thresh;
  float max_thresh = default_shadow_thresh;
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
      max_thresh = rec;
      if(rec<thresh)
        thresh = rec;
      if(thresh < default_shadow_thresh)
        thresh = default_shadow_thresh;
      if(i == (n-1)){
        done = true;
        start = false;
      }
      //int xi = std::get<2>(shstp_vals[i]), yi = std::get<3>(shstp_vals[i]);
      //shadow_prob(xi, yi) = 1.0;
      continue;
    }
    done = true;
    start = false;
  }
  // max_thresh is set at end of shad step line scan
  // thresh set to minimum
  
  // finally scan and classify shadow
  start = false;
  done = false;
  for(size_t i = 0; i<n&&!done; ++i){
    int xi = std::get<2>(shstp_vals[i]), yi = std::get<3>(shstp_vals[i]);
    bool inv = invalid(xi, yi);
    float rect = std::get<1>(shstp_vals[i]);
    bool shadow = rect<thresh;
    //bool shadow = rect<max_thresh;
    //bool shadow = rect<default_shadow_thresh;
    bool ss = std::get<4>(shstp_vals[i]);
    //if(!start&&(shadow||ss)){
    if(!start&&(shadow)){
      start = true;
    }
    //if(start && (ss || shadow)){
    if(start && (shadow)){
      if(!inv && rect <= thresh)
      //if(!inv && rect <= default_shadow_thresh)
        shadow_prob(xi, yi) = 1.0;
      else if(!inv && (thresh<max_thresh) && (rect < max_thresh)){
        // float p = (1.0 - (float(rect) - thresh)/float(max_thresh-rect));
        float p = exp(-2.5*(float(rect) - thresh)/float(max_thresh-rect));
        if(p<0.0f) p = 0.0f;
        if(p>1.0f) p = 1.0f;
        shadow_prob(xi, yi)= p;
      }
      if(i == (n-1)){
        done = true;
        start = false;
      }
    }
  }
}

template <class T>
void bsgm_shadow_prob(vil_image_view<T> const& rect_img, vil_image_view<bool> const& invalid,
                      vgl_vector_2d<float> const& sun_dir, float default_shadow_thresh, 
                      vil_image_view<float> const& shadow_step_prob,
                      vil_image_view<float>& shadow_prob,
                      float scan_length, float ss_thresh){
  size_t ni = rect_img.ni(), nj = rect_img.nj();
  shadow_prob.set_size(ni, nj);
  shadow_prob.fill(0.0f);
  vil_image_view<unsigned short> temp;
  if(std::is_same<T, unsigned short>::value)
    temp = rect_img;
  else // byte to short
    vil_convert_cast(rect_img, temp);
  for (size_t j = 0; j < nj; ++j)
    for (size_t i = 0; i < ni; ++i) 
      if(!invalid(i,j) && ((shadow_step_prob(i, j) >= ss_thresh)||(rect_img(i,j)<default_shadow_thresh)))
        adaptive_shadow_prob(i, j, invalid, temp, shadow_step_prob,
                             sun_dir,shadow_prob, scan_length, 
                             default_shadow_thresh, ss_thresh);
}
static void one_d_dialation(std::vector<bool> const& vals, size_t gap, std::vector<bool>& dialated_vals, bool print = false) {
  dialated_vals = vals;
  size_t n = vals.size();
  size_t nq = gap + 2;
  if (n < nq)
    return;
  std::vector<bool> queue(nq);
  for (size_t i = 0; i < n - nq; ++i) {
    for (size_t k = 0; k < nq; ++k)
      queue[k] = vals[i + k];
    if (queue[0] && queue[nq - 1])
      for (size_t k = 1; k < nq - 1; ++k)
        dialated_vals[i + k] = true;
  }
  if (print)
      int jj = 0;
  //find shadow end
  size_t sh_end = 0;
  bool done = false;
  for (size_t i = 0; i < n-1&&!done ; ++i){
    bool d = dialated_vals[i], dp = dialated_vals[i+1];
    if(d && !dp){//end of shadow
      sh_end = i;
      done = true;
    }
  }
  // add gap at end
  size_t sh_last = sh_end+1 + gap;
  if(sh_last >= n) sh_last = n-1;
  for(size_t i = sh_end+1; i <sh_last ; ++i)
    dialated_vals[i] = true;
}
static void one_d_tail_erode(std::vector<bool> const& vals, size_t rem, std::vector<bool>& eroded_vals) {
  eroded_vals = vals;
  size_t n = vals.size();
  size_t nq = rem + 1;
  if (n < nq)
    return;
  std::vector<bool> queue(nq);
  for (size_t i = 0; i < n - nq; ++i) {
    for (size_t k = 0; k < nq; ++k)
      queue[k] = vals[i + k];
    if (queue[0] && !queue[nq - 1])
      for (size_t k = 0; k < nq - 1; ++k)
        eroded_vals[i + k] = false;
  }
}

template <class T>
void bsgm_check_nonunique(
  vil_image_view<float>& disp_img,
  const vil_image_view<unsigned short>& disp_cost,
  const vil_image_view<T>& img,
  float invalid_disparity,
  unsigned short shadow_thresh,
  int disp_thresh,
  const vgl_box_2d<int>& img_window)
{
  int img_start_x, img_start_y;
  if (img_window.is_empty()) {
    img_start_x = 0;
    img_start_y = 0;
  }
  else {
    img_start_x = img_window.min_x();
    img_start_y = img_window.min_y();
  }

  int w = disp_img.ni(), h = disp_img.nj();
  std::vector<unsigned short> inv_cost(w);
  std::vector<int> inv_disp(w);

  for (int y = 0, img_y = img_start_y; y < h; y++, img_y++) {

    // Initialize an inverse disparity map for this row
    for (int x = 0; x < w; x++) {
      inv_cost[x] = 65535;
      inv_disp[x] = -1;
    }

    // Construct the inverse disparity map
    for (int x = 0; x < w; x++) {

      if (std::isnan(invalid_disparity)) {
        if (std::isnan(disp_img(x, y)))
          continue;
      }
      else if (disp_img(x, y) == invalid_disparity) continue;

      // Get an integer disparity and location in reference image
      int d = static_cast<int>(std::round(disp_img(x, y)));
      int x_l = x + d;

      if (x_l < 0 || x_l >= w) continue;

      // Record the min cost pixel mapping back to x_l
      if (inv_cost[x_l] > disp_cost(x, y)) {
        inv_cost[x_l] = disp_cost(x, y);
        inv_disp[x_l] = d;
      }
    } //x

    // Check the uniqueness of each disparity.
    for (int x = 0, img_x = img_start_x; x < w; x++, img_x++) {

      if (std::isnan(invalid_disparity)) {
        if (std::isnan(disp_img(x, y)))
          continue;
      }
      else if (disp_img(x, y) == invalid_disparity) continue;

      // Label dark pixels as invalid, if thresh=0 nothing happens
      if (img(img_x, img_y) < shadow_thresh) {
        disp_img(x, y) = invalid_disparity;
        continue;
      }

      // Compute the floor and ceiling of each disparity
      int d_floor = static_cast<int>(std::floor(disp_img(x, y)));
      int d_ceil = static_cast<int>(std::ceil(disp_img(x, y)));
      int x_floor = x + d_floor, x_ceil = x + d_ceil;

      if (x_floor < 0 || x_ceil < 0 || x_floor >= w || x_ceil >= w)
        continue;

      // Check if either inverse disparity is consistent and flag if not.
      if (abs(inv_disp[x_floor] - d_floor) > disp_thresh &&
        abs(inv_disp[x_ceil] - d_ceil) > disp_thresh)
        disp_img(x, y) = invalid_disparity;
    } //x
  } //y

}

//: Compute a map of invalid pixels based on seeing the 'border_val'
// in either target or reference images.
template <class T >
void bsgm_compute_invalid_map(
  const vil_image_view<T>& img_tar,
  const vil_image_view<T>& img_ref,
  vil_image_view<bool>& invalid_tar,
  int min_disparity,
  int num_disparities,
  T border_val,
  const vgl_box_2d<int>& target_window)
{
  int max_disparity = min_disparity + num_disparities;

  // Iteration bounds
  int w, h, img_start_x, img_start_y, img_stop_x, img_stop_y;
  if (target_window.is_empty()) {
    w = img_tar.ni();
    h = img_tar.nj();
    img_start_x = 0;
    img_start_y = 0;
    img_stop_x = w;
    img_stop_y = h;
  }
  else {
    w = target_window.width();
    h = target_window.height();
    img_start_x = target_window.min_x();
    img_start_y = target_window.min_y();
    img_stop_x = target_window.max_x();
    img_stop_y = target_window.max_y();
  }
  
  // the x coordinate of the left-most possible reference pixel mapped to from the target
  int img_ref_start_x = std::max(0, img_start_x + min_disparity);

  // the x coordinate of the right-most possible reference pixel mapped to from the target
  int img_ref_stop_x = std::min<int>(img_ref.ni(), img_stop_x + max_disparity);

  invalid_tar.set_size( w, h );

  // Initialize map
  for (int invalid_y = 0; invalid_y < h; invalid_y++)
    for (int invalid_x = 0; invalid_x < w; invalid_x++)
      invalid_tar(invalid_x, invalid_y) = false;

  // Find the border in the target image
  for (int invalid_y = 0, img_y = img_start_y; invalid_y < h; invalid_y++, img_y++) {

    // Fill in the left border
    for (int invalid_x = 0, img_x = img_start_x; invalid_x < w; invalid_x++, img_x++) {
        if (img_tar(img_x, img_y) == border_val) {
            invalid_tar(invalid_x, invalid_y) = true;
       }
      else
        break;
    } //x
    
    // Fill in the right border
    for (int invalid_x = w - 1, img_x = img_stop_x - 1; invalid_x >= 0; invalid_x--, img_x--) {
      if (img_tar(img_x, img_y) == border_val)
        invalid_tar(invalid_x, invalid_y) = true;
      else
        break;
    } //x
  } //y
#if 0 // for debug
  vil_image_view<vxl_byte> tar_vis(w, h);
  for (int y = 0; y < h; y++)
      for (int x = 0; x < w; x++)
          tar_vis(x, y) = invalid_tar(x, y) ? 255 : 0;
  vil_save( vis, "D:/tests/WorldCup/AOI4/debug/invalid.png" );
#endif
  // Find the border in the reference image
  for (int invalid_y = 0, img_y = img_start_y; invalid_y < h; invalid_y++, img_y++) {

    // Find the left border
    int lb_ref = img_ref_start_x;
    for ( ; lb_ref < img_ref_stop_x; lb_ref++)
      if (img_ref(lb_ref, img_y) != border_val)
        break;

    int lb_target = lb_ref - min_disparity;
    int lb_invalid = lb_target - img_start_x;

    // Mask any pixels in the target image which map into the left border
    for (int invalid_x = 0; invalid_x < std::min(w, lb_invalid); invalid_x++)
      invalid_tar(invalid_x, invalid_y) = true;
   
    // Find the right border
    int rb_ref = img_ref_stop_x - 1;
    for ( ; rb_ref >= img_ref_start_x; rb_ref--)
      if (img_ref(rb_ref, img_y) != border_val)
        break;

    int rb_target = rb_ref - max_disparity;
    int rb_invalid = rb_target - img_start_x;
    
    // Mask any pixels in the target image which map into the right border
    for (int invalid_x = w - 1; invalid_x > std::max(0, rb_invalid); invalid_x--)
      invalid_tar(invalid_x, invalid_y) = true;
    
  } //y
  
  vil_image_view<vxl_byte> vis( w, h );
  for( int y = 0; y < h; y++ )
    for( int x = 0; x < w; x++ )
      vis(x,y) = invalid_tar(x,y) ? 255 : 0;
}

//: Fill in disparity pixels flagged as errors via multi-directional
// sampling.
template <class T>
void bsgm_interpolate_errors(
  vil_image_view<float>& disp_img,
  const vil_image_view<bool>& invalid,
  const vil_image_view<T>& img,
  float invalid_disparity,
  unsigned short shadow_thresh,
  const vgl_box_2d<int>& img_window)
{
  int img_start_x, img_start_y;
  if (img_window.is_empty()) {
    img_start_x = 0;
    img_start_y = 0;
  }
  else {
    img_start_x = img_window.min_x();
    img_start_y = img_window.min_y();
  }

  int num_sample_dirs = 8;
  float sample_percentile = 0.5f;
  float shadow_sample_percentile = 0.75f;

  int w = disp_img.ni(), h = disp_img.nj();
  std::vector<float> sample_vol( w*h*num_sample_dirs, 0.0f );
  vil_image_view<vxl_byte> sample_count( w, h );
  sample_count.fill( 0 );

  // Setup buffers
  std::vector<float> dir_sample_cur( w, 0.0f );
  std::vector<float> dir_sample_prev( w, 0.0f );

  // The following directional smoothing is adapted from run_multi_dp
  for( int dir = 0; dir < num_sample_dirs; dir++ ){

    int dx, dy, temp_dx = 0, temp_dy = 0;
    int x_start, y_start, x_end, y_end;

    // - - -
    // X X X
    // - - -
    if( dir == 0 ){
      dx = -1; dy = 0;
      x_start = 1; x_end = w-1;
      y_start = 0; y_end = h-1;

    } else if( dir == 1 ){
      dx = 1; dy = 0;
      x_start = w-2; x_end = 0;
      y_start = h-1; y_end = 0;

    // X - -
    // - X -
    // - - X
    } else if( dir == 2 ){
      dx = -1; dy = -1;
      x_start = 1; x_end = w-1;
      y_start = 1; y_end = h-1;

    } else if( dir == 3 ){
      dx = 1; dy = 1;
      x_start = w-2; x_end = 0;
      y_start = h-2; y_end = 0;

    // - X -
    // - X -
    // - X -
    } else if( dir == 4 ){
      dx = 0; dy = -1;
      x_start = 0; x_end = w-1;
      y_start = 1; y_end = h-1;

    } else if( dir == 5 ){
      dx = 0; dy = 1;
      x_start = w-1; x_end = 0;
      y_start = h-2; y_end = 0;

    // - - X
    // - X -
    // X - -
    } else if( dir == 6 ){
      dx = 1; dy = -1;
      x_start = w-2; x_end = 0;
      y_start = 1; y_end = h-1;

    } else if( dir == 7 ){
      dx = -1; dy = 1;
      x_start = 1; x_end = w-1;
      y_start = h-2; y_end = 0;
    }

    // Automatically determine iteration direction from end points
    int x_inc = (x_start < x_end) ? 1 : -1;
    int y_inc = (y_start < y_end) ? 1 : -1;

    // Initialize previous row
    for( int v = 0; v < w; v++ )
      dir_sample_prev[v] = invalid_disparity;

    // Loop through rows
    for( int y = y_start; y != y_end + y_inc; y += y_inc ){

      // Re-initialize current row in case dir follows row
      for( int x = 0; x < w; x++ )
        dir_sample_cur[x] = invalid_disparity;

      for( int x = x_start; x != x_end + x_inc; x += x_inc ){

        // If good sample at this pixel, record it
        if (std::isnan(invalid_disparity)) {
          if (std::isnan(disp_img(x, y)))
            continue;
        }
        else if( disp_img(x,y) != invalid_disparity ){
          dir_sample_cur[x] = disp_img(x,y);

        } else {

          // Otherwise propagate previous sample
          if( dy == 0 )
            dir_sample_cur[x] = dir_sample_cur[x+dx];
          else
            dir_sample_cur[x] = dir_sample_prev[x+dx];

          // And add sample to this pixel's sample set
          if (std::isnan(invalid_disparity)) {
            if (std::isnan(disp_img(x, y)))
              continue;
		  }
          if( dir_sample_cur[x] != invalid_disparity ){
            sample_vol[ num_sample_dirs*(y*w + x) + sample_count(x,y) ] =
              dir_sample_cur[x];
            sample_count(x,y)++;
          }
        }

      } //x

      // Copy current row to prev
      dir_sample_prev = dir_sample_cur;
    } //y
  }//dir

  // Interpolate any invalid pixels by taking the median (or specified
  // percentile) of accumulated sample set.
  std::vector<float>::iterator sample_itr = sample_vol.begin();
  for (int y = 0, img_y = img_start_y; y < h; y++, img_y++) {
    for (int x = 0, img_x = img_start_x; x < w; x++, img_x++, sample_itr += num_sample_dirs) {
      if (invalid(x, y)) continue;

      // Require half of the directions return valid samples, which prevents
      // bogus interpolation on the boundary of the disparity map
      if( sample_count(x,y) <= 4 ) continue;

      std::sort( sample_itr, sample_itr + sample_count(x,y) );

      int interp_idx;

      // Choose a sample index depending on whether pixel is shadow or not
      if (img(img_x, img_y) < shadow_thresh)
        interp_idx = static_cast<int>((shadow_sample_percentile*sample_count(x, y)));
      else
        interp_idx = static_cast<int>((sample_percentile*sample_count(x, y)));

      disp_img(x,y) = *( sample_itr + interp_idx );
    }
  }

}
#undef BSGM_ERROR_CHECKING_INSTANTIATE
#define BSGM_ERROR_CHECKING_INSTANTIATE(T) \
template void bsgm_check_shadows(vil_image_view<float>& , const vil_image_view<T>&,               \
                                 float, unsigned short, const vgl_box_2d<int>&);                  \
template void bsgm_interpolate_errors(vil_image_view<float>& ,const vil_image_view<bool>&,        \
                                      const vil_image_view<T>&,  float, unsigned short,           \
                                      const vgl_box_2d<int>&);                                    \
template void bsgm_compute_invalid_map(const vil_image_view<T>& , const vil_image_view<T>&,       \
                                       vil_image_view<bool>& , int, int, T,                       \
                                       const vgl_box_2d<int>&);                                   \
template void bsgm_check_nonunique(vil_image_view<float>& , const vil_image_view<unsigned short>&,\
                                   const vil_image_view<T>&, float, unsigned short, int,          \
                                   const vgl_box_2d<int>&);                                       \
template void bsgm_shadow_step_filter(const vil_image_view<T>&, const vil_image_view<bool>&,      \
                                      vil_image_view<float>&, const vgl_vector_2d<float>&,        \
                                      int, int, int);                   \
template  void bsgm_shadow_prob(vil_image_view<T> const&, vil_image_view<bool> const&, \
vgl_vector_2d<float> const&, float, vil_image_view<float> const&, vil_image_view<float>&, \
                                  float, float)

#endif // bsgm_error_checking_h_
