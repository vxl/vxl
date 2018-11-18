// This is brl/bbas/volm/pro/processes/vpgl_affine_rectify_images_process.cxx
#include <iostream>
#include <limits>
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
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_math.h>

//:
bool vpgl_affine_rectify_images_process_cons(bprb_func_process& pro)
{
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr");  // image1
  input_types.emplace_back("vpgl_camera_double_sptr");  // camera1
  input_types.emplace_back("vil_image_view_base_sptr");  // image2
  input_types.emplace_back("vpgl_camera_double_sptr");  // camera2
  input_types.emplace_back("double");    // min point x (e.g. lower left corner of a scene bbox)
  input_types.emplace_back("double");    // min point y
  input_types.emplace_back("double");    // min point z
  input_types.emplace_back("double");    // max point x (e.g. upper right corner of a scene bbox)
  input_types.emplace_back("double");    // max point y
  input_types.emplace_back("double");    // max point z
  input_types.emplace_back("unsigned");    // n_points -- randomly sample this many points form the voxel volume, e.g. 100
  input_types.emplace_back("double");    // local z plane height, supposedly the height of the ground plane, e.g. 5
                                      //  when the points are only sampled from this plane, then the output warped images are ground plane stabilized
                                      //     --> i.e. points on the ground plane are at the same location in each image

  std::vector<std::string> output_types;
  output_types.emplace_back("vil_image_view_base_sptr"); // warped image1
  output_types.emplace_back("vpgl_camera_double_sptr"); // warped camera1
  output_types.emplace_back("vil_image_view_base_sptr"); // warped image2
  output_types.emplace_back("vpgl_camera_double_sptr"); // warped camera2
  return pro.set_input_types(input_types)
      && pro.set_output_types(output_types);
}

