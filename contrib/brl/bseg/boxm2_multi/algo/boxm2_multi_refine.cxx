#include <iostream>
#include <algorithm>
#include "boxm2_multi_refine.h"
//:
// \file

#include <boxm2_multi_util.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_util.h>
#include <bocl/bocl_manager.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boxm2/ocl/boxm2_opencl_cache1.h>
#include <boxm2/ocl/algo/boxm2_ocl_camera_converter.h>

#include <bocl/bocl_mem.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <vul/vul_timer.h>

std::map<std::string, bocl_kernel*> boxm2_multi_refine::refine_tree_kernels_;
std::map<std::string, bocl_kernel*> boxm2_multi_refine::refine_data_kernels_;


float boxm2_multi_refine::refine(boxm2_multi_cache& cache, float thresh)
{
  //debug clearing
  cache.clear();

  std::cout<<"------------ boxm2_multi_render -----------------------"<<std::endl;
  //verify appearance model
  vul_timer rtime; rtime.mark();
  std::string data_type, options;
  int apptypesize;
  if ( !boxm2_multi_util::get_scene_appearances(cache.get_scene(), data_type, options, apptypesize) )
    return 0.0f;

  //set up image lists
  std::vector<cl_command_queue> queues;

  //--------------------------------
  //prep buffers for each device
  //--------------------------------
  std::vector<BlockMemMap > sizeMaps, copyMaps, newDataMaps;
  std::vector<BlockIntMap > newDataSizes;
  std::vector<std::vector<boxm2_block_id> > vis_orders;
  std::vector<bocl_mem_sptr> out_mems, lookups, prob_mems;
  std::size_t maxBlocks = 0;
  std::vector<boxm2_opencl_cache1*> ocl_caches = cache.ocl_caches();
  for (auto ocl_cache : ocl_caches) {
    //grab sub scene and it's cache
    boxm2_scene_sptr    sub_scene = ocl_cache->get_scene();
    bocl_device_sptr    device    = ocl_cache->get_device();

    // create a command queue.
    int status=0;
    cl_command_queue queue = clCreateCommandQueue( device->context(),
                                                   *(device->device_id()),
                                                   CL_QUEUE_PROFILING_ENABLE,
                                                   &status );
    queues.push_back(queue);
    if (status!=0) {
      std::cout<<"boxm2_multi_store_aux::store_aux unable to create command queue"<<std::endl;
      return 0.0f;
    }

    //prob threshold buffer
    bocl_mem_sptr prob_thresh = new bocl_mem(device->context(), &thresh, sizeof(float), "prob_thresh buffer");
    prob_thresh->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
    prob_mems.push_back(prob_thresh);

    // Output Array
    auto* output_arr = new float[100];
    std::fill(output_arr, output_arr+100, 0.0f);
    bocl_mem_sptr  cl_output=new bocl_mem(device->context(), output_arr, sizeof(float)*100, "output buffer");
    cl_output->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
    out_mems.push_back(cl_output);

    // bit lookup buffer
    cl_uchar lookup_arr[256];
    boxm2_ocl_util::set_bit_lookup(lookup_arr);
    bocl_mem_sptr lookup=new bocl_mem(device->context(), lookup_arr, sizeof(cl_uchar)*256, "bit lookup buffer");
    lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
    lookups.push_back(lookup);

    //keep track of block IDS per subscene
    std::vector<boxm2_block_id> ids = sub_scene->get_block_ids();
    vis_orders.push_back(ids);
    maxBlocks = std::max(maxBlocks, ids.size());

    //initialize sizebuff map (id->mem), and blockCopy map (id->mem)
    BlockMemMap sizeMap, copyMap, newDataMap;
    BlockIntMap newSizeMap;
    sizeMaps.push_back(sizeMap);
    copyMaps.push_back(copyMap);
    newDataMaps.push_back(newDataMap);
    newDataSizes.push_back(newSizeMap);
  }

  //------------------------------------------------------------------
  //STEP ONE for each device/cache, refine trees
  //------------------------------------------------------------------
  for (unsigned int blk=0; blk<maxBlocks; ++blk) {
    for (unsigned int i=0; i<ocl_caches.size(); ++i) {
      //grab sub scene and it's cache
      boxm2_opencl_cache1* ocl_cache = ocl_caches[i];
      boxm2_scene_sptr    sub_scene = ocl_cache->get_scene();
      bocl_device_sptr    device    = ocl_cache->get_device();

      //Run block store aux
      std::vector<boxm2_block_id>& vis_order = vis_orders[i];
      if (blk >= vis_order.size())
        continue;
      boxm2_block_id id = vis_order[blk];
      int numTrees = sub_scene->num_trees_in_block(id);
      refine_trees_per_block(id, ocl_cache, queues[i], numTrees,
                             sizeMaps[i], copyMaps[i],
                             prob_mems[i], lookups[i], out_mems[i]);
    }

    //finish
    for (auto & queue : queues)
      clFinish(queue);
  }

  //------------------------------------------------------------------
  //STEP TWO: read out tree_sizes and do cumulative sum on it
  //------------------------------------------------------------------
  unsigned num_refined   = 0;     //number of cells that split
  for (unsigned int i=0; i<queues.size(); ++i) {
    boxm2_opencl_cache1* ocl_cache = ocl_caches[i];
    boxm2_scene_sptr    sub_scene = ocl_cache->get_scene();

    BlockMemMap& sizeMap = sizeMaps[i];
    BlockMemMap& copyMap = copyMaps[i];
    BlockIntMap& newSizeMap = newDataSizes[i];
    BlockMemMap::iterator iter;

    // read tree sizes into memory, run cumulative sum
    for (iter = sizeMap.begin(); iter != sizeMap.end(); ++iter) {
      bocl_mem_sptr tree_sizes = iter->second;
      tree_sizes->read_to_buffer(queues[i]);

      boxm2_block_id id = iter->first;
      boxm2_block_metadata& data = sub_scene->get_block_metadata(id);
      int numTrees = data.sub_block_num_.x() *
                     data.sub_block_num_.y() *
                     data.sub_block_num_.z();
      //do cumsum
      int newDataSize = cumsum((int*)tree_sizes->cpu_buffer(), numTrees);
      newSizeMap[id] = newDataSize;
      tree_sizes->write_to_buffer(queues[i]);

      //calculate old size vs new size
      bocl_mem* alpha = ocl_cache->get_data<BOXM2_ALPHA>(id);
      auto dataLen = (std::size_t) (alpha->num_bytes() / sizeof(float));
      //std::cout<<"  New data size: "<<newDataSize<<", old data: "<<dataLen<<'\n'
      //        <<"  num refined: "<<(newDataSize-dataLen)/8<<std::endl;
      num_refined += (unsigned) ( (newDataSize-dataLen)/8 );
    }

    //read in the actual refined trees
    for (iter = copyMap.begin(); iter != copyMap.end(); ++iter) {
      bocl_mem_sptr buff = iter->second;
      buff->read_to_buffer(queues[i]);
    }
  }


  //------------------------------------------------------------------
  //STEP THREE: swap data
  //------------------------------------------------------------------
  for (unsigned int blk=0; blk<maxBlocks; ++blk) {
    for (unsigned int i=0; i<ocl_caches.size(); ++i) {
      //grab sub scene and it's cache
      boxm2_opencl_cache1* ocl_cache = ocl_caches[i];
      boxm2_scene_sptr    sub_scene = ocl_cache->get_scene();
      bocl_device_sptr    device    = ocl_cache->get_device();

      //Run block store aux
      std::vector<boxm2_block_id>& vis_order = vis_orders[i];
      if (blk >= vis_order.size())
        continue;
      boxm2_block_id id = vis_order[blk];
      int numTrees = sub_scene->num_trees_in_block(id);
      swap_data_per_block(sub_scene,id,numTrees, ocl_cache, queues[i],
                          sizeMaps[i], copyMaps[i], newDataMaps[i], newDataSizes[i],
                          out_mems[i], lookups[i],
                          data_type, apptypesize, prob_mems[i]);
    }

    //finish
    for (auto & queue : queues)
      clFinish(queue);
  }

  //STEP FOUR: Clean up
  std::cout<<" Total Num Refined: "<<num_refined<<std::endl;
  for (unsigned int i=0; i<queues.size(); ++i) {
    boxm2_opencl_cache1* ocl_cache = ocl_caches[i];
    BlockMemMap& sizeMap = sizeMaps[i];
    BlockMemMap& copyMap = copyMaps[i];
    BlockMemMap::iterator iter;
    for (iter=sizeMap.begin(); iter != sizeMap.end(); ++iter) {
      bocl_mem_sptr tree_sizes = iter->second;
      delete[] (cl_int*) tree_sizes->cpu_buffer();
      ocl_cache->unref_mem(tree_sizes.ptr());
    }
    for (iter=copyMap.begin(); iter!=copyMap.end(); ++iter) {
      bocl_mem_sptr blk_copy = iter->second;
      delete[] (cl_uint16*) blk_copy->cpu_buffer();
      ocl_cache->unref_mem(blk_copy.ptr());
    }
  }

  for (auto & queue : queues)
    clReleaseCommandQueue(queue);
  return 0.0f;
}


