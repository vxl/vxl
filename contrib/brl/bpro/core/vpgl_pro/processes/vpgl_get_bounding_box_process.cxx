// This is brl/bpro/core/vpgl_pro/processes/vpgl_get_bounding_box_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_intersection.h>
#include <vsph/vsph_camera_bounds.h>
#include <vgl/vgl_polygon_scan_iterator.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_box_2d.h>
#include <vsl/vsl_binary_io.h>
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>

//: initialization
bool vpgl_get_bounding_box_process_cons(bprb_func_process& pro)
{
  //this process takes one input and has no output:
  std::vector<std::string> input_types;
  input_types.emplace_back("vcl_string");    //directory of perspective cameras
  std::vector<std::string> output_types;
  return pro.set_input_types(input_types)
      && pro.set_output_types(output_types);
}

//: Execute the process
bool vpgl_get_bounding_box_process(bprb_func_process& pro)
{
  if (pro.n_inputs()<1) {
    std::cout << "vpgl_get_bounding_box_process: The number of inputs should be 1 (a string)" << std::endl;
    return false;
  }

  // get the inputs
  int i=0;
  std::string cam_dir = pro.get_input<std::string>(i);
  float      zplane  = 0.0;

  //populate vector of cameras
  //: returns a list of cameras from specified directory
  std::vector<vpgl_perspective_camera<double> > cams = cameras_from_directory(cam_dir, 0.0);
  std::cout << "found " << cams.size() << " cameras in the directory..\n";

  //run planar bounding box
  vgl_box_2d<double> bbox;
  bool good = vsph_camera_bounds::planar_bounding_box(cams,bbox,zplane);
  if (good)
    std::cout<<"Bounding box found: "<<bbox<<std::endl;
  else
    std::cout<<"Bounding box not found."<<std::endl;


  //---------------------------------------------------------------------------
  //create zplane count map
  //---------------------------------------------------------------------------
  //determine the resolution of a pixel on the z plane
  vpgl_perspective_camera<double> cam = cams[0];
  vgl_point_2d<double> pp = (cam.get_calibration()).principal_point();
  vgl_ray_3d<double> cone_axis;
  double cone_half_angle, solid_angle;
  vsph_camera_bounds::pixel_solid_angle(cam, int(pp.x()/4), int(pp.y()/4), cone_axis, cone_half_angle, solid_angle);
  vgl_point_3d<double> cc = cam.camera_center();
  vgl_point_3d<double> zc( bbox.centroid().x(), bbox.centroid().y(), zplane);
  double res = 2*(cc-zc).length()*cone_half_angle;

  //create an image with this res, and count each pixel
  auto ni = (unsigned) (bbox.width()/res);
  auto nj = (unsigned) (bbox.height()/res);
  vil_image_view<vxl_byte> cntimg(ni, nj);
  std::cout<<"Created Box size: "<<ni<<','<<nj<<std::endl;
  for (const auto & i : cams)
  {
    //project the four corners to the ground plane
    cam = i;
    vgl_ray_3d<double> ul = cam.backproject(0.0, 0.0);
    vgl_ray_3d<double> ur = cam.backproject(2*pp.x(), 0.0);
    vgl_ray_3d<double> bl = cam.backproject(0.0, 2*pp.y());
    vgl_ray_3d<double> br = cam.backproject(2*pp.x(), 2*pp.y());

    //define z plane
    vgl_plane_3d<double> zp( vgl_point_3d<double>( 1.0,  1.0, zplane),
                             vgl_point_3d<double>( 1.0, -1.0, zplane),
                             vgl_point_3d<double>(-1.0,  1.0, zplane) );

    //intersect each ray with z plane
    vgl_point_3d<double> ulp, urp, blp, brp;
    bool good =    vgl_intersection(ul, zp, ulp);
    good = good && vgl_intersection(ur, zp, urp);
    good = good && vgl_intersection(bl, zp, blp);
    good = good && vgl_intersection(br, zp, brp);
    if (!good) {
        std::cout << "ERROR: lines do not intersect" << __FILE__ << __LINE__ << std::endl;
    }

    //convert the four corners into image coordinates
    typedef vgl_polygon<double>::point_t        Point_type;
    typedef vgl_polygon<double>                 Polygon_type;
    typedef vgl_polygon_scan_iterator<double>   Polygon_scan;
    Polygon_type poly;
    poly.new_sheet();
    poly.push_back( Point_type( (ulp.x()-bbox.min_x())/res, (ulp.y()-bbox.min_y())/res ) );
    poly.push_back( Point_type( (urp.x()-bbox.min_x())/res, (urp.y()-bbox.min_y())/res ) );
    poly.push_back( Point_type( (blp.x()-bbox.min_x())/res, (blp.y()-bbox.min_y())/res ) );
    poly.push_back( Point_type( (brp.x()-bbox.min_x())/res, (brp.y()-bbox.min_y())/res ) );

    // There will be scan lines at y=0, 1 and 2.
    Polygon_scan it(poly, false);
    int y=0;
    for (it.reset(); it.next(); ++y) {
      int y = it.scany();
      for (int x = it.startx(); x <= it.endx(); ++x) {
        int yy = nj-y;
        if (x>=0 && (unsigned)x<ni && yy>=0 && (unsigned)yy<nj) {
          cntimg(x, yy) += (vxl_byte) 1;
        }
#ifdef DEBUG
        else {
          std::cout<<"X and Y in scan iterator are out of bounds: "<<x<<','<<y<<std::endl;
        }
#endif
      }
    }
  }

  //use count image to create a tighter bounding box
  vil_save(cntimg, "countImage.png");

  return good;
}
