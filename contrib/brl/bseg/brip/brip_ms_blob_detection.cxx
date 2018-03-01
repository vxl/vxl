#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vil/vil_convert.h>
#include <vil/vil_load.h>
#include <vil/vil_crop.h>
#include <vil/vil_resample_bilin.h>
#include <vil/vil_save.h>
#include <vil/algo/vil_blob.h>
#include <vgl/algo/vgl_h_matrix_2d_compute_4point.h>
#include <vgl/vgl_polygon.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_inverse.h>
#include <vpgl/vpgl_local_rational_camera.h>
#include <vpgl/vpgl_lvcs.h>
#include <brip/brip_vil_float_ops.h>

#include "brip_ms_blob_detection.h"
#include <brad/brad_worldview3_functions.h>
#include <brad/brad_spectral_angle_mapper.h>


//===================================================================
// brip_blob_connect_comp_bb
//===================================================================
void brip_blob_connect_comp_bb(
  const vil_image_view<float>& image,
  const vil_image_view<bool>& material_mask,
  const vil_image_view<bool>& valid_mask,
  const float lambda0,
  const float lambda1,
  std::vector<unsigned>& i_min,
  std::vector<unsigned>& j_min,
  std::vector<unsigned>& i_max,
  std::vector<unsigned>& j_max,
  std::vector<vgl_polygon<double> >& poly
) {

  int ni = image.ni();
  int nj = image.nj();

  if (material_mask.ni() != ni || material_mask.nj() != nj || valid_mask.ni() != ni || valid_mask.nj() != nj) {
    std::cerr << "image, material_mask, and valid_mask all must have the same number of rows and columns";
    return;
  }

  // Setup WV3 bands
  std::vector<float> bands_min, bands_max;
  brad_wv3_bands(bands_min, bands_max, 8);
  brad_spectral_angle_mapper sam(bands_min, bands_max);
  sam.add_material_per_pixel("mat", image, material_mask);

  // creating spectral angle map
  vil_image_view<float> conf_img;
  sam.compute_sam_img(image, "mat", conf_img);

  // perform blob detection on sam
  float theta_interval = 5.0f;
  float cutoff_ratio = 0.01f;
  vil_image_view<float> blob = brip_vil_float_ops::fast_extrema_rotational(
    conf_img,
    lambda0, lambda1, theta_interval,
    true,/*bright*/
    false,/*mag*/
    false,/*signed*/
    true,/*scale invariant*/
    false,/*non_max_suppress*/
    cutoff_ratio);

  // set all invalid pixels to 0
  for (int j = 0; j < nj; j++) {
    for (int i = 0; i < ni; i++) {
      if (!valid_mask(i, j)) blob(i, j) = 0;
    }
  }

  // threshold image to get binary
  vil_image_view<bool> thresh(blob.ni(), blob.nj());
  thresh.fill(false);

  vil_image_view<vxl_byte> thresh_byte(blob.ni(), blob.nj());
  thresh_byte.fill(0);

  float blob_thresh = 0.035f;
  for (int j = 0; j < image.nj(); j++) {
    for (int i = 0; i < image.ni(); i++) {
      if (blob(i, j) > blob_thresh) thresh(i, j) = true;
      if (blob(i, j) > blob_thresh) thresh_byte(i, j) = 255;
    }
  }

  // connected components
  vil_image_view<unsigned> label;
  vil_blob_connectivity conn = vil_blob_4_conn;
  vil_blob_labels(thresh, conn, label);
  unsigned max_label = 0;
  for (int j = 0; j < label.nj(); j++) {
    for (int i = 0; i < label.ni(); i++) {
      if (label(i, j) > max_label) max_label = label(i, j);
    }
  }

  // calculate bounding box for each connected components
  std::vector<unsigned> i_min_full(max_label - 1, label.ni() - 1);
  std::vector<unsigned> j_min_full(max_label - 1, label.nj() - 1);
  std::vector<unsigned> i_max_full(max_label - 1, 0);
  std::vector<unsigned> j_max_full(max_label - 1, 0);
  for (unsigned lab = 1; lab <= max_label; lab++) {
    for (int j = 0; j < label.nj(); j++) {
      for (int i = 0; i < label.ni(); i++) {
        if (label(i, j) != lab) continue;
        if (i_min_full[lab - 1] > i) i_min_full[lab - 1] = i;
        if (j_min_full[lab - 1] > j) j_min_full[lab - 1] = j;
        if (i_max_full[lab - 1] < i) i_max_full[lab - 1] = i;
        if (j_max_full[lab - 1] < j) j_max_full[lab - 1] = j;
      }
    }
  }

  // prune bounding box list to only include boxes that fit the approximate
  // size we are looking for as determined by lambda0 and lambda1, and
  // prune bounding box list to only include boxes that contain blobs with a
  // limited range of best angles
  for (unsigned lab = 0; lab < max_label; lab++) {
    float dist = sqrt(pow(i_max_full[lab] - i_min_full[lab], 2) + pow(j_max_full[lab] - j_min_full[lab], 2));
    if (dist > 6 * std::max(lambda0, lambda1) || dist < 3 * std::max(lambda0, lambda1)) continue; // this bounding box does not fit our size constraint
    if ((i_max_full[lab] - i_min_full[lab]) < (3 * std::min(lambda0, lambda1)) || (j_max_full[lab] - j_min_full[lab]) < (3 * std::min(lambda0, lambda1))) continue; // this bounding box does not fit our size constraint
    i_min.push_back(i_min_full[lab]);
    j_min.push_back(j_min_full[lab]);
    i_max.push_back(i_max_full[lab]);
    j_max.push_back(j_max_full[lab]);
  }

  // convert to a vector of vgl_polygons
  poly.resize(i_min.size());
  std::vector<double> i_coord(5);
  std::vector<double> j_coord(5);
  for (int i = 0; i < poly.size(); i++) {
    i_coord[0] = double(i_min[i]); j_coord[0] = double(j_min[i]);
    i_coord[1] = double(i_max[i]); j_coord[1] = double(j_min[i]);
    i_coord[2] = double(i_max[i]); j_coord[2] = double(j_max[i]);
    i_coord[3] = double(i_min[i]); j_coord[3] = double(j_max[i]);
    i_coord[4] = double(i_min[i]); j_coord[4] = double(j_min[i]);
    poly[i] = vgl_polygon<double>(i_coord.data(), j_coord.data(), 4);
  }
}