//: Refines trees, keeps track of new sizes
float boxm2_multi_refine::refine_trees_per_block(const boxm2_block_id& id,
                                                 boxm2_opencl_cache1* ocl_cache,
                                                 cl_command_queue& queue,
                                                 int numTrees,
                                                 BlockMemMap&  sizebuffs,
                                                 BlockMemMap&  blockCopies,
                                                 bocl_mem_sptr& prob_thresh,
                                                 bocl_mem_sptr& lookup,
                                                 bocl_mem_sptr& cl_output )
{
  //std::cout<<"Refining Block "<< id << std::endl;

  //set up tree copy and store for later use
  //std::cout<<"  creating tree copy"<<std::endl;
  bocl_mem_sptr blk_copy = ocl_cache->alloc_mem(numTrees*sizeof(cl_uchar16), new cl_uchar16[numTrees], "refine trees block copy buffer");
  blk_copy->create_buffer(CL_MEM_READ_WRITE| CL_MEM_COPY_HOST_PTR);
  blockCopies[id] = blk_copy;

  //set up tree size (first find num trees)
  //std::cout<<"  creating tree sizes buff"<<std::endl;
  bocl_mem_sptr tree_sizes = ocl_cache->alloc_mem(sizeof(cl_int)*numTrees, new cl_int[numTrees], "refine tree sizes buffer");
  tree_sizes->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  sizebuffs[id] = tree_sizes;

  //write the image values to the buffer
  vul_timer transfer;
  bocl_mem* blk       = ocl_cache->get_block(id);
  bocl_mem* alpha     = ocl_cache->get_data<BOXM2_ALPHA>(id);
  bocl_mem* blk_info  = ocl_cache->loaded_block_info();
  std::size_t lThreads[] = {64, 1};
  std::size_t gThreads[] = {RoundUp(numTrees,lThreads[0]), 1};

#if 0 // TODO - add a STOP LIST to pass around
  float alphasize=(float)alpha->num_bytes()/1024/1024;
  if (alphasize >= (float)data.max_mb_/10.0) {
      std::cout<<"  Refine STOP !!!"<<std::endl;
      continue;
  }
#endif
  //set first kernel args
  bocl_device_sptr device = ocl_cache->get_device();
  bocl_kernel* kern = get_refine_tree_kernel(device, "");
  kern->set_arg( blk_info );
  kern->set_arg( blk );
  kern->set_arg( blk_copy.ptr() );
  kern->set_arg( alpha );
  kern->set_arg( tree_sizes.ptr() );
  kern->set_arg( prob_thresh.ptr() );
  kern->set_arg( lookup.ptr() );
  kern->set_arg( cl_output.ptr() );
  kern->set_local_arg( lThreads[0]*10*sizeof(cl_uchar) );
  kern->set_local_arg( lThreads[0]*sizeof(cl_uchar16) );
  kern->set_local_arg( lThreads[0]*sizeof(cl_uchar16) );

  //execute kernel
  float gpu_time =  kern->execute( queue, 2, lThreads, gThreads);
  kern->clear_args();

  return gpu_time;
}


