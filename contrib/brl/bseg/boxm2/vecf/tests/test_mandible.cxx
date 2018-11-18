//:
// \file
// \author J.L. Mundy
// \date 08/28/15


#include <iostream>
#include <fstream>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_timer.h>
#include <vbl/vbl_array_3d.h>
#include <boxm2/cpp/algo/boxm2_surface_distance_refine.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/io/boxm2_cache.h>
#include <boct/boct_bit_tree.h>
#include "../boxm2_vecf_mandible_params.h"
#include "../boxm2_vecf_mandible.h"
#include "../boxm2_vecf_mandible_scene.h"
#include "../boxm2_vecf_spline_field.h"
//#define BUILD_TEST_MANDIBLE
void test_mandible()
{
#ifdef BUILD_TEST_MANDIBLE

  std::string base_dir = "c:/Users/mundy/VisionSystems/Janus/RelevantPapers/FacialMusclesExpression/skull/";
  std::string scene_dir = "c:/Users/mundy/VisionSystems/Janus/experiments/vector_flow/mandible/";
  std::string axis_path4 = base_dir  + "mandible_spline_2x_zero_samp_1.0_r35_norm.txt";
  std::string axis_display_path = base_dir  + "mandible_axis_vrml.wrl";
  std::string cross_path = base_dir + "mandible-2x-zero-samp-1.0-r35-norm.txt";
  std::string cross_display_path = base_dir + "mandible_display_cross.wrl";
  std::string cross_normal_display_path = base_dir + "mandible_display_cross_normal.wrl";
  std::string cross_disks_display_path = base_dir + "mandible_display_cross_disks.wrl";

#if 0
  std::ifstream istr(axis_path4.c_str());
   if(!istr){
     std::cout << "Bad axis file path " << axis_path4 << '\n';
     return;
   }
  std::ifstream cistr(cross_path.c_str());
  if(!cistr){
    std::cout << "Bad cross file path " << cross_path << '\n';
    return;
  }

  boxm2_vecf_mandible mand;
  mand.read_axis_spline(istr);
  mand.load_cross_section_pointsets(cistr);


  // translate the mandible axis
  vgl_vector_3d<double> v(-3.46635, 0.0, 0.0);
  mand.translate(v);
  std::ofstream ostr(axis_path1.c_str());
  ostr << mand.axis();
  ostr.close();

  // scale the mandible along x
  double offset = 5.0;
  mand.offset_axis(offset);
  // tilt the ramus
  double dz_at_top = 5.0;
  mand.tilt_ramus(dz_at_top);

  double dy_at_chin = 5.0;
  mand.tilt_body(dy_at_chin);
#endif

#if 0
  vgl_point_3d<double> pt(-0.746202,-61.601116,78.412971),pc;
  vgl_point_3d<double> pto(-0.746202,-51.601116,78.412971),pco;
  bool good = mand.closest_point(pt, pc);
  good = mand.closest_point(pto, pco);
  double d = (pto-pco).length();
#endif
   //
   // deformation goes here
    // scale the mandible along x
#if 0
  double offset = 15.0;
  boxm2_vecf_spline_field field = mand.offset_axis(offset);

  boxm2_vecf_mandible mand_v = mand.apply_vector_field(field);

    // display the mandible axis
  std::ofstream vostr(axis_display_path.c_str());
  if(!vostr){
    std::cout << "Bad axis display file path " << axis_display_path << '\n';
    return;
  }
  mand.display_axis_spline(vostr);

   //
   // display the cross section plane normals
  std::ofstream cnostr(cross_normal_display_path.c_str());
  if(!cnostr){
    std::cout << "Bad cross section normal  display file path " << cross_normal_display_path << '\n';
    return;
  }
  mand.display_cross_section_planes(cnostr);

  // display the mandible cross section pointsets
  std::ofstream costr(cross_display_path.c_str());
  if(!costr){
    std::cout << "Bad cross section display file path " << cross_display_path << '\n';
    return;
  }
  mand.display_cross_section_pointsets(costr);

  std::ofstream cdostr(cross_disks_display_path.c_str());
  if(!cdostr){
    std::cout << "Bad cross section display file path " << cross_disks_display_path << '\n';
    return;
  }
  mand.display_surface_disks(cdostr);
#endif
  std::string mandible_scene_path = scene_dir + "mandible_8.xml";
  if(!vul_file::exists(mandible_scene_path))
    {
      std::cout<<"mandible scene file) does not exist"<<std::endl;
      return;
    }

  std::string mandible_geo_path = scene_dir + "mandible_geo.txt";
  if(!vul_file::exists(mandible_geo_path))
    {
      std::cout<<"mandible geometry file) does not exist"<<std::endl;
      return;
    }

  //boxm2_vecf_mandible mand(mandible_geo_path);

  // boxm2_vecf_mandible_scene mscene(mandible_scene_path, mandible_geo_path);
  boxm2_vecf_mandible_scene mscene(mandible_scene_path);
  //  const boxm2_vecf_mandible& mand = mscene.mandible_geo();
  boxm2_scene_sptr scene = mscene.scene();
  if(!scene)
    return;
  //boxm2_cache::instance()->write_to_disk();

  std::vector<std::string> prefixes;

  prefixes.push_back("alpha");
  prefixes.push_back("boxm2_mog3_grey");
  prefixes.push_back("boxm2_num_obs");
  prefixes.push_back("boxm2_pixel_mandible");

  //boxm2_surface_distance_refine<boxm2_vecf_mandible>(mand, scene, prefixes);
  //boxm2_surface_distance_refine<boxm2_vecf_mandible>(mand, scene, prefixes);
 // boxm2_surface_distance_refine<boxm2_vecf_mandible>(mand, scene, prefixes);
 // boxm2_cache::instance()->write_to_disk();
 // mscene.rebuild();
#if 0
  std::map<boxm2_block_id, boxm2_block_metadata> blocks = scene->blocks();
  std::map<boxm2_block_id, boxm2_block_metadata>::iterator bit = blocks.begin();
  boxm2_block_id id = bit->first;
  boxm2_block * blk = boxm2_cache::instance()->get_block(scene,id);

  // test leaf neighbors
  vgl_point_3d<double> probe(3.355096, -71.952629, 78.2);
  std::vector<vgl_point_3d<double> > nbrs;
  std::vector<double> nbr_edge_lengths;
  std::vector<unsigned> data_indices;
  blk->leaf_neighbors(probe, 1.5, nbrs, nbr_edge_lengths, data_indices);
#endif
#if 0
  std::string target_scene_path = scene_dir + "target_mandible_8.xml";
//std::string target_scene_path = scene_dir + "../skull/target_skull_8.xml";

  if(!vul_file::exists(target_scene_path))
    {
      std::cout<<"target scene file) does not exist"<<std::endl;
      return;
    }
  boxm2_scene_sptr target = new boxm2_scene(target_scene_path);
  if(!target)
    return;
  mscene.map_to_target(target);
  boxm2_cache::instance()->write_to_disk();
#endif

#if 0
  std::map<boxm2_block_id, boxm2_block_metadata> tblocks = target->blocks();
  std::map<boxm2_block_id, boxm2_block_metadata>::iterator bitrg = tblocks.begin();
  boxm2_block_id tid = bitrg->first;
  boxm2_block_sptr blk_trg = boxm2_cache::instance()->get_block(target,tid);
  boxm2_array_3d<uchar16> trees = blk->trees_copy();
  uchar16 tree_bits = trees(8, 6, 16);
  boct_bit_tree cur_tree( (unsigned char*)tree_bits.data_block(), 4); // the bit tree with the required refinement
  std::cout << "Refined tree depth " << cur_tree.depth() << '\n';
  std::cout << cur_tree << '\n';
  const boxm2_array_3d<uchar16>& ttrees = blk_trg->trees();
  std::size_t n1 = ttrees.get_row1_count(), n2 = ttrees.get_row2_count(), n3 = ttrees.get_row3_count();
  vbl_array_3d<int> required_depth(n1, n2, n3, 0);
  required_depth(8, 6, 16) = 2;
  vul_timer t;
  boxm2_match_refine(target, blk_trg, required_depth);
  std::cout << "Time to match refine " << t.real() << " msecs.\n";
  uchar16 trg_bits = ttrees(8,6,16);
  boct_bit_tree ref_tree((unsigned char*)trg_bits.data_block(),4);
  std::cout << "ref_tree \n" << ref_tree << '\n';
#endif
#endif
}
TESTMAIN( test_mandible );
