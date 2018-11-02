// This is brl/bbas/volm/pro/processes/volm_generate_height_map_from_ply_process.cxx
#include <iostream>
#include <limits>
#include <bprb/bprb_func_process.h>
//:
// \file
//         Read a set of ply files and generate a height map
//
//
#include <bprb/bprb_parameters.h>
#include <vil/vil_image_view.h>
#include <volm/volm_category_io.h>
#include <volm/volm_io_tools.h>
#include <vul/vul_file_iterator.h>
#include <rply.h>   //.ply parser

#include <bmsh3d/bmsh3d_mesh.h>
#include <bmsh3d/algo/bmsh3d_fileio.h>
#include <bmsh3d/bmsh3d_mesh_mc.h>
#include <vgl/vgl_polygon_scan_iterator.h>
#include <core/bbas_pro/bbas_1d_array_float.h>
#include <vil/vil_math.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//:
//  Take a colored segmentation output and map it to volm labels
bool volm_generate_height_map_from_ply_process_cons(bprb_func_process& pro)
{
  std::vector<std::string> input_types;
  input_types.emplace_back("vcl_string");  // path to ply files
  input_types.emplace_back("unsigned");  // ni
  input_types.emplace_back("unsigned");  // nj
  std::vector<std::string> output_types;
  output_types.emplace_back("vil_image_view_base_sptr"); // output height map
  return pro.set_input_types(input_types)
      && pro.set_output_types(output_types);
}

//: Execute the process
bool volm_generate_height_map_from_ply_process(bprb_func_process& pro)
{
  if (pro.n_inputs() < 3) {
    std::cout << "volm_map_osm_process: The number of inputs should be 1" << std::endl;
    return false;
  }

  // get the inputs
  std::string path = pro.get_input<std::string>(0);
  auto ni = pro.get_input<unsigned>(1);
  auto nj = pro.get_input<unsigned>(2);

  vil_image_view<float> out_map(ni, nj);
  out_map.fill(0.0f);

  std::string ply_glob=path+"/*.ply";
  vul_file_iterator ply_it(ply_glob.c_str());
  while (ply_it) {
    std::string name(ply_it());
    std::cout << " name: " << name << std::endl;

    auto *  bmesh = new bmsh3d_mesh_mc();
    bmsh3d_load_ply(bmesh,name.c_str());
    auto it = bmesh->facemap().begin();
    for (; it != bmesh->facemap().end(); it++) {
      int id = (*it).first;
      bmsh3d_face* tmpF = (*it).second;
      std::vector<bmsh3d_vertex*> vertices;
      tmpF->get_ordered_Vs(vertices);

      vgl_polygon<double> poly(1);
      double height = 0.0f;
      for (unsigned j = 0; j < vertices.size(); j++) {
        std::cout << "vertex[" << j << "]: x:" << vertices[j]->get_pt().x() << " y: " << vertices[j]->get_pt().y() << " z: " << vertices[j]->get_pt().z() << std::endl;
        poly.push_back(vertices[j]->get_pt().x(), nj-vertices[j]->get_pt().y());
        height += vertices[j]->get_pt().z();
      }
      height /= vertices.size();

      vgl_polygon_scan_iterator<double> psi(poly, false);
      for (psi.reset(); psi.next();) {
        int y = psi.scany();
        for (int x = psi.startx(); x<=psi.endx(); ++x)
        {
          int u = (int)std::floor(x+0.5);
          int v = (int)std::floor(y+0.5);
          if (u >= (int)ni || v >= (int)nj || u < 0 || v < 0)
            continue;
          out_map(u,v) = height;
        }
      }
    }
    ++ply_it;
  }

  vil_image_view_base_sptr out_img_sptr = new vil_image_view<float>(out_map);
  pro.set_output_val<vil_image_view_base_sptr>(0, out_img_sptr);
  return true;
}