//------------------------------------------
//: Runs non-blocking refine on block
//------------------------------------------
void boxm2_multi_refine::swap_data_per_block( const boxm2_scene_sptr& scene,
                                              const boxm2_block_id& id,
                                              int numTrees,
                                              boxm2_opencl_cache1* ocl_cache,
                                              cl_command_queue& queue,
                                              BlockMemMap&  sizebuffs,
                                              BlockMemMap&  blockCopies,
                                              BlockMemMap&   /*newDatas*/,
                                              BlockIntMap&  newDataSizes,
                                              const bocl_mem_sptr& cl_output,
                                              const bocl_mem_sptr& lookup,
                                              const std::string&  /*data_type*/,
                                              int   /*apptypesize*/,
                                              const bocl_mem_sptr& prob_thresh )
{
  bocl_device_sptr device = ocl_cache->get_device();
  int newDataSize = newDataSizes[id];
  bocl_mem_sptr blk_copy = blockCopies[id];
  bocl_mem_sptr tree_sizes = sizebuffs[id];

  //local/global sizes
  std::size_t lThreads[] = {64, 1};
  std::size_t gThreads[] = {RoundUp(numTrees,lThreads[0]), 1};

  //swap data into place
  std::vector<std::string> data_types = scene->appearances();
  data_types.push_back(boxm2_data_traits<BOXM2_ALPHA>::prefix());
  for (const auto & data_type : data_types)
  {
    //std::cout<<"  Swapping data of type: "<<data_types[i]<<std::endl;
    std::string options = get_option_string( boxm2_data_info::datasize(data_type) );
    bocl_kernel* kern = get_refine_data_kernel(device, options);

    //get bocl_mem data independent of CPU pointer
    bocl_mem* dat = ocl_cache->get_data(id, data_type);

    //get a new data pointer (with newSize), will create CPU buffer and GPU buffer
    //std::cout<<"  Data_type "<<data_types[i]<<" new size is: "<<newDataSize<<std::endl;
    int dataBytes = boxm2_data_info::datasize(data_type) * newDataSize;
    bocl_mem* new_dat = ocl_cache->alloc_mem(dataBytes, nullptr, "new data buffer " + data_type);
    new_dat->create_buffer(CL_MEM_READ_WRITE, queue);

    //grab the block out of the cache as well
    bocl_mem* blk = ocl_cache->get_block(id);
    bocl_mem* blk_info = ocl_cache->loaded_block_info();

    //is alpha buffer
    bool is_alpha_buffer[1] = { (data_type == boxm2_data_traits<BOXM2_ALPHA>::prefix()) };
    bocl_mem is_alpha(device->context(), is_alpha_buffer, sizeof(cl_bool), "is_alpha buffer");
    is_alpha.create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

    //copy parent behavior.. if true, Data copies its parent
    bool copy_parent_buffer[1];
    if (data_type == boxm2_data_traits<BOXM2_MOG3_GREY>::prefix() ||
        data_type == boxm2_data_traits<BOXM2_MOG3_GREY_16>::prefix() ||
        data_type == boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix() )
      (*copy_parent_buffer) = true;
    else
      (*copy_parent_buffer) = false;
    bocl_mem copy_parent(device->context(), copy_parent_buffer, sizeof(cl_bool), "copy_parent buffer");
    copy_parent.create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

    //make it a reference so the destructor isn't called at the end...
    kern->set_arg( blk_info );
    kern->set_arg( blk );
    kern->set_arg( blk_copy.ptr() );
    kern->set_arg( tree_sizes.ptr() );
    kern->set_arg( dat );
    kern->set_arg( new_dat );
    kern->set_arg( prob_thresh.ptr());
    kern->set_arg( &is_alpha );
    kern->set_arg( &copy_parent );
    kern->set_arg( lookup.ptr() );
    kern->set_arg( cl_output.ptr() );
    kern->set_local_arg( lThreads[0]*sizeof(cl_uchar16) );
    kern->set_local_arg( lThreads[0]*sizeof(cl_uchar16) );
    kern->set_local_arg( lThreads[0]*73*sizeof(cl_uchar) );

    //execute kernel
    kern->execute( queue, 2, lThreads, gThreads);
    kern->clear_args();

    //debug stuff---------
    clFinish(queue);
    ocl_cache->deep_replace_data(id, data_type, new_dat);
    if (data_type == boxm2_data_traits<BOXM2_ALPHA>::prefix()) {
      //std::cout<<"  Writing refined trees."<<std::endl;
      blk->read_to_buffer(queue);
    }
    ocl_cache->unref_mem(new_dat);
  }
}