void out_image_size(unsigned ni, unsigned nj, vnl_matrix_fixed<double, 3, 3>& H1, double& min_i, double& min_j, double& max_i, double& max_j)
{
  std::vector<vnl_vector_fixed<double, 3> > cs;
  cs.emplace_back(0,0,1);
  cs.emplace_back(ni,0,1);
  cs.emplace_back(ni,nj,1);
  cs.emplace_back(0,nj,1);

  // warp the corners, initialize with first corner
  vnl_vector_fixed<double, 3> oc = H1*cs[0];
  double ii = oc[0]/oc[2];
  double jj = oc[1]/oc[2];
  min_i = ii; min_j = jj; max_i = ii; max_j = jj;
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
  out_img.fill(std::numeric_limits<float>::quiet_NaN());
  for (unsigned i = 0; i < out_img.ni(); i++)
    for (unsigned j = 0; j < out_img.nj(); j++) {
      double ii = i + mini;
      double jj = j + minj;
      vnl_vector_fixed<double,3> v(ii, jj, 1);
      vnl_vector_fixed<double,3> wv = Hinv*v;

      float pix_in_x = wv[0] / wv[2];
      float pix_in_y = wv[1] / wv[2];
      // calculate weights and pixel values
      auto x0 = (unsigned)std::floor(pix_in_x);
      auto x1 = (unsigned)std::ceil(pix_in_x);
      float x0_weight = (float)x1 - pix_in_x;
      float x1_weight = 1.0f - (float)x0_weight;
      auto y0 = (unsigned)std::floor(pix_in_y);
      auto y1 = (unsigned)std::ceil(pix_in_y);
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
    std::cout << "vpgl_affine_rectify_images_process: The number of inputs should be 11" << std::endl;
    return false;
  }

  // get the inputs
  unsigned i = 0;
  vil_image_view_base_sptr img1_sptr = pro.get_input<vil_image_view_base_sptr>(i++);
  vpgl_camera_double_sptr cam1 = pro.get_input<vpgl_camera_double_sptr>(i++);
  vil_image_view_base_sptr img2_sptr = pro.get_input<vil_image_view_base_sptr>(i++);
  vpgl_camera_double_sptr cam2 = pro.get_input<vpgl_camera_double_sptr>(i++);
  auto min_x = pro.get_input<double>(i++);
  auto min_y = pro.get_input<double>(i++);
  auto min_z = pro.get_input<double>(i++);
  auto max_x = pro.get_input<double>(i++);
  auto max_y = pro.get_input<double>(i++);
  auto max_z = pro.get_input<double>(i++);
  auto n_points = pro.get_input<unsigned>(i++);
  auto z = pro.get_input<double>(i++);
  if (n_points <= 3) {
    n_points = 10;   // make it minimum 10 points
  }

  auto* aff_camera1 = dynamic_cast<vpgl_affine_camera<double>*> (cam1.as_pointer());
  if (!aff_camera1) {
    std::cout << pro.name() <<" :--  Input camera 1 is not an affine camera!\n";
    return false;
  }
  auto* aff_camera2 = dynamic_cast<vpgl_affine_camera<double>*> (cam2.as_pointer());
  if (!aff_camera2) {
    std::cout << pro.name() <<" :--  Input camera 2 is not an affine camera!\n";
    return false;
  }
  vil_image_view<float> img1 = *vil_convert_cast(float(), img1_sptr);
  vil_image_view<float> img2 = *vil_convert_cast(float(), img2_sptr);

  double width = max_x - min_x;
  double depth = max_y - min_y;
  double height = max_z - min_z;

  std::cout << " Using: " << n_points << " to find the affine rectification homographies!\n";
  std::cout << " w: " << width << " d: " << depth << " h: " << height << '\n';

  std::vector< vnl_vector_fixed<double, 3> > img_pts1, img_pts2;

  vnl_random rng(10);
  std::cout << " using z = " << z << " as local ground plane height and will sample points on this plane randomly! the mid point z height would be: " << 0.5*height + min_z << '\n';
  for (unsigned i = 0; i < n_points; i++) {
    vgl_point_3d<float> corner_world;
    double x = rng.drand64()*width + min_x;  // sample in local coords
    double y = rng.drand64()*depth + min_y;
    double u, v;
    cam1->project(x,y,z,u,v);
    img_pts1.emplace_back(u,v,1);
    cam2->project(x,y,z,u,v);
    img_pts2.emplace_back(u,v,1);
  }


  vpgl_affine_fundamental_matrix<double> FA;
  if (!vpgl_affine_rectification::compute_affine_f(aff_camera1,aff_camera2, FA)) {
    std::cout << pro.name() <<" :--  problems in computing an affine fundamental matrix!\n";
    return false;
  }

  vnl_matrix_fixed<double, 3, 3> H1, H2;
  if (!vpgl_affine_rectification::compute_rectification(FA, img_pts1, img_pts2, H1, H2)) {
    std::cout << pro.name() <<" :--  problems in computing an affine fundamental matrix!\n";
    return false;
  }

  // find output image sizes
  unsigned oni, onj; //, oni2, onj2;
  double mini1, minj1, mini2, minj2, maxi1, maxj1, maxi2, maxj2;
  out_image_size(img1_sptr->ni(), img1_sptr->nj(), H1, mini1, minj1, maxi1, maxj1);
  out_image_size(img2_sptr->ni(), img2_sptr->nj(), H2, mini2, minj2, maxi2, maxj2);
  double mini, minj, maxi, maxj;
  if (maxi1-mini1 < maxi2-mini2) {
    mini = mini1;
    maxi = maxi1;
  } else {
    mini = mini2;
    maxi = maxi2;
  }

  if (maxj1-minj1 < maxj2-minj2) {
    minj = minj1;
    maxj = maxj1;
  } else {
    minj = minj2;
    maxj = maxj2;
  }

  oni = (unsigned)std::ceil(std::abs(maxi-mini));
  onj = (unsigned)std::ceil(std::abs(maxj-minj));

  // warp the images bilinearly
  vil_image_view<float> out_img1(oni, onj);
  vil_image_view<float> out_img2(oni, onj);
  warp_bilinear(img1, H1, out_img1, mini, minj);
  warp_bilinear(img2, H2, out_img2, mini, minj);

  // fix H1 and H2 to map pixels of output images
  vnl_matrix_fixed<double, 3, 3> H;
  H.set_identity();
  H[0][2] = -mini;
  H[1][2] = -minj;
  H1 = H*H1;
  H2 = H*H2;

  vpgl_camera_double_sptr out_aff_camera1 = new vpgl_affine_camera<double>(H1*aff_camera1->get_matrix());
  vpgl_camera_double_sptr out_aff_camera2 = new vpgl_affine_camera<double>(H2*aff_camera2->get_matrix());

  vil_image_view_base_sptr out_img1sptr = new vil_image_view<float>(out_img1);
  pro.set_output_val<vil_image_view_base_sptr>(0, out_img1sptr);
  pro.set_output_val<vpgl_camera_double_sptr>(1, out_aff_camera1);

  vil_image_view_base_sptr out_img2sptr = new vil_image_view<float>(out_img2);
  pro.set_output_val<vil_image_view_base_sptr>(2, out_img2sptr);
  pro.set_output_val<vpgl_camera_double_sptr>(3, out_aff_camera2);

  return true;
}



