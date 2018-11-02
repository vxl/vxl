// This is brl/bbas/volm/pro/processes/volm_refine_bvxm_height_map_process.cxx
//:
// \file
#include <string>
#include <iostream>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_image_view.h>
#include <vul/vul_file.h>
#include <vil/algo/vil_region_finder.h>
#include <bil/algo/bil_blob_finder.h>
#include <vil/algo/vil_binary_dilate.h>
#include <vil/algo/vil_binary_erode.h>
#include <bkml/bkml_write.h>
#include <vpgl/file_formats/vpgl_geo_camera.h>
#include <vgl/vgl_point_3d.h>

// for debug
#if 0
#include <vil/vil_save.h>
#endif

namespace volm_refine_bvxm_height_map_process_globals
{
  constexpr unsigned int n_inputs_ = 3;
  constexpr unsigned int n_outputs_ = 1;
  float neighbor_min_height(std::vector<unsigned> const& ri, std::vector<unsigned> const& rj, vil_image_view<float> const& in_img)
  {
    // create a neighbor list
    static int const nbrs8_delta[8][2] = { { 1, 0}, { 1,-1}, { 0,-1}, {-1,-1},
                                         {-1, 0}, {-1, 1}, { 0, 1}, { 1, 1} };
    unsigned num_nbrs = 8;
    float min_h = 10000.0f;
    // loop over all pixel to obtain min height (TO DO: speed up to avoid searching pixels that are inside the region)
    for (unsigned k = 0; k < ri.size(); k++) {
      unsigned i = ri[k];  unsigned j = rj[k];
      for (unsigned c = 0; c < num_nbrs; c++) {
        auto nbr_i = (unsigned)( (signed)i + nbrs8_delta[c][0] );
        auto nbr_j = (unsigned)( (signed)j + nbrs8_delta[c][1] );
        if (nbr_i < in_img.ni() && nbr_j < in_img.nj())
          if ( in_img(nbr_i, nbr_j) < min_h )
            min_h = in_img(nbr_i, nbr_j);
      }
    }
    return min_h;
  }
}

