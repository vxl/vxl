// This is brl/bseg/boxm2/volm/pro/processes/boxm2_create_index_process2.cxx
//:
// \file
// \brief  A process for creating an index for each location hypothesis of a scene, uses volm_geo_index to access location hypotheses and save index files
//         this process assumes that all the hypotheses in the input set are in the same UTM zone of the world, hypo set generation makes sure the zone is the same for all
//
// \author Ozge C. Ozcanli
// \date Oct 21, 2012

#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>

#include <boxm2/boxm2_scene.h>
#include <boxm2/volm/boxm2_volm_wr3db_index.h>
#include <boxm2/volm/boxm2_volm_wr3db_index_sptr.h>
#include <bbas/volm/volm_spherical_shell_container.h>
#include <bbas/volm/volm_spherical_shell_container_sptr.h>
#include <bbas/volm/volm_loc_hyp.h>
#include <bbas/volm/volm_geo_index.h>
#include <vul/vul_timer.h>
#include <vul/vul_file.h>
#include <vil/vil_save.h>
#include <bkml/bkml_write.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boxm2/boxm2_util.h>
//directory utility
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <bvgl/algo/bvgl_2d_geo_index.h>

namespace boxm2_create_index_process2_globals
{
  constexpr unsigned n_inputs_ = 14;
  constexpr unsigned n_outputs_ = 0;

  void compile_kernel(const bocl_device_sptr& device,std::vector<bocl_kernel*> & vec_kernels)
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
    src_paths.push_back(source_dir + "bit/compute_index.cl");
    src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");

    //set kernel options
    //std::string options = " -D RENDER_DEPTH -D COMPINDEX ";
    std::string options = " -D COMPINDEX -D DETERMINISTIC";
    options += " -D RENDER_VISIBILITY ";
    options += " -D STEP_CELL=step_cell_compute_index(tblock,linfo->block_len,aux_args.alpha,data_ptr,d*linfo->block_len,aux_args.vis,aux_args.expdepth,aux_args.expdepthsqr,aux_args.probsum,aux_args.t)";
    auto* compute_index = new bocl_kernel();

    compute_index->create_kernel(&device->context(),device->device_id(), src_paths, "compute_loc_index", options, "compute_loc_index");
    vec_kernels.push_back(compute_index);

    //create normalize image kernel
    auto * norm_kernel=new bocl_kernel();
    if (!norm_kernel) {
      std::cout << " cannot allocate kernel object!\n" << std::endl; std::cout.flush(); }
    else
      std::cout << " got a new kernel object!\n";

    norm_kernel->create_kernel(&device->context(),device->device_id(), src_paths, "normalize_index_depth_kernel", options,
                               "normalize_index_depth_kernel"); //kernel identifier (for error checking)

    vec_kernels.push_back(norm_kernel);

    return ;
  }
  static std::map<std::string,std::vector<bocl_kernel*> > kernels;

  // function to construct 2d_geo_index quad-tree for scene blocks
  bvgl_2d_geo_index_node_sptr construct_2d_geo_index_for_blks(const boxm2_scene_sptr& scene)
  {
    vgl_box_3d<double> scene_bbox = scene->bounding_box();
    // set the leaf size 4 times larger than the block size
    vgl_box_3d<double> blk_bbox = scene->blocks().begin()->second.bbox();
    float min_size = 4*(blk_bbox.max_x() - blk_bbox.min_x());
    vgl_box_2d<double> scene_bbox_2d(scene_bbox.min_x(), scene_bbox.max_x(), scene_bbox.min_y(), scene_bbox.max_y());
    bvgl_2d_geo_index_node_sptr root = bvgl_2d_geo_index::construct_tree<std::vector<boxm2_block_id> >(scene_bbox_2d, min_size);
    return root;
  }
}