bool vpgl_affine_rectify_images_process2_cons(bprb_func_process& pro)
{
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr");  // image1
  input_types.emplace_back("vpgl_camera_double_sptr");  // camera1
  input_types.emplace_back("vpgl_camera_double_sptr");  // camera1 local rational
  input_types.emplace_back("vil_image_view_base_sptr");  // image2
  input_types.emplace_back("vpgl_camera_double_sptr");  // camera2
  input_types.emplace_back("vpgl_camera_double_sptr");  // camera2 local rational
  input_types.emplace_back("double");    // min point x (e.g. lower left corner of a scene bbox)
  input_types.emplace_back("double");    // min point y
  input_types.emplace_back("double");    // min point z
  input_types.emplace_back("double");    // max point x (e.g. upper right corner of a scene bbox)
  input_types.emplace_back("double");    // max point y
  input_types.emplace_back("double");    // max point z
  input_types.emplace_back("unsigned");    // n_points -- randomly sample this many points form the local z plane, e.g. 100
  input_types.emplace_back("double");    // local z plane height, supposedly the height of the ground plane, e.g. 5
                                      //  when the points are only sampled from this plane, then the output warped images are ground plane stabilized
                                      //     --> i.e. points on the ground plane are at the same location in each image
  input_types.emplace_back("vcl_string"); // output path to write H1
  input_types.emplace_back("vcl_string"); // output path to write H2
  std::vector<std::string> output_types;
  output_types.emplace_back("vil_image_view_base_sptr"); // warped image1
  output_types.emplace_back("vpgl_camera_double_sptr"); // warped camera1
  output_types.emplace_back("vil_image_view_base_sptr"); // warped image2
  output_types.emplace_back("vpgl_camera_double_sptr"); // warped camera2
  return pro.set_input_types(input_types)
      && pro.set_output_types(output_types);
}