//:
//  Take a ground-truth height map and another height map and compare the two pixel by pixel to compute a plot of #correct heights/#gt pixels vs height difference used to compute correct heights
//  note that we cannot plot a classical ROC as there is no Negative class (algo always reports a height)
bool volm_generate_height_map_plot_process_cons(bprb_func_process& pro)
{
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr");  // gt height map
  input_types.emplace_back("vil_image_view_base_sptr");  // input height map
  input_types.emplace_back("float");  // initial height difference
  input_types.emplace_back("float");  // final height difference
  input_types.emplace_back("float");  // height increments
  input_types.emplace_back("float");  // fix the ground truth height

  std::vector<std::string> output_types;
  output_types.emplace_back("bbas_1d_array_float_sptr");  // #correct rate
  output_types.emplace_back("bbas_1d_array_float_sptr");  // #height difs
  output_types.emplace_back("vil_image_view_base_sptr");  // output image with pixels given by threshold of 0.8 tpr marked red
  output_types.emplace_back("vil_image_view_base_sptr");  // output image with pixels as difference in value between gt and input height maps

  return pro.set_input_types(input_types)
      && pro.set_output_types(output_types);
}

//: Execute the process
bool volm_generate_height_map_plot_process(bprb_func_process& pro)
{
  if (pro.n_inputs() < 5) {
    std::cout << "volm_map_osm_process: The number of inputs should be 1" << std::endl;
    return false;
  }

  // get the inputs
  vil_image_view_base_sptr gt_height_sptr = pro.get_input<vil_image_view_base_sptr>(0);
  vil_image_view_base_sptr height_map_sptr = pro.get_input<vil_image_view_base_sptr>(1);
  auto dif_init = pro.get_input<float>(2);
  auto dif_final = pro.get_input<float>(3);
  auto dif_increments = pro.get_input<float>(4);
  auto gt_fix = pro.get_input<float>(5);

  vil_image_view<float> gt_height(gt_height_sptr);
  std::cout << "gt ni: " << gt_height.ni() << " nj: " << gt_height.nj() << std::endl;
  vil_image_view<float> height(height_map_sptr);
  std::cout << "height ni: " << height.ni() << " nj: " << height.nj() << std::endl;
  if (gt_height.ni() != height.ni() || gt_height.nj() != height.nj()) {
    std::cerr << " The input images have inconsistent sizes!\n";
    return false;
  }

  // fix the ground truth height
  for (unsigned i = 0; i < gt_height.ni(); i++)
    for (unsigned j = 0; j < gt_height.nj(); j++)
    {
      float gt_val = gt_height(i,j);
      if ( (gt_val)*(gt_val) > 1E-5)
        gt_height(i,j) += gt_fix;
    }

  float min_val, max_val;
  vil_math_value_range(height, min_val, max_val);
  float dif_min_max = max_val-min_val;
  vil_image_view<vil_rgb<vxl_byte> > height_out(height.ni(), height.nj());
  vil_image_view<float> height_out_dif(height.ni(), height.nj());
  for (unsigned i = 0; i < height.ni(); i++)
    for (unsigned j = 0; j < height.nj(); j++) {
      auto val = vxl_byte(((height(i,j)-min_val)/dif_min_max)*255);
      vil_rgb<vxl_byte> col(val, val, val);
      height_out(i,j) = col;
      height_out_dif(i,j) = std::numeric_limits<float>::quiet_NaN();
    }

  float dif_mark = dif_final;
  unsigned int numPoints = 0;

  for (float dif = dif_init;  dif <= dif_final; dif += dif_increments) {
    numPoints++;
  }

  auto * height_difs = new bbas_1d_array_float(numPoints);
  auto * correct_rate = new bbas_1d_array_float(numPoints);

  for (unsigned int pnt=0; pnt<numPoints; pnt++) {
    correct_rate->data_array[pnt]=0.0f;
  }

  // if input height value is valid (> 0), and gt height value is defined (> 0)
  // retrieve height from input height map and from gt map, if the absolute difference is less than current "dif" amount then accept as true positive, otherwise it is a false positive
  unsigned int pnt = 0;
  float min_val_act = 1000000;
  float max_val_act = -1000000;
  for (float dif = dif_init;  dif <= dif_final; dif += dif_increments, pnt++) {
    unsigned gt_cnt = 0;
    height_difs->data_array[pnt] = dif;
    for (unsigned i = 0; i < gt_height.ni(); i++) {
      for (unsigned j = 0; j < gt_height.nj(); j++) {
        float gt_val = gt_height(i,j);
        float val = height(i,j);
        if ( (gt_val)*(gt_val) > 1E-5) {
          gt_cnt++;
          if (val > 0) {
            float dif_actual_val = gt_val - val;
            height_out_dif(i,j) = dif_actual_val;
            if (dif_actual_val < min_val_act)
              min_val_act = dif_actual_val;
            if (dif_actual_val > max_val_act)
              max_val_act = dif_actual_val;
            float dif_val = std::abs(dif_actual_val);
            if (dif_val <= dif) {
              correct_rate->data_array[pnt]++;
              if (dif_mark == dif)
                height_out(i,j) = vil_rgb<vxl_byte>(255,0,0);
            }
          } else {
            height_out_dif(i,j) = std::numeric_limits<float>::infinity();
          }
        }
      }
    }
    std::cout << "ratio: " << correct_rate->data_array[pnt] << " gt pixel: " << gt_cnt << std::endl;
    correct_rate->data_array[pnt] /= gt_cnt;
  }

  std::cout << " !!!!!!!!!! minimum difference value: " << min_val_act << " max difference val: " << max_val_act << std::endl;

  pro.set_output_val<bbas_1d_array_float_sptr>(0, correct_rate);
  pro.set_output_val<bbas_1d_array_float_sptr>(1, height_difs);
  pro.set_output_val<vil_image_view_base_sptr>(2, new vil_image_view<vil_rgb<vxl_byte> >(height_out));
  pro.set_output_val<vil_image_view_base_sptr>(3, new vil_image_view<float>(height_out_dif));
  return true;
}

