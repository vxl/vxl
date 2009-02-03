//:
// \file
#include <bprb/bprb_func_process.h>
#include <vgl/vgl_polygon.h>
#include <vgl/io/vgl_io_polygon.h>
#include <vil/vil_image_view.h>

#include <bmdl/bmdl_mesh.h>

bool trace_boundaries(vil_image_view_base_sptr label_img, vcl_string fpath)
{
  if (label_img->pixel_format() != VIL_PIXEL_FORMAT_UINT_32) {
    vcl_cout << "bmdl_trace_boundaries_process::the Label Image pixel format" << label_img->pixel_format() << " undefined" << vcl_endl;
    return false;
  }

  vil_image_view<unsigned int>* img = static_cast<vil_image_view<unsigned int>* > (label_img.as_pointer());
  vcl_vector<vgl_polygon<double> > polygons =
    bmdl_mesh::trace_boundaries(*img);

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

bool bmdl_trace_boundaries_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 2) {
    vcl_cout << "lidar_roi_process: The input number should be 2" << vcl_endl;
    return false;
  }

  unsigned int i=0;
  vil_image_view_base_sptr img = pro.get_input<vil_image_view_base_sptr>(i++);
  vcl_string fpath = pro.get_input<vcl_string>(i++);

  if (!img) {
    vcl_cout << "bmdl_classify_process -- Label image is not set!\n";
    return false;
  }

  trace_boundaries(img, fpath);

  return true;
}

bool bmdl_trace_boundaries_process_cons(bprb_func_process& pro)
{
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vil_image_view_base_sptr");
  input_types.push_back("vcl_string");
  return pro.set_input_types(input_types);
}