//===================================================================
// brip_blob_local_max_bb
//===================================================================
void brip_blob_local_max_bb(
  const vil_image_view<float>& image,
  const vil_image_view<bool>& material_mask,
  const vil_image_view<bool>& valid_mask,
  const float lambda0,
  const float lambda1,
  const int n_size,
  std::vector<unsigned>& i_min,
  std::vector<unsigned>& j_min,
  std::vector<unsigned>& i_max,
  std::vector<unsigned>& j_max,
  std::vector<vgl_polygon<double> >& poly,
  std::vector<float>& conf
) {

  int ni = image.ni();
  int nj = image.nj();

  if (material_mask.ni() != ni || material_mask.nj() != nj) {
    std::cerr << "image, material_mask all must have the same number of rows and columns";
    return;
  }

  // Setup WV3 bands
  std::vector<float> bands_min, bands_max;
  brad_wv3_bands(bands_min, bands_max, 8);
  brad_spectral_angle_mapper sam(bands_min, bands_max);
  sam.add_material_per_pixel("mat", image, material_mask);

  // creating spectral angle map
  vil_image_view<float> conf_img;
  sam.compute_sam_img(image, "mat", conf_img);

  // perform blob detection on sam
  float theta_interval = 5.0f;
  float cutoff_ratio = 0.01f;
  vil_image_view<float> blob = brip_vil_float_ops::fast_extrema_rotational(
    conf_img,
    lambda0, lambda1, theta_interval,
    true,/*bright*/
    false,/*mag*/
    false,/*signed*/
    true,/*scale invariant*/
    false,/*non_max_suppress*/
    cutoff_ratio);

  // compare to all pixels in an n_size x n_size radius neighborhood
  // if largest in neighborhood add polygon to poly
  float blob_conf;
  float blob_thresh = -0.02f;
  for (int j = n_size; j < nj - n_size - 1; j++) { // loop over the image
    for (int i = n_size; i < ni - n_size - 1; i++) { // loop over the image
      bool is_max = true;
      blob_conf = blob(i, j);
      for (int filter_j = -n_size; filter_j <= n_size; filter_j++) { // loop over the filter (neighborhood) size
        for (int filter_i = -n_size; filter_i <= n_size; filter_i++) { // loop over the filter (neighborhood) size
          if (filter_i == 0 && filter_j == 0) continue; // not necessary to compare to self
          if (blob(i, j) <= blob_thresh) is_max = false; // if this pixel does not excede our theshold, it can't be a max
          if (blob(i, j) < blob(i + filter_i, j + filter_j)) is_max = false;
        } // filter_i
      } // filter_j
      if (is_max && valid_mask(i,j)) {
        i_min.push_back(i - n_size);
        i_max.push_back(i + n_size);
        j_min.push_back(j - n_size);
        j_max.push_back(j + n_size);
        conf.push_back(blob_conf);
      }
    } // i
  } // j

  // convert to a vector of vgl_polygons
  poly.resize(i_min.size());
  std::vector<double> i_coord(5);
  std::vector<double> j_coord(5);
  for (int i = 0; i < poly.size(); i++) {
    i_coord[0] = double(i_min[i]); j_coord[0] = double(j_min[i]);
    i_coord[1] = double(i_max[i]); j_coord[1] = double(j_min[i]);
    i_coord[2] = double(i_max[i]); j_coord[2] = double(j_max[i]);
    i_coord[3] = double(i_min[i]); j_coord[3] = double(j_max[i]);
    i_coord[4] = double(i_min[i]); j_coord[4] = double(j_min[i]);
    poly[i] = vgl_polygon<double>(i_coord.data(), j_coord.data(), 4);
  }

}
