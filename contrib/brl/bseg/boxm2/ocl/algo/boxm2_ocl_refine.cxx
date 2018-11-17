#include <iostream>
#include <algorithm>
#include "boxm2_ocl_refine.h"
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <boxm2/ocl/boxm2_ocl_util.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_timer.h>

// declare bocl_kernel* maps
std::map<std::string, bocl_kernel *> boxm2_ocl_refine::tree_kernels_;
std::map<std::string, bocl_kernel *> boxm2_ocl_refine::data_kernels_;

int boxm2_ocl_refine::refine_scene(const bocl_device_sptr& device,
                                   const boxm2_scene_sptr& scene,
                                   const boxm2_opencl_cache_sptr& opencl_cache,
                                   float thresh) {
  return boxm2_ocl_refine::refine_scene(device, scene, opencl_cache, thresh,
                                        scene->appearances());
}

int boxm2_ocl_refine::refine_scene(const bocl_device_sptr& device,
                                   boxm2_scene_sptr scene,
                                   const boxm2_opencl_cache_sptr& opencl_cache,
                                   float thresh,
                                   std::vector<std::string> prefixes) {
  float transfer_time = 0.0f;
  float gpu_time = 0.0f;
  unsigned num_cells = 0;   // number of cells in the scene after refine
  unsigned num_refined = 0; // number of cells that split

  if (std::find(prefixes.begin(), prefixes.end(), "alpha") == prefixes.end()) {
    prefixes.push_back(boxm2_data_traits<BOXM2_ALPHA>::prefix());
    std::cout << "Added alpha prefix to list of datatypes, alpha must be "
                "present for refinement."
             << std::endl;
  }

  // get the inputs
  // create a command queue.
  int status = 0;
  cl_command_queue queue =
      clCreateCommandQueue(device->context(), *(device->device_id()),
                           CL_QUEUE_PROFILING_ENABLE, &status);
  if (!check_val(status, CL_SUCCESS,
                 "UPDATE EXECUTE FAILED: " + error_to_string(status)))
    return -1;

  float prob_buff[1];
  prob_buff[0] = thresh;
  bocl_mem_sptr prob_thresh = new bocl_mem(device->context(), prob_buff,
                                           sizeof(float), "prob_thresh buffer");
  prob_thresh->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  float output_arr[100];
  for (float & i : output_arr)
    i = 0.0f;
  bocl_mem_sptr cl_output = new bocl_mem(device->context(), output_arr,
                                         sizeof(float) * 100, "output buffer");
  cl_output->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  // bit lookup buffer
  cl_uchar lookup_arr[256];
  boxm2_ocl_util::set_bit_lookup(lookup_arr);
  bocl_mem_sptr lookup =
      new bocl_mem(device->context(), lookup_arr, sizeof(cl_uchar) * 256,
                   "bit lookup buffer");
  lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  // 2. set workgroup size
  opencl_cache->clear_cache();
  std::map<boxm2_block_id, boxm2_block_metadata> blocks = scene->blocks();
  std::map<boxm2_block_id, boxm2_block_metadata>::iterator blk_iter;
  for (blk_iter = blocks.begin(); blk_iter != blocks.end(); ++blk_iter) {
    //----- IF THE BLOCK IS NOT RANDOMLY DISTRIBUTED, USE NEW METHOD -----------
    // New Method Summary:
    //  - NEED TO CLEAR OUT THE GPU CACHE BEFORE YOU START.. so you don't
    //  overwrite stuff accidentally...
    //  - Create Block Copy, refine trees into that copy, maintaining old copy
    //  and array of new tree sizes
    //  - Do scan on size vector (cum sum)
    //  - Swap data into new buffers: For each data type
    //    - get BOCL_MEM* data independent of cpu pointer (from cache)
    //    - remove the BOCL_MEM* from the gpu cache (don't delete it)
    //    - do a deep delete (delete CPU buffer from CPU cache)
    //    - get a new data pointer (with newSize), will create CPU buffer and
    //    GPU buffer
    //    - Run refine_data_kernel with the two buffers
    //    - delete the old BOCL_MEM*, and that's it...
    // clear cache
    opencl_cache->clear_cache();
    boxm2_block_metadata data = blk_iter->second;
    boxm2_block_id id = blk_iter->first;
    std::cout << "Refining Block " << id << "...";
    bocl_kernel *kern = get_refine_tree_kernel(device);

    ////////////////////////////////////////////////////////////////////////////
    // Step One... currently mimics C++ implementation
    // get id and refine block into tree copy, and calc vector of new tree sizes
    int numTrees = data.sub_block_num_.x() * data.sub_block_num_.y() *
                   data.sub_block_num_.z();

    // set up tree copy
    bocl_mem_sptr blk_copy = opencl_cache->alloc_mem(
        numTrees * sizeof(cl_uchar16), new cl_uchar16[numTrees],
        "refine trees block copy buffer");
    blk_copy->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

    // set up tree size (first find num trees)
    bocl_mem_sptr tree_sizes =
        opencl_cache->alloc_mem(sizeof(cl_int) * numTrees, new cl_int[numTrees],
                                "refine tree sizes buffer");
    tree_sizes->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

    // write the image values to the buffer
    vul_timer transfer;
    bocl_mem *blk = opencl_cache->get_block(scene, id);
    bocl_mem *alpha = opencl_cache->get_data<BOXM2_ALPHA>(scene, id);
    bocl_mem *blk_info = opencl_cache->loaded_block_info();
    transfer_time += (float)transfer.all();
    std::size_t lThreads[] = {64, 1};
    std::size_t gThreads[] = {RoundUp(numTrees, lThreads[0]), 1};

    float alphasize = (float)alpha->num_bytes() / 1024 / 1024;
    if (alphasize >= (float)blk_iter->second.max_mb_ / 10.0) {
      std::cout << "  Refine STOP !!!" << std::endl;
      opencl_cache->unref_mem(blk_copy.ptr());
      opencl_cache->unref_mem(tree_sizes.ptr());
      continue;
    }
    // set first kernel args
    kern->set_arg(blk_info);
    kern->set_arg(blk);
    kern->set_arg(blk_copy.ptr());
    kern->set_arg(alpha);
    kern->set_arg(tree_sizes.ptr());
    kern->set_arg(prob_thresh.ptr());
    kern->set_arg(lookup.ptr());
    kern->set_arg(cl_output.ptr());
    kern->set_local_arg(lThreads[0] * 10 * sizeof(cl_uchar));
    kern->set_local_arg(lThreads[0] * sizeof(cl_uchar16));
    kern->set_local_arg(lThreads[0] * sizeof(cl_uchar16));

    // execute kernel
    kern->execute(queue, 2, lThreads, gThreads);
    status = clFinish(queue);
    if (!check_val(status, CL_SUCCESS,
                   "REFINE EXECUTE FAILED 1: " + error_to_string(status)))
      return -1;
    gpu_time += kern->exec_time();

    // clear render kernel args so it can reset em on next execution
    kern->clear_args();
    blk_copy->read_to_buffer(queue);
    status = clFinish(queue);
    /////////////////////////////////////////////////////////////////////////
    // STEP TWO
    // read out tree_sizes and do cumulative sum on it
    vul_timer scan_time;
    tree_sizes->read_to_buffer(queue);
    status = clFinish(queue);
    int *sizebuff = (int *)tree_sizes->cpu_buffer();
    for (int i = 1; i < numTrees; ++i)
      sizebuff[i] += sizebuff[i - 1];
    int newDataSize = sizebuff[numTrees - 1];
    for (int i = numTrees - 1; i > 0; --i)
      sizebuff[i] = sizebuff[i - 1];
    sizebuff[0] = 0;
    tree_sizes->write_to_buffer((queue));
    status = clFinish(queue);
    int dataLen = alpha->num_bytes() / sizeof(float);
    std::cout << "  new data size: " << newDataSize << ", old data: " << dataLen
             << '\n'
             << "  num refined: " << (newDataSize - dataLen) / 8 << '\n'
             << "  scan data sizes time: " << scan_time.all() << std::endl;
    transfer_time += scan_time.all();
    num_cells += (unsigned)newDataSize;
    num_refined += (unsigned)((newDataSize - dataLen) / 8);
    /////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////////
    // STEP Three
    //  - Swap data into new buffers: For each data type
    //    - get BOCL_MEM* data independent of cpu pointer (from cache)
    //    - remove the BOCL_MEM* from the gpu cache (don't delete it)
    //    - do a deep delete (delete CPU buffer from CPU cache)
    //    - get a new data pointer (with newSize), will create CPU buffer and
    //    GPU buffer
    //    - Run refine_data_kernel with the two buffers
    //    - delete the old BOCL_MEM*, and that's it...
    // POSSIBLE PROBLEMS: data may not exist in cache and may need to be
    // initialized...
    // this vector will be passed in (listing data types to refine)
    for (unsigned int i = 0; i < prefixes.size(); ++i) {
      std::cout << "  Swapping data of type: " << prefixes[i] << std::endl;
      bocl_kernel *kern = get_refine_data_kernel(device, prefixes[i]);

      // get bocl_mem data independent of CPU pointer
      bocl_mem *dat = opencl_cache->get_data(scene, id, prefixes[i]);

      // get a new data pointer (with newSize), will create CPU buffer and GPU
      // buffer
      int dataBytes = boxm2_data_info::datasize(prefixes[i]) * newDataSize;
      std::cout << "# of bytes " << prefixes[i] << ' ' << dataBytes << std::endl;
      bocl_mem *new_dat = opencl_cache->alloc_mem(
          dataBytes, nullptr, "new data buffer " + prefixes[i]);
      new_dat->create_buffer(CL_MEM_READ_WRITE, queue);

      // grab the block out of the cache as well
      bocl_mem *blk = opencl_cache->get_block(scene, id);
      bocl_mem *blk_info = opencl_cache->loaded_block_info();

      // is alpha buffer
      bool is_alpha_buffer[1] = {
          (prefixes[i] == boxm2_data_traits<BOXM2_ALPHA>::prefix())};
      bocl_mem is_alpha(device->context(), is_alpha_buffer, sizeof(cl_bool),
                        "is_alpha buffer");
      is_alpha.create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

      // copy parent behavior.. if true, Data copies its parent
      bool copy_parent_buffer[1];
      *copy_parent_buffer =
          (prefixes[i] == boxm2_data_traits<BOXM2_MOG3_GREY>::prefix() ||
           prefixes[i] == boxm2_data_traits<BOXM2_MOG3_GREY_16>::prefix() ||
           prefixes[i] == boxm2_data_traits<BOXM2_MOG6_VIEW>::prefix() ||
           prefixes[i] ==
               boxm2_data_traits<BOXM2_MOG6_VIEW_COMPACT>::prefix() ||
           prefixes[i] == boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix());
      bocl_mem copy_parent(device->context(), copy_parent_buffer,
                           sizeof(cl_bool), "copy_parent buffer");
      copy_parent.create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

      // make it a reference so the destructor isn't called at the end...
      kern->set_arg(blk_info);
      kern->set_arg(blk);
      kern->set_arg(blk_copy.ptr());
      kern->set_arg(tree_sizes.ptr());
      kern->set_arg(dat);
      kern->set_arg(new_dat);
      kern->set_arg(prob_thresh.ptr());
      kern->set_arg(&is_alpha);
      kern->set_arg(&copy_parent);
      kern->set_arg(lookup.ptr());
      kern->set_arg(cl_output.ptr());
      kern->set_local_arg(lThreads[0] * sizeof(cl_uchar16));
      kern->set_local_arg(lThreads[0] * sizeof(cl_uchar16));
      kern->set_local_arg(lThreads[0] * 73 * sizeof(cl_uchar));

      // set workspace
      std::size_t lThreads[] = {64, 1};
      std::size_t gThreads[] = {RoundUp(numTrees, lThreads[0]), 1};

      // execute kernel
      kern->execute(queue, 2, lThreads, gThreads);
      status = clFinish(queue);
      if (!check_val(status, CL_SUCCESS,
                     "REFINE EXECUTE FAILED 2: " + error_to_string(status)))
        return -1;

      kern->clear_args();
      gpu_time += kern->exec_time();

      // write the data to buffer
      opencl_cache->deep_replace_data(scene, id, prefixes[i], new_dat);
      // TODO use index instead of value
      if (i == prefixes.size() - 1) {
        blk->read_to_buffer(queue);
      }

      // ocl cache shifted new dat into data buffer,
      opencl_cache->unref_mem(new_dat);
    }

    // write trees to cpu mem (triggers block.recompute_cells() which
    // recalculates cached cell counts for the block)
    // TODO seems like the trees are already copied over to cpu mem,
    // so it'd be nice to create a way to trigger recompute_cells() without
    // set_trees, which does a mempcy
    boxm2_array_3d<boxm2_block::uchar16> refined_tree_array(
        data.sub_block_num_.x(), data.sub_block_num_.y(),
        data.sub_block_num_.z(),
        static_cast<boxm2_block::uchar16 *>(blk->cpu_buffer()));
    boxm2_block *cpu_blk = opencl_cache->get_cpu_cache()->get_block(scene, id);
    cpu_blk->set_trees(refined_tree_array);
    blk->write_to_buffer(queue);

    // tree copy had CPU mem allocated, needs to be deleted
    delete[](cl_uint16 *) blk_copy->cpu_buffer();
    delete[](cl_int *) tree_sizes->cpu_buffer();
    opencl_cache->unref_mem(blk_copy.ptr());
    opencl_cache->unref_mem(tree_sizes.ptr());
  }
  clFinish(queue);
  std::cout << " Refine GPU Time: " << gpu_time
           << ", transfer time: " << transfer_time << std::endl;
  std::cout << " Number of cells in scene (after refine): " << num_cells
           << std::endl;
  clReleaseCommandQueue(queue);

  // set output
  return num_refined;
}

