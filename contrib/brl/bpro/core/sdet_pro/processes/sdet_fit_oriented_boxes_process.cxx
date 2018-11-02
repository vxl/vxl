// This is brl/bpro/core/sdet_pro/processes/sdet_fit_oriented_boxes_process.cxx
#include <iostream>
#include <bprb/bprb_func_process.h>
//:
// \file   given a blob image with each image having a unique different color, fit an oriented box to each blob and return 4 corners of it

#include <vil/vil_image_view.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bbas_pro/bbas_1d_array_float.h>
#include <sdet/sdet_region.h>

// assume the object holds reusable storage, such as a heap-allocated buffer mArray
bool operator<(const vil_rgb<vxl_byte>& l, const vil_rgb<vxl_byte>& r)
{
   unsigned r1 = l.R();
   unsigned r2 = r.R();
   unsigned g1 = l.G();
   unsigned g2 = r.G();
   unsigned b1 = l.B();
   unsigned b2 = r.B();

   if (r1 < r2)
     return true;
   else if (g1 < g2)
     return true;
   else if (b1 < b2)
     return true;
   else
     return false;
}

//: Constructor
bool sdet_fit_oriented_boxes_process_cons(bprb_func_process& pro)
{
  //this process takes 3 inputs
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr");  // randomly colored blob image

  std::vector<std::string> output_types;
  output_types.emplace_back("unsigned");  // return the number of blobs, N
  output_types.emplace_back("bbas_1d_array_float_sptr");  // an array of size N*8, four points with (u,v) image coordinates for each corner of each oriented box
  output_types.emplace_back("bbas_1d_array_float_sptr");  // an array of size N*2, which output the length and width of the fitting oriented box
  return pro.set_input_types(input_types)
      && pro.set_output_types(output_types);
}

//: Execute the process
bool sdet_fit_oriented_boxes_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 1) {
    std::cout << "sdet_fit_oriented_boxes_process: The number of inputs should be 1" << std::endl;
    return false;
  }

  // get the inputs
  unsigned i=0;
  vil_image_view_base_sptr img_ptr = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view<vil_rgb<vxl_byte> > color_img(img_ptr);

  unsigned ni = color_img.ni();
  unsigned nj = color_img.nj();

  vil_rgb<vxl_byte> bg_color = vil_rgb<vxl_byte>(0,0,0);
  // create a map of colors to pixels
  std::map<vil_rgb<vxl_byte>, std::vector<vgl_point_2d<float> > > color_map;
  for (unsigned i = 0; i < ni; i++)
    for (unsigned j = 0; j < nj; j++) {
      vil_rgb<vxl_byte> current = color_img(i,j);
      if (current == bg_color)
        continue;
      auto iter = color_map.find(current);
      if (iter != color_map.end()) {
        (iter->second).emplace_back(i,j);
      } else {
        std::vector<vgl_point_2d<float> > tmp;
        tmp.emplace_back(i,j);
        color_map[current] = tmp;
      }
    }

  auto iter = color_map.begin();
  int cnt = 0;
  for ( ; iter != color_map.end(); iter++)
    cnt++;

  std::cout << "Found " << cnt << " blobs!" << std::endl;
  auto * corners = new bbas_1d_array_float(cnt*8);
  auto * box_dim = new bbas_1d_array_float(cnt*2);
  iter = color_map.begin();
  cnt = 0;
  for ( ; iter != color_map.end(); iter++) {
    std::vector<vgl_point_2d<float> > v = iter->second;
    auto* xp = new float[v.size()];
    auto* yp = new float[v.size()];
    auto* Ip = new unsigned short[v.size()];  // supposed to be grey values for the blob but here we don't care so just fill with same color

    for (unsigned i = 0; i < v.size(); i++) {
      xp[i] = v[i].x();
      yp[i] = v[i].y();
      Ip[i] = 100;
    }
    sdet_region sr(v.size(), xp, yp, Ip);
    vgl_oriented_box_2d<float> box = sr.obox();
    std::vector<vgl_point_2d<float> > cc = box.corners();
#if 0
    std::cout << "color: " << iter->first << ", corners: " << std::endl;
    for (std::vector<vgl_point_2d<float> >::iterator vit = cc.begin(); vit != cc.end(); ++vit)
      std::cout << "  " << *vit << std::endl;
    std::cout << "axis width: " << box.width() << std::endl;
#endif
    for (unsigned i = 0; i < cc.size(); i++) {
       corners->data_array[8*cnt+2*i] = cc[i].x();
       corners->data_array[8*cnt+2*i+1] = cc[i].y();
    }
    box_dim->data_array[2*cnt]   = box.width();
    box_dim->data_array[2*cnt+1] = box.height();
    cnt++;
    delete [] xp;
    delete [] yp;
    delete [] Ip;
  }

  pro.set_output_val<unsigned>(0, cnt);
  pro.set_output_val<bbas_1d_array_float_sptr>(1, corners);
  pro.set_output_val<bbas_1d_array_float_sptr>(2, box_dim);
  return true;
}
