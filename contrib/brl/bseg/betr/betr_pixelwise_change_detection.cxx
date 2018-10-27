#include <cmath>
#include <vil_image_view.h>
#include <vil_save.h>
#include <vil_convert.h>
#include <vil/vil_new.h>
#include <vil/vil_image_view.h>
#include <vgl/algo/vgl_h_matrix_2d_compute_linear.h>
#include <vgl/algo/vgl_h_matrix_2d_compute_4point.h>
#include <vsol/vsol_point_2d.h>
#include <bsol/bsol_algs.h>

#include <baml/baml_warp.h>

#include "betr_pixelwise_change_detection.h"
#include "betr_pixelwise_change_detection_params.h"
#include "betr_event_trigger.h"



bool betr_pixelwise_change_detection::process() {
  // Hardcoded variables
  float change_prior = 0.01f;

  // Get params
  auto* cd_params =
    dynamic_cast<betr_pixelwise_change_detection_params*>(params_.ptr());
  // Get a bounding box for the event polygon
  vsol_box_2d_sptr evt_bb = evt_evt_poly_->get_bounding_box();
  int bb_minx = evt_bb->get_min_x(), bb_miny = evt_bb->get_min_y();
  int bb_width = evt_bb->width(), bb_height = evt_bb->height();
  if (bb_minx < 0 || bb_minx + bb_width >= static_cast<int>(evt_imgr_->ni()) ||
    bb_miny < 0 || bb_miny + bb_height >= static_cast<int>(evt_imgr_->nj())) {
    std::cout << "WARNING: betr_pixelwise_change_detection invalid bounding box\n";
    avg_prob_ = -1.0;
    return true;
  }

  // Get cropped event image
  vil_image_view<vxl_uint_16> evt_img = evt_imgr_->get_view(
    bb_minx, bb_width, bb_miny, bb_height);

  // Convert polygon vertices
  int num_vertices = evt_evt_poly_->size();
  std::vector< vgl_homg_point_2d<double> > vert_evt(num_vertices);
  std::vector< vgl_homg_point_2d<double> > vert_ref(num_vertices);
  if (ref_evt_polys_.size() != 1 && multiple_ref_ == false) {
    std::cout << "not exactly one reference ref_poly in pixelwise_change_detection\n";
    return false;
  }

  // loop over all reference images and warp/crop to region of interest
  std::vector< vil_image_view<vxl_uint_16> > ref_img(ref_rescs_.size());
  std::vector< vil_image_view<vxl_uint_16> > ref_cropped(ref_rescs_.size());
  for (int i = 0; i < ref_cropped.size(); i++) {
    for (int p = 0; p < num_vertices; p++) {
      vert_evt[p].set(
        evt_evt_poly_->vertex(p)->x(), evt_evt_poly_->vertex(p)->y());
      vert_ref[p].set(
        ref_evt_polys_[i]->vertex(p)->x(), ref_evt_polys_[i]->vertex(p)->y());
    }

    // Compute a homography from ref image to evt image using the polygons
    //vgl_h_matrix_2d_compute_4point hc;
    vgl_h_matrix_2d_compute_linear hc;
    vgl_h_matrix_2d<double> ref_to_evt = hc.compute(vert_ref, vert_evt);

    // Compute homography to cropped event region and compose
    vgl_h_matrix_2d<double> evt_to_cropped;
    evt_to_cropped.set_identity();
    evt_to_cropped.set_translation(-bb_minx, -bb_miny);
    vgl_h_matrix_2d<double> ref_to_evt_cropped(evt_to_cropped.get_matrix()*ref_to_evt.get_matrix());
    vgl_h_matrix_2d<double> ref_to_evt_cropped2 = evt_to_cropped*ref_to_evt;


    if (!multiple_ref_) { // single image
      if (ref_rescs_.size() != 1) {
        std::cout << "not exactly one reference image\n";
        return false;
      }
      ref_img[0] = ref_rescs_[0]->get_view();
      baml_warp_perspective(ref_img[0], ref_to_evt_cropped, bb_width, bb_height, ref_cropped[0]);
    }
    else { // multiple images
      ref_img[i] = ref_rescs_[i]->get_view();
      baml_warp_perspective(ref_img[i], ref_to_evt_cropped, bb_width, bb_height, ref_cropped[i]);
    }
  }

  // TODO: GET REFERENCE REGIONS FOR BT

  // Setup pixel-wise probability buffers
  std::vector< vil_image_view<bool> > valid(ref_rescs_.size());
  for (int i = 0; i < ref_rescs_.size(); i++) {
    valid[i].set_size(bb_width, bb_height);
    valid[i].fill(true);
  }
  vil_image_view<float> evt_change_prob;

  std::string save_dir = "C:/Users/sca0161/Documents/hamadan_test/debug/";
  vil_save(evt_img, (save_dir + "event.tif").c_str());
  vil_save(ref_cropped[0], (save_dir + "ref.tif").c_str());

  // Compute pixel-wise likelihood using specified metric
  baml_change_detection cd( cd_params->pw_params_ );
  bool cd_success;
  if (!multiple_ref_) {
    cd_success = cd.detect(evt_img, ref_cropped[0], valid[0], evt_change_prob);
  }
  else {
    cd_success = cd.multi_image_detect(evt_img, ref_cropped, valid, evt_change_prob);
  }
  if(! cd_success ){
    std::cout << "WARNING: betr_pixelwise_change_detection unsuccessful change detection\n";
    avg_prob_ = -1.0;
    return true;
  }

  // Convert to byte and save as prob map
  vil_image_view<vxl_byte> vis;
  vil_convert_stretch_range_limited( evt_change_prob, vis, 0.0f, 1.0f );

  i_offset_ = bb_minx; j_offset_ = bb_miny;

  vil_save(vis, (save_dir + "change_prob.tif").c_str());

  // create integral image to find area of highest average probability of change
  vil_image_view<float> integral_im;
  integral_im.set_size(bb_width + 1, bb_height + 1);
  integral_im.fill(0.0);
  // create integral image of pixels in polygon to use for normalization to find area of highest average probability of change
  vil_image_view<float> integral_im_poly;
  integral_im_poly.set_size(bb_width + 1, bb_height + 1);
  integral_im_poly.fill(0.0);

  //create vgl_polygon so that we can check if each pixel is in the polygon using with vgl contains function
  vgl_polygon<double> evt_vgl_poly = bsol_algs::vgl_from_poly(evt_evt_poly_);

  vil_image_view<float> temp;

  int num_in_poly = 0;
  // create integral images
  for( int y = 0; y < bb_height; y++ ){
    for (int x = 0; x < bb_width; x++) {
      if (!evt_vgl_poly.contains(x + bb_minx, y + bb_miny)) {
        vis(x, y) = static_cast<vxl_byte>(0);
        integral_im_poly(x + 1, y + 1) = 0 + integral_im_poly(x, y + 1) + integral_im_poly(x + 1, y) - integral_im_poly(x, y);
        evt_change_prob(x, y) = 0; // outside of the polygon there should be 0 probability of change
      }
      else {
        num_in_poly++;
        integral_im_poly(x + 1, y + 1) = 1 + integral_im_poly(x, y + 1) + integral_im_poly(x + 1, y) - integral_im_poly(x, y);
      }
      integral_im( x + 1, y + 1 ) = evt_change_prob(x, y) + integral_im(x, y + 1) + integral_im(x + 1, y) - integral_im(x, y);
    }
  }
  change_img_ = vil_new_image_resource_of_view(vis);
  // find region of highest probability
  int eventHeight = cd_params->pw_params_.event_height;
  int eventWidth = cd_params->pw_params_.event_width;
  if (eventHeight > evt_change_prob.nj()) eventHeight = evt_change_prob.nj();
  if (eventWidth > evt_change_prob.ni())eventWidth = evt_change_prob.ni();
  avg_prob_ = 0.0;
  bool valid_event = false;
  for (int x = 0; x < evt_change_prob.ni() - eventWidth + 1; x++) {
    for (int y = 0; y < evt_change_prob.nj() - eventHeight + 1; y++) {
      if ((integral_im_poly(x, y) + integral_im_poly(x + eventWidth, y + eventHeight) - integral_im_poly(x, y + eventHeight) - integral_im_poly(x + eventWidth, y)) == (eventHeight*eventWidth)) {
        float cur_average = (integral_im(x, y) + integral_im(x + eventWidth, y + eventHeight) - integral_im(x, y + eventHeight) - integral_im(x + eventWidth, y)) /
          (eventHeight*eventWidth);
        valid_event = true;
        if (cur_average > avg_prob_) {
          avg_prob_ = cur_average;
        }
      }
    }
  }
  if (!valid_event) avg_prob_ = integral_im(bb_width, bb_height) / integral_im_poly(bb_width, bb_height);
  return true;

  //// find region of highest probability
  //int eventHeight = cd_params->pw_params_.event_height;
  //int eventWidth = cd_params->pw_params_.event_width;
  //if (eventHeight > evt_change_prob.nj()) eventHeight = evt_change_prob.nj();
  //if (eventWidth > evt_change_prob.ni())eventWidth = evt_change_prob.ni();
  //avg_prob_ = 0.0;
  //for (int x = 0; x < evt_change_prob.ni() - eventWidth + 1; x++) {
  //  for (int y = 0; y < evt_change_prob.nj() - eventHeight + 1; y++) {
  //    float cur_average = (integral_im(x, y) + integral_im(x + eventWidth, y + eventHeight) - integral_im(x, y + eventHeight) - integral_im(x + eventWidth, y)) /
  //      (integral_im_poly(x, y) + integral_im_poly(x + eventWidth, y + eventHeight) - integral_im_poly(x, y + eventHeight) - integral_im_poly(x + eventWidth, y));
  //    if (cur_average > avg_prob_) {
  //      avg_prob_ = cur_average;
  //    }
  //  }
  //}
  //return true;
}
