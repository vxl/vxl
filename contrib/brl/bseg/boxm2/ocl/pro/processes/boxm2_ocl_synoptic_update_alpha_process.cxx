// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_synoptic_update_alpha_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for computing alpha in a synoptic process.
//
// \author Vishal Jain
// \date Mar 19, 2012

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/boxm2_data_traits.h>

#include <boxm2/io/boxm2_stream_block_cache.h>
#include <boxm2/cpp/algo/boxm2_synoptic_function_functors.h>
#include <boxm2/cpp/algo/boxm2_data_serial_iterator.h>

#include <boxm2/ocl/boxm2_ocl_util.h>
//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vul/vul_timer.h>
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>

namespace boxm2_ocl_synoptic_update_alpha_process_globals
{
  constexpr unsigned n_inputs_ = 5;
  constexpr unsigned n_outputs_ = 0;
  void compile_kernel(const bocl_device_sptr& device,std::vector<bocl_kernel*> & vec_kernels)
  {
    std::vector<std::string> src_paths;
    std::string source_dir = boxm2_ocl_util::ocl_src_root();
    src_paths.push_back(source_dir + "scene_info.cl");
    src_paths.push_back(source_dir     + "bit/bit_tree_library_functions.cl");
    src_paths.push_back(source_dir + "batch/synoptic_function_kernels.cl");

    //compilation options

    auto* compute_cubic = new bocl_kernel();
    std::string opts = " -D COMPUTE_SYNOPTIC_ALPHA -D JOINT";

    compute_cubic->create_kernel(&device->context(), device->device_id(), src_paths, "compute_synoptic_alpha", opts, "kernel: compute_synoptic_alpha");
    vec_kernels.push_back(compute_cubic);
  }
  static std::map<cl_device_id*,std::vector<bocl_kernel*> > kernels;
}

