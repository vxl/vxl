#include <vil_image_view.h>
#include <vgl/algo/vgl_h_matrix_2d_compute_linear.h>
#include <vsol/vsol_point_2d.h>

#include <baml/baml_detect_change.h>
#include <baml/baml_warp.h>

#include "betr_census_change_detection.h"
#include "betr_event_trigger.h"

#include <cmath>


bool betr_census_change_detection::process(){

  // Get a bounding box for the event polygon
  vsol_box_2d_sptr evt_bb = evt_evt_poly_->get_bounding_box();
  //vsol_box_2d_sptr ref_bb = ref_evt_poly_->get_bounding_box();
  int bb_minx = evt_bb->get_min_x(), bb_miny = evt_bb->get_min_y();
  int bb_width = evt_bb->width(), bb_height = evt_bb->height();

  // Get cropped event image
  vil_image_view<vxl_uint_16> evt_img = evt_imgr_->get_view( 
    bb_minx, bb_width, bb_miny, bb_height );

  // Convert polygon vertices
  int num_vertices = evt_evt_poly_->size();
  std::vector< vgl_homg_point_2d<double> > vert_evt(num_vertices); 
  std::vector< vgl_homg_point_2d<double> > vert_ref(num_vertices);

  for( int p = 0; p < num_vertices; p++ ){
    vert_evt[p].set( 
      evt_evt_poly_->vertex(p)->x(), evt_evt_poly_->vertex(p)->y() );
    vert_ref[p].set( 
      ref_evt_poly_->vertex(p)->x(), ref_evt_poly_->vertex(p)->y() );
  }

  // Compute a homography from ref image to evt image using the polygons
  vgl_h_matrix_2d<double> ref_to_evt;  
  vgl_h_matrix_2d_compute_linear hc;
  hc.compute( vert_ref, vert_evt );

  // Compute homography to cropped event region and compose
  vgl_h_matrix_2d<double> evt_to_cropped;
  evt_to_cropped.set_identity();
  evt_to_cropped.set_translation( -bb_minx, -bb_miny );
  vgl_h_matrix_2d<double> ref_to_evt_cropped = evt_to_cropped*ref_to_evt;

  // Get the ref image and warp into the cropped event image
  vil_image_view<vxl_uint_16> ref_img = ref_imgr_->get_view();
  vil_image_view<vxl_uint_16> ref_cropped;
  baml_warp_perspective( ref_img, ref_to_evt_cropped, bb_width, bb_height, ref_cropped );

  // TODO: BOUND CHECK BEFORE DOING THE ABOVE

  // Compute pixel-wise census probability
  vil_image_view<bool> valid( bb_width, bb_height );
  valid.fill( true );
  vil_image_view<float> evt_lh;
  if( !baml_detect_change_census( evt_img, ref_cropped, valid, evt_lh ) ){
    std::cout << "warning census_change_detection failed" << std::endl;
    avg_prob_ = -1.0;
    return true;
  }
  baml_sigmoid( evt_lh, evt_lh, 0.1f );

  // Compute average probability over the area
  float psum = 0.0f;
  int pcount = 0;
  for( int y = 0; y < bb_height; y++ ){
    for( int x = 0; x < bb_width; x++ ){
      // TODO: CHECK EACH PIXEL IN POLYGON
      psum += evt_lh(x,y);
      pcount++;
    }
  }

  avg_prob_ = psum/pcount;

  return true; 
}
