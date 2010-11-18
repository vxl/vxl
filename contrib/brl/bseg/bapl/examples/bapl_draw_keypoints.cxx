// This is brl/bseg/bapl/examples/bapl_draw_keypoints.cxx

//:
// \file

#include <vcl_sstream.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vul/vul_arg.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_convert.h>
#include <vil/vil_new.h>
#include <bapl/bapl_lowe_keypoint.h>
#include <bapl/bapl_keypoint.h>
#include <bapl/bapl_keypoint_extractor.h>
#include <ipts/ipts_draw.h>

// Format of keypoint file if available:
// <number of keypoints> <length of descriptor, n>
// <location_x> <location_y> <scale> <orientation angle in radians>
// <vector of length n>
// ...

// e.g.
// 1303 128
// 131.77 226.6 55.01 -0.454
//  0 0 0 0 0 . . . 43 56 78 0 0 1
// .
// .
// .

int main( int argc, char* argv[] )
{
  vul_arg<vcl_string> in_path("-i","Input image");
  vul_arg<vcl_string> key_path("-keys", "Just display keypoints if keypoint file is available, otherwise will extract keys");
  vul_arg<vcl_string> out_path("-o","Output image");
  vul_arg_parse(argc, argv);

  if (!in_path.set())
    vul_arg_display_usage_and_exit();

  bool extract = true;
  if (key_path.set()) {
    extract = false;
  }

  vil_image_resource_sptr color_image_sptr = vil_load_image_resource(in_path().c_str());
  vil_image_resource_sptr grey_image_sptr;
  grey_image_sptr = vil_new_image_resource_of_view( *vil_convert_to_grey_using_rgb_weighting ( color_image_sptr->get_view() ) );
  if (grey_image_sptr->ni()==0)
  {
    vcl_cout<<"Failed to load image."<<vcl_endl;
    return 1;
  }

  vcl_vector< bapl_keypoint_sptr > keypoints;
  if (extract) {
    vcl_cout << "Finding Keypoints" << vcl_endl;
    bapl_keypoint_extractor( grey_image_sptr, keypoints);
  }
  else {
    vcl_cout << "Reading Keypoints from file: " << key_path().c_str() << vcl_endl;
    vcl_ifstream ifs(key_path().c_str());
    if (!ifs.is_open()) {
      vcl_cerr << "Failed to open file " << key_path().c_str() << '\n';
      return 0;
    }
    int n; ifs >> n; int len; ifs >> len;
    vcl_cout << "Found " << n << " keypoints.\n";
    for (int i = 0; i < n; i++) {
      float loc_x, loc_y, scale, orientation;
      ifs >> loc_x; ifs >> loc_y; ifs >> scale; ifs >> orientation;

      vnl_vector_fixed<double, 128> desc;
      for (int j = 0; j < len; j++) {
        int val;
        ifs >> val;
        desc[j] = val;
      }
      bapl_lowe_pyramid_set_sptr py;
      bapl_lowe_keypoint_sptr kp = new bapl_lowe_keypoint(py, loc_y, loc_x, scale, orientation, desc);
      keypoints.push_back(kp);
    }
    ifs.close();
  }

  vcl_cout << "Drawing Keypoints" << vcl_endl;

  vil_image_view<vxl_byte> color_img(color_image_sptr->get_view());
  for (unsigned i=0;i<keypoints.size();++i){
    bapl_lowe_keypoint_sptr kp;
    kp.vertical_cast(keypoints[i]);
    int ii = int(kp->location_i()+0.5); int jj = int(kp->location_j()+0.5);
    if (ii >= 0 && jj >= 0 && ii < (int)color_img.ni() && jj < (int)color_img.nj())
      if (kp->scale()>1.1)
        ipts_draw_cross(color_img, ii,jj,int(kp->scale()+0.5), vxl_byte(255) );
  }

  vcl_cout << "Saving the results" << vcl_endl;

  vil_save(color_img, out_path().c_str() );


  vcl_cout <<  "done!" <<vcl_endl;
  return 0;
}

