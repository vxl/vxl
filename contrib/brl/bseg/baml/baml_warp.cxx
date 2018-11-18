// This is brl/bseg/baml/baml_warp.cxx

#include <vnl/vnl_inverse.h>
#include <vil/vil_warp.h>
#include <vil/vil_bilin_interp.h>
#include <vgl/algo/vgl_h_matrix_2d_compute_4point.h>

#include "baml_warp.h"


//---------------------------------------------------------------------------
void baml_warp_perspective(
  const vil_image_view<vxl_uint_16>& img1,
  const vgl_h_matrix_2d<double>& img1_to_img2,
  int img2_width,
  int img2_height,
  vil_image_view<vxl_uint_16>& img2,
  bool inverse_homography )
{
  double tol = 0.0000001;
  vxl_uint_16 border_val = 0;

  int num_planes = img1.nplanes();
  int img1_width_safe = img1.ni()-2;
  int img1_height_safe = img1.nj()-2;

  // Invert homography unless already inverted
  vnl_matrix_fixed<double,3,3> img2_to_img1;
  if( inverse_homography == true )
    img2_to_img1 = img1_to_img2.get_matrix();
  else
    img2_to_img1 = vnl_inverse( img1_to_img2.get_matrix() );

  // Allocate image 2 and set to border_val
  img2.set_size( img2_width, img2_height, num_planes );
  img2.fill( border_val );

  for( int y = 0; y < img2_height; y++ ){

    // Speed up coordinate transform by pre-computing last two column
    // multiplications since they don't change with x.
    vnl_vector_fixed<double,3> col23 =
      img2_to_img1*vnl_vector_fixed<double,3>(0.0,(double)y,1.0);
    vnl_vector_fixed<double,3> col1(
      img2_to_img1(0,0), img2_to_img1(1,0), img2_to_img1(2,0) );

    for( int x = 0; x < img2_width; x++ ){

      // Compute the back-projection of pixel x,y in image 1
      vnl_vector_fixed<double,3> wxy = (double)x*col1 + col23;

      // Convert homogeneous coords, do not warp if at infinity
      if( std::fabs(wxy[2]) < tol ) continue;
      double wx = wxy[0]/wxy[2];
      double wy = wxy[1]/wxy[2];

      // Check coordinates are safe for interpolation
      if( wx < 0.0 || wx > img1_width_safe ||
          wy < 0.0 || wy > img1_height_safe ) continue;

      // Interpolate
      for( int p = 0; p < num_planes; p++ ){
        // The below is code from vil_bilin_interp_safe
        int p1x=int(wx);
        double normx = wx-p1x;
        int p1y=int(wy);
        double normy = wy-p1y;

        double i1 = img1(p1x,p1y) +
          ( img1(p1x,p1y+1) - (double)img1(p1x,p1y) )*normy;
        double i2 = img1(p1x+1,p1y) +
          ( img1(p1x+1,p1y+1) - (double)img1(p1x+1,p1y) )*normy;

        img2(x,y,p) = (vxl_uint_16)( i1+(i2-i1)*normx );

        //img2(x,y,p) = vil_bilin_interp_safe<vxl_uint_16>( img1, wx, wy, p );
      }
    } //x
  } //y

}
void baml_warp_perspective(
  const vil_image_view<float>& img1,
  const vgl_h_matrix_2d<double>& img1_to_img2,
  int img2_width,
  int img2_height,
  vil_image_view<float>& img2,
  bool inverse_homography){
  double tol = 0.0000001;
  float border_val = 0.0f;

  int num_planes = img1.nplanes();
  int img1_width_safe = img1.ni()-2;
  int img1_height_safe = img1.nj()-2;

  // Invert homography unless already inverted
  vnl_matrix_fixed<double,3,3> img2_to_img1;
  if( inverse_homography == true )
    img2_to_img1 = img1_to_img2.get_matrix();
  else
    img2_to_img1 = vnl_inverse( img1_to_img2.get_matrix() );

  // Allocate image 2 and set to border_val
  img2.set_size( img2_width, img2_height, num_planes );
  img2.fill( border_val );

  for( int y = 0; y < img2_height; y++ ){

    // Speed up coordinate transform by pre-computing last two column
    // multiplications since they don't change with x.
    vnl_vector_fixed<double,3> col23 =
      img2_to_img1*vnl_vector_fixed<double,3>(0.0,(double)y,1.0);
    vnl_vector_fixed<double,3> col1(
      img2_to_img1(0,0), img2_to_img1(1,0), img2_to_img1(2,0) );

    for( int x = 0; x < img2_width; x++ ){

      // Compute the back-projection of pixel x,y in image 1
      vnl_vector_fixed<double,3> wxy = (double)x*col1 + col23;

      // Convert homogeneous coords, do not warp if at infinity
      if( std::fabs(wxy[2]) < tol ) continue;
      double wx = wxy[0]/wxy[2];
      double wy = wxy[1]/wxy[2];

      // Check coordinates are safe for interpolation
      if( wx < 0.0 || wx > img1_width_safe ||
          wy < 0.0 || wy > img1_height_safe ) continue;

      // Interpolate
      for( int p = 0; p < num_planes; p++ ){
        // The below is code from vil_bilin_interp_safe
        int p1x=int(wx);
        double normx = wx-p1x;
        int p1y=int(wy);
        double normy = wy-p1y;

        double i1 = img1(p1x,p1y) +
          ( img1(p1x,p1y+1) - (double)img1(p1x,p1y) )*normy;
        double i2 = img1(p1x+1,p1y) +
          ( img1(p1x+1,p1y+1) - (double)img1(p1x+1,p1y) )*normy;

        img2(x,y,p) = (float)( i1+(i2-i1)*normx );

        //img2(x,y,p) = vil_bilin_interp_safe<float>( img1, wx, wy, p );
      }
    } //x
  } //y

}

