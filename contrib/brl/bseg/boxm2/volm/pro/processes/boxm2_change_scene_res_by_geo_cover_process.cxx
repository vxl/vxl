// This is brl/bseg/boxm2/volm/pro/process/boxm2_change_scene_res_by_geo_cover_process.cxx
#include <iostream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A process to prepare x y z images from geo_cover iamge with open stree map objects ingested on it
// NOTE: GeoCover image name is assumed to contain the bounding box information and vpgl_geo_camera is loaded from image header
//
// \author Yi Dong
// \date August 17, 2013
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <volm/volm_tile.h>
#include <volm/volm_category_io.h>
#include <vgl/vgl_intersection.h>
#include <bseg/boxm2/boxm2_block_metadata.h>
#include <bseg/boxm2/boxm2_scene.h>


//: A process to change the resolution/refinement of blocks based on the land type
//: e.g: rural regon like mountain can be low resolution but urban region requires high resolution
namespace boxm2_change_scene_res_by_geo_cover_process_globals
{
  constexpr unsigned n_inputs_ = 3;
  constexpr unsigned n_outputs_ = 1;

  // function that modify the block metadata accordingly
  void change_block_metadata(boxm2_block_metadata& md, volm_osm_category_io::geo_cover_values const& land_cover, int const& refine_coef)
  {
    if (land_cover == volm_osm_category_io::GEO_BARREN) {
      md.max_level_ = 1;
      return;
    }
    else if (land_cover == volm_osm_category_io::GEO_URBAN) {
      md.max_level_ = 4;
      vgl_vector_3d<double> sb_blk_dim = md.sub_block_dim_/refine_coef;
      vgl_vector_3d<unsigned> sb_blk_num = md.sub_block_num_*refine_coef;
      md.sub_block_dim_ = sb_blk_dim;
      md.sub_block_num_ = sb_blk_num;
      return;
    }
    else if (land_cover == volm_osm_category_io::GEO_AGRICULTURE_GENERAL) {
      md.max_level_ = 4;
      return;
    }
    else {
      std::cerr << "WARNING: boxm2_change_scene_res_by_geo_cover_process: unknown land cover to change the blk metadata\n";
      return;
    }
  }
}