//
//:  Find the minimum and maximum height values for a given 2-d box region
bool volm_find_min_max_height_process_cons(bprb_func_process& pro)
{
  std::vector<std::string> input_types(5);
  input_types[0] = "double";      // 2-d region lower left lon
  input_types[1] = "double";      // 2-d region lower left lat
  input_types[2] = "double";      // 2-d region upper right lon
  input_types[3] = "double";      // 2-d region upper right lat
  input_types[4] = "vcl_string";  // folder of the orthogonal height maps

  std::vector<std::string> output_types(2);
  output_types[0] = "double";     // minimum elevation value for the given region from height maps
  output_types[1] = "double";     // maximum elevation value for the given region from height maps

  return pro.set_input_types(input_types) && pro.set_output_types(output_types);
}


//: Execute the process
bool volm_find_min_max_height_process(bprb_func_process& pro)
{
  // check input
  if (!pro.verify_inputs()) {
    std::cerr << pro.name() << ": Wrong Inputs!!!\n";
    return false;
  }
  // get inputs
  unsigned in_i = 0;
  auto ll_lon = pro.get_input<double>(in_i++);
  auto ll_lat = pro.get_input<double>(in_i++);
  auto ur_lon = pro.get_input<double>(in_i++);
  auto ur_lat = pro.get_input<double>(in_i++);
  std::string dem_folder = pro.get_input<std::string>(in_i++);

  // load DEM images
  std::vector<volm_img_info> infos;
  volm_io_tools::load_aster_dem_imgs(dem_folder, infos);
  if (infos.empty()) {
    std::cerr << pro.name() << ": can not find any height map in the folder: " << dem_folder << "\n";
    return false;
  }

  // find the min and max elevation from height maps
  vgl_point_2d<double>  lower_left(ll_lon, ll_lat);
  vgl_point_2d<double> upper_right(ur_lon, ur_lat);
  double min_elev = 10000.0, max_elev = -10000.0;
  if (!volm_io_tools::find_min_max_height(lower_left, upper_right, infos, min_elev, max_elev)) {
    std::cerr << pro.name() << " can not find elevation for the given region " << lower_left << ", " << upper_right << "\n";
    return false;
  }

  // output
  unsigned out_i = 0;
  pro.set_output_val<double>(out_i++, min_elev);
  pro.set_output_val<double>(out_i++, max_elev);
  return true;
}
