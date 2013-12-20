// This is brl/bbas/volm/pro/processes/vpgl_affine_rectify_images_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
//         Take 2 cams and a 3d scene bounding box to compute an affine fundamental matrix and rectification homographies
//         warp the images and their cameras using the homographies
//
//
#include <bprb/bprb_parameters.h>
#include <vil/vil_image_view.h>
#include <vpgl/vpgl_camera_double_sptr.h>
#include <vpgl/algo/vpgl_affine_rectification.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/vnl_random.h>
#include <vil/vil_convert.h>
#include <vcl_limits.h>
#include <vnl/vnl_math.h>

//:
bool vpgl_affine_rectify_images_process_cons(bprb_func_process& pro)
{
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vil_image_view_base_sptr");  // image1
  input_types.push_back("vpgl_camera_double_sptr");  // camera1
  input_types.push_back("vil_image_view_base_sptr");  // image2
  input_types.push_back("vpgl_camera_double_sptr");  // camera2
  input_types.push_back("double");    // min point x (e.g. lower left corner of a scene bbox)
  input_types.push_back("double");    // min point y
  input_types.push_back("double");    // min point z
  input_types.push_back("double");    // max point x (e.g. upper right corner of a scene bbox)
  input_types.push_back("double");    // max point y
  input_types.push_back("double");    // max point z
  input_types.push_back("unsigned");    // n_points -- randomly sample this many points form the voxel volume, e.g. 100 
  vcl_vector<vcl_string> output_types;
  output_types.push_back("vil_image_view_base_sptr"); // warped image1
  output_types.push_back("vpgl_camera_double_sptr"); // warped camera1
  output_types.push_back("vil_image_view_base_sptr"); // warped image2
  output_types.push_back("vpgl_camera_double_sptr"); // warped camera2
  return pro.set_input_types(input_types)
      && pro.set_output_types(output_types);
}

void out_image_size(unsigned ni, unsigned nj, vnl_matrix_fixed<double, 3, 3>& H1, double& min_i, double& min_j, double& max_i, double& max_j)
{
  vcl_vector<vnl_vector_fixed<double, 3> > cs;
  cs.push_back(vnl_vector_fixed<double, 3>(0,0,1));
  cs.push_back(vnl_vector_fixed<double, 3>(ni,0,1));
  cs.push_back(vnl_vector_fixed<double, 3>(ni,nj,1));
  cs.push_back(vnl_vector_fixed<double, 3>(0,nj,1));

  // warp the corners, initialize with first corner
  vnl_vector_fixed<double, 3> oc = H1*cs[0];
  double ii = oc[0]/oc[2];
  double jj = oc[1]/oc[2];
  
  min_i = ii; min_j = jj; max_i = ii; max_j = ii;  
  for (unsigned i = 1; i < 4; i++) {
    vnl_vector_fixed<double, 3> oc = H1*cs[i];
    double ii = oc[0]/oc[2];
    double jj = oc[1]/oc[2];
  
    if (ii > max_i)
      max_i = ii;
    if (jj > max_j)
      max_j = jj;
    if (ii < min_i)
      min_i = ii;
    if (jj < min_j)
      min_j = jj;
  }
}
void warp_bilinear(vil_image_view<float>& img, vnl_matrix_fixed<double, 3, 3>& H, vil_image_view<float>& out_img, double mini, double minj)
{
  // use the inverse to map output pixels to input pixels, so we can sample bilinearly from the input image
  vnl_svd<double> temp(H);  // use svd to get the inverse
  vnl_matrix_fixed<double, 3, 3> Hinv = temp.inverse();
  //out_img.fill(0.0f);
  out_img.fill(vcl_numeric_limits<float>::quiet_NaN());
  for (unsigned i = 0; i < out_img.ni(); i++) 
    for (unsigned j = 0; j < out_img.nj(); j++) {
      double ii = i + mini;
      double jj = j + minj;
      vnl_vector_fixed<double,3> v(ii, jj, 1);
      vnl_vector_fixed<double,3> wv = Hinv*v;

      float pix_in_x = wv[0] / wv[2];
      float pix_in_y = wv[1] / wv[2];
      // calculate weights and pixel values
      unsigned x0 = (unsigned)vcl_floor(pix_in_x);
      unsigned x1 = (unsigned)vcl_ceil(pix_in_x);
      float x0_weight = (float)x1 - pix_in_x;
      float x1_weight = 1.0f - (float)x0_weight;
      unsigned y0 = (unsigned)vcl_floor(pix_in_y);
      unsigned y1 = (unsigned)vcl_ceil(pix_in_y);
      float y0_weight = (float)y1 - pix_in_y;
      float y1_weight = 1.0f - (float)y0_weight;
      vnl_vector_fixed<unsigned,4>xvals(x0,x0,x1,x1);
      vnl_vector_fixed<unsigned,4>yvals(y0,y1,y0,y1);
      vnl_vector_fixed<float,4> weights(x0_weight*y0_weight,
                                        x0_weight*y1_weight,
                                        x1_weight*y0_weight,
                                        x1_weight*y1_weight);

      for (unsigned k=0; k<4; k++) {
        // check if input pixel is inbounds
        if (xvals[k] < img.ni() &&
            yvals[k] < img.nj()) {
          // pixel is good
          if (vnl_math::isnan(out_img(i,j)))
            out_img(i,j) = 0.0f;
          out_img(i,j) += img(xvals[k],yvals[k])*weights[k];
        }
      }
    }

}