//----------------------------------------------
// compile and cache kernels
//----------------------------------------------
bocl_kernel* boxm2_multi_refine::get_refine_tree_kernel(const bocl_device_sptr& device, const std::string& options)
{
  // check to see if this device has compiled kernels already
  std::string identifier = device->device_identifier() + options;
  if (refine_tree_kernels_.find(identifier) != refine_tree_kernels_.end())
    return refine_tree_kernels_[identifier];

  //gather all render sources... seems like a lot for rendering...
  std::vector<std::string> src_paths;
  std::string source_dir = boxm2_ocl_util::ocl_src_root();
  src_paths.push_back(source_dir + "scene_info.cl");
  src_paths.push_back(source_dir + "basic/linked_list.cl");
  src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
  src_paths.push_back(source_dir + "bit/refine_bit_scene.cl");

  //create refine trees kernel (refine trees deterministic.  MOG type is necessary
  // to define, but not used by the kernel - using default value here
  auto* refine_tree_kernel = new bocl_kernel();
  refine_tree_kernel->create_kernel( &device->context(), device->device_id(), src_paths,
                                     "refine_trees", " -D MOG_TYPE_8 ",
                                     "boxm2 opencl refine trees (pass one)"); //kernel identifier (for error checking)

  //store/return
  refine_tree_kernels_[identifier] = refine_tree_kernel;
  return refine_tree_kernel;
}