//: simple algorithm to refine the height map generate from bvxm_scene
bool volm_refine_bvxm_height_map_process_cons(bprb_func_process& pro)
{
  using namespace volm_refine_bvxm_height_map_process_globals;
  // inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";          // original height map image
  input_types_[1] = "float";               // predominant height for sky mask
  input_types_[2] = "float";               // predominant height for ground mask

  // output
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool volm_refine_bvxm_height_map_process(bprb_func_process& pro)
{
  using namespace volm_refine_bvxm_height_map_process_globals;
  // input check
  if (!pro.verify_inputs()) {
    std::cout << pro.name() << ": invalid inputs" << std::endl;
    return false;
  }
  // get input
  unsigned i = 0;
  vil_image_view_base_sptr i_img_res = pro.get_input<vil_image_view_base_sptr>(i++);
  auto sky_h = pro.get_input<float>(i++);
  auto grd_h = pro.get_input<float>(i++);

  auto* in_img = dynamic_cast<vil_image_view<float>*>(i_img_res.ptr());
  if (!in_img) {
    std::cout << pro.name() << ": The image pixel format: " << i_img_res->pixel_format() << " is not supported" << std::endl;
    return false;
  }
  unsigned ni = in_img->ni();
  unsigned nj = in_img->nj();

  // generate sky mask
  vil_image_view<vxl_byte> sky_mask = vil_image_view<vxl_byte>(ni, nj);
  sky_mask.fill(0);
  for (unsigned i = 0; i < ni; i++)
    for (unsigned j = 0; j < nj; j++)
      if ( (*in_img)(i,j) > sky_h )
        sky_mask(i,j) = 1;

  // generate ground mask
  vil_image_view<vxl_byte> grd_mask = vil_image_view<vxl_byte>(ni, nj);
  grd_mask.fill(0);
  for (unsigned i = 0; i < ni; i++)
    for (unsigned j = 0; j < nj; j++)
      if ( (*in_img)(i,j) < grd_h )
        grd_mask(i,j) = 1;

  // create an output image
  auto* out_img = new vil_image_view<float>(ni, nj);
  out_img->deep_copy(*in_img);

#if 0
  // output for debug
  std::string sky_fname = "d:/work/find/phase_1b/satellite_modeling/wr2/scene_11278_small/sky_mask.tif";
  std::string grd_fname = "d:/work/find/phase_1b/satellite_modeling/wr2/scene_11278_small/grd_mask.tif";
  vil_save(sky_mask, sky_fname.c_str());
  vil_save(grd_mask, grd_fname.c_str());
#endif

  // refine sky/grd mask
  vil_region_finder<vxl_byte> sky_region_finder(sky_mask, vil_region_finder_8_conn);
  vil_region_finder<vxl_byte> grd_region_finder(grd_mask, vil_region_finder_8_conn);
  std::map<unsigned, std::pair<float, std::pair<std::vector<unsigned>, std::vector<unsigned> > > > sky_regions;
  std::map<unsigned, std::pair<float, std::pair<std::vector<unsigned>, std::vector<unsigned> > > > grd_regions;
  for (unsigned i = 0; i < ni; i++) {
    for (unsigned j = 0; j < nj; j++) {
      // refined sky mask
      if (sky_mask(i,j) != 0) {
        std::vector<unsigned> ri;  std::vector<unsigned> rj;
        sky_region_finder.same_int_region(i, j, ri, rj);
        if (ri.empty())
          continue;
        // search perimeter around the super pixel to obtain minimum neighbor height
        float min_h = neighbor_min_height(ri, rj, *in_img);

        unsigned key = (i+j)*(i+j+1)/2 + j;
        std::pair<float, std::pair<std::vector<unsigned>, std::vector<unsigned> > >tmp_pair(min_h, std::pair<std::vector<unsigned>, std::vector<unsigned> >(ri, rj));
        sky_regions.insert(std::pair<unsigned, std::pair<float, std::pair<std::vector<unsigned>, std::vector<unsigned> > > >(key, tmp_pair));
        //std::cout << " for super pixel (" << i << " x " << j << "), sky mask has height value: " << min_h << std::endl;
      }
      if (grd_mask(i,j) != 0) {
        std::vector<unsigned> ri;  std::vector<unsigned> rj;
        grd_region_finder.same_int_region(i, j, ri, rj);
        if (ri.empty())
          continue;
        float min_h = neighbor_min_height(ri, rj, *in_img);
        unsigned key = (i+j)*(i+j+1)/2 + j;
        std::pair<float, std::pair<std::vector<unsigned>, std::vector<unsigned> > >tmp_pair(min_h, std::pair<std::vector<unsigned>, std::vector<unsigned> >(ri, rj));
        grd_regions.insert(std::pair<unsigned, std::pair<float, std::pair<std::vector<unsigned>, std::vector<unsigned> > > >(key, tmp_pair));
        //std::cout << " for super pixel (" << i << " x " << j << "), grd mask has height value: " << min_h << std::endl;
      }
    }
  }

  // modify the output images with mask values
  std::map<unsigned, std::pair<float, std::pair<std::vector<unsigned>, std::vector<unsigned> > > >::iterator mit;
  for (mit = sky_regions.begin();  mit != sky_regions.end(); ++mit) {
    std::vector<unsigned> ri;  std::vector<unsigned> rj;
    float min_h = mit->second.first;
    ri = mit->second.second.first;  rj = mit->second.second.second;
    for (unsigned k = 0; k < ri.size(); k++)
      (*out_img)(ri[k],rj[k]) = min_h;
  }

  for (mit = grd_regions.begin();  mit != grd_regions.end(); ++mit) {
    std::vector<unsigned> ri;  std::vector<unsigned> rj;
    float min_h = mit->second.first;
    ri = mit->second.second.first;  rj = mit->second.second.second;
    for (unsigned k = 0; k < ri.size(); k++)
      (*out_img)(ri[k],rj[k]) = min_h;
  }

  pro.set_output_val<vil_image_view_base_sptr>(0, out_img);

  return true;
}

// extract the building outlines (with n vertices in global wgs84 coords) from the ortho classification map (byte image with each pixel the id of the class)
// output is a .csv file where each line is:
//    height, volume, area, confidence, cent_lon, cent_lat, lon_0, lat_0, ..., lon_i, lat_i, ..., lon_n, lat_n;
bool volm_extract_building_outlines_process_cons(bprb_func_process& pro)
{

  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr"); // height map
  input_types.emplace_back("vil_image_view_base_sptr"); // classification map
  input_types.emplace_back("vpgl_camera_double_sptr"); // geo camera
  input_types.emplace_back("vcl_string"); // output building .csv filename
  input_types.emplace_back("vcl_string"); // output building kml filename

  std::vector<std::string> output_types;
  output_types.emplace_back("vil_image_view_base_sptr"); // binary map
  output_types.emplace_back("vil_image_view_base_sptr"); // binary map
  output_types.emplace_back("vil_image_view_base_sptr"); // binary map
  return pro.set_input_types(input_types)
     &&  pro.set_output_types(output_types);
}

//: Execute the process
bool volm_extract_building_outlines_process(bprb_func_process& pro)
{
  if (pro.n_inputs()< 3) {
    std::cout << "volm_extract_building_outlines_process: The number of inputs should be 3" << std::endl;
    return false;
  }

  unsigned i=0;
  vil_image_view_base_sptr height_sptr = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr class_img_sptr = pro.get_input<vil_image_view_base_sptr>(i++);
  vpgl_camera_double_sptr cam = pro.get_input<vpgl_camera_double_sptr>(i++);
  auto* geocam = dynamic_cast<vpgl_geo_camera*> (cam.ptr());
  std::string csv_filename = pro.get_input<std::string>(i++);
  std::string kml_filename = pro.get_input<std::string>(i++);

  // convert image to float
  vil_image_view<float> height(height_sptr);
  unsigned ni = height.ni(); unsigned nj = height.nj();
  std::cout << "ni: " << ni << " nj: " << nj << std::endl;

  vil_image_view<vxl_byte> class_img(class_img_sptr);
  if (class_img_sptr->ni() != ni || class_img_sptr->nj() != nj) {
    std::cout << "volm_extract_building_outlines_process: The input image sizes are not compatible!" << std::endl;
    return false;
  }

  // first make the class image binary
  vil_image_view<bool> class_img_binary(class_img.ni(), class_img.nj());
  vil_image_view<bool> class_img_binary_E(class_img.ni(), class_img.nj());
  vil_image_view<bool> class_img_binary_D(class_img.ni(), class_img.nj());
  class_img_binary.fill(false);
  for (unsigned i = 0; i < class_img.ni(); i++)
    for (unsigned j = 0; j < class_img.nj(); j++) {
      //if (class_img(i,j) == 34 || class_img(i,j) == 15)
      //  class_img_binary(i,j) = true;
      // use height map
      //if (height(i,j) > 15)
      //if (height(i,j) > 10 && height(i,j) <= 15)
      //if ((class_img(i,j) == 34 || class_img(i,j) == 15) && height(i,j) > 10 && height(i,j) <= 15)
      if ((class_img(i,j) == 34 || class_img(i,j) == 15) && height(i,j) > 5 && height(i,j) <= 10)
        class_img_binary(i,j) = true;
    }

  vil_structuring_element se;
  se.set_to_disk(3);
  vil_binary_erode(class_img_binary,class_img_binary_E,se);
  vil_binary_dilate(class_img_binary_E,class_img_binary_D,se);

  std::vector<int> bi,bj;
  bil_blob_finder finder(class_img_binary_D);
  std::vector<std::vector<vgl_point_3d<double> > > bldgs;
  while (finder.next_8con_region(bi,bj))
  {
    std::cout<<"Blob boundary length: "<<bi.size()<<std::endl;
    std::vector<vgl_point_3d<double> > poly;
    for (unsigned i = 0; i < bi.size(); i++) {
      double lon, lat;
      geocam->img_to_global(bi[i], bj[i], lon, lat);
      poly.emplace_back(lon, lat, height(bi[i], bj[i]));
    }
    bldgs.push_back(poly);
  }

  // find blobs again to compute avg heights (need the region representation this time)
  bil_blob_finder finder2(class_img_binary_D);
  std::vector<double> bldg_heights;
  std::vector<vil_chord> region;
  while (finder2.next_8con_region(region))
  {
    std::cout<<"Blob region number of rows: "<<region.size()<<std::endl;
    double avg_height = 0.0;
    unsigned cnt = 0;
    double area = 0.0;
    for (auto & k : region) {
      for (unsigned i = k.ilo; i < k.ihi; i++) {
        double lon, lat;
        avg_height += height(i, k.j);
        cnt++;
      }
    }
    avg_height /= cnt;
    bldg_heights.push_back(avg_height);
  }
  std::cout << " there are: " << bldgs.size() << " buildings and " << bldg_heights.size() << " building heights.\n";
  std::cout.flush();

  std::ofstream ofs(kml_filename.c_str());
  bkml_write::open_document(ofs);

  std::ofstream ofs_csv(csv_filename.c_str());

  for (unsigned i = 0; i < bldgs.size(); i++) {
    vgl_polygon<double> poly(1);
    double cent_lon = 0.0, cent_lat = 0.0;
    for (auto & j : bldgs[i]) {
      poly[0].push_back(vgl_point_2d<double>(j.x(), j.y()));
      cent_lon += j.x();
      cent_lat += j.y();
    }
    cent_lon /= bldgs[i].size();
    cent_lat /= bldgs[i].size();
    std::stringstream avg_height_str; avg_height_str << "h: " << bldg_heights[i] << " " << cent_lon << " " << cent_lat;
    bkml_write::write_polygon(ofs, poly, avg_height_str.str(),avg_height_str.str());
                            /*double const& scale = 1.0,
                            double const& line_width = 3.0,
                            double const& alpha = 0.45,
                            unsigned char const& r = 0,
                            unsigned char const& g = 255,
                            unsigned char const& b = 0);*/

    // for csv each building is one line:   height, volume (=0.0 for now), area (=0.0 for now), confidence (=0.5 for now), cent_lon, cent_lat, lon_0, lat_0, ..., lon_i, lat_i, ..., lon_n, lat_n;
    ofs_csv << bldg_heights[i] << ",0.0,0.0,0.5," << cent_lon << ',' << cent_lat;
    for (auto & j : bldgs[i])
      ofs_csv << ',' << j.x() << ',' << j.y();
    ofs_csv << '\n';
  }
  bkml_write::close_document(ofs);
  ofs_csv.close();

  //set output
  pro.set_output_val<vil_image_view_base_sptr>(0, new vil_image_view<bool>(class_img_binary));
  pro.set_output_val<vil_image_view_base_sptr>(1, new vil_image_view<bool>(class_img_binary_E));
  pro.set_output_val<vil_image_view_base_sptr>(2, new vil_image_view<bool>(class_img_binary_D));
  //pro.set_output_val<vil_image_view_base_sptr>(0, new vil_image_view<vil_rgb<vxl_byte> >(out_img));
  //pro.set_output_val<vil_image_view_base_sptr>(1, new vil_image_view<float >(height));
  return true;
}

namespace volm_stereo_height_fix_process_globals
{
  constexpr unsigned int n_inputs_ = 2;
  constexpr unsigned int n_outputs_ = 0;
}

bool volm_stereo_height_fix_process_cons(bprb_func_process& pro)
{
  using namespace volm_stereo_height_fix_process_globals;
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";       // original height map image
  input_types_[1] = "float";
  std::vector<std::string> output_types_(n_outputs_);
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool volm_stereo_height_fix_process(bprb_func_process& pro)
{
  using namespace volm_stereo_height_fix_process_globals;
  if (!pro.verify_inputs()) {
    std::cout << pro.name() << ": invalid inputs" << std::endl;
    return false;
  }
  // get inputs
  unsigned i = 0;
  vil_image_view_base_sptr i_img_res = pro.get_input<vil_image_view_base_sptr>(i++);
  auto h_fix = pro.get_input<float>(i++);
  auto* in_img = dynamic_cast<vil_image_view<float>*>(i_img_res.ptr());
  if (!in_img) {
    std::cout << pro.name() << ": The image pixel format: " << i_img_res->pixel_format() << " is not supported" << std::endl;
    return false;
  }
  unsigned ni = in_img->ni();
  unsigned nj = in_img->nj();

  for (unsigned i = 0; i < ni; i++) {
    for (unsigned j = 0; j < nj; j++) {
      if ( (*in_img)(i,j) != 0 )
        (*in_img)(i,j) += h_fix;
    }
  }
  return true;
}