//: Execute the process
bool vpgl_affine_rectify_images_process2(bprb_func_process& pro)
{
  if (pro.n_inputs() < 16) {
    std::cout << "vpgl_affine_rectify_images_process: The number of inputs should be 11" << std::endl;
    return false;
  }

  // get the inputs
  unsigned i = 0;
  vil_image_view_base_sptr img1_sptr = pro.get_input<vil_image_view_base_sptr>(i++);
  vpgl_camera_double_sptr cam1 = pro.get_input<vpgl_camera_double_sptr>(i++);
  vpgl_camera_double_sptr cam1_rational = pro.get_input<vpgl_camera_double_sptr>(i++);
  vil_image_view_base_sptr img2_sptr = pro.get_input<vil_image_view_base_sptr>(i++);
  vpgl_camera_double_sptr cam2 = pro.get_input<vpgl_camera_double_sptr>(i++);
  vpgl_camera_double_sptr cam2_rational = pro.get_input<vpgl_camera_double_sptr>(i++);
  auto min_x = pro.get_input<double>(i++);
  auto min_y = pro.get_input<double>(i++);
  auto min_z = pro.get_input<double>(i++);
  auto max_x = pro.get_input<double>(i++);
  auto max_y = pro.get_input<double>(i++);
  auto max_z = pro.get_input<double>(i++);
  auto n_points = pro.get_input<unsigned>(i++);
  auto z = pro.get_input<double>(i++);
  std::string output_path_H1 = pro.get_input<std::string>(i++);
  std::string output_path_H2 = pro.get_input<std::string>(i++);
  if (n_points <= 3) {
    n_points = 10;   // make it minimum 10 points
  }

  auto* aff_camera1 = dynamic_cast<vpgl_affine_camera<double>*> (cam1.as_pointer());
  if (!aff_camera1) {
    std::cout << pro.name() <<" :--  Input camera 1 is not an affine camera!\n";
    return false;
  }
  auto* aff_camera2 = dynamic_cast<vpgl_affine_camera<double>*> (cam2.as_pointer());
  if (!aff_camera2) {
    std::cout << pro.name() <<" :--  Input camera 2 is not an affine camera!\n";
    return false;
  }
  vil_image_view<float> img1 = *vil_convert_cast(float(), img1_sptr);
  vil_image_view<float> img2 = *vil_convert_cast(float(), img2_sptr);

  double width = max_x - min_x;
  double depth = max_y - min_y;
  double height = max_z - min_z;

  std::cout << " Using: " << n_points << " to find the affine rectification homographies!\n";
  std::cout << " w: " << width << " d: " << depth << " h: " << height << '\n';

  std::vector< vnl_vector_fixed<double, 3> > img_pts1, img_pts2;

  vnl_random rng;

  std::cout << " using z = " << z << " as local ground plane height and will sample points on this plane randomly! the mid point z height would be: " << 0.5*height + min_z << '\n';
  for (unsigned i = 0; i < n_points; i++) {
    vgl_point_3d<float> corner_world;
    double x = rng.drand64()*width + min_x;  // sample in local coords
    double y = rng.drand64()*depth + min_y;
    double u, v;
    cam1_rational->project(x,y,z,u,v);
    img_pts1.emplace_back(u,v,1);
    cam2_rational->project(x,y,z,u,v);
    img_pts2.emplace_back(u,v,1);
  }

  vpgl_affine_fundamental_matrix<double> FA;
  if (!vpgl_affine_rectification::compute_affine_f(aff_camera1,aff_camera2, FA)) {
    std::cout << pro.name() <<" :--  problems in computing an affine fundamental matrix!\n";
    return false;
  }

  vnl_matrix_fixed<double, 3, 3> H1, H2;
  if (!vpgl_affine_rectification::compute_rectification(FA, img_pts1, img_pts2, H1, H2)) {
    std::cout << pro.name() <<" :--  problems in computing an affine fundamental matrix!\n";
    return false;
  }


  // find output image sizes
  unsigned oni, onj; //, oni2, onj2;
  double mini1, minj1, mini2, minj2, maxi1, maxj1, maxi2, maxj2;
  out_image_size(img1_sptr->ni(), img1_sptr->nj(), H1, mini1, minj1, maxi1, maxj1);
  out_image_size(img2_sptr->ni(), img2_sptr->nj(), H2, mini2, minj2, maxi2, maxj2);
  double mini, minj, maxi, maxj;
  if (maxi1-mini1 < maxi2-mini2) {
    mini = mini1;
    maxi = maxi1;
  } else {
    mini = mini2;
    maxi = maxi2;
  }

  if (maxj1-minj1 < maxj2-minj2) {
    minj = minj1;
    maxj = maxj1;
  } else {
    minj = minj2;
    maxj = maxj2;
  }

  oni = (unsigned)std::ceil(std::abs(maxi-mini));
  onj = (unsigned)std::ceil(std::abs(maxj-minj));

  // warp the images bilinearly
  vil_image_view<float> out_img1(oni, onj);
  vil_image_view<float> out_img2(oni, onj);
  warp_bilinear(img1, H1, out_img1, mini, minj);
  warp_bilinear(img2, H2, out_img2, mini, minj);

  // fix H1 and H2 to map pixels of output images
  vnl_matrix_fixed<double, 3, 3> H;
  H.set_identity();
  H[0][2] = -mini;
  H[1][2] = -minj;
  H1 = H*H1;
  H2 = H*H2;

  out_image_size(img1_sptr->ni(), img1_sptr->nj(), H1, mini1, minj1, maxi1, maxj1);

  vpgl_camera_double_sptr out_aff_camera1 = new vpgl_affine_camera<double>(H1*aff_camera1->get_matrix());
  vpgl_camera_double_sptr out_aff_camera2 = new vpgl_affine_camera<double>(H2*aff_camera2->get_matrix());

  vil_image_view_base_sptr out_img1sptr = new vil_image_view<float>(out_img1);
  pro.set_output_val<vil_image_view_base_sptr>(0, out_img1sptr);
  pro.set_output_val<vpgl_camera_double_sptr>(1, out_aff_camera1);

  vil_image_view_base_sptr out_img2sptr = new vil_image_view<float>(out_img2);
  pro.set_output_val<vil_image_view_base_sptr>(2, out_img2sptr);
  pro.set_output_val<vpgl_camera_double_sptr>(3, out_aff_camera2);

  std::ofstream ofs(output_path_H1.c_str());
  ofs << H1;
  ofs.close();
  std::ofstream ofs2(output_path_H2.c_str());
  ofs2 << H2;
  ofs2.close();

  return true;
}


