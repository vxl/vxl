#include "boxm2_convert_nvm_txt.h"
#include "boxm2_point_util.h"
//:
// \file
#include <vsph/vsph_camera_bounds.h>
#include <vidl/vidl_image_list_istream.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_quaternion.h>
#include <vcl_cassert.h>
#include <vul/vul_file.h>

//: Main boxm2_convert_nvm_txt function
//  Takes in bundle.out file and image directory that created img_dir
void boxm2_util_convert_nvm_txt(vcl_string nvm_file,
                                    vcl_string img_dir,
                                    vcl_map<vcl_string, vpgl_perspective_camera<double>* >& cams,
                                    vcl_map<vcl_string, vcl_string >& img_name_mapping)
{

  boxm2_convert_nvm_txt b2s(nvm_file, img_dir);
  cams        = b2s.get_cams();
  img_name_mapping = b2s.get_img_name_mapping();
}

// reads bundler file and populates list of cameras, and a scene bounding box
boxm2_convert_nvm_txt::boxm2_convert_nvm_txt(vcl_string nvm_file, vcl_string img_dir)
{
  img_dir_ = img_dir;
  nvm_file_ = nvm_file;

  // verify image dir
  if (!vul_file::is_directory(img_dir.c_str()))
  {
    vcl_cout<<"boxm2_convert_nvm_txt::Image directory does not exist"<<vcl_endl;
    return;
  }

  // open the bundler file
  vcl_ifstream bfile( nvm_file.c_str() );
  if (!bfile)
  {
    vcl_cout<<"boxm2_convert_nvm_txt::Error Opening Bundler output file: " << nvm_file <<vcl_endl;
    return;
  }

  this->read_cameras(bfile);

  //write final mapping
  for (unsigned i = 0; i < cams_.size(); ++i) {
    vcl_string old_file_name = vul_file::strip_directory(old_names_[i]) ;
    vcl_cout << "Old file name " <<  old_file_name << vcl_endl;
    CamType* cam = new CamType(cams_[i]);
    final_cams_[old_file_name] = cam;
    img_name_map_[old_file_name] = names_[i];
  }

}

//------------------------------------------------------------------------
// reading the cameras from nvm file
//------------------------------------------------------------------------
bool boxm2_convert_nvm_txt::read_cameras(vcl_ifstream& in)
{

  int rotation_parameter_num = 4;
  vcl_string token;
  //read the header
  for(int i = 0; i < 19; i++) {
      vcl_getline(in, token); // was: in >> token; //file header
      vcl_cout << token << vcl_endl;
  }


  // read # of cameras
  int ncam = 0;
  in >> ncam;
  if (ncam <= 1) {
    vcl_cout<<"Found fewer than 1 camera in NVM file (" << ncam<<')' <<vcl_endl;
    return false;
  }
  vcl_cout<<"Found "<<ncam<<" cameras in nvm file"<<vcl_endl;


  //read the camera parameters
  cams_.resize(ncam); // allocate the camera data
  names_.resize(ncam); // allocate token data
  old_names_.resize(ncam); // allocate token data

  for (int i = 0; i < ncam; ++i)
  {
    vcl_getline(in, token); //empty line
    vcl_getline(in, token); //empty line

    vcl_string img_name,old_file_name;
    vcl_getline(in, img_name);
    vcl_getline(in, old_file_name);

    //internal param
    double f, q[9], c[3], p_x,p_y;
    in >> f;
    in >>  p_x >> p_y;


    vgl_point_2d<double> ppoint(p_x,p_y);
    vpgl_calibration_matrix<double> K(f,ppoint);

    //external
    vcl_getline(in, token);
    in >> c[0] >> c[1] >> c[2];
    for (int j = 0; j < 3; ++j) in >> q[j]; //camera center....
    vcl_getline(in, token);
    for (int j = 0; j < 3; ++j) in >> q[j]; //axis angle rot
    vcl_getline(in, token);
    for (int j = 0; j < 4; ++j) in >> q[j]; //quaternion rot
    vcl_getline(in, token);
    for (int j = 0; j < 9; ++j) in >> q[j]; //rot matrix
    vcl_getline(in, token);
    vcl_getline(in, token);

    vnl_matrix_fixed<double,3,3> r;
    r(0,0) = q[0];  r(0,1) = q[1];  r(0,2) = q[2];
    r(1,0) = q[3];  r(1,1) = q[4];  r(1,2) = q[5];
    r(2,0) = q[6];  r(2,1) = q[7];  r(2,2) = q[8];

    vgl_rotation_3d<double> rot(r);
    vgl_vector_3d<double> t(c[0],c[1],c[2]);

    vpgl_perspective_camera<double> cam(K,rot,t);
    cams_[i] = cam;
    vcl_cout << cam << vcl_endl;

    names_[i] = img_name;
    old_names_[i] = old_file_name;
  }
  return true;
}


