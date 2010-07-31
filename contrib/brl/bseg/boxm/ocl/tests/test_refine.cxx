#include <testlib/testlib_test.h>
//:
// \file
#include <testlib/testlib_root_dir.h>
#include "open_cl_test_data.h"
#include <boxm/ocl/boxm_refine_manager.h>
#include <boxm/ocl/boxm_refine_scene_manager.h>
#include <boxm/boxm_scene.h>
#include <boct/boct_tree.h>
#include <boct/boct_tree_cell.h>
#include <boct/boct_loc_code.h>
#include <vnl/vnl_vector_fixed.h>
#include <vcl_vector.h>
#include <vcl_stack.h>
#include <vcl_cstring.h> //(memcpy)
#include <boxm/ocl/boxm_ocl_utils.h>
#include <boxm/algo/boxm_refine.h>


bool test_refine_apl()
{
  typedef vnl_vector_fixed<int,4> int4;
  typedef vnl_vector_fixed<float,16> float16;

  vcl_cout<<"Comparing single block of APL scene "<<vcl_endl;

  //load apl scene
  vcl_string cpu_path = "/media/VXL/data/APl/try4/scene_refined/scene.xml";
  boxm_ocl_scene cpu_scene(cpu_path);
  vcl_cout<<cpu_scene<<vcl_endl;
  cpu_scene.save();

  vcl_string gpu_path = "/media/VXL/data/APl/try4/small_block_refined/scene.xml";
  boxm_ocl_scene gpu_scene(gpu_path);

  //compare data scene printing
  float16 ssd(0.0);
  float16 diffCount(0.0);
  vbl_array_3d<int4> cpu_blocks = cpu_scene.blocks();
  vbl_array_3d<int4> gpu_blocks = gpu_scene.blocks();
  vbl_array_2d<int4> cpu_buffer = cpu_scene.tree_buffers();
  vbl_array_2d<int4> gpu_buffer = gpu_scene.tree_buffers();
  vbl_array_2d<float16> cpu_data = cpu_scene.data_buffers();
  vbl_array_2d<float16> gpu_data = gpu_scene.data_buffers();
  for (unsigned int i=0; i<cpu_blocks.get_row1_count(); i++) {
    for (unsigned int j=0; j<cpu_blocks.get_row2_count(); j++) {
      for (unsigned int k=0; k<cpu_blocks.get_row3_count(); k++)
      {
        //get tree info for CPU block
        int cpuBuffIndex = cpu_blocks[i][j][k][0];
        int cpuBuffOffset = cpu_blocks[i][j][k][1];
        int cpuBlkSize = cpu_blocks[i][j][k][2];

        //get tree info for GPU Block
        int gpuBuffIndex = gpu_blocks[i][j][k][0];
        int gpuBuffOffset = gpu_blocks[i][j][k][1];
        int gpuBlkSize = gpu_blocks[i][j][k][2];

        //make sure they agree
        if (gpuBlkSize != cpuBlkSize) {
          vcl_cerr<<"Tree @ block ("<<i<<','<<j<<','<<k<<") doesn't match\n";
          return false;
        }
#if 0
        vcl_cout<<"BLK @ ("<<i<<','<<j<<','<<k<<") with size "<<gpuBlkSize<<vcl_endl;
#endif
        //compare the tree's data
        //tree cell is (parent, child, data, block)
        for (int l=0; l<gpuBlkSize; l++) {
#if 0
          vcl_cout<<"  cell @ "<<l<<'\n'
                  <<"    cpu cell "<<cpu_buffer[cpuBuffIndex][cpuBuffOffset+l]<<'\n'
                  <<"    gpu cell "<<gpu_buffer[gpuBuffIndex][gpuBuffOffset+l]<<vcl_endl;
#endif
          int cpuDatIndex = cpu_buffer[cpuBuffIndex][cpuBuffOffset+l][2];
          int gpuDatIndex = gpu_buffer[gpuBuffIndex][gpuBuffOffset+l][2];
          int childPtr = cpu_buffer[cpuBuffIndex][cpuBuffOffset+l][1];
          if (cpuDatIndex > 0 && gpuDatIndex > 0 && childPtr < 0) {
            float16 cpuDatCell = cpu_data[cpuBuffIndex][cpuDatIndex];
            float16 gpuDatCell = gpu_data[gpuBuffIndex][gpuDatIndex];
#if 0
            vcl_cout<<"    "<<gpuBlkSize<<','<<cpuBlkSize<<','<<l<<' '
                    <<"Alphas: (cpu,gpu) ("<<cpuDatCell[0]<<','<<gpuDatCell[0]<<')'<<vcl_endl;
#endif
            //sum up squared differences
            for (int i=0; i<16; i++) {
              ssd[i] += (cpuDatCell[i] - gpuDatCell[i]) * (cpuDatCell[i] - gpuDatCell[i]);
            }
          }
        }
      }
    }
  }

  vcl_cout<<"Data SSD = "<<ssd<<vcl_endl;
  return true;
}