bool boxm2_ocl_synoptic_update_alpha_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_synoptic_update_alpha_process_globals;

  //process takes 5 inputs, no output
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "boxm2_scene_sptr";
  input_types_[2] = "boxm2_opencl_cache_sptr";
  input_types_[3] = "unsigned";         //: number of obs
  input_types_[4] = "vcl_string";       //: identifiers name file

  std::vector<std::string>  output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_ocl_synoptic_update_alpha_process(bprb_func_process& pro)
{
  using namespace boxm2_ocl_synoptic_update_alpha_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The number of inputs should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  bocl_device_sptr device = pro.get_input<bocl_device_sptr>(i++);
  boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_opencl_cache_sptr opencl_cache= pro.get_input<boxm2_opencl_cache_sptr>(i++);
  auto nobs = pro.get_input<unsigned>(i++);
  std::string identifier_filename = pro.get_input<std::string>(i++);

  boxm2_cache_sptr cache = opencl_cache->get_cpu_cache();
  //: Read data types and identifier file names.
  std::ifstream ifs(identifier_filename.c_str());
  if (!ifs.good()) {
    std::cerr << "error opening file " <<identifier_filename << '\n';
    return false;
  }
  std::vector<std::string> image_ids;
  unsigned int n_images = 0;
  ifs >> n_images;
  for (unsigned int i=0; i<n_images; ++i) {
    std::string img_id;
    ifs >> img_id;
    image_ids.push_back(img_id);
    std::cout<<img_id<<std::endl;
  }
  ifs.close();

  std::vector<std::string> type_names;
  type_names.emplace_back("aux0");
  type_names.emplace_back("aux2");
  type_names.emplace_back("aux3");
  // create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue(device->context(),
                                                *(device->device_id()),
                                                CL_QUEUE_PROFILING_ENABLE,&status);
  if (status!=0) return false;
  // compile the kernel
  if (kernels.find((device->device_id()))==kernels.end())
  {
    std::cout<<"===========Compiling kernels==========="<<std::endl;
    std::vector<bocl_kernel*> ks;
    compile_kernel(device,ks);
    kernels[(device->device_id())]=ks;
  }
  // create all buffers
  // bit lookup buffer
  cl_uchar lookup_arr[256];
  boxm2_ocl_util::set_bit_lookup(lookup_arr);
  bocl_mem_sptr lookup=new bocl_mem(device->context(), lookup_arr, sizeof(cl_uchar)*256, "bit lookup buffer");
  lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  vul_timer t;
  t.mark();
  boxm2_stream_block_cache str_blk_cache(scene, type_names, image_ids);

  std::vector<boxm2_block_id> block_ids = scene->get_block_ids();
  std::vector<boxm2_block_id>::iterator id;
  for (id = block_ids.begin(); id != block_ids.end(); ++id)
  {
    boxm2_block_metadata mdata = scene->get_block_metadata(*id);
    std::size_t lThreads[] = {1};
    std::size_t gThreads[1];
    gThreads[0] = (unsigned)(mdata.sub_block_num_.x()
                            *mdata.sub_block_num_.y()
                            *mdata.sub_block_num_.z());
    //choose correct render kernel
    bocl_mem* blk = opencl_cache->get_block(scene,*id);
    bocl_mem* blk_info = opencl_cache->loaded_block_info();
    bocl_mem* alpha = opencl_cache->get_data<BOXM2_ALPHA>(scene,*id,0,false);
    int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
    auto* info_buffer = (boxm2_scene_info*) blk_info->cpu_buffer();
    info_buffer->data_buffer_length = (int) (alpha->num_bytes()/alphaTypeSize);

    //grab an appropriately sized AUX data buffer
    bocl_kernel * kern = kernels[(device->device_id())][0];
    str_blk_cache.init(*id);

    std::cout<<"Block Id: "<<(*id)<<std::endl;
    int datasize = str_blk_cache.block_size_in_bytes_["aux0"]/ sizeof(float);

    boxm2_data_base * data_type0 = str_blk_cache.data_types_["aux0"];
    bocl_mem_sptr bocl_data_type0 = new bocl_mem(device->context(),data_type0->data_buffer(),data_type0->buffer_length(),"");
    if (!bocl_data_type0->create_buffer(CL_MEM_USE_HOST_PTR,queue))
      std::cout<<"Aux0 buffer was not created"<<std::endl;

    boxm2_data_base * data_type2 = str_blk_cache.data_types_["aux2"];
    bocl_mem_sptr bocl_data_type2 = new bocl_mem(device->context(),data_type2->data_buffer(),data_type2->buffer_length(),"");
    if (!bocl_data_type2->create_buffer(CL_MEM_USE_HOST_PTR,queue))
      std::cout<<"Aux2 buffer was not created"<<std::endl;

    boxm2_data_base * data_type3 = str_blk_cache.data_types_["aux3"];
    bocl_mem_sptr bocl_data_type3 = new bocl_mem(device->context(),data_type3->data_buffer(),data_type3->buffer_length(),"");
    if (!bocl_data_type3->create_buffer(CL_MEM_USE_HOST_PTR,queue))
      std::cout<<"Aux3 buffer was not created"<<std::endl;

    std::cout<<"Block Id: "<<(*id)<<std::endl;
    bocl_mem_sptr  nobs_mem=new bocl_mem(device->context(), &nobs, sizeof(int), "Number of Obs");
    nobs_mem->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

    bocl_mem_sptr  datasize_mem=new bocl_mem(device->context(), &datasize, sizeof(int), "Data Size");
    datasize_mem->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

    kern->set_arg(blk_info);
    kern->set_arg(blk);
    kern->set_arg(alpha);
    kern->set_arg(bocl_data_type0.ptr());
    kern->set_arg(bocl_data_type2.ptr());
    kern->set_arg(bocl_data_type3.ptr());
    kern->set_arg(nobs_mem.ptr());
    kern->set_arg(datasize_mem.ptr());
    kern->set_arg(lookup.ptr());
    kern->set_local_arg(16*lThreads[0]*sizeof(unsigned char)); // local trees
    kern->set_local_arg( lThreads[0]*10*sizeof(cl_uchar) );    // cumsum buffer
    kern->execute(queue, 1, lThreads, gThreads);
    clFinish(queue);
    std::cout<<"Time taken "<< kern->exec_time()<<std::endl;

    //clear render kernel args so it can reset em on next execution
    kern->clear_args();
    alpha->read_to_buffer(queue);
    clFinish(queue);
  }
  clReleaseCommandQueue(queue);

  std::cout<<"Finished update batch alpha "<<t.all()<<" ms"<<std::endl;
  return true;
}