// process to compute the affine fundamental matrix from two affine cameras
bool vpgl_affine_f_matrix_process_cons(bprb_func_process& pro)
{
  std::vector<std::string> input_types;
  input_types.emplace_back("vpgl_camera_double_sptr");  // camera1
  input_types.emplace_back("vpgl_camera_double_sptr");  // camera2
  input_types.emplace_back("vcl_string"); // output path to write fundamental matrix
  std::vector<std::string> output_types;
  return pro.set_input_types(input_types)
      && pro.set_output_types(output_types);
}


//: Execute the process
bool vpgl_affine_f_matrix_process(bprb_func_process& pro)
{
  if (pro.n_inputs() < 3) {
    std::cout << "vpgl_affine_rectify_images_process: The number of inputs should be 3" << std::endl;
    return false;
  }

  // get the inputs
  unsigned i = 0;
  vpgl_camera_double_sptr cam1 = pro.get_input<vpgl_camera_double_sptr>(i++);
  vpgl_camera_double_sptr cam2 = pro.get_input<vpgl_camera_double_sptr>(i++);
  std::string output_path = pro.get_input<std::string>(i++);

  auto* aff_camera1 = dynamic_cast<vpgl_affine_camera<double>*> (cam1.as_pointer());
  if (!aff_camera1) {
    std::cout << pro.name() <<" :--  Input camera 1 is not an affine camera!\n";
    return false;
  }
  auto* aff_camera2 = dynamic_cast<vpgl_affine_camera<double>*> (cam2.as_pointer());
  if (!aff_camera2) {
    std::cout << pro.name() <<" :--  Input camera 2 is not an affine camera!\n";
    return false;
  }

  vpgl_affine_fundamental_matrix<double> FA;
  if (!vpgl_affine_rectification::compute_affine_f(aff_camera1,aff_camera2, FA)) {
    std::cout << pro.name() <<" :--  problems in computing an affine fundamental matrix!\n";
    return false;
  }

  std::ofstream ofs(output_path.c_str());
  ofs << FA;
  ofs.close();
  return true;
}


// input the disparity map given by stereo matching of rectified image pairs, disparity map is for H1 warped image1
// output an ortograhic height map using the input bounding box
bool vpgl_construct_height_map_process_cons(bprb_func_process& pro)
{
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr");  // image1
  input_types.emplace_back("vpgl_camera_double_sptr");  // camera1 local rational
  input_types.emplace_back("vcl_string");
  input_types.emplace_back("float");
  input_types.emplace_back("vil_image_view_base_sptr");  // image2
  input_types.emplace_back("vpgl_camera_double_sptr");  // camera2 local rational
  input_types.emplace_back("double");    // min point x (e.g. lower left corner of a scene bbox)
  input_types.emplace_back("double");    // min point y
  input_types.emplace_back("double");    // min point z
  input_types.emplace_back("double");    // max point x (e.g. upper right corner of a scene bbox)
  input_types.emplace_back("double");    // max point y
  input_types.emplace_back("double");    // max point z
  input_types.emplace_back("double");    // voxel size (e.g. 1 m for a geo-registered ortho map with GSD 1 meter
  input_types.emplace_back("vcl_string"); // input path to read H1
  input_types.emplace_back("vcl_string"); // input path to read H2
  std::vector<std::string> output_types;
  output_types.emplace_back("vil_image_view_base_sptr"); // orthographic height map
  output_types.emplace_back("vil_image_view_base_sptr");  // disparity map for image1, print the txt input as an image
  return pro.set_input_types(input_types)
      && pro.set_output_types(output_types);
}

#include <vil/vil_math.h>
#include <vil/vil_save.h>