boxm_scene<boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > > create_simple_scene()
{
  typedef boxm_sample<BOXM_APM_MOG_GREY> data_type;
  typedef boct_tree<short, data_type> tree_type;
  typedef boxm_scene<tree_type> scene_type;

  bgeo_lvcs lvcs(33.33,44.44,10.0, bgeo_lvcs::wgs84, bgeo_lvcs::DEG, bgeo_lvcs::METERS);
  vgl_point_3d<double> origin(0,0,0);
  vgl_vector_3d<double> block_dim(10,10,10);
  vgl_vector_3d<unsigned> world_dim(1,1,1);
  scene_type scene(lvcs, origin, block_dim, world_dim);
  scene.set_appearance_model(BOXM_APM_MOG_GREY);

  vcl_string scene_dir = vcl_string(VCL_SOURCE_ROOT_DIR)+"/contrib/brl/bseg/boxm/ocl/tests/boxm_scene1";
  vcl_string xml_path = scene_dir + "/scene1.xml";
  scene.set_paths(scene_dir, "block");
  vul_file::make_directory(scene_dir);
  vcl_ofstream os(xml_path.c_str());
  x_write(os, scene, "scene");
  os.close();

  unsigned max_level=10, init_level=7;
  scene.set_octree_levels(max_level, init_level);

  // default model (alpha = .001)
  bsta_gauss_f1 simple_gauss_f1(0.0f,0.1f);
  bsta_num_obs<bsta_gauss_f1> simple_obs_gauss_val_f1(simple_gauss_f1,1);
  bsta_mixture_fixed<bsta_num_obs<bsta_gauss_f1>, 3>  simple_mix_gauss_val_f1;

  simple_mix_gauss_val_f1.insert(simple_obs_gauss_val_f1,0.1f);
  simple_mix_gauss_val_f1.insert(simple_obs_gauss_val_f1,0.1f);
  simple_mix_gauss_val_f1.insert(simple_obs_gauss_val_f1,0.1f);

  typedef bsta_mixture_fixed<bsta_num_obs<bsta_gauss_f1>,3>  simple_bsta_mixture_fixed_f1_3;
  bsta_num_obs<simple_bsta_mixture_fixed_f1_3>  simple_obs_mix_gauss_val_f1(simple_mix_gauss_val_f1);

  data_type default_sample;
  default_sample.alpha=0.11f;
  default_sample.set_appearance(simple_obs_mix_gauss_val_f1);

  // sample 1 (alpha = .6)
  bsta_gauss_f1 s1_simple_gauss_f1(0.5f,0.1f);
  bsta_num_obs<bsta_gauss_f1> s1_simple_obs_gauss_val_f1(s1_simple_gauss_f1,1);
  bsta_mixture_fixed<bsta_num_obs<bsta_gauss_f1>, 3>  s1_simple_mix_gauss_val_f1;

  s1_simple_mix_gauss_val_f1.insert(s1_simple_obs_gauss_val_f1,1);

  typedef bsta_mixture_fixed<bsta_num_obs<bsta_gauss_f1>,3>  s1_simple_bsta_mixture_fixed_f1_3;
  bsta_num_obs<s1_simple_bsta_mixture_fixed_f1_3>  s1_simple_obs_mix_gauss_val_f1(s1_simple_mix_gauss_val_f1);

  data_type s1_sample;
  s1_sample.alpha=6.0f;
  s1_sample.set_appearance(s1_simple_obs_mix_gauss_val_f1);

  // sample 2 (alpha = .6)
  bsta_gauss_f1 s2_simple_gauss_f1(1.0f,0.1f);
  bsta_num_obs<bsta_gauss_f1> s2_simple_obs_gauss_val_f1(s2_simple_gauss_f1,1);
  bsta_mixture_fixed<bsta_num_obs<bsta_gauss_f1>, 3>  s2_simple_mix_gauss_val_f1;

  s2_simple_mix_gauss_val_f1.insert(s2_simple_obs_gauss_val_f1,1);

  typedef bsta_mixture_fixed<bsta_num_obs<bsta_gauss_f1>,3>  s2_simple_bsta_mixture_fixed_f1_3;
  bsta_num_obs<s2_simple_bsta_mixture_fixed_f1_3>  s2_simple_obs_mix_gauss_val_f1(s2_simple_mix_gauss_val_f1);

  data_type s2_sample;
  s2_sample.alpha=6.0f;
  s2_sample.set_appearance(s2_simple_obs_mix_gauss_val_f1);

  float count=6.0f;
  boxm_block_iterator<tree_type> iter(&scene);
  while (!iter.end())
  {
    scene.load_block(iter.index().x(),iter.index().y(),iter.index().z());
    boxm_block<tree_type>* block=scene.get_active_block();
    tree_type* tree;
    //make first block one level further initialized than others
    if (iter.index().x()==0 && iter.index().y()==0 && iter.index().z()==0) {
      tree = new tree_type(max_level,init_level);
    }
    else {
      tree = new tree_type(max_level,init_level-1);
    }
    boct_tree_cell<short,data_type>* cel11 = tree->locate_point(vgl_point_3d<double>(0.01,0.01,0.9));
    s2_sample.alpha=count;
    cel11->set_data(s2_sample);
#if 0
    boct_tree_cell<short,data_type>* cell2=tree->locate_point(vgl_point_3d<double>(0.51,0.51,0.51));
    cell2->set_data(s2_sample);
#endif
    block->init_tree(tree);
    scene.write_active_block();
    iter++;
    count+=0.1f;
  }

  return scene;
}