//---------------------------------------------------------------------
bool baml_warp_via_ground_plane(
  const vil_image_view<vxl_uint_16>& img1,
  const vpgl_camera<double>& cam1,
  const vgl_box_2d<int>& img2_region,
  const vpgl_camera<double>& cam2,
  float z_world,
  vil_image_view<vxl_uint_16>& img2 )
{
  double nbhd = 1000.0;

  int region_width = img2_region.width();
  int region_height = img2_region.height();

  std::vector< vgl_homg_point_2d<double> > corners1(4), corners2(4);

  // Project a planar neighborhood of the origin into the images
  for( int c = 0; c < 4; c++ ){
    double x, y, u, v;
    if( c == 0 || c == 1 ) x = -nbhd;
    else x = nbhd;
    if( c == 0 || c == 3 ) y = -nbhd;
    else y = nbhd;

    cam1.project( x, y, z_world, u, v );
    corners1[c].set( u, v, 1.0 );
    cam2.project( x, y, z_world, u, v );
    corners2[c].set( u, v, 1.0 );
  }

  // Compute homography from reference to target
  vgl_h_matrix_2d<double> img1_to_img2;
  vgl_h_matrix_2d_compute_4point hc;
  hc.compute( corners1, corners2, img1_to_img2 );

  // Add translation to region
  vgl_h_matrix_2d<double> img2_to_region;
  img2_to_region.set_identity();
  img2_to_region.set_translation( -img2_region.min_x(), -img2_region.min_y() );
//std::cerr << "TEMPORARY HACK in baml_warp_via_ground_plane\n";
//img2_to_region.set_translation( -img2_region.min_x()-3, -img2_region.min_y() );
  vgl_h_matrix_2d<double> img1_to_region = img2_to_region*img1_to_img2;

  // Warp the image
  baml_warp_perspective(
    img1, img1_to_region, region_width, region_height, img2 );

  return true;
}


//----------------------------------------------------------------------
bool baml_warp_via_dem(
  const vil_image_view<vxl_uint_16>&  /*img1*/,
  const vpgl_camera<double>&  /*cam1*/,
  int  /*img2_width*/,
  int  /*img2_height*/,
  const vpgl_camera<double>&  /*cam2*/,
  const vgl_box_2d<double>&  /*bounding_box_world*/,
  const vil_image_view<float>&  /*dem*/,
  const vgl_h_matrix_2d<double>&  /*dem_to_world*/,
  vil_image_view<vxl_uint_16>&  /*img2*/,
  vil_image_view<bool>&  /*valid2*/ )
{
  // NOT YET IMPLEMENTED

  // For each pixel in dem
  //   warp four corners of pixel bounding box into both images
  //   compute homography from box1 to box2
  //   find pixels in img1 which are contained in the box1
  //   use the homography to map these pixels into box2, record coords
  //
  // Occlusion?
  return true;
}