//: Execute the process
bool vpgl_affine_rectify_images_process(bprb_func_process& pro)
{
  if (pro.n_inputs() < 11) {
    vcl_cout << "vpgl_affine_rectify_images_process: The number of inputs should be 11" << vcl_endl;
    return false;
  }

  // get the inputs
  unsigned i = 0;
  vil_image_view_base_sptr img1_sptr = pro.get_input<vil_image_view_base_sptr>(i++);
  vpgl_camera_double_sptr cam1 = pro.get_input<vpgl_camera_double_sptr>(i++);
  vil_image_view_base_sptr img2_sptr = pro.get_input<vil_image_view_base_sptr>(i++);
  vpgl_camera_double_sptr cam2 = pro.get_input<vpgl_camera_double_sptr>(i++);
  double min_x = pro.get_input<double>(i++);
  double min_y = pro.get_input<double>(i++);
  double min_z = pro.get_input<double>(i++);
  double max_x = pro.get_input<double>(i++);
  double max_y = pro.get_input<double>(i++);
  double max_z = pro.get_input<double>(i++);
  unsigned n_points = pro.get_input<unsigned>(i++);
  if (n_points <= 3) {
    n_points = 10;   // make it minimum 10 points
  }

  vpgl_affine_camera<double>* aff_camera1 = dynamic_cast<vpgl_affine_camera<double>*> (cam1.as_pointer());
  if (!aff_camera1) {
    vcl_cout << pro.name() <<" :--  Input camera 1 is not an affine camera!\n";
    return false;
  }
  vpgl_affine_camera<double>* aff_camera2 = dynamic_cast<vpgl_affine_camera<double>*> (cam2.as_pointer());
  if (!aff_camera2) {
    vcl_cout << pro.name() <<" :--  Input camera 2 is not an affine camera!\n";
    return false;
  }
  vil_image_view<float> img1 = *vil_convert_cast(float(), img1_sptr);
  vil_image_view<float> img2 = *vil_convert_cast(float(), img2_sptr);
  
  double width = max_x - min_x; 
  double depth = max_y - min_y; 
  double height = max_z - min_z; 

  vcl_cout << " Using: " << n_points << " to find the affine rectification homographies!\n";
  vcl_cout << " w: " << width << " d: " << depth << " h: " << height << '\n';

  vcl_vector< vnl_vector_fixed<double, 3> > img_pts1, img_pts2;

  vnl_random rng;
  for (unsigned i = 0; i < n_points; i++) {
    vgl_point_3d<float> corner_world;
    double x = rng.drand64()*width + min_x;  // sample in local coords
    double y = rng.drand64()*depth + min_y;
    double z = rng.drand64()*height + min_z;
    double u, v;
    cam1->project(x,y,z,u,v);  
    img_pts1.push_back(vnl_vector_fixed<double, 3>(u,v,1));
    cam2->project(x,y,z,u,v);  
    img_pts2.push_back(vnl_vector_fixed<double, 3>(u,v,1));
  }

  vpgl_affine_fundamental_matrix<double> FA;
  if (!vpgl_affine_rectification::compute_affine_f(aff_camera1,aff_camera2, FA)) {
    vcl_cout << pro.name() <<" :--  problems in computing an affine fundamental matrix!\n";
    return false;
  }

  vnl_matrix_fixed<double, 3, 3> H1, H2;
  if (!vpgl_affine_rectification::compute_rectification(FA, img_pts1, img_pts2, H1, H2)) {
    vcl_cout << pro.name() <<" :--  problems in computing an affine fundamental matrix!\n";
    return false;
  }
  vpgl_camera_double_sptr out_aff_camera1 = new vpgl_affine_camera<double>(H1*aff_camera1->get_matrix());
  vpgl_affine_camera<double>* cam1_ptr = dynamic_cast<vpgl_affine_camera<double>*>(out_aff_camera1.ptr());
  //vcl_cout << "out affine cam1: \n" << cam1_ptr->get_matrix();
  vpgl_camera_double_sptr out_aff_camera2 = new vpgl_affine_camera<double>(H2*aff_camera2->get_matrix());
  vpgl_affine_camera<double>* cam2_ptr = dynamic_cast<vpgl_affine_camera<double>*>(out_aff_camera2.ptr());
  //vcl_cout << "out affine cam2: \n" << cam2_ptr->get_matrix();
  
  // find output image sizes
  unsigned oni, onj; //, oni2, onj2;
  double mini1, minj1, mini2, minj2, maxi1, maxj1, maxi2, maxj2;
  out_image_size(img1_sptr->ni(), img1_sptr->nj(), H1, mini1, minj1, maxi1, maxj1);
  out_image_size(img2_sptr->ni(), img2_sptr->nj(), H2, mini2, minj2, maxi2, maxj2);
  double mini = mini1 < mini2 ? mini1 : mini2;
  double minj = minj1 < minj2 ? minj1 : minj2;
  double maxi = maxi1 > maxi2 ? maxi1 : maxi2;
  double maxj = maxj1 > maxj2 ? maxj1 : maxj2;

  oni = (unsigned)vcl_ceil(vcl_abs(maxi-mini));
  onj = (unsigned)vcl_ceil(vcl_abs(maxj-minj));
  //vcl_cout << "mini: " << mini1 << " minj: " << minj1 << " maxi: " << maxi1 << " maxj: " << maxj1 << " oni1: " << oni1 << " onj1: " << onj1 << vcl_endl;
  
  // warp the images bilinearly
  vil_image_view<float> out_img1(oni, onj);
  vil_image_view<float> out_img2(oni, onj);
  warp_bilinear(img1, H1, out_img1, mini, minj);
  warp_bilinear(img2, H2, out_img2, mini, minj);
  
  vil_image_view_base_sptr out_img1sptr = new vil_image_view<float>(out_img1);
  pro.set_output_val<vil_image_view_base_sptr>(0, out_img1sptr);
  pro.set_output_val<vpgl_camera_double_sptr>(1, out_aff_camera1);

  vil_image_view_base_sptr out_img2sptr = new vil_image_view<float>(out_img2);
  pro.set_output_val<vil_image_view_base_sptr>(2, out_img2sptr);
  pro.set_output_val<vpgl_camera_double_sptr>(3, out_aff_camera2);
  
  return true;
}