//: Execute the process
bool vpgl_construct_height_map_process(bprb_func_process& pro)
{
  if (pro.n_inputs() < 14) {
    std::cout << "vpgl_affine_rectify_images_process: The number of inputs should be 11" << std::endl;
    return false;
  }

  // get the inputs
  unsigned i = 0;
  vil_image_view_base_sptr img1_sptr = pro.get_input<vil_image_view_base_sptr>(i++);
  vpgl_camera_double_sptr cam1_rational = pro.get_input<vpgl_camera_double_sptr>(i++);
  //vil_image_view_base_sptr img1_disp_sptr = pro.get_input<vil_image_view_base_sptr>(i++);
  std::string disp_name = pro.get_input<std::string>(i++);
  auto min_disparity = pro.get_input<float>(i++);
  vil_image_view_base_sptr img2_sptr = pro.get_input<vil_image_view_base_sptr>(i++);
  vpgl_camera_double_sptr cam2_rational = pro.get_input<vpgl_camera_double_sptr>(i++);
  auto min_x = pro.get_input<double>(i++);
  auto min_y = pro.get_input<double>(i++);
  auto min_z = pro.get_input<double>(i++);
  auto max_x = pro.get_input<double>(i++);
  auto max_y = pro.get_input<double>(i++);
  auto max_z = pro.get_input<double>(i++);
  auto voxel_size = pro.get_input<double>(i++);
  std::string path_H1 = pro.get_input<std::string>(i++);
  std::string path_H2 = pro.get_input<std::string>(i++);

  vnl_matrix_fixed<double, 3, 3> H1, H2;
  std::ifstream ifs(path_H1.c_str());
  ifs >> H1;
  ifs.close();
  std::ifstream ifs2(path_H2.c_str());
  ifs2 >> H2;
  ifs2.close();
  std::cout << "read H1:\n " << H1 << "\n H2:\n " << H2 << "\n";

  vil_image_view<float> img1 = *vil_convert_cast(float(), img1_sptr);

  std::ifstream ifsd(disp_name.c_str());
  if (!ifsd) {
    std::cerr << "In vpgl_construct_height_map_process() -- cannot open disparity file: " << disp_name << std::endl;
    return false;
  }
  unsigned ni, nj;
  ifsd >> nj; ifsd >> ni;
  vil_image_view<float> img1_disp(ni, nj);
  for (unsigned j = 0; j < nj; j++) {
    for (unsigned i = 0; i < ni; i++) {
      float val;
      ifsd >> val;
      img1_disp(i,j) = val;
    }
  }
  ifsd.close();

  vil_image_view<float> img2 = *vil_convert_cast(float(), img2_sptr);
  double width = max_x-min_x;
  double depth = max_y-min_y;
  double height = max_z - min_z;
  auto img_size_x = (unsigned)std::ceil(width/voxel_size);
  auto img_size_y = (unsigned)std::ceil(depth/voxel_size);
  vil_image_view<float> out_map(img_size_x, img_size_y);
  out_map.fill((float)min_z);

  for (double x = 0; x < width; x += voxel_size)
    for (double y = 0; y < depth; y += voxel_size) {
      // try each height
      double min_dif = 2.0;  // 2 pixels error in projection
      double best_z = min_z;
      for (double z = min_z; z < height; z += (voxel_size/2.0)) {
        // project this x,y,z using the camera onto the images
        double u1,v1,u2,v2;
        cam1_rational->project(x+min_x, max_y-y, z, u1, v1);
        //cam1_rational->project(x/2.0+min_x, max_y-y/2.0, z, u1, v1);
        cam2_rational->project(x+min_x, max_y-y, z, u2, v2);
        //cam2_rational->project(x/2.0+min_x, max_y-y/2.0, z, u2, v2);

        // warp this point with H1, H2
        vnl_vector_fixed<double,3> p1(u1, v1, 1);
        vnl_vector_fixed<double,3> p1w = H1*p1;
        auto u1w = (unsigned)std::floor((p1w[0]/p1w[2])+0.5);
        auto v1w = (unsigned)std::floor((p1w[1]/p1w[2])+0.5);

        if (u1w >= img1_disp.ni() || v1w >= img1_disp.nj())
          continue;

        float disp = img1_disp(u1w, v1w);
        if (disp < min_disparity)
          continue;

        vnl_vector_fixed<double,3> p2(u2, v2, 1);
        vnl_vector_fixed<double,3> p2w = H2*p2;
        int u2w = (int)std::floor((p2w[0]/p2w[2])+0.5);
        int v2w = (int)std::floor((p2w[1]/p2w[2])+0.5);

        // check if with disparity the warped pixels are exactly the same, i.e. (u1w-d,v1w) = (u2w,v2w)
        double dif = std::sqrt((u1w-disp-u2w)*(u1w-disp-u2w) + (v1w-v2w)*(v1w-v2w));
        if (dif < min_dif) {
          min_dif = dif;
          best_z = z;
        }
      }
      auto xx = (unsigned)std::floor(x/voxel_size+0.5);
      auto yy = (unsigned)std::floor(y/voxel_size+0.5);
      if (xx < img_size_x && yy < img_size_y)
        out_map(xx,yy) = (float)best_z;
    }

  vil_image_view_base_sptr out_sptr = new vil_image_view<float>(out_map);
  pro.set_output_val<vil_image_view_base_sptr>(0, out_sptr);
  pro.set_output_val<vil_image_view_base_sptr>(1, new vil_image_view<float>(img1_disp));
  return true;
}