bool boxm2_create_index_process2_cons(bprb_func_process& pro)
{
  using namespace boxm2_create_index_process2_globals;

  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "boxm2_scene_sptr";
  input_types_[2] = "boxm2_opencl_cache_sptr";
  input_types_[3] = "vcl_string"; // folder to read the geo index and the hypo files for each leaf node, pass the string with file separator at the end
  input_types_[4] = "unsigned"; // the id of the tile to index
  input_types_[5] = "float"; // elevation difference to adjust local heights, some scenes need a height adjustment according to their resolution
  input_types_[6] = "float"; // minimum voxel resolution to create spherical container
  input_types_[7] = "float"; // maximum distance in the world that the spherical container will cover
  input_types_[8] = "float"; // the solid angle for the spherical container, the resolution of the voxels will get coarser based on this angle
  input_types_[9] = "vcl_string"; // name of the binary file to read spherical rays
  input_types_[10] = "vcl_string"; // name of output folder to save index binaries for each leaf node
  input_types_[11] = "float"; // visibility threshold to declare a ray a sky ray, it's strictly very small if occupied, so could be as small as 0.3f
  input_types_[12] = "float"; // buffer capacity on CPU RAM for the indices to be cached before being written to disc in chunks
  input_types_[13] = "int"; // the leaf id of the tile, if passed as -1 then run on all leaves

  std::vector<std::string>  output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_create_index_process2(bprb_func_process& pro)
{
  using namespace boxm2_create_index_process2_globals;

  std::size_t local_threads[2]={8,1};
  std::size_t global_threads[2]={0,1};  // global threads size is to be determined later

  //sanity check inputs
  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  float transfer_time=0.0f;
  float gpu_time=0.0f;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  bocl_device_sptr device = pro.get_input<bocl_device_sptr>(i++);
  std::cout << " device: " << device->info().device_name_ << " total memory: " << device->info().total_global_memory_ << '\n'
           << " max allowed work items in a group: " << device->info().max_work_group_size_ << '\n'
           << " max work item sizes in each dimensions: " << device->info().max_work_item_sizes_ << '\n';
  boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(i++);
  vpgl_lvcs lvcs = scene->lvcs();
  boxm2_opencl_cache_sptr  opencl_cache = pro.get_input<boxm2_opencl_cache_sptr>(i++);
  std::string geo_index_folder = pro.get_input<std::string>(i++);
  auto tile_id = pro.get_input<unsigned>(i++);
  boxm2_volm_wr3db_index_params params;
  params.start = 0;
  params.skip = 1;
  auto elev_dif = pro.get_input<float>(i++);
  params.vmin = pro.get_input<float>(i++);
  params.dmax = pro.get_input<float>(i++);
  params.solid_angle = pro.get_input<float>(i++);
  std::string ray_file = pro.get_input<std::string>(i++);
  std::string out_index_folder = pro.get_input<std::string>(i++);
  auto vis_thres = pro.get_input<float>(i++);
  auto buffer_capacity = pro.get_input<float>(i++);
  int leaf_id = pro.get_input<int>(i++);

  volm_spherical_container_sptr sph2 = new volm_spherical_container(params.solid_angle,params.vmin,params.dmax);
  std::cout << "number of depth intervals in container: " << sph2->get_depth_offset_map().size() << " with solid angle: " << params.solid_angle << " vmin: " << params.vmin << " dmax: " << params.dmax << std::endl;

  // read the geo index
  std::stringstream file_name_pre; file_name_pre << geo_index_folder << "geo_index_tile_" << tile_id;
  std::cout << "constructing: " << file_name_pre.str() << std::endl;
  float min_size;
  volm_geo_index_node_sptr r = volm_geo_index::read_and_construct(file_name_pre.str() + ".txt", min_size);
  volm_geo_index::read_hyps(r, file_name_pre.str());
  unsigned size = volm_geo_index::hypo_size(r);
  std::vector<volm_geo_index_node_sptr> leaves;
  //volm_geo_index::get_leaves(r, leaves);
  volm_geo_index::get_leaves_with_hyps(r, leaves);
  std::stringstream out_file_name_pre; out_file_name_pre << out_index_folder << "geo_index_tile_" << tile_id;
  if (!leaves.size()) {
    std::cout << " there are no leaves in this tile!.. returning!\n";
    return true;
  }
  else
    std::cout << " there are " << leaves.size() << " leaves with total: " << size << " hyps in this tile!\n";

  if (leaf_id >= (int)leaves.size()) {
    std::cout << " leaf id: " << leaf_id << " is larger than the number of leaves: " << leaves.size() << ".. returning!\n";
    return false;
  }

  // read spherical shell container
  vsl_b_ifstream ifs(ray_file);
  volm_spherical_shell_container_sptr sph_shell = new volm_spherical_shell_container;
  sph_shell->b_read(ifs);
  ifs.close();

  params.layer_size = (unsigned)sph_shell->get_container_size();
  int layer_size = (int)params.layer_size;

  std::stringstream out_sph_namet; out_sph_namet << out_index_folder << "geo_index_tile_" << tile_id << "_index_sph_shell.vrml";
  sph_shell->draw_template(out_sph_namet.str());

  // write the params file once to the folder for each tile
  std::stringstream out_params_file;  out_params_file << out_index_folder << "geo_index_tile_" << tile_id << "_index";
  std::cout << "writing params to: " << out_params_file.str() + ".params";
  if (!params.write_params_file(out_params_file.str())) {
    std::cerr << "Cannot write params file to " << out_params_file.str() + ".params!\n";
    return false;
  }


  // adjust dmax if scene has very few blocks
  float dmax = params.dmax;
  if (scene->get_block_ids().size() < 5)
    dmax = (float)(scene->bounding_box().height()+scene->bounding_box().width());

  global_threads[0] = RoundUp(layer_size, (int)local_threads[0]);
  std::cout << "layer_size: " << layer_size << ", # of global threads: " << global_threads[0] << '\n';

  //cache size sanity check
  long binCache = (long)(opencl_cache.ptr()->bytes_in_cache());
  std::cout<<"Update MBs in cache: "<<binCache/(1024.0*1024.0)<<std::endl;

  // create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue( device->context(),
                                                 *(device->device_id()),
                                                 CL_QUEUE_PROFILING_ENABLE,
                                                 &status);
  if (status!=0)
    return false;

  // compile the kernel if not already compiled
  std::string identifier=device->device_identifier();
  if (kernels.find(identifier)==kernels.end()) {
    std::cout<<"===========Compiling kernels==========="<<std::endl;
    std::vector<bocl_kernel*> ks;
    compile_kernel(device,ks);
    kernels[identifier]=ks;
  }

  // bit lookup buffer
  cl_uchar lookup_arr[256];
  boxm2_ocl_util::set_bit_lookup(lookup_arr);
  bocl_mem* lookup=new bocl_mem(device->context(), lookup_arr, sizeof(cl_uchar)*256, "bit lookup buffer");
  lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  // create directions buffer
  auto* ray_dirs = new cl_float[4*layer_size];
  std::vector<vgl_point_3d<double> > cart_points = sph_shell->cart_points();
  for (int i = 0; i < layer_size; ++i) {
    ray_dirs[4*i  ] = (cl_float)cart_points[i].x();
    ray_dirs[4*i+1] = (cl_float)cart_points[i].y();
    ray_dirs[4*i+2] = (cl_float)cart_points[i].z();
    ray_dirs[4*i+3] = 0.0f;
  }

  bocl_mem* ray_dir_buffer = new bocl_mem(device->context(), ray_dirs, sizeof(cl_float4)*layer_size, "ray directions buffer");
  ray_dir_buffer->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  bocl_mem* ray_dim_mem = new bocl_mem(device->context(), &(layer_size), sizeof(int), "ray directions size");
  ray_dim_mem->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

  bocl_mem* max_dist = new bocl_mem(device->context(), &(params.dmax), sizeof(int), "max distance to shoot rays");
  max_dist->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );
  std::cout << " will stop ray casting at distance: " << params.dmax << std::endl;

  std::map<boxm2_block_id, boxm2_block_metadata>& blocks = scene->blocks();
  std::cout << "number of blocks: " << blocks.size() << std::endl;
  std::cout.flush();
  // get subblk dimension
  boxm2_block_metadata mdata = scene->get_block_metadata(blocks.begin()->first);
  auto subblk_dim = (float)mdata.sub_block_dim_.x();
  std::cout << "subblk_dim: " << subblk_dim << std::endl;
  bocl_mem*  subblk_dim_mem=new bocl_mem(device->context(), &(subblk_dim), sizeof(float), "sub block dim buffer");
  subblk_dim_mem->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

  std::map<boxm2_block_id, std::vector<boxm2_block_id> > order_cache;

  boxm2_block_id curr_block;

  // create 2d_geo_index for scene blocks for quick location block where the given location resides
  // the quad tree is in scene local coordinate system
  bvgl_2d_geo_index_node_sptr blk_2d_tree = construct_2d_geo_index_for_blks(scene);
  // fill in the block ids into leaves
  std::vector<bvgl_2d_geo_index_node_sptr> blk_leaves_all;
  bvgl_2d_geo_index::get_leaves(blk_2d_tree, blk_leaves_all);
  for (unsigned l_idx = 0; l_idx < blk_leaves_all.size(); l_idx++) {
    auto* leaf_ptr =
      dynamic_cast<bvgl_2d_geo_index_node<std::vector<boxm2_block_id> >* >(blk_leaves_all[i].ptr());
    leaf_ptr->contents_.clear();
  }
  std::map<boxm2_block_id, boxm2_block_metadata> blks = scene->blocks();
  for (auto & blk : blks) {
    boxm2_block_id curr_blk_id = blk.first;
    vgl_box_2d<double> curr_blk_bbox_2d(blk.second.bbox().min_x(), blk.second.bbox().max_x(), blk.second.bbox().min_y(), blk.second.bbox().max_y());
    std::vector<bvgl_2d_geo_index_node_sptr> leaves;
    bvgl_2d_geo_index::get_leaves(blk_2d_tree, leaves, curr_blk_bbox_2d);
    if (leaves.empty())
      continue;
    for (auto & leave : leaves) {
      auto* leaf_ptr =
        dynamic_cast<bvgl_2d_geo_index_node<std::vector<boxm2_block_id> >* >(leave.ptr());
      leaf_ptr->contents_.push_back(curr_blk_id);
    }
  }

  //zip through each location hypothesis
  std::vector<volm_geo_index_node_sptr> leaves2;
  if (leaf_id < 0) leaves2 = leaves;
  else leaves2.push_back(leaves[leaf_id]);
  std::cout << " will index " << leaves2.size() << " leaves!\n"; std::cout.flush();
  for (auto & li : leaves2) {
    if (!li->hyps_)
      continue;
    std::cout << " will index " << volm_geo_index::hypo_size(li) << " indices in leaf: " << li->get_hyp_name("") << std::endl; std::cout.flush();

    // create a binary index file for each hypo set in a leaf
    boxm2_volm_wr3db_index_sptr ind = new boxm2_volm_wr3db_index(layer_size, buffer_capacity);
    std::string index_file = li->get_index_name(out_file_name_pre.str());
    if (!ind->initialize_write(index_file)) {
      std::cerr << "Cannot initialize " << index_file << " for write!\n";
      return false;
    }

    vgl_point_3d<double> h_pt;
    unsigned indexed_cnt = 0;
    while (li->hyps_->get_next(0, 1, h_pt))
    {
      //std::cout << "Processing hypothesis lon: " << h_pt.x() << " lat: " << h_pt.y() << " z: " << h_pt.z() << std::endl;
      if (indexed_cnt%1000 == 0) std::cout << indexed_cnt << "." << std::flush;
      double lx, ly, lz;
      lvcs.global_to_local(h_pt.x(), h_pt.y(), h_pt.z(), vpgl_lvcs::wgs84, lx, ly, lz);
      //lz = 2.0*(std::ceil(lz/2.0)); // round to next multiple of 2 meters // this is the height in the voxel model
      lz = std::ceil(lz); // get ceil to get terrain height for world with res 1 m
      lz = lz - elev_dif;  // the camera is elev dif above the terrain height
      vgl_point_3d<double> local_h_pt_d(lx, ly, lz);

      cl_float loc_arr[4];
      loc_arr[0] = (cl_float)lx; loc_arr[1] = (cl_float)ly; loc_arr[2] = (cl_float)lz;
      loc_arr[3] = 1.0f;
      bocl_mem* hypo_location = new bocl_mem(device->context(), loc_arr, sizeof(cl_float4), "location buffer");
      hypo_location->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

      // Output Arrays
      auto* buff = new float[layer_size];
      for (int i=0;i<layer_size;++i) buff[i]=0.0f;
      auto* vis_buff = new float[layer_size];
      for (int i=0;i<layer_size;++i) vis_buff[i]=1.0f;
      auto* prob_buff = new float[layer_size];
      for (int i=0;i<layer_size;++i) prob_buff[i]=0.0f;
      auto* t_infinity_buff = new float[layer_size];
      for (int i=0;i<layer_size;++i) t_infinity_buff[i]=0.0f;

      bocl_mem* exp_depth=new bocl_mem(device->context(),buff,layer_size*sizeof(float),"exp depth buffer");
      exp_depth->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

      bocl_mem* vis=new bocl_mem(device->context(),vis_buff,layer_size*sizeof(float),"visibility buffer");
      vis->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

      bocl_mem* probs=new bocl_mem(device->context(),prob_buff,layer_size*sizeof(float),"vis x omega buffer");
      probs->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

      bocl_mem* t_infinity=new bocl_mem(device->context(),t_infinity_buff,layer_size*sizeof(float),"t infinity buffer");
      t_infinity->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

      // find its block for local_h_pt
      bvgl_2d_geo_index_node_sptr curr_leaf = nullptr;
      bvgl_2d_geo_index::get_leaf(blk_2d_tree, curr_leaf, vgl_point_2d<double>(local_h_pt_d.x(), local_h_pt_d.y()));
      if (curr_leaf) {
        auto* curr_leaf_ptr =
          dynamic_cast<bvgl_2d_geo_index_node<std::vector<boxm2_block_id> >* >(curr_leaf.ptr());
        bool found_blk = false;
        for (auto & content : curr_leaf_ptr->contents_) {
          vgl_box_3d<double> curr_blk_bbox = scene->blocks()[content].bbox();
          if (curr_blk_bbox.contains(local_h_pt_d)) {
            curr_block = content;
            found_blk = true;
          }
        }
        if (!found_blk) {
          std::cerr << " Scene does not contain hypothesis lon: " << h_pt.x() << " lat: " << h_pt.y() << ' ' << local_h_pt_d << " writing empty array for it!\n";
          std::vector<unsigned char> values(layer_size, 0);
          ind->add_to_index(values);
          ++indexed_cnt;
          // release the device and host memories
          delete exp_depth;  // calls release_memory() which enqueues a mem delete event, call clFinish to make sure it is executed
          delete vis;
          delete probs;
          delete t_infinity;
          delete hypo_location;
          status = clFinish(queue);
          check_val(status, MEM_FAILURE, "release memory FAILED: " + error_to_string(status));
          if (!buff)
            std::cout << "buff is zero after release mem!\n"; std::cout.flush();
          delete [] buff;
          delete [] vis_buff;
          delete [] prob_buff;
          delete [] t_infinity_buff;
          continue;
        }
      }
      else {
        std::cerr << " Scene does not contain hypothesis lon: " << h_pt.x() << " lat: " << h_pt.y() << ' ' << local_h_pt_d << " writing empty array for it!\n";
        std::vector<unsigned char> values(layer_size, 0);
        ind->add_to_index(values);
        ++indexed_cnt;
        // release the device and host memories
        delete exp_depth;  // calls release_memory() which enqueues a mem delete event, call clFinish to make sure it is executed
        delete vis;
        delete probs;
        delete t_infinity;
        delete hypo_location;
        status = clFinish(queue);
        check_val(status, MEM_FAILURE, "release memory FAILED: " + error_to_string(status));
        if (!buff)
          std::cout << "buff is zero after release mem!\n"; std::cout.flush();
        delete [] buff;
        delete [] vis_buff;
        delete [] prob_buff;
        delete [] t_infinity_buff;
        continue;
      }


#if 0
      vgl_point_3d<double> local;
      if (!scene->block_contains(local_h_pt_d, curr_block, local))
      {
        if (!scene->contains(local_h_pt_d, curr_block, local)) {
          std::cerr << " Scene does not contain hypothesis lon: " << h_pt.x() << " lat: " << h_pt.y() << ' ' << local_h_pt_d << " writing empty array for it!\n";
          std::vector<unsigned char> values(layer_size, 0);
          ind->add_to_index(values);
          ++indexed_cnt;
          // release the device and host memories
          delete exp_depth;  // calls release_memory() which enqueues a mem delete event, call clFinish to make sure it is executed
          delete vis;
          delete probs;
          delete t_infinity;
          delete hypo_location;
          status = clFinish(queue);
          check_val(status, MEM_FAILURE, "release memory FAILED: " + error_to_string(status));
          if (!buff)
            std::cout << "buff is zero after release mem!\n"; std::cout.flush();
          delete [] buff;
          delete [] vis_buff;
          delete [] prob_buff;
          delete [] t_infinity_buff;
          continue;
        }
      }
#endif

      auto ord_iter = order_cache.find(curr_block);
      if (!(ord_iter != order_cache.end())) {
        order_cache[curr_block] =  boxm2_util::order_about_a_block(scene, curr_block, dmax);
        if (order_cache.size() > 100) {// kick the first one
          auto to_kick = order_cache.begin();
          if (to_kick->first != curr_block)
            order_cache.erase(to_kick);
          else { ++to_kick; order_cache.erase(to_kick); }
        }
      }
      std::vector<boxm2_block_id>& vis_blocks = order_cache[curr_block];

      std::vector<boxm2_block_id>::iterator blk_iter_inner;

      for (blk_iter_inner = vis_blocks.begin(); blk_iter_inner != vis_blocks.end(); ++blk_iter_inner) {
        boxm2_block_id id_inner = *blk_iter_inner;

        //load tree and alpha
        bocl_kernel* kern =  kernels[identifier][0];

        vul_timer transfer;

        bocl_mem* blk = opencl_cache->get_block(scene, id_inner);
        bocl_mem* blk_info = opencl_cache->loaded_block_info();
        bocl_mem* alpha = opencl_cache->get_data<BOXM2_ALPHA>(scene, id_inner);

        transfer_time += (float) transfer.all();

        kern->set_arg( blk_info );
        kern->set_arg( ray_dir_buffer);
        kern->set_arg( ray_dim_mem);
        kern->set_arg( blk );
        kern->set_arg( lookup  );
        kern->set_arg( alpha  );
        kern->set_arg( hypo_location );
        kern->set_arg( max_dist );
        kern->set_arg( exp_depth );
        kern->set_arg( vis );
        kern->set_arg( probs );
        kern->set_arg( t_infinity );
        kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_uchar16) );//local tree,
        kern->set_local_arg( local_threads[0]*local_threads[1]*10*sizeof(cl_uchar) ); //cumsum buffer, imindex buffer

        //execute kernel
        if (!kern->execute(queue, 2, local_threads, global_threads)) {
          std::cerr << "INDEX KERNEL EXECUTE FAILED!\n";
          return false;
        }
        int status = clFinish(queue);
        if (status != 0) {
          std::cerr << "status: " << status << "  INDEX EXECUTE FAILED: " + error_to_string(status) << '\n';
          return false;
        }
        gpu_time += kern->exec_time();

        //clear render kernel args so it can reset em on next execution
        kern->clear_args();

        //remove from device memory unnecessary items
        opencl_cache->shallow_remove_block(scene, id_inner);
        opencl_cache->shallow_remove_data(scene, id_inner,boxm2_data_traits<BOXM2_ALPHA>::prefix());
        //opencl_cache->clear_cache();
      }
      if (vis_blocks.size() != 0)  // normalize
      {
        bocl_kernel* normalize_kern= kernels[identifier][1];
        normalize_kern->set_arg( exp_depth );
        normalize_kern->set_arg( vis );
        normalize_kern->set_arg( t_infinity );
        normalize_kern->set_arg( subblk_dim_mem );
        normalize_kern->execute( queue, 2, local_threads, global_threads);
        clFinish(queue);
        gpu_time += normalize_kern->exec_time();

        //clear render kernel args so it can reset em on next execution
        normalize_kern->clear_args();

        //read from gpu
        exp_depth->read_to_buffer(queue);
        vis->read_to_buffer(queue);

        status = clFinish(queue);
        check_val(status, MEM_FAILURE, "read to output buffers FAILED: " + error_to_string(status));
      }
    #if 0
      std::cout << "exp depths after normalization:\n";
      for (unsigned i = 0; i < layer_size; ++i) {
        std::cout << buff[i] << " (" << vis_buff[i] << ") ";
      }
      std::cout << std::endl;
    #endif
      // find each depth interval using spherical container
      std::vector<unsigned char> values;
      for (int i = 0; i < layer_size; ++i) {
        // check if sky
        if (vis_buff[i] > vis_thres) {
          if (buff[i] > 0)  //  if the ray goes into the world vis stays 1 but depth stays 0 too, so don't confuse that with sky
            values.push_back((unsigned char)254);
          else
              values.push_back((unsigned char)253); // pass an invalid depth interval, not a valid occupied surface, vis = 1 but depth = 0
        }
        else
          values.push_back(sph2->get_depth_interval((double)buff[i]));
      }
    #if 0
      std::cout << "values array: \n";
      for (unsigned i = 0; i < layer_size; i++) {
        std::cout << (int)values[i] << " ";
      }
      std::cout << std::endl;
      sph_shell->draw_template("./test.vrml", values, 254);
    #endif
      // add to index
      ind->add_to_index(values);
      ++indexed_cnt;

      // release the device and host memories
      delete exp_depth;  // calls release_memory() which enqueues a mem delete event, call clFinish to make sure it is executed
      delete vis;
      delete probs;
      delete t_infinity;
      delete hypo_location;

      status = clFinish(queue);
      check_val(status, MEM_FAILURE, "release memory FAILED: " + error_to_string(status));

      if (!buff)
        std::cout << "buff is zero after release mem!\n"; std::cout.flush();
      delete [] buff;
      delete [] vis_buff;
      delete [] prob_buff;
      delete [] t_infinity_buff;
    }
    ind->finalize();
    boxm2_volm_wr3db_index_params::write_size_file(index_file, indexed_cnt);

    opencl_cache->clear_cache();
  }

  delete ray_dir_buffer;
  delete ray_dim_mem;
  delete lookup;
//  delete subblk_dim_mem;

  status = clFinish(queue);
  check_val(status, MEM_FAILURE, "release memory FAILED: " + error_to_string(status));

  delete [] ray_dirs;

  std::cout<<"\nGPU Execute time "<<gpu_time<<" ms = " << gpu_time/(1000.0*60.0) << " secs. " << std::endl;
  std::cout<<"GPU Transfe time "<<transfer_time<<" ms = " << transfer_time/(1000.0*60.0) << " secs. " << std::endl;
  clReleaseCommandQueue(queue);

  //cache size sanity check
  binCache = (long)(opencl_cache.ptr()->bytes_in_cache());
  std::cout<<"At the end of process MBs in cache: "<<binCache/(1024.0*1024.0)<<std::endl;

  return true;
}
