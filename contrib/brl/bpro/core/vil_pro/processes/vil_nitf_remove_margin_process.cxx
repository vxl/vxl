//This is brl/bpro/core/vil_pro/processes/vil_nitf_remove_margin_process.cxx
#include <iostream>
#include <cmath>
#include <bprb/bprb_func_process.h>
//:
// \file
//
//  a process to remove margin existed in satellite imagery, assuming the margin is a rectangular region and have zero pixel value in all the bands
//

#include <vil/vil_image_resource.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_new.h>
#include <vil/vil_image_view.h>
#include <vil/vil_blocked_image_resource.h>
#include <vil/vil_blocked_image_resource_sptr.h>
#include <vil/vil_crop.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_save.h>
#include <vil/vil_plane.h>

//: set the input and output type
bool vil_nitf_remove_margin_process_cons(bprb_func_process& pro)
{
  // process takes one input
  std::vector<std::string> input_types_(1);
  input_types_[0]  = "vil_image_resource_sptr";
  // process has one output
  std::vector<std::string> output_types_(4);
  output_types_[0] = "unsigned";  // top left valid image position vi
  output_types_[1] = "unsigned";  // top left valid image position vj
  output_types_[2] = "unsigned";  // valid image size vni
  output_types_[3] = "unsigned";  // valid image size vnj
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

//: execute the process
bool vil_nitf_remove_margin_process(bprb_func_process& pro)
{
  // sanity check
  if (!pro.verify_inputs()) {
    std::cerr << pro.name() << ": Invalid inputs!\n";
    return false;
  }
  // get input
  unsigned in_i = 0;
  vil_image_resource_sptr img_res_sptr = pro.get_input<vil_image_resource_sptr>(in_i);
  if (!img_res_sptr) {
    std::cerr << pro.name() << ": problem with the inut image resource handle!\n";
    return false;
  }
  if (img_res_sptr->pixel_format() != VIL_PIXEL_FORMAT_UINT_16) {
    std::cerr << pro.name() << ": unsupported pixel format -- " << img_res_sptr->pixel_format() << "!\n";
    return false;
  }

  unsigned int ni = img_res_sptr->ni();
  unsigned int nj = img_res_sptr->nj();

  unsigned int vi = 0, vj = 0, vni = 0, vnj = 0;

  // create blocked image resource for IO speed up
  vil_blocked_image_resource_sptr bir = blocked_image_resource(img_res_sptr);

  // start by trimming from top left corner
  bool found = false;
  unsigned sub_i0, sub_j0, sub_size_i, sub_size_j;
  sub_i0 = 0;
  sub_j0 = 0;
  sub_size_i = std::floor(ni / 10.0);
  sub_size_j = std::floor(nj / 10.0);
  for (unsigned sub_idx_i = 1; (sub_idx_i <= 10 && !found);  sub_idx_i++)
    for (unsigned sub_idx_j = 1; (sub_idx_j <= 10 && !found);  sub_idx_j++)
    {
      unsigned sni = sub_idx_i * sub_size_i;
      if (sub_size_i == 10)  sni = ni;
      unsigned snj = sub_idx_j * sub_size_j;
      if (sub_size_j == 10)  snj = nj;
      if (sni == 0 || snj == 0)
        continue;
      // get the block view from blocked image resource
      vil_image_view_base_sptr roi = bir->get_copy_view(sub_i0, sni, sub_j0, snj);
      if (!roi) {
        std::cerr << pro.name() << " can not crop from image with size " << ni << 'x' << nj << " at position (" << sub_i0 << ',' << sub_j0
                 << ") of size (" << sni << ',' << snj << ")!\n";
        return false;
      }
      // check pixels inside roi
      vil_image_view<vxl_uint_16> img(roi);
      for (unsigned p = 0; (p < img.nplanes() && !found); p++)
        for (unsigned i = 0; (i < img.ni() && !found); i++)
          for (unsigned j = 0; (j < img.nj() && !found); j++)
            if (img(i,j,p) != 0) {
              found = true;
              vi = sub_i0 + i; vj = sub_j0 + j;
            }
    }
  if (!found) {
    std::cerr << pro.name() << ": entire image is invalid!\n";
    return false;
  }

  // trimming from the bottom right
  found = false;
  unsigned int bottom_i = ni, bottom_j = nj;
  for (int sub_idx_i = 10; (sub_idx_i >= 0 && !found); sub_idx_i--) {
    for (int sub_idx_j = 10; (sub_idx_j >= 0 && !found); sub_idx_j--)
    {
      sub_i0 = sub_idx_i * sub_size_i;
      sub_j0 = sub_idx_j * sub_size_j;
      unsigned sni = ni - sub_i0, snj = nj - sub_j0;
      if (sni == 0 || snj == 0)
        continue;
      // get the block view from blocked image resource
      vil_image_view_base_sptr roi = bir->get_copy_view(sub_i0, sni, sub_j0, snj);
      if (!roi) {
        std::cerr << pro.name() << " can not crop from image with size " << ni << 'x' << nj << " at position (" << sub_i0 << ',' << sub_j0
                 << ") of size (" << sni << ',' << snj << ")!\n";
        return false;
      }
      // check pixels inside roi
      vil_image_view<vxl_uint_16> img(roi);
      for (unsigned p = 0; (p < img.nplanes() && !found); p++)
        for (int i = (int)(img.ni()-1); (i >= 0 && !found); i--)
          for (int j = (int)(img.nj()-1); (j >= 0 && !found); j--)
            if (img(i,j,p) != 0) {
              found = true;
              bottom_i = sub_i0+i;  bottom_j = sub_j0+j;
#if 0
              // save the image for debugging purpose
              std::cout << "image has valid pixel: sub_i0: " << sub_i0 << ", sub_j0: " << sub_j0 << ", sni: " << sni << ", snj: " << snj << std::endl;
              std::cout << " valid pixel is caught at pixel (" << i << ',' << j << ',' << p << ")" << std::endl;
              std::string out_folder = "d:/work/Dropbox/pyscripts/expt_scripts/src/11jul10003056-m1bs-500081228070_01_p001/";
              std::string band_c_file = out_folder + "coastal.tif";
              vil_save(vil_image_view<vxl_uint_16>(vil_plane(img, 0)), band_c_file.c_str());
              std::string band_b_file = out_folder + "blue.tif";
              vil_save(vil_image_view<vxl_uint_16>(vil_plane(img, 1)), band_b_file.c_str());
              std::string band_g_file = out_folder + "green.tif";
              vil_save(vil_image_view<vxl_uint_16>(vil_plane(img, 2)), band_g_file.c_str());
              std::string band_y_file = out_folder + "yellow.tif";
              vil_save(vil_image_view<vxl_uint_16>(vil_plane(img, 3)), band_y_file.c_str());
              std::string band_r_file = out_folder + "red.tif";
              vil_save(vil_image_view<vxl_uint_16>(vil_plane(img, 4)), band_r_file.c_str());
              std::string band_re_file = out_folder + "red_edge.tif";
              vil_save(vil_image_view<vxl_uint_16>(vil_plane(img, 5)), band_re_file.c_str());
              std::string band_nir1_file = out_folder + "nir1.tif";
              vil_save(vil_image_view<vxl_uint_16>(vil_plane(img, 6)), band_nir1_file.c_str());
              std::string band_nir2_file = out_folder + "nir2.tif";
              vil_save(vil_image_view<vxl_uint_16>(vil_plane(img, 7)), band_nir2_file.c_str());
#endif
            }
    }
  }
  if (!found) {
    std::cerr << pro.name() << ": entire image is invalid!\n";
    return false;
  }

  // calculate image size
  vni = bottom_i - vi;
  vnj = bottom_j - vj;

  // truncate 50 pixels on each boundary
  unsigned margin = 50;
  vi += margin;
  vj += margin;
  if (vni > margin)  vni -= margin;
  if (vnj > margin)  vnj -= margin;

  unsigned out_i = 0;
  pro.set_output_val<unsigned>(out_i++,  vi);
  pro.set_output_val<unsigned>(out_i++,  vj);
  pro.set_output_val<unsigned>(out_i++, vni);
  pro.set_output_val<unsigned>(out_i++, vnj);

  return true;
}
