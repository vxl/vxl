#include "bmdl_generate_mesh_process.h"
//:
// \file

#include <vcl_cassert.h>
#include <vcl_cstring.h>
#include <vcl_iostream.h>

#include <vgl/vgl_polygon.h>
#include <vgl/io/vgl_io_polygon.h>

#include <vil/vil_image_view.h>
#include <vil/vil_pixel_format.h>

#include <bprb/bprb_parameters.h>

#include <bmdl/bmdl_mesh.h>
#include <imesh/imesh_mesh.h>
#include <imesh/imesh_fileio.h>

bmdl_generate_mesh_process::bmdl_generate_mesh_process()
{
  //this process takes 1 input:
  input_data_.resize(5, brdb_value_sptr(0));
  input_types_.resize(5);

  int i=0;
  input_types_[i++] = "vcl_string";                 // file path (bin) for the polygons to be read  
  input_types_[i++] = "vil_image_view_base_sptr";   // label image
  input_types_[i++] = "vil_image_view_base_sptr";   // height image
  input_types_[i++] = "vil_image_view_base_sptr";   // ground image
  input_types_[i++] = "vcl_string";                 // file path (ply2) for the meshes

  //output
  output_data_.resize(0,brdb_value_sptr(0));
  output_types_.resize(0);

}

bool bmdl_generate_mesh_process::execute()
{
  // Sanity check
  if (!this->verify_inputs())
    return false;

  // get the inputs:
  brdb_value_t<vcl_string>* input0 =
    static_cast<brdb_value_t< vcl_string>* >(input_data_[0].ptr());
  vcl_string file_poly = input0->value();

  // label image
  brdb_value_t<vil_image_view_base_sptr>* input1 =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[1].ptr());
  vil_image_view_base_sptr label_img = input1->value();

  if (!label_img) {
    vcl_cout << "bmdl_classify_process -- Label image is not set!\n";
    return false;
  }

  // height image
  brdb_value_t<vil_image_view_base_sptr>* input2 =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[2].ptr());
  vil_image_view_base_sptr height_img = input2->value();

  if (!height_img) {
    vcl_cout << "bmdl_classify_process -- Label image is not set!\n";
    return false;
  }

  // ground image
  brdb_value_t<vil_image_view_base_sptr>* input3 =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[3].ptr());
  vil_image_view_base_sptr ground_img = input3->value();

  if (!ground_img) {
    vcl_cout << "bmdl_classify_process -- Label image is not set!\n";
    return false;
  }

  brdb_value_t<vcl_string>* input4 =
    static_cast<brdb_value_t< vcl_string>* >(input_data_[4].ptr());
  vcl_string file_mesh = input4->value();

  generate_mesh(file_poly, label_img, height_img, ground_img, file_mesh);

  return true;
}

bool 
bmdl_generate_mesh_process::generate_mesh(vcl_string fpath_poly,
                                          vil_image_view_base_sptr label_img, 
                                          vil_image_view_base_sptr height_img, 
                                          vil_image_view_base_sptr ground_img,
                                          vcl_string fpath_mesh)
{
  if (label_img->pixel_format() != VIL_PIXEL_FORMAT_UINT_32) {
    vcl_cout << "bmdl_generate_mesh_process::the Label Image pixel format" << label_img->pixel_format() << " undefined" << vcl_endl;
    return false;
  }
  vil_image_view<vxl_uint_32> labels(label_img);

  if (height_img->pixel_format() != VIL_PIXEL_FORMAT_DOUBLE) {
    vcl_cout << "bmdl_generate_mesh_process::the Height Image pixel format" << height_img->pixel_format() << " undefined" << vcl_endl;
    return false;
  }
  vil_image_view<double> heights(height_img);

  if (ground_img->pixel_format() != VIL_PIXEL_FORMAT_DOUBLE) {
    vcl_cout << "bmdl_generate_mesh_process::the Ground Image pixel format" << ground_img->pixel_format() << " undefined" << vcl_endl;
    return false;
  }
  vil_image_view<double> ground(ground_img);

  // read polygons
  vsl_b_ifstream os(fpath_poly);
  unsigned char ver; //version();
  vsl_b_read(os, ver);
  unsigned int size;
  vsl_b_read(os, size);
  vgl_polygon<double> polygon;
  vcl_vector<vgl_polygon<double> > boundaries;
  for (unsigned i = 0; i < size; i++) {
    vsl_b_read(os, polygon);
    boundaries.push_back(polygon);
  }

  imesh_mesh mesh;
  bmdl_mesh::mesh_lidar(boundaries , labels, heights, ground, mesh);
  imesh_write_obj(fpath_mesh, mesh);

  return true;
}

