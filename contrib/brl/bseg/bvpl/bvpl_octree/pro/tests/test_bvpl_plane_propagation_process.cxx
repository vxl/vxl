#include <iostream>
#include <vector>
#include <testlib/testlib_test.h>
//:
// \file
// \brief  This test creates a tree with 8 leaf cells and inserts only one plane to each
//         After the boxm_plane_propagation_process is applied, each cell will insert their
//         neighbors' observation planes and insert into itself. At the end each cell should
//         have 8 planes in the observation list since each cell has 7 neighbors (all the
//         other cells in the tree)
//
// \author Gamze Tunali
// \date   March 24, 2010
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <brdb/brdb_value.h>
#include <brdb/brdb_query.h>
#include <brdb/brdb_selection.h>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_macros.h>
#include <bprb/bprb_parameters_sptr.h>
#include <bprb/bprb_parameters.h>

#include <boct/boct_tree_cell.h>
#include <boct/boct_tree.h>

#include <boxm/boxm_scene.h>
#include <boxm/sample/boxm_edge_tangent_sample.h>
#include <bvpl/bvpl_octree/pro/bvpl_octree_processes.h>
#include <vul/vul_file.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

static void test_bvpl_plane_propagation_process()
{
  typedef boxm_edge_tangent_sample<float> data_type;
  typedef boct_tree<short,data_type > tree_type;
  typedef boxm_scene<boct_tree<short,data_type > > scene_type;

  // create the main scene
  vpgl_lvcs lvcs(33.33,44.44,10.0, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
  vgl_point_3d<double> origin(10,10,20);
  vgl_vector_3d<double> block_dim(10,10,10);
  vgl_vector_3d<unsigned> world_dim(1,1,1);
  scene_type scene(lvcs, origin, block_dim, world_dim);
  scene.set_octree_levels(3,2);
  scene.set_paths("./boxm_scene", "block");
  scene.set_appearance_model(BOXM_EDGE_LINE);
  x_write(std::cout, scene, "scene");
  vul_file::make_directory("./boxm_scene");
  scene.write_scene();

  boxm_block_iterator<boct_tree<short,data_type > > iter(&scene);
  while (!iter.end())
  {
    scene.load_block(iter.index().x(),iter.index().y(),iter.index().z());
    boxm_block<boct_tree<short,data_type > > * block=scene.get_active_block();
    boct_tree<short,data_type > * tree= block->get_tree();

    // get the leaf nodes and insert one plane to each
    std::vector<boct_tree_cell<short,data_type >*> cells=tree->leaf_cells();
    for (unsigned i=0; i<cells.size(); i++) {
      boxm_plane_obs<float> p(vgl_homg_plane_3d<float>(float(i+1),0.f,0.f,1.f), 1.0f);
      data_type data = cells[i]->data();
      data.insert(p);
      cells[i]->set_data(data);
    }
    block->init_tree(tree);
    block->get_tree()->print();
    scene.write_active_block();
    iter++;
  }

  DECLARE_FUNC_CONS(bvpl_plane_propagate_process);
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_plane_propagate_process, "bvplPlanePropagateProcess");
  REGISTER_DATATYPE(boxm_scene_base_sptr);

  //: set the inputs
  brdb_value_sptr v0 = new brdb_value_t<boxm_scene_base_sptr>(new boxm_scene<boct_tree<short,data_type > >(scene));
  brdb_value_sptr v1 = new brdb_value_t<std::string>(scene.path());
  brdb_value_sptr v2 = new brdb_value_t<std::string>("new_scene");
  brdb_value_sptr v3 = new brdb_value_t<std::string>("new_scene.xml");

  //: inits with the default params
  bool good = bprb_batch_process_manager::instance()->init_process("bvplPlanePropagateProcess");
  good = good && bprb_batch_process_manager::instance()->set_input(0, v0);
  good = good && bprb_batch_process_manager::instance()->set_input(1, v1);
  good = good && bprb_batch_process_manager::instance()->set_input(2, v2);
  good = good && bprb_batch_process_manager::instance()->set_input(3, v3);
  good = good && bprb_batch_process_manager::instance()->run_process();

  unsigned id;
  good = good && bprb_batch_process_manager::instance()->commit_output(0, id);
  TEST("process commits output", good, true);

  // check if the results are in DB
  brdb_query_aptr Q = brdb_query_comp_new("id", brdb_query::EQ, id);
  brdb_selection_sptr S = DATABASE->select("boxm_scene_base_sptr_data", std::move(Q));
  if (S->size()!=1){
    std::cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
             << " no selections\n";
  }
  brdb_value_sptr value;
  if (!S->get_value(std::string("value"), value)) {
    std::cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
             << " didn't get value\n";
  }
  bool non_null = (value != nullptr);
  TEST("the output scene non-null", non_null, true);

  auto* result=static_cast<brdb_value_t<boxm_scene_base_sptr>* >(value.ptr());
  boxm_scene_base_sptr scene_sptr = result->value();

  // get the new scene with updated cells
  auto *scene2 = static_cast<scene_type *>(scene_sptr.as_pointer());
  boxm_block_iterator<boct_tree<short,data_type > > iter2(scene2);

  bool obs_num_ok=true;
  while (!iter2.end())
  {
    scene2->load_block(iter2.index().x(),iter2.index().y(),iter2.index().z());
    boxm_block<tree_type > * block=scene2->get_active_block();
    tree_type * tree= block->get_tree();

    // get the leaf nodes and insert one plane to each
    std::vector<boct_tree_cell<short,data_type >*> nodes=tree->leaf_cells();
    for (auto & node : nodes) {
      unsigned num=node->data().num_obs();
      // each node has 8 neighbors in the 3x3 neighborhood including itself
      if (num != 8)
        obs_num_ok = false;
    }
    iter2++;
  }
  TEST("The number of expected observations at each cell", obs_num_ok, true);
}

TESTMAIN(test_bvpl_plane_propagation_process);