//: Test refine on a simple ocl scene
bool test_refine_ocl_scene()
{
  vcl_cout<<vcl_endl<<"Testing multi block refine "<<vcl_endl;
  float prob_thresh = .001;

  //set up multiple blocks of small trees
  typedef boxm_sample<BOXM_APM_MOG_GREY> data_type;
  typedef boct_tree<short,data_type> tree_type;
  typedef boxm_scene<tree_type> scene_type;

  //cpu refine test
  scene_type scene = create_simple_scene();
  boxm_refine_scene(scene, prob_thresh);

  //GPU refine test
  scene_type scene2 = create_simple_scene();
  int num_buffers = 1;
  boxm_ocl_scene ocl_scene;
  boxm_ocl_convert<data_type>::convert_scene(&scene2, num_buffers, ocl_scene, 100);
  vcl_cout<<ocl_scene<<vcl_endl;

  //create the manager and startup the refining
  boxm_refine_scene_manager* mgr = boxm_refine_scene_manager::instance();
  mgr->init_refine(&ocl_scene, prob_thresh);
  mgr->run_refine();
  boxm_ocl_scene* scene_ptr = mgr->get_scene();

  vcl_cout<<"Scene ptr "<<scene_ptr<<" ?= scene address "<<&ocl_scene<<'\n'
          <<ocl_scene<<vcl_endl;

  return true;
}