// compile kernels and place in static map
bocl_kernel *boxm2_ocl_refine::get_refine_tree_kernel(const bocl_device_sptr& device,
                                                      const std::string&  /*opts*/) {
  // find cached kernel
  std::string identifier = device->device_identifier();
  std::string tree_identifier = identifier + "tree";
  if (tree_kernels_.find(tree_identifier) != tree_kernels_.end())
    return tree_kernels_[tree_identifier];

  // otherwise compile
  std::cout << "== Compiling tree kernel for device " << identifier
           << "==" << std::endl;
  std::vector<std::string> src_paths;
  std::string source_dir = boxm2_ocl_util::ocl_src_root();
  src_paths.push_back(source_dir + "scene_info.cl");
  src_paths.push_back(source_dir + "basic/linked_list.cl");
  src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
  src_paths.push_back(source_dir + "bit/refine_bit_scene.cl");

  // refine trees kernel
  auto *refine_tree_kernel = new bocl_kernel();
  refine_tree_kernel->create_kernel(
      &device->context(), device->device_id(), src_paths, "refine_trees",
      " -D MOG_TYPE_8",
      "boxm2 opencl refine trees (pass one)"); // kernel identifier (for error
                                               // checking)
  tree_kernels_[tree_identifier] = refine_tree_kernel;
  return tree_kernels_[tree_identifier];
}

