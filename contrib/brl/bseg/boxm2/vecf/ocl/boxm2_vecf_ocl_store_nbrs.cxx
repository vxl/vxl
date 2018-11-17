#include "boxm2_vecf_ocl_store_nbrs.h"
#include <bocl/bocl_cl.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_block_metadata.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/boxm2_util.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boct/boct_bit_tree.h>
#include <bocl/bocl_kernel.h>
#include <vcl_where_root_dir.h>


typedef vnl_vector_fixed<unsigned char,16> uchar16;
bool boxm2_vecf_ocl_store_nbrs::get_scene_appearance( const boxm2_scene_sptr& scene,
                                    std::string& options)
{
    std::vector<std::string> apps = scene->appearances();
    bool foundDataType = false;
    for (const auto & app : apps) {
        if ( app == boxm2_data_traits<BOXM2_MOG3_GREY>::prefix() )
        {
            app_type_ = app;
            foundDataType = true;
            options=" -D MOG_TYPE_8 ";
        }
        else if ( app == boxm2_data_traits<BOXM2_MOG3_GREY_16>::prefix() )
        {
            app_type_ = app;
            foundDataType = true;
            options=" -D MOG_TYPE_16 ";
        }
    }
    if (!foundDataType) {
        std::cout<<"ERROR: boxm2_vecf_ocl_store_nbrs: unsupported appearance type"<<std::endl;
        return false;
    }
    //set apptype size
    apptypesize_ = (int) boxm2_data_info::datasize(app_type_);
    return true;
}
boxm2_vecf_ocl_store_nbrs::boxm2_vecf_ocl_store_nbrs(boxm2_scene_sptr& source_scene,
                                   const boxm2_opencl_cache_sptr& ocl_cache)
  : source_scene_(source_scene),
    opencl_cache_(ocl_cache)
{
  device_=opencl_cache_->get_device();
  this->compile_kernel();
  this->init_ocl_store();
}

