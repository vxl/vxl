// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_make_inside_voxels_empty_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for making the inside cells empty
//
// TODO: implement a vis_sphere initializer kernel.
// \author Ali Osman Ulusoy
// \date Oct 10, 2011

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boxm2/ocl/algo/boxm2_ocl_camera_converter.h>

//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vul/vul_timer.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>

namespace boxm2_ocl_make_inside_voxels_empty_process_globals
{
  constexpr unsigned n_inputs_ = 4;
  constexpr unsigned n_outputs_ = 0;
  enum {
      COMPUTE_VIS = 0,
      DECIDE_INSIDE = 1
  };

  void compile_kernel(const bocl_device_sptr& device,std::vector<bocl_kernel*> & vec_kernels,const std::string& opts)
  {
    //gather all render sources... seems like a lot for rendering...
    std::vector<std::string> src_paths;
    std::string source_dir = boxm2_ocl_util::ocl_src_root();
    src_paths.push_back(source_dir + "scene_info.cl");
    src_paths.push_back(source_dir + "cell_utils.cl");
    src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
    src_paths.push_back(source_dir + "basic/sort_vector.cl");
    src_paths.push_back(source_dir + "backproject.cl");
    src_paths.push_back(source_dir + "statistics_library_functions.cl");
    src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
    src_paths.push_back(source_dir + "bit/compute_vis.cl");
    src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");

    //compilation options
    std::string options = opts+ "-D INTENSITY ";

    auto* compute_vis = new bocl_kernel();
    std::string seg_opts = options + "-D COMPVIS -D STEP_CELL=step_cell_computevis(aux_args,data_ptr,llid,d)";
    compute_vis->create_kernel(&device->context(),device->device_id(), src_paths, "compute_vis", seg_opts, "compute_vis");
    vec_kernels.push_back(compute_vis);

    auto* decide_inside_cell = new bocl_kernel();
    decide_inside_cell->create_kernel(&device->context(),device->device_id(), src_paths, "decide_inside_cell", seg_opts, "decide_inside_cell");
    vec_kernels.push_back(decide_inside_cell);
    return ;
  }

  static std::map<std::string,std::vector<bocl_kernel*> > kernels;
}

bool boxm2_ocl_make_inside_voxels_empty_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_make_inside_voxels_empty_process_globals;

  //process takes 4 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "boxm2_scene_sptr";
  input_types_[2] = "boxm2_opencl_cache_sptr";
  input_types_[3] = "bool";

  // process has no outputs
  std::vector<std::string>  output_types_(n_outputs_);
  bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);

  return good;
}


