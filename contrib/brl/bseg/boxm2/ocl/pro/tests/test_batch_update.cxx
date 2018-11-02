//:
// \file
// \author Vishal JAin
// \date 01-Mar-2010
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#include <vcl_where_root_dir.h>

#include <bocl/bocl_manager.h>
#include <bocl/bocl_buffer_mgr.h>
#include <bocl/bocl_kernel.h>
#include <bocl/bocl_mem.h>
#include <boxm2/ocl/pro/boxm2_ocl_processes.h>

#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/io/boxm2_cache.h>
#include <boxm2/io/boxm2_lru_cache.h>

#include <boxm2/basic/boxm2_block_id.h>
#include <boxm2/io/boxm2_sio_mgr.h>
#include <vnl/vnl_vector_fixed.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <brdb/brdb_value.h>

class boxm2_process
{
 public:
  bool init_kernel(_cl_context**, _cl_device_id**, std::string const&) { return false; }
};

void test_batch_update_kernels()
{
  //----------------------------------------------------------------------------
  //--- BEGIN BOXM2 API EXAMPLE ------------------------------------------------
  //----------------------------------------------------------------------------
  //start out reading the scene.
  std::string test_dir  = testlib_root_dir()+ "/contrib/brl/bseg/boxm2/ocl/pro/tests/";
  std::string test_dir2  ="f:/visdt/";
  std::string test_file = test_dir2 + "boxm2_empty/scene.xml";

  boxm2_scene_sptr scene = new boxm2_scene(test_file);

  //initialize a block and data cache
  boxm2_lru_cache::create(scene);
  boxm2_cache_sptr cache = boxm2_cache::instance();

#if 0 // boxm2_opencl_processor does not exist anymore
  //initialize gpu pro / manager
  boxm2_opencl_processor* gpu_pro = boxm2_opencl_processor::instance();
  gpu_pro->set_scene(scene.ptr());
  gpu_pro->set_cpu_cache(cache);
  gpu_pro->init();
  std::string update_opts=" -D MOG_TYPE_8 ";
  boxm2_process update;
  update.init_kernel(&gpu_pro->context(), &gpu_pro->devices()[0],update_opts);

  std::string cam_dir=test_dir2+"cams24";
  std::string img_dir=test_dir2+"imgs24";
  std::vector<brdb_value_sptr> input, output;

  brdb_value_sptr brdb_scene = new brdb_value_t<boxm2_scene_sptr>(scene);
  brdb_value_sptr brdb_camdir = new brdb_value_t<std::string>(cam_dir);
  brdb_value_sptr brdb_imgdir = new brdb_value_t<std::string>(img_dir);

  input.push_back(brdb_scene);
  input.push_back(brdb_camdir);
  input.push_back(brdb_imgdir);

  gpu_pro->run(input, output);
#endif
  std::map<boxm2_block_id, boxm2_block_metadata> blk_map=scene->blocks();

  std::map<boxm2_block_id, boxm2_block_metadata>::iterator iter=blk_map.begin();
  typedef vnl_vector_fixed<unsigned char, 16> uchar16;    //defines a bit tree
  typedef vnl_vector_fixed<unsigned char, 8> uchar8;    //defines a bit tree
  typedef vnl_vector_fixed<float , 4> float4;    //defines a bit tree
  typedef vnl_vector_fixed<float , 8> float8;    //defines a bit tree

  for (;iter!=blk_map.end();iter++)
  {
    boxm2_block *     blk     = cache->get_block(scene, iter->first);
    boxm2_array_3d<uchar16> trees=blk->trees();

    std::cout<<" DATA buffers "<< blk->num_buffers()<<std::endl;
    boxm2_data_base * data_base = cache->get_data_base(scene, iter->first,boxm2_data_traits<BOXM2_AUX>::prefix());
    boxm2_data<BOXM2_AUX> *aux_data=new boxm2_data<BOXM2_AUX>(data_base->data_buffer(),data_base->buffer_length(),data_base->block_id());

    boxm2_data_base * hist_base = cache->get_data_base(scene, iter->first,boxm2_data_traits<BOXM2_BATCH_HISTOGRAM>::prefix());
    boxm2_data<BOXM2_BATCH_HISTOGRAM> *hist_data=new boxm2_data<BOXM2_BATCH_HISTOGRAM>(hist_base->data_buffer(),hist_base->buffer_length(),hist_base->block_id());
    boxm2_data_base * alpha_data_base  = cache->get_data_base(scene, iter->first,boxm2_data_traits<BOXM2_ALPHA>::prefix());
    boxm2_data<BOXM2_ALPHA> *alpha_data =new boxm2_data<BOXM2_ALPHA>(alpha_data_base->data_buffer(),alpha_data_base->buffer_length(),alpha_data_base->block_id());

    boxm2_array_1d<float4> data=aux_data->data();
    boxm2_array_1d<float8> hist_data_array=hist_data->data();
    boxm2_array_1d<float>  alpha_data_array=alpha_data->data();

    float sumP=0.0;
    int count =0;
    for (unsigned int ti=0;ti<trees.get_row1_count();ti++)
    {
      for (unsigned int tj=0;tj<trees.get_row2_count();tj++)
      {
        for (unsigned int tk=0;tk<trees.get_row3_count();tk++)
        {
          uchar16 curr_tree=trees[ti][tj][tk];
          int buff_offset=(int)curr_tree[10];
          buff_offset<<=8;
          buff_offset+=(int)curr_tree[11];

          int buff_index=(int)curr_tree[12];
          buff_index<<=8;
          buff_index+=(int)curr_tree[13];
          if (tk==63)
          {
            std::cout<<ti<<','<<tj<<','<<tk<<" [";
            for (unsigned vecindex=0;vecindex<8;vecindex++)
              std::cout<<hist_data_array[(buff_index*65536+buff_offset)][vecindex]<<',';
            std::cout<<"] "
                    <<data[(buff_index*65536+buff_offset)/4][(buff_index*65536+buff_offset)%4]
                    <<"  " <<alpha_data_array[(buff_index*65536+buff_offset)]<<std::endl;
          }
          sumP+=alpha_data_array[(buff_index*65536+buff_offset)];
          count++;
        }
      }
    }
    std::cout<<"MEAN P "<<sumP/(float)count<<std::endl;
  }
}


void test_batch_update()
{
  test_batch_update_kernels();
}


TESTMAIN( test_batch_update );