bool boxm2_change_scene_res_by_geo_cover_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_change_scene_res_by_geo_cover_process_globals;
  // process takes 2 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";           // boxm2_scene
  input_types_[1] = "vcl_string";                 // image filename that satisfies certain rule e.g. Geocover_S33W071_S1x1.tif
  input_types_[2] = "int";
  // process takes 1 outputs
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "boxm2_scene_sptr";
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_change_scene_res_by_geo_cover_process(bprb_func_process& pro)
{
  using namespace boxm2_change_scene_res_by_geo_cover_process_globals;

  if (pro.n_inputs() < n_inputs_) {
    std::cout << pro.name() << ": The number of inputs should be " << n_inputs_ << std::endl;
    return false;
  }
  // get the inputs
  unsigned i = 0;
  boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(i++);
  std::string fname = pro.get_input<std::string>(i++);
  int refine_coefficient = pro.get_input<int>(i++);

  if (!(refine_coefficient == 0) && !(refine_coefficient & (refine_coefficient - 1)) && (refine_coefficient != 1) && (refine_coefficient != 2)) {
    std::cout << pro.name() << ": the refine coefficient need to be power of 2" << std::endl;
    return false;
  }
  // load the geo cover image
  if (!vul_file::exists(fname)) {
    std::cout << pro.name() << ": can not find image: " << fname << std::endl;
    return false;
  }
  vil_image_view_base_sptr img_sptr = vil_load(fname.c_str());
  auto* img = dynamic_cast<vil_image_view<vxl_byte> * >(img_sptr.ptr());

  // find the image bounding box
  // geo camera inside tile will be used for translation between geo coords and img pixels
  vpgl_lvcs lv = scene->lvcs();
  volm_tile tile(fname, img->ni(), img->nj());
  vgl_box_2d<float> tbbox = tile.bbox();
  std::cout << " geo_cover image has size: "<< img->ni() << " x " << img->nj() << std::endl;
  std::cout << " image bounding box in geo coords: " << tbbox << std::endl;

  // copy all necessary information from previous scene
  boxm2_scene_sptr changed_scene = new boxm2_scene(scene->data_path(), scene->local_origin());
  changed_scene->set_appearances(scene->appearances());
  changed_scene->set_lvcs(lv);
  changed_scene->set_num_illumination_bins(scene->num_illumination_bins());
  std::map<boxm2_block_id, boxm2_block_metadata>& changed_scene_blks = changed_scene->blocks();

  // a map to store the geo cover land category for current colum of blocks
  std::map<unsigned, volm_osm_category_io::geo_cover_values> blk_land_cover;

  // loop over all previous scene blocks
  std::map<boxm2_block_id, boxm2_block_metadata>& blks = scene->blocks();
  std::cout << " number of blocks in the scene " << blks.size() << std::endl;
  std::cout << " refine parameter: " << refine_coefficient << std::endl;
  unsigned cnt = 0;
  for (auto & blk : blks)
  {
    boxm2_block_id blk_id = blk.first;
    boxm2_block_metadata md = blk.second;

    // get the bounding box for current box and transfer it to geo coords
    vgl_box_3d<double> blk_box = md.bbox();
    double min_lon, min_lat, min_alt;
    double max_lon, max_lat, max_alt;
    lv.local_to_global(blk_box.min_x(), blk_box.min_y(), blk_box.min_z(), vpgl_lvcs::wgs84, min_lon, min_lat, min_alt);
    lv.local_to_global(blk_box.max_x(), blk_box.max_y(), blk_box.max_z(), vpgl_lvcs::wgs84, max_lon, max_lat, max_alt);
    // if current block is out of current tile dem image, put it directly
    vgl_box_2d<float> blk_box_2d(vgl_point_2d<float>((float)min_lon, (float)min_lat),
                                 vgl_point_2d<float>((float)max_lon, (float)max_lat));

    // if current block is out of current img range, put it directly
    vgl_box_2d<float> intersect_box = vgl_intersection(tbbox, blk_box_2d);
    if (intersect_box.is_empty())
    {
      changed_scene_blks[blk_id] = md;
      continue;
    }
    // intersect with image, check the land cover for current blk
    unsigned key = (blk_id.i() + blk_id.j())*(blk_id.i() + blk_id.j() + 1)/2 + blk_id.j();
    volm_osm_category_io::geo_cover_values land_cover = volm_osm_category_io::GEO_BARREN;
    if (blk_land_cover.find(key) == blk_land_cover.end()) {
      // obtain pixels for current block
      unsigned min_ni, min_nj, max_ni, max_nj;
      tile.global_to_img(intersect_box.min_x(), intersect_box.min_y(), min_ni, min_nj);
      tile.global_to_img(intersect_box.max_x(), intersect_box.max_y(), max_ni, max_nj);
      unsigned bd_min_ni = min_ni;  unsigned bd_min_nj = min_nj;
      unsigned bd_max_ni = max_ni;  unsigned bd_max_nj = max_nj;
      if (min_ni > max_ni) {  bd_min_ni = max_ni;  bd_max_ni = min_ni;  }
      if (min_nj > max_nj) {  bd_min_nj = max_nj;  bd_max_nj = min_nj;  }
      // look for the land cover of current block (consider urban or agriculture here)
      bool loop = true;
      // if any pixel is urban, the land cover for this blk is urban
      // else if any pixel is agriculture, the land cover for this blk is agriculture
      // if no urban and no agriculture, the land cover for this blk is barren land, i.e., the lowest max_level, largest dims
      for (unsigned i = bd_min_ni; i <= bd_max_ni && loop; i++) {
        for (unsigned j = bd_min_nj; j <= bd_max_nj && loop; j++) {
          if ( (*img)(i,j) == volm_osm_category_io::GEO_URBAN) {
            land_cover = volm_osm_category_io::GEO_URBAN;  loop = false;
          }
          else if ( (*img)(i,j) == volm_osm_category_io::GEO_AGRICULTURE_GENERAL || (*img)(i,j) == volm_osm_category_io::GEO_AGRICULTURE_GENERAL) {
            if (land_cover != volm_osm_category_io::GEO_URBAN) {
              land_cover = volm_osm_category_io::GEO_AGRICULTURE_GENERAL;
            }
          }
        }
      }
      // update the land cover for current column of blocks
      blk_land_cover.insert(std::pair<unsigned, volm_osm_category_io::geo_cover_values>(key, land_cover));

    }
    else {
      land_cover = blk_land_cover.find(key)->second;
    }
    // update the blk resolution/ max_level base on the searched land_cover
    std::cout << "for blk " << blk_id << " land_cover is " << land_cover << std::endl;
    change_block_metadata(md, land_cover, refine_coefficient);
    cnt++;
    changed_scene_blks[blk_id] = md;
  }
  std::cout << " number of blocks whose max level changed " << cnt << std::endl;

  // output
  i=0;  // store scene smart pointer
  pro.set_output_val<boxm2_scene_sptr>(i++, changed_scene);
  return true;
}