boxm2_vecf_ocl_store_nbrs::~boxm2_vecf_ocl_store_nbrs()
{
  delete kern;
  opencl_cache_->clear_cache();
}
bool boxm2_vecf_ocl_store_nbrs::compile_kernel()
{
  std::string options;
  // sets apptypesize_ and app_type
  get_scene_appearance(source_scene_, options);
  std::cout << options << '\n';
  std::vector<std::string> src_paths;
  std::string source_dir = std::string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/boxm2/ocl/cl/";
  std::string vecf_source_dir = std::string(VCL_SOURCE_ROOT_DIR)+ "/contrib/brl/bseg/boxm2/vecf/ocl/cl/";
  src_paths.push_back(source_dir     + "scene_info.cl");
  src_paths.push_back(source_dir     + "bit/bit_tree_library_functions.cl");
  src_paths.push_back(vecf_source_dir + "store_neighbor_information.cl");
  this->kern = new bocl_kernel();
  return kern->create_kernel(&device_->context(),device_->device_id(), src_paths, "extract_neighbors_block", options, "neighbor_info");
}
bool boxm2_vecf_ocl_store_nbrs::init_ocl_store()
{
    centerX = new bocl_mem(device_->context(), boct_bit_tree::centerX, sizeof(cl_float)*585, "centersX lookup buffer");
    centerY = new bocl_mem(device_->context(), boct_bit_tree::centerY, sizeof(cl_float)*585, "centersY lookup buffer");
    centerZ = new bocl_mem(device_->context(), boct_bit_tree::centerZ, sizeof(cl_float)*585, "centersZ lookup buffer");
    centerX->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
    centerY->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
    centerZ->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
    // output buffer for debugging
    output = new bocl_mem(device_->context(), output_buff, sizeof(float)*1000, "output" );
    output->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR );
    // bit lookup buffer
    cl_uchar lookup_arr[256];
    boxm2_ocl_util::set_bit_lookup(lookup_arr);
    lookup=new bocl_mem(device_->context(), lookup_arr, sizeof(cl_uchar)*256, "bit lookup buffer");
    lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
    status = 0;
    queue = clCreateCommandQueue(device_->context(),*(device_->device_id()),CL_QUEUE_PROFILING_ENABLE,&status);
    ocl_depth = nullptr;
    blk_info_source = nullptr;
    info_buffer = nullptr;
    info_buffer_source = nullptr;
    blk_source = nullptr;
    mog_source = nullptr;
    return true;
}
// input is a block (source scene) output is the source scene with neighbor information
// neighbor existence and expected intensity of neighbors
bool boxm2_vecf_ocl_store_nbrs::augment_1_blk(){
  std::cout << "inside augment_1_blk" << std::endl;
  int depth = 3;
  std::size_t local_threads[1]={64};
  static std::size_t global_threads[1]={1};

  ocl_depth = new bocl_mem(device_->context(), &(depth), sizeof(int), "  depth of octree " );
  ocl_depth->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );
  std::vector<boxm2_block_id> blocks = source_scene_->get_block_ids();
  auto iter_blk = blocks.begin();
  blk_source = opencl_cache_->get_block(source_scene_, *iter_blk);
  info_buffer = source_scene_->get_blk_metadata(*iter_blk);
  blk_info_source  = new bocl_mem(device_->context(), info_buffer, sizeof(boxm2_scene_info), " Scene Info" );
  blk_info_source->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  if(app_type_ == "boxm2_mog3_grey") {
    mog_source       = opencl_cache_->get_data<BOXM2_MOG3_GREY>(source_scene_, *iter_blk,0,true);
  }
  else if(app_type_ == "boxm2_mog3_grey_16") {
    mog_source       = opencl_cache_->get_data<BOXM2_MOG3_GREY_16>(source_scene_, *iter_blk,0,true);
  }
  else {
    std::cout << "Unknown appearance type for source_scene " << app_type_ << '\n';
    return false;
  }
  global_threads[0] = (unsigned) RoundUp(info_buffer->scene_dims[0]*info_buffer->scene_dims[1]*info_buffer->scene_dims[2],(int)local_threads[0]);

  std::size_t nbytes_mog = mog_source->num_bytes();
  std::size_t nvox = nbytes_mog/apptypesize_;
  alpha_source = opencl_cache_->get_data<BOXM2_ALPHA>(source_scene_, *iter_blk, 0, true);
  std::size_t ex_size = boxm2_data_traits<BOXM2_CHAR8>::datasize();
  std::size_t nbytes_nbr_exint = ex_size*nvox;
  nbr_exint = opencl_cache_->get_data_new(source_scene_, *iter_blk, boxm2_data_traits<BOXM2_CHAR8>::prefix("nbr_exint"), nbytes_nbr_exint, false);
  std::size_t prob_size = boxm2_data_traits<BOXM2_FLOAT8>::datasize();
  std::size_t nbytes_nbr_prob = prob_size*nvox;
  nbr_prob = opencl_cache_->get_data_new(source_scene_, *iter_blk, boxm2_data_traits<BOXM2_FLOAT8>::prefix("nbr_prob"), nbytes_nbr_prob, false);
  std::size_t exist_size = boxm2_data_traits<BOXM2_CHAR8>::datasize();
  std::size_t nbytes_nbr_exist = exist_size*nvox;
  nbr_exists = opencl_cache_->get_data_new(source_scene_, *iter_blk, boxm2_data_traits<BOXM2_CHAR8>::prefix("nbr_exist"), nbytes_nbr_exist, false);
  // set kernel args
  kern->set_arg(centerX.ptr());
  kern->set_arg(centerY.ptr());
  kern->set_arg(centerZ.ptr());
  kern->set_arg(lookup.ptr());
  kern->set_arg(blk_info_source);
  kern->set_arg(blk_source);
  kern->set_arg(mog_source);
  kern->set_arg(alpha_source);
  kern->set_arg(nbr_exint);
  kern->set_arg(nbr_prob);
  kern->set_arg(nbr_exists);
  kern->set_arg(ocl_depth);
  kern->set_arg(output.ptr());
  kern->set_local_arg(16*local_threads[0]*sizeof(unsigned char)); // local trees
  kern->set_local_arg(16*local_threads[0]*sizeof(unsigned char)); // local neighbor trees
  kern->execute( queue, 1, local_threads, global_threads);
  clFinish( queue);
  check_val(status, MEM_FAILURE, "EXECUTE FAILED: " + error_to_string(status));
  nbr_exint->read_to_buffer(queue);
  nbr_exists->read_to_buffer(queue);
  nbr_prob->read_to_buffer(queue);
  output->read_to_buffer(queue);
  clFinish( queue);
  boxm2_lru_cache::instance()->write_to_disk(source_scene_);
  kern->clear_args();
  opencl_cache_->unref_mem(ocl_depth);
  blk_info_source->release_memory();
  delete info_buffer_source;
  ocl_depth->release_memory();
#if 0
  for(int i = 0; i< 100; i+=2)
    std::cout << output_buff[i] << ' ' << output_buff[i+1] << '\n';
#endif
  return true;
}