bool boxm2_ocl_make_inside_voxels_empty_process(bprb_func_process& pro)
{
  using namespace boxm2_ocl_make_inside_voxels_empty_process_globals;
  std::size_t local_threads[2]={8,8};
  std::size_t global_threads[2]={8,8};

  //sanity check inputs
  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  float transfer_time=0.0f;
  float gpu_time=0.0f;

  //get the inputs
  unsigned i = 0;
  bocl_device_sptr         device = pro.get_input<bocl_device_sptr>(i++);
  boxm2_scene_sptr         scene = pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_opencl_cache_sptr  opencl_cache = pro.get_input<boxm2_opencl_cache_sptr>(i++);
  bool use_sum = false;    use_sum = pro.get_input<bool>(i++);

  //cache size sanity check
  long binCache = opencl_cache.ptr()->bytes_in_cache();
  std::cout<<"Update MBs in cache: "<<binCache/(1024.0*1024.0)<<std::endl;

  //make correct data types are here
  std::string data_type,num_obs_type,options;

  if (use_sum) {
    options="-D USESUM ";
    std::cout << "Using sum to compute visibility" << std::endl;
  }


  // create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue( device->context(),
                                                 *(device->device_id()),
                                                 CL_QUEUE_PROFILING_ENABLE,
                                                 &status);
  if (status!=0)
    return false;

  // compile the kernel if not already compiled
  std::string identifier=device->device_identifier()+options;
  if (kernels.find(identifier)==kernels.end()) {
    std::cout<<"===========Compiling kernels==========="<<std::endl;
    std::vector<bocl_kernel*> ks;
    compile_kernel(device,ks,options);
    kernels[identifier]=ks;
  }

  // bit lookup buffer
  cl_uchar lookup_arr[256];
  boxm2_ocl_util::set_bit_lookup(lookup_arr);
  bocl_mem_sptr lookup=new bocl_mem(device->context(), lookup_arr, sizeof(cl_uchar)*256, "bit lookup buffer");
  lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  // dodecahedron directions lookup buffer
  cl_float4 dodecahedron_dir[12];
  boxm2_ocl_util::set_dodecahedron_dir_lookup(dodecahedron_dir);
  bocl_mem_sptr dodecahedron_dir_lookup=new bocl_mem(device->context(), dodecahedron_dir, sizeof(cl_float4)*12, "dodecahedron directions lookup buffer");
  dodecahedron_dir_lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  cl_bool contain_point[1];
  bocl_mem_sptr contain_point_mem =new bocl_mem(device->context(), contain_point, sizeof(cl_bool), "contains point buffer");
  contain_point_mem->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  cl_uint datasize[1];
  bocl_mem_sptr datasize_mem =new bocl_mem(device->context(), datasize, sizeof(cl_uint), "data buffer size");
  datasize_mem->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  //zip through each block
  std::map<boxm2_block_id, boxm2_block_metadata> blocks = scene->blocks();
  std::map<boxm2_block_id, boxm2_block_metadata>::iterator blk_iter;
  for (unsigned int i=0; i<kernels[identifier].size(); ++i)
  {
      //remove all the alphas and points from opencl cache
      if (i == DECIDE_INSIDE) {
          for (blk_iter = blocks.begin(); blk_iter != blocks.end(); ++blk_iter)
          {
            boxm2_block_id id = blk_iter->first;
            //opencl_cache->shallow_remove_data(id,boxm2_data_traits<BOXM2_ALPHA>::prefix());
            opencl_cache->shallow_remove_data(scene,id,boxm2_data_traits<BOXM2_POINT>::prefix());
          }
      }

      for (blk_iter = blocks.begin(); blk_iter != blocks.end(); ++blk_iter)
      {
        boxm2_block_id id = blk_iter->first;
        std::cout << "Processing block: " << id << std::endl;

        //get kernel
        bocl_kernel* kern =  kernels[identifier][i];

        vul_timer transfer;

        //load normals
        bocl_mem* normals = opencl_cache->get_data<BOXM2_NORMAL>(scene,blk_iter->first,0,false);
        std::size_t normalsTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_NORMAL>::prefix());

        //load block info
        datasize[0] = (unsigned)(normals->num_bytes()/normalsTypeSize);
        datasize_mem->write_to_buffer((queue));

        transfer_time += (float) transfer.all();
        if (i==COMPUTE_VIS) {

            //array to store visibilities computed around a sphere
            //ask for a new BOXM2_VIS_SPHERE data so that it gets initialized properly.
            std::size_t visTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_VIS_SPHERE>::prefix());
            bocl_mem *vis_sphere = opencl_cache->get_data_new<BOXM2_VIS_SPHERE>(scene,blk_iter->first, (normals->num_bytes()/normalsTypeSize)*visTypeSize, false);

            //zip through each block
            std::map<boxm2_block_id, boxm2_block_metadata>::iterator blk_iter_inner;
            for (blk_iter_inner = blocks.begin(); blk_iter_inner != blocks.end(); ++blk_iter_inner) {

              transfer.mark();
              boxm2_block_id id_inner = blk_iter_inner->first;
              //std::cout << "--Loading block " << id_inner << std::endl;

              //load tree and alpha
              boxm2_block_metadata mdata = blk_iter_inner->second;
              vul_timer transfer;
              bocl_mem* blk = opencl_cache->get_block(scene,blk_iter_inner->first);
              bocl_mem* blk_info = opencl_cache->loaded_block_info();
              bocl_mem* alpha = opencl_cache->get_data<BOXM2_ALPHA>(scene,blk_iter_inner->first,0,false);
              auto* info_buffer = (boxm2_scene_info*) blk_info->cpu_buffer();
              int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
              info_buffer->data_buffer_length = (int) (alpha->num_bytes()/alphaTypeSize);
              blk_info->write_to_buffer((queue));

              bocl_mem* points = opencl_cache->get_data<BOXM2_POINT>(scene,blk_iter->first,0,false);

              if (id == id_inner)
                contain_point[0] = true;
              else
                contain_point[0] = false;
              contain_point_mem->write_to_buffer(queue);

              transfer_time += (float) transfer.all();

              local_threads[0] = 64;
              local_threads[1] = 1;
              global_threads[0] = RoundUp((normals->num_bytes()/normalsTypeSize), local_threads[0]);
              global_threads[1]=1;

              kern->set_arg( datasize_mem.ptr() );
              kern->set_arg( blk_info );
              kern->set_arg( dodecahedron_dir_lookup.ptr());
              kern->set_arg( blk );
              kern->set_arg( lookup.ptr()  );
              kern->set_arg( alpha  );
              kern->set_arg( points );
              kern->set_arg( normals );
              kern->set_arg( vis_sphere);
              kern->set_arg( contain_point_mem.ptr());
              kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_uchar16) );//local tree,
              kern->set_local_arg( local_threads[0]*local_threads[1]*10*sizeof(cl_uchar) ); //cumsum buffer, imindex buffer

              //execute kernel
              kern->execute(queue, 2, local_threads, global_threads);
              int status = clFinish(queue);
              check_val(status, MEM_FAILURE, "VISIBIITY EXECUTE FAILED: " + error_to_string(status));
              gpu_time += kern->exec_time();

              //clear render kernel args so it can reset em on next execution
              kern->clear_args();
            }

          //read from gpu
          vis_sphere->read_to_buffer(queue);
          int status = clFinish(queue);
          check_val(status, MEM_FAILURE, "READ VIS_SPHERE FAILED: " + error_to_string(status));
        }
        else if (i == DECIDE_INSIDE) {
          transfer.mark();

          //load tree
          boxm2_block_metadata mdata = blk_iter->second;
          vul_timer transfer;
          /* bocl_mem* blk = */ opencl_cache->get_block(scene,blk_iter->first);
          bocl_mem* blk_info = opencl_cache->loaded_block_info();
          auto* info_buffer = (boxm2_scene_info*) blk_info->cpu_buffer();
          info_buffer->data_buffer_length = (int) (normals->num_bytes()/normalsTypeSize);
          blk_info->write_to_buffer((queue));
          bocl_mem* alpha = opencl_cache->get_data<BOXM2_ALPHA>(scene,blk_iter->first,0,false);

          //load visibilities
          bocl_mem* vis_sphere = opencl_cache->get_data<BOXM2_VIS_SPHERE>(scene,blk_iter->first,0,false);

          //array to store final visibility score of a point
          bocl_mem* vis = opencl_cache->get_data<BOXM2_VIS_SCORE>(scene,blk_iter->first, (normals->num_bytes()/normalsTypeSize)
                                                  *boxm2_data_info::datasize(boxm2_data_traits<BOXM2_VIS_SCORE>::prefix()),false);

          transfer_time += (float) transfer.all();

          local_threads[0] = 128;
          local_threads[1] = 1;
          global_threads[0] = RoundUp((normals->num_bytes()/normalsTypeSize), local_threads[0]);
          global_threads[1]=1;

          kern->set_arg( blk_info );
          kern->set_arg( alpha );
          kern->set_arg( vis );
          kern->set_arg( vis_sphere);
          //execute kernel
          kern->execute(queue, 2, local_threads, global_threads);
          int status = clFinish(queue);
          check_val(status, MEM_FAILURE, "DECIDE NORMAL DIR EXECUTE FAILED: " + error_to_string(status));
          gpu_time += kern->exec_time();

          //read normals and vis from gpu
          alpha->read_to_buffer(queue);
          status = clFinish(queue);
          check_val(status, MEM_FAILURE, "READ NORMALS FAILED: " + error_to_string(status));

          //clear render kernel args so it can reset em on next execution
          kern->clear_args();
      }

      //shallow remove from ocl cache unnecessary items from ocl cache.
      opencl_cache->shallow_remove_data(scene,id,boxm2_data_traits<BOXM2_VIS_SPHERE>::prefix());
    }
  }

  std::cout<<"Gpu time "<<gpu_time<<" transfer time "<<transfer_time<<std::endl;
  clReleaseCommandQueue(queue);
  return true;
}
