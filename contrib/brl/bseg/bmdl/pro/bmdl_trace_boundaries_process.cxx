#include "bmdl_trace_boundaries_process.h"
//:
// \file

#include <vcl_cassert.h>
#include <vcl_cstring.h>

#include <vgl/vgl_polygon.h>
#include <vgl/io/vgl_io_polygon.h>

#include <vil/vil_load.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vil/vil_pixel_format.h>

#include <bprb/bprb_parameters.h>

#include <bmdl/bmdl_mesh.h>

bmdl_trace_boundaries_process::bmdl_trace_boundaries_process()
{
  //this process takes 1 input:
  input_data_.resize(1, brdb_value_sptr(0));
  input_types_.resize(1);

  int i=0;
  input_types_[i++] = "vil_image_view_base_sptr";   // label image
  input_types_[i++] = "vcl_string";                 // file path for the polygons to be saved  

  //output
  output_data_.resize(0,brdb_value_sptr(0));
  output_types_.resize(0);

}

bool bmdl_trace_boundaries_process::execute()
{
  // Sanity check
  if (!this->verify_inputs())
    return false;

  // get the inputs:
  // image
  brdb_value_t<vil_image_view_base_sptr>* input0 =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[0].ptr());
  vil_image_view_base_sptr img = input0->value();

  if (!img) {
    vcl_cout << "bmdl_classify_process -- Label image is not set!\n";
    return false;
  }

  brdb_value_t<vcl_string>* input1 =
    static_cast<brdb_value_t< vcl_string>* >(input_data_[1].ptr());
  vcl_string fpath = input1->value();

  return true;
}

bool bmdl_trace_boundaries_process::trace_bundaries(vil_image_view_base_sptr label_img, vcl_string fpath)
{
  if (label_img->pixel_format() != VIL_PIXEL_FORMAT_UINT_32) {
    vcl_cout << "bmdl_trace_boundaries_process::the Label Image pixel format" << label_img->pixel_format() << " undefined" << vcl_endl;
    return false;
  }

  vil_image_view<unsigned int>* img = static_cast<vil_image_view<unsigned int>* > (label_img.as_pointer());
  vcl_vector<vgl_polygon<double> > polygons =
    bmdl_mesh::trace_boundaries(*img);
  bmdl_mesh::simplify_boundaries(polygons);

  // save polygons
  vsl_b_ofstream os(fpath);
  unsigned char ver = 1; //version();
  vsl_b_write(os, ver);
  vsl_b_write(os, polygons.size());
  for (unsigned i = 0; i < polygons.size(); i++) {
    vsl_b_write(os, polygons[i]);
  }

  return true;
}