bool test_refine_simple_scene()
{
  // Set up test tree
  typedef boxm_sample<BOXM_APM_MOG_GREY> data_type;
  typedef boct_tree<short,data_type> tree_type;
  tree_type* tree = open_cl_test_data::simple_tree<data_type>();
  float prob_thresh = .3f;

  //-------- GPU side: refine the scene using the opencl refine manager -------
  boxm_refine_manager<data_type>* mgr = boxm_refine_manager<data_type>::instance();
  if (!mgr->init(tree, prob_thresh)) {
    TEST("Error : boxm_refine : mgr->init() failed\n", false, true);
    return false;
  }
  if (!mgr->run_tree()) {
    TEST("Error : boxm_refine : mgr->run_tree() failed\n", false, true);
    return false;
  }

  // extract the output scene from the manager
  int* tree_array = mgr->get_tree();
  int  tree_size = mgr->get_tree_size();
  float* data = mgr->get_data();
  unsigned int  data_size = mgr->get_data_size();
  if (!tree_array) {
    TEST("Error : boxm_refine : mgr->get_tree() returned NULL\n", false, true);
    return false;
  }
  if (!tree_size) {
    TEST("ERROR : boxm_refine : mgr->get_tree_size() returned NULL\n", false, true);
    return false;
  }

  // Verify that the tree is formatted correctly
  vcl_vector<vnl_vector_fixed<int,4> > tree_vector;
  for (int i=0,j=0; j<tree_size; i+=4,j++) {
    vnl_vector_fixed<int,4> cell;
    for (unsigned k=0; k<4; k++)
      cell[k] = tree_array[i+k];
    tree_vector.push_back(cell);
  }
  bool correctFormat = boxm_ocl_utils::verify_format(tree_vector);
  TEST("test_refine_simple_scene output format", correctFormat, true);


  //--------- CPU side refine ------------------------------
  unsigned num_split = 0;
  typedef boxm_block<tree_type> block_type;
  block_type block(tree->bounding_box(), tree);
  boxm_refine_block(&block, prob_thresh, num_split);

  //use vectors to build the tree up
  vcl_vector<vnl_vector_fixed<int, 4> > cell_input;
  vcl_vector<vnl_vector_fixed<float, 16>  > data_input;

  // put the root into the cell array and its data in the data array
  int cell_ptr = 0;
  vnl_vector_fixed<int, 4> root_cell(0);
  root_cell[0]=-1; // no parent
  root_cell[1]=-1; // no children at the moment
  root_cell[2]=-1; // no data at the moment
  cell_input.push_back(root_cell);
  boct_tree_cell<short,data_type>* root = tree->root();
  boxm_ocl_convert<data_type>::copy_to_arrays(root, cell_input, data_input, cell_ptr);

  //verify that tree_vector and cell_input are same size
  TEST("CPU refine and GPU refine tree output same size ", (tree_vector.size()), cell_input.size());
  TEST("CPU refine and GPU refine data output same size ", (data_input.size()), data_size);

  //Verify the tree's structure is correct
  bool good = true;
  for (unsigned i=0; i<tree_vector.size(); i++){
    for (int j=0; j<2; j++) //0 and 1 are parent and child pointers
      good = good && (tree_vector[i][j] == cell_input[i][j]);
  }
  TEST("CPU/GPU refine tree output same parent/child pointers ", good, true);

  //verify that the data for each node is the same
  float ssd = 0;
  for (unsigned i=0; i<tree_vector.size(); i++){

    //cpu side data
    int dataIndex = cell_input[i][2];
    vnl_vector_fixed<float, 16> datum = data_input[dataIndex];

    //gpu side data
    dataIndex = 16*tree_vector[i][2];

    //compare
    for (int j=0; j<16; j++)
      ssd += (datum[j]-data[dataIndex+j])*(datum[j]-data[dataIndex+j]);
  }
  TEST("CPU/GPU refine tree output same data ", (ssd<10e-8), true);
  vcl_cout<<"SSD between cpu/gpu data = "<<ssd<<vcl_endl;

  // free memory used by the manager
  mgr->clean_refine();
  return true;
}

static void test_refine()
{
#if 0
  if (test_refine_simple_scene())
    vcl_cout<<"test_refine, simple scene"<<vcl_endl;
  if (test_refine_ocl_scene())
    vcl_cout<<"test_multi_block_refine, simple scene"<<vcl_endl;
#endif

  test_refine_apl();
}

TESTMAIN(test_refine);
