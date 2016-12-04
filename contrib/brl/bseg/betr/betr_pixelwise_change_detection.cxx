#include <vil_image_view.h>
#include <vil_save.h>
#include <vil_convert.h>
#include <vil/vil_new.h>
#include <vil/vil_image_view.h>
#include <vgl/algo/vgl_h_matrix_2d_compute_linear.h>
#include <vgl/algo/vgl_h_matrix_2d_compute_4point.h>
#include <vsol/vsol_point_2d.h>

#include <baml/baml_warp.h>

#include "betr_pixelwise_change_detection.h"
#include "betr_pixelwise_change_detection_params.h"
#include "betr_event_trigger.h"

#include <cmath>


bool betr_pixelwise_change_detection::process(){

  // Hardcoded variables
  float change_prior = 0.01f;

  // Get params
  betr_pixelwise_change_detection_params* cd_params = 
    dynamic_cast<betr_pixelwise_change_detection_params*>(params_.ptr());

  // Get a bounding box for the event polygon
  vsol_box_2d_sptr evt_bb = evt_evt_poly_->get_bounding_box();
  int bb_minx = evt_bb->get_min_x(), bb_miny = evt_bb->get_min_y();
  int bb_width = evt_bb->width(), bb_height = evt_bb->height();

  if( bb_minx < 0 || bb_minx + bb_width >= static_cast<int>(evt_imgr_->ni()) ||
    bb_miny < 0 || bb_miny + bb_height >= static_cast<int>(evt_imgr_->nj()) ){
    std::cout << "WARNING: betr_pixelwise_change_detection failure 1\n";
    avg_prob_ = -1.0;
    return true;
  }

  // Get cropped event image
  vil_image_view<vxl_uint_16> evt_img = evt_imgr_->get_view( 
    bb_minx, bb_width, bb_miny, bb_height );  

  // Convert polygon vertices
  int num_vertices = evt_evt_poly_->size();
  std::vector< vgl_homg_point_2d<double> > vert_evt(num_vertices); 
  std::vector< vgl_homg_point_2d<double> > vert_ref(num_vertices);
  if(ref_evt_polys_.size()!=1){
    std::cout << "not exactly one reference ref_poly in pixelwise_change_detection\n";
    return false;
  }
  for( int p = 0; p < num_vertices; p++ ){
    vert_evt[p].set( 
      evt_evt_poly_->vertex(p)->x(), evt_evt_poly_->vertex(p)->y() );
    vert_ref[p].set( 
      ref_evt_polys_[0]->vertex(p)->x(), ref_evt_polys_[0]->vertex(p)->y() );
  }

  // Compute a homography from ref image to evt image using the polygons
  //vgl_h_matrix_2d_compute_4point hc;
  vgl_h_matrix_2d_compute_linear hc;
  vgl_h_matrix_2d<double> ref_to_evt = hc.compute( vert_ref, vert_evt );

  // Compute homography to cropped event region and compose
  vgl_h_matrix_2d<double> evt_to_cropped;
  evt_to_cropped.set_identity();
  evt_to_cropped.set_translation( -bb_minx, -bb_miny );
  vgl_h_matrix_2d<double> ref_to_evt_cropped( evt_to_cropped.get_matrix()*ref_to_evt.get_matrix() );
  vgl_h_matrix_2d<double> ref_to_evt_cropped2 = evt_to_cropped*ref_to_evt;

  // Get the ref image and warp into the cropped event image
  if(ref_rescs_.size()!=1){
    std::cout << "not exactly one reference image\n";
    return false;
  }
  vil_image_view<vxl_uint_16> ref_img = ref_rescs_[0]->get_view();
  vil_image_view<vxl_uint_16> ref_cropped;
  baml_warp_perspective( ref_img, ref_to_evt_cropped, bb_width, bb_height, ref_cropped );

//vil_save( evt_img, "D:/results/chip1.tif" );
//vil_save( ref_cropped, "D:/results/chip2.tif" );

  // TODO: GET REFERENCE REGIONS FOR BT

  // Setup pixel-wise probability buffers
  vil_image_view<bool> valid( bb_width, bb_height );
  valid.fill( true );
  vil_image_view<float> evt_change_prob;
  
  // Compute pixel-wise likelihood using specified metric
  baml_change_detection cd( cd_params->pw_params_ );
  bool cd_success = cd.detect( evt_img, ref_cropped, valid, evt_change_prob );
    
  if(! cd_success ){
    std::cout << "WARNING: betr_pixelwise_change_detection failure 2\n";
    avg_prob_ = -1.0;
    return true;
  }

  // Convert to byte and save as prob map
  vil_image_view<vxl_byte> vis;
  vil_convert_stretch_range_limited( evt_change_prob, vis, 0.0f, 1.0f );
  change_img_ = vil_new_image_resource_of_view(vis);

  i_offset_ = bb_minx; j_offset_ = bb_miny;

  // Compute average probability over the area
  float psum = 0.0f;
  int pcount = 0;
  for( int y = 0; y < bb_height; y++ ){
    for( int x = 0; x < bb_width; x++ ){
      // TODO: CHECK THAT EACH PIXEL IS IN POLYGON
      psum += evt_change_prob(x,y);
      pcount++;
    }
  }

  avg_prob_ = psum/pcount;

  return true; 
}