bocl_kernel* boxm2_multi_refine::get_refine_data_kernel(const bocl_device_sptr& device, const std::string& option)
{
  // check to see if this device has compiled kernels already
  std::string identifier = device->device_identifier() + option;
  if (refine_data_kernels_.find(identifier) != refine_data_kernels_.end())
    return refine_data_kernels_[identifier];

  std::vector<std::string> src_paths;
  auto* refine_data_kernel = new bocl_kernel();
  std::string source_dir = boxm2_ocl_util::ocl_src_root();
  src_paths.push_back(source_dir + "scene_info.cl");
  src_paths.push_back(source_dir + "basic/linked_list.cl");
  src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
  src_paths.push_back(source_dir + "bit/refine_bit_scene.cl");
  refine_data_kernel->create_kernel( &device->context(), device->device_id(),
                                     src_paths, "refine_data", option,
                                     "boxm2 opencl refine data size 2 (pass three)");
  //store/return
  refine_data_kernels_[identifier] = refine_data_kernel;
  return refine_data_kernel;
}

std::string boxm2_multi_refine::get_option_string(int datasize)
{
  std::string options="";
  switch (datasize)
  {
    case 2:
      options= "-D MOG_TYPE_2 ";break;
    case 4:
      options= "-D MOG_TYPE_4 ";break;
    case 6:
      options= "-D MOG_TYPE_6 ";break;
    case 8:
      options= "-D MOG_TYPE_8 ";break;
    case 16:
      options= "-D MOG_TYPE_16 ";break;
    default:
      break;
  }
  return options;
}
