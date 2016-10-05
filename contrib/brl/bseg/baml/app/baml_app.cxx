#include <string>
#include <vector>
#include <iostream>
#include <sstream>

#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vil/vil_convert.h>
#include <vil/vil_crop.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/algo/vil_gauss_filter.h>
#include <vgl/vgl_intersection.h>

#include <vpgl/vpgl_local_rational_camera.h>
#include <vpgl/vpgl_lvcs.h>
#include <vpgl/vpgl_rational_camera.h>

#include <bil/bil_convert_to_grey.h>

#include <baml/baml_warp.h>
#include <baml/baml_detect_change.h>


int main(int argc,char * argv[])
{
  
  vpgl_lvcs lvcs( 35.1996380169, 48.6745313514, 1670.27321333 );
  vgl_box_2d<int> region( 2900, 3600, 900, 1900 );
  //vgl_box_2d<int> region( 2000, 3000, 3500, 4375 );
  //int region_length = 400;
  double z_ground = 25.0;

  /*/ Usage information
  if( argc != 6 ){
    std::cout << "Usage : baml_app.exe target_img target_cam ref_img ref_cam output_namebase\n";
    return -1;
  }

  // Read inputs
  std::string target_img_file(argv[1]);
  std::string target_cam_file(argv[2]);
  std::string ref_img_file(argv[3]);
  std::string ref_cam_file(argv[4]);
  std::string output_namebase(argv[5]);*/

  std::string target_img_file("D:/data/sattel/hamadan/20160820_073052_0c76.tif");
  std::string target_cam_file("D:/data/sattel/hamadan/20160820_073052_0c76.tif_RPC.txt");
  //std::string target_img_file("D:/data/sattel/hamadan/20160701_091758_0c64.tif");
  //std::string target_cam_file("D:/data/sattel/hamadan/20160701_091758_0c64.tif_RPC.txt");
  std::string ref_img_file("D:/data/sattel/hamadan/20160831_063745_0e0d.tif");
  std::string ref_cam_file("D:/data/sattel/hamadan/20160831_063745_0e0d.tif_RPC.txt");
  std::string output_namebase("D:/results/a");

  // Load images
  vil_image_resource_sptr target_imgr = 
    vil_load_image_resource( target_img_file.c_str() );
  vil_image_resource_sptr ref_imgr = 
    vil_load_image_resource( ref_img_file.c_str() );

  // Convert to grey using special bil function
  bil_convert_resource_to_grey cnv;
  vil_image_resource_sptr target_img_grey, ref_img_grey;
  cnv(target_imgr, target_img_grey, true );
  cnv(ref_imgr, ref_img_grey, true );
  vil_image_view<vxl_uint_16> target_img = target_img_grey->get_view();
  vil_image_view<vxl_uint_16> ref_img = ref_img_grey->get_view();

  //vil_image_view<vxl_uint_16> target_img = //vil_load( target_img_file.c_str() );
  //  vil_convert_to_grey_using_rgb_weighting( vil_load( target_img_file.c_str() ) );
  //vil_image_view<vxl_uint_16> ref_img = //vil_load( ref_img_file.c_str() );
  //  vil_convert_to_grey_using_rgb_weighting( vil_load( ref_img_file.c_str() ) );

  // Check images
  if( !(target_img.ni() > 0 && ref_img.ni() > 0) ){
    std::cerr << "Failed to load images\n";
    return 1;
  }  

  // Load cameras
  vpgl_rational_camera<double>* target_rpc = 
    read_rational_camera_from_txt<double>( target_cam_file );
  vpgl_rational_camera<double>* ref_rpc = 
    read_rational_camera_from_txt<double>( ref_cam_file );

  // Check cameras
  if( target_rpc == NULL || ref_rpc == NULL ){
    std::cerr << "Failed to load cameras\n";
    return 1;
  }  

  // Convert cameras 
  vpgl_local_rational_camera<double> target_cam( lvcs, *target_rpc );
  vpgl_local_rational_camera<double> ref_cam( lvcs, *ref_rpc );
  delete target_rpc; delete ref_rpc;

  /*/ Get the projection of the LVCS origin and build an image region around it
  Doesn't work because LVCS origin may not be in image
  double lvcs_cx, lvcs_cy;
  target_cam.project( 0.0, 0.0, 0.0, lvcs_cx, lvcs_cy );
  int rx = (int)( lvcs_cx-0.5*region_length );
  int ry = (int)( lvcs_cy-0.5*region_length );
  vgl_box_2d<int> region( rx, rx+region_length, ry, ry+region_length );
  vgl_box_2d<int> img_box( 0, target_img.ni()-1, 0, target_img.nj()-1 );
  region = vgl_intersection( img_box, region );

  if( !region.area() > 0 ){
    std::cerr << "ERROR: LVCS not contained in image\n";
    return 1;
  }*/

  // Crop the target image and blur slightly to compensate for resampling
  // in the reference image
  vil_image_view<vxl_uint_16> tar_cropped = vil_crop( target_img, 
    region.min_x(), region.width(), region.min_y(), region.height() );
  vil_image_view<vxl_uint_16> tar_blur;
  vil_gauss_filter_2d( tar_cropped, tar_blur, 1.0, 2 );

  // Warp the reference image
  vil_image_view<vxl_uint_16> ref_warped;
  vil_image_view<bool> ref_valid;
  baml_warp_via_ground_plane( 
    ref_img, ref_cam, region, 
    target_cam, z_ground, ref_warped );

  // Output the cropped images
  vil_save( tar_blur, (output_namebase + "_img1.tif").c_str() );
  vil_save( ref_warped, (output_namebase + "_img2.tif").c_str() );

  // Make valid image
  ref_valid.set_size( region.width(), region.height() );
  ref_valid.fill( true );
  for( int y = 0; y < region.height(); y++ ){
    for( int x = 0; x < region.width(); x++ ){
      if( tar_cropped(x,y)==0 || ref_warped(x,y)==0 ) ref_valid = false;
    }
  }

  // Detect changes
  vil_image_view<float> tar_prob;
  //baml_detect_change_bt( tar_blur, ref_warped, ref_valid, tar_prob );
  //baml_detect_change_census( tar_blur, ref_warped, ref_valid, tar_prob );
  baml_detect_change_mi( tar_blur, ref_warped, ref_valid, tar_prob );

  // Visualize and save
  vil_image_view<vxl_byte> change_vis;
  vil_convert_stretch_range_limited( tar_prob, change_vis, 0.0f, 1.0f );
  vil_save( change_vis, (output_namebase + "_change.tif").c_str() );
  

  return 0;
};