bocl_kernel *boxm2_ocl_refine::get_refine_data_kernel(const bocl_device_sptr& device,
                                                      const std::string& data_type) {
  std::string identifier = device->device_identifier();
  std::string options = get_option_string(boxm2_data_info::datasize(data_type));
  std::string data_identifier = identifier + options;
  if (data_kernels_.find(data_identifier) != data_kernels_.end())
    return data_kernels_[data_identifier];

  // otherwise compile data kernels
  std::cout << "== Compiling refine data kernel for device " << identifier
           << "==" << std::endl;
  std::vector<std::string> src_paths;
  std::string source_dir = boxm2_ocl_util::ocl_src_root();
  src_paths.push_back(source_dir + "scene_info.cl");
  src_paths.push_back(source_dir + "basic/linked_list.cl");
  src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
  src_paths.push_back(source_dir + "bit/refine_bit_scene.cl");

  auto *refine_data_kernel = new bocl_kernel();
  refine_data_kernel->create_kernel(
      &device->context(), device->device_id(), src_paths, "refine_data",
      options, "boxm2 opencl refine data size 2 (pass three)");
  data_kernels_[data_identifier] = refine_data_kernel;
  return data_kernels_[data_identifier];
}

std::string boxm2_ocl_refine::get_option_string(int datasize) {
  std::cout << "DATA SIZE " << datasize << std::endl;
  std::string options = "";
  switch (datasize) {
  case 2:
    options = "-D MOG_TYPE_2 ";
    break;
  case 4:
    options = "-D MOG_TYPE_4 ";
    break;
  case 6:
    options = "-D MOG_TYPE_6 ";
    break;
  case 8:
    options = "-D MOG_TYPE_8 ";
    break;
  case 16:
    options = "-D MOG_TYPE_16 ";
    break;
  case 32:
    options = "-D FLOAT8";
    break;
  default:
    break;
  }
  return options;
}