#include <vpgl/file_formats/vpgl_geo_camera.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>

// input a height map for an ortho scene with min, max 3D points, construct a disparity map for image1
// output an ortograhic height map using the input bounding box
bool vpgl_construct_disparity_map_process_cons(bprb_func_process& pro)
{
  std::vector<std::string> input_types;
  input_types.emplace_back("vpgl_camera_double_sptr");  // camera1 local rational
  input_types.emplace_back("vpgl_camera_double_sptr");  // camera2 local rational
  input_types.emplace_back("vil_image_view_base_sptr");  // ortho height map (should be created for this scene with 3D corner points supplied as input)
  input_types.emplace_back("vpgl_camera_double_sptr"); // camera for ortho height map
  input_types.emplace_back("unsigned");  // ni for rectified version of image 1
  input_types.emplace_back("unsigned");  // nj for rectified version of image 1
  input_types.emplace_back("double"); // GSD of DEM
  input_types.emplace_back("double"); // rough GSD for rectified version of image 1 , a downsampling factor will be computed from the two GSDs
  input_types.emplace_back("vcl_string"); // input path to read H1 : should change these to vectors passed from python instead of read from files
  input_types.emplace_back("vcl_string"); // input path to read H2
  std::vector<std::string> output_types;
  output_types.emplace_back("vil_image_view_base_sptr"); // disparity map for image1 at the resolution of DEM, this needs to be resized accordingly
  return pro.set_input_types(input_types)
      && pro.set_output_types(output_types);
}


