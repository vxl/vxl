//:
// \file
#include <bprb/bprb_func_process.h>
#include <vgl/vgl_polygon.h>
#include <vgl/io/vgl_io_polygon.h>
#include <vil/vil_image_view.h>

#include <bmdl/bmdl_mesh.h>

bool trace_boundaries(const vil_image_view_base_sptr& label_img, const std::string& fpath)
{
  if (label_img->pixel_format() != VIL_PIXEL_FORMAT_UINT_32) {
    std::cout << "bmdl_trace_boundaries_process::the Label Image pixel format" << label_img->pixel_format() << " undefined" << std::endl;
    return false;
  }

  auto* img = static_cast<vil_image_view<unsigned int>* > (label_img.as_pointer());
  std::vector<vgl_polygon<double> > polygons =
    bmdl_mesh::trace_boundaries(*img);

  // save polygons
  vsl_b_ofstream os(fpath);
  unsigned char ver = 1; //version();
  vsl_b_write(os, ver);
  vsl_b_write(os, polygons.size());
  for (const auto & polygon : polygons) {
    vsl_b_write(os, polygon);
  }

  return true;
}

bool bmdl_trace_boundaries_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 2) {
    std::cout << "lidar_roi_process: The input number should be 2" << std::endl;
    return false;
  }

  unsigned int i=0;
  vil_image_view_base_sptr img = pro.get_input<vil_image_view_base_sptr>(i++);
  std::string fpath = pro.get_input<std::string>(i++);

  if (!img) {
    std::cout << "bmdl_classify_process -- Label image is not set!\n";
    return false;
  }

  trace_boundaries(img, fpath);

  return true;
}

bool bmdl_trace_boundaries_process_cons(bprb_func_process& pro)
{
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr");
  input_types.emplace_back("vcl_string");
  return pro.set_input_types(input_types);
}
