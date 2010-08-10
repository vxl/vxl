#include "breg3d_gdbicp_homography_generator.h"

#include <vcl_string.h>
#include <vcl_sstream.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>

#include <vcl_cstdlib.h> // for "system" function

#include <vul/vul_file.h>
#include <vnl/vnl_matrix.h>
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>
#include <vil/vil_convert.h>
#include <vimt/vimt_transform_2d.h>

#include <vnl/vnl_matrix.h>
#include <vnl/vnl_matrix_fixed.h>

vimt_transform_2d breg3d_gdbicp_homography_generator::compute_homography()
{
  // this command must be in path.
  vcl_string gdbicp_command("gdbicp");

  // create local dir for i/o
  vcl_string io_dirname("./temp_gdbicp_io");
  if(!vul_file::exists(io_dirname)) {
    vcl_cout << "creating gdbicp io directory " << io_dirname << vcl_endl;
    if(!vul_file::make_directory(io_dirname)) {
      vcl_cerr << "error creating directory!" << vcl_endl;
      return vimt_transform_2d();
    }
  }
  // move to io directory
  vcl_string og_dir = vul_file::get_cwd();
  vul_file::change_directory(io_dirname);

  // create input files
  vcl_string img0_fname("img0.tiff");
  vcl_string img1_fname("img1.tiff");
  vcl_string mask0_fname("mask0.tiff");
  vcl_string mask1_fname("mask1.tiff");

  // save image0
  vil_image_view<vxl_byte> img0_byte(img0_->ni(),img0_->nj());
  vil_convert_stretch_range_limited<float>(*img0_,img0_byte,0.0f,255.0f);
  vil_save(img0_byte,img0_fname.c_str());

  // save image1
  vil_image_view<vxl_byte> img1_byte(img1_->ni(),img1_->nj());
  vil_convert_stretch_range_limited<float>(*img1_,img1_byte,0.0f,255.0f);
  vil_save(img1_byte,img1_fname.c_str());

  if (use_mask0_) {
    // save mask0
    vil_image_view<vxl_byte> mask0_byte(mask0_->ni(),mask0_->nj());
    vil_convert_stretch_range_limited<float>(*mask0_,mask0_byte,0.0f,1.0f);
    vil_save(mask0_byte,mask0_fname.c_str());
  }
  if (use_mask1_) {
    // save mask1
    vil_image_view<vxl_byte> mask1_byte(mask1_->ni(),mask1_->nj());
    vil_convert_stretch_range_limited<float>(*mask1_,mask1_byte,0.0f,1.0f);
    vil_save(mask1_byte,mask1_fname.c_str());
  }
  vcl_stringstream command;
  command << gdbicp_command << " " << img0_fname << " " << img1_fname << " ";
  if (use_mask0_) {
    command << "-mask_from " << mask0_fname << " ";
  }
  if (use_mask1_) {
    command << "-mask_to " << mask1_fname << " ";
  }
  if (compute_projective_) {
    command << "-model 1 ";
  } else {
    command << "-model 0 ";
  }
  //command << "-no_render";

  vcl_cout << "running " << command.str() << vcl_endl;
  int retval = vcl_system(command.str().c_str());
  if (retval) {
    vcl_cerr << "vcl_system(" << command << ") returned " << retval << "." << vcl_endl;
    vcl_cerr << "  make sure " << gdbicp_command << " is in your path." << vcl_endl;
    vul_file::change_directory(og_dir);
    return vimt_transform_2d();
  }

  // read in output file
  vcl_string output_fname("mosaic_" + vul_file::strip_extension(img0_fname) +"_to_" + vul_file::strip_extension(img1_fname) + ".xform");
  if (!vul_file::exists(output_fname)) {
    vcl_cerr << "error: output file " << output_fname << " does not exist!" << vcl_endl;
    vul_file::change_directory(og_dir);
    return vimt_transform_2d();
  }
  vimt_transform_2d xform = parse_gdbicp_output(output_fname);

  vul_file::change_directory(og_dir);
  return xform;
}

vimt_transform_2d breg3d_gdbicp_homography_generator::parse_gdbicp_output(vcl_string filename)
{
  vcl_ifstream ifs(filename.c_str());

  char curr_char = 0;
  while(curr_char != '<' && ifs.good()) {
    ifs.read(&curr_char,1);
  }
  vcl_string htype;
  ifs >> htype;
  vcl_cout << "transformation type = " << htype << vcl_endl;
  unsigned hdim = 0;
  ifs >> hdim;
  vcl_cout << "transformation dimension = " << hdim << vcl_endl;

  vnl_matrix_fixed<double,3,3> H_centered;
  double cx,cy,dx,dy;
  if (compute_projective_) {
    // full projective transformation
    ifs >> H_centered;
    vcl_cout << "H_centered = " << vcl_endl << H_centered << vcl_endl;
    ifs >> cx >> cy >> dx >> dy;
  }else {
    // affine transformation
    vnl_matrix_fixed<double,2,2> A;
    ifs >> A;
    vcl_cout << "A = " << vcl_endl << A << vcl_endl;
    H_centered.fill(0.0);
    H_centered.update(A,0,0);
    H_centered(2,2) = 1.0;
    vcl_cout << "H_centered = " << vcl_endl << H_centered << vcl_endl;
    ifs >> dx >> dy >> cx >> cy;
  }
  vcl_cout << "cx=" << cx << " cy=" << cy << " dx=" << dx << " dy=" << dy << vcl_endl;
  vnl_matrix_fixed<double,3,3> C,D;
  C.set_identity();
  D.set_identity();
  C(0,2) = -cx;
  C(1,2) = -cy;
  D(0,2) = dx;
  D(1,2) = dy;
  vnl_matrix_fixed<double,3,3> H = D*H_centered*C;
  vcl_cout << "H = " << H << vcl_endl;

  vimt_transform_2d xform;
  if (compute_projective_) {
    xform.set_projective(H);
  }
  else {
    vnl_matrix_fixed<double,2,3> H_affine;
    for (unsigned i=0; i<2; ++i) {
      for (unsigned j=0; j<3; ++j) {
        H_affine(i,j) = H(i,j);
      }
    }
    xform.set_affine(H_affine);
  }
  return xform;
}