//: Execute the process
bool vpgl_construct_disparity_map_process(bprb_func_process& pro)
{
  if (pro.n_inputs() < 8) {
    std::cout << "vpgl_construct_disparity_map_process: The number of inputs should be 14" << std::endl;
    return false;
  }

  // get the inputs
  unsigned i = 0;
  vpgl_camera_double_sptr cam1_rational = pro.get_input<vpgl_camera_double_sptr>(i++);
  vpgl_camera_double_sptr cam2_rational = pro.get_input<vpgl_camera_double_sptr>(i++);
  vil_image_view_base_sptr coarse_DEM_sptr = pro.get_input<vil_image_view_base_sptr>(i++);

  vpgl_camera_double_sptr cam = pro.get_input<vpgl_camera_double_sptr>(i++);
  auto* geocam = dynamic_cast<vpgl_geo_camera*>(cam.ptr());
  auto *cam1 = dynamic_cast<vpgl_local_rational_camera<double>*>(cam1_rational.as_pointer());  // these are actually local rational cameras but we want to bypass lvcs
                                                                                                                 //  and directly work in global (lat, lon) coordinate system
  auto *cam2 = dynamic_cast<vpgl_local_rational_camera<double>*>(cam2_rational.as_pointer());  // these are actually local rational cameras but we want to bypass lvcs
                                                                                                                 //  and directly work in global (lat, lon) coordinate system
  // somehow casting to vpgl_rational_camera doesn't work, try creating new rational_camera objects
  vpgl_rational_camera<double> cam1_global(*cam1);
  vpgl_rational_camera<double> cam2_global(*cam2);

  auto out_ni = pro.get_input<unsigned>(i++);
  auto out_nj = pro.get_input<unsigned>(i++);
  auto GSD_DEM = pro.get_input<double>(i++);
  auto GSD_img = pro.get_input<double>(i++);
  std::string path_H1 = pro.get_input<std::string>(i++);
  std::string path_H2 = pro.get_input<std::string>(i++);

  double factor = GSD_DEM/GSD_img;
  int out_ni2 = (int) std::floor(((double)out_ni / factor) + 0.5);
  int out_nj2 = (int) std::floor(((double)out_nj / factor) + 0.5);
  std::cout << "ni, nj: " << out_ni << ", " << out_nj << '\n';
  std::cout << "new ni, nj: " << out_ni2 << ", " << out_nj2 << '\n';

  //out_ni2 = out_ni;
  //out_nj2 = out_nj;
  //int nn_size = 60;
  int nn_size = 2;

  vil_image_view<float> out_disparity_map(out_ni2, out_nj2);
  out_disparity_map.fill(std::numeric_limits<float>::quiet_NaN());
  vil_image_view<bool> out_disparity_map_filled(out_ni2, out_nj2);
  out_disparity_map_filled.fill(false);

  vnl_matrix_fixed<double, 3, 3> H1, H2;
  std::ifstream ifs(path_H1.c_str());
  ifs >> H1;
  ifs.close();
  std::ifstream ifs2(path_H2.c_str());
  ifs2 >> H2;
  ifs2.close();
  std::cout << "read H1:\n " << H1 << "\n H2:\n " << H2 << "\n";

  vil_image_view<float> DEM_img = *vil_convert_cast(float(), coarse_DEM_sptr);

  for (unsigned xx = 0; xx < DEM_img.ni(); xx++)
    for (unsigned yy = 0; yy < DEM_img.nj(); yy++) {

      double lon, lat;
      geocam->img_to_global(xx, yy, lon, lat);
      double z = DEM_img(xx,yy);

      // project this x,y,z using the camera onto the images
      double u1,v1,u2,v2;
      //cam1_rational->project(x, y, z, u1, v1);
      //cam2_rational->project(x, y, z, u2, v2);
      cam1_global.project(lon, lat, z, u1, v1);
      cam2_global.project(lon, lat, z, u2, v2);

      // warp this point with H1, H2
      vnl_vector_fixed<double,3> p1(u1, v1, 1);
      vnl_vector_fixed<double,3> p1w = H1*p1;
      int u1w = (int)std::floor((p1w[0]/p1w[2])+0.5);
      int v1w = (int)std::floor((p1w[1]/p1w[2])+0.5);

      if (u1w >= out_ni || v1w >= out_nj || u1w < 0 || v1w < 0)
        continue;

      vnl_vector_fixed<double,3> p2(u2, v2, 1);
      vnl_vector_fixed<double,3> p2w = H2*p2;
      int u2w = (int)std::floor((p2w[0]/p2w[2])+0.5);
      int v2w = (int)std::floor((p2w[1]/p2w[2])+0.5);

      // now the disparity equation is u1w-d = u2w --> d = u1w - u2w
      int disp = u1w - u2w;

      int down_u1w = std::floor(double(u1w)/factor);
      int down_v1w = std::floor(double(v1w)/factor);
      if (down_u1w < 0 || down_v1w < 0 || down_u1w >= out_ni2 || down_v1w >= out_nj2)
        continue;
      out_disparity_map(down_u1w, down_v1w) = (float)disp;
      out_disparity_map_filled(down_u1w, down_v1w) = true;

      //out_disparity_map(u1w, v1w) = (float)disp;
      //out_disparity_map_filled(u1w, v1w) = true;

    }

  // interpolate the intensities
  for (int i = 0; i < out_ni2; i++)
    for (int j = 0; j < out_nj2; j++) {
      if (out_disparity_map_filled(i,j) == false) {
        // locate the 4 nearest neighbors of this pixel with some value
        std::map<double, double > nns;
        vgl_point_2d<int> this_p(i,j);
        for (int ii = i-nn_size; ii < i+nn_size; ii++)
          for (int jj = j - nn_size; jj < j+nn_size; jj++) {
            if (ii < 0 || jj < 0 || ii >= out_ni2 || jj >= out_nj2 || !out_disparity_map_filled(ii,jj))
              continue;
            vgl_point_2d<int> p(ii,jj);
            double d = (this_p-p).length();
            nns[d] = out_disparity_map(ii,jj);
          }
        if (nns.size() > 1) {
          double total_weight = 0.0;
          double disp = 0.0;
          // turn distances into weights: inverse distance weighting
          for (auto & nn : nns) {
            total_weight += (1.0/nn.first);
            disp += (1.0/nn.first)*nn.second;
          }
          disp = disp/total_weight;
          out_disparity_map(i,j) = disp;
        }

      }
    }

  vil_image_view_base_sptr out_sptr = new vil_image_view<float>(out_disparity_map);
  pro.set_output_val<vil_image_view_base_sptr>(0, out_sptr);
  return true;
}
