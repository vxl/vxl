// This is brl/bseg/boxm2/volm/pro/processes/boxm2_create_index_process.cxx
//:
// \file
// \brief  A process for creating an index for each location hypothesis of a scene
//
// \author Ozge C. Ozcanli
// \date Oct 21, 2012

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
#include <vul/vul_file_iterator.h>
#include <vil/vil_save.h>
#include <bkml/bkml_write.h>

#include <vcl_fstream.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boxm2/boxm2_util.h>
//directory utility
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>

namespace boxm2_create_index_process_globals
{
  const unsigned n_inputs_ = 14;
  const unsigned n_outputs_ = 0;

  void compile_kernel(bocl_device_sptr device,vcl_vector<bocl_kernel*> & vec_kernels)
  {
    //gather all render sources... seems like a lot for rendering...
    vcl_vector<vcl_string> src_paths;
    vcl_string source_dir = boxm2_ocl_util::ocl_src_root();
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
    //vcl_string options = " -D RENDER_DEPTH -D COMPINDEX ";
    vcl_string options = " -D COMPINDEX -D DETERMINISTIC";
    options += " -D RENDER_VISIBILITY ";
    options += " -D STEP_CELL=step_cell_compute_index(tblock,aux_args.alpha,data_ptr,d*linfo->block_len,aux_args.vis,aux_args.expdepth,aux_args.expdepthsqr,aux_args.probsum,aux_args.t)";
    bocl_kernel* compute_index = new bocl_kernel();

    compute_index->create_kernel(&device->context(),device->device_id(), src_paths, "compute_loc_index", options, "compute_loc_index");
    vec_kernels.push_back(compute_index);

    //create normalize image kernel
    bocl_kernel * norm_kernel=new bocl_kernel();
    if (!norm_kernel) {
      vcl_cout << " cannot allocate kernel object!" << vcl_endl; }
    else
      vcl_cout << " got a new kernel object!" << vcl_endl;

    norm_kernel->create_kernel(&device->context(),device->device_id(), src_paths, "normalize_index_depth_kernel", options,
                               "normalize_index_depth_kernel"); //kernel identifier (for error checking)


    vec_kernels.push_back(norm_kernel);

    return ;
  }
  static vcl_map<vcl_string,vcl_vector<bocl_kernel*> > kernels;
}

bool boxm2_create_index_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_create_index_process_globals;

  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "boxm2_scene_sptr";
  input_types_[2] = "boxm2_opencl_cache_sptr";
  input_types_[3] = "vcl_string"; // binary hypotheses file with lat, lon, elev positions to generate indices for
  input_types_[4] = "unsigned"; // start value, which hypo to start indexing from
  input_types_[5] = "unsigned"; // skip value, how many hypos will be skipped before being processed
  input_types_[6] = "float"; // elevation difference to adjust local heights, some scenes need a height adjustment according to their resolution
  input_types_[7] = "float"; // minimum voxel resolution to create spherical container
  input_types_[8] = "float"; // maximum distance in the world that the spherical container will cover
  input_types_[9] = "float"; // the solid angle for the spherical container, the resolution of the voxels will get coarser based on this angle
  input_types_[10] = "vcl_string"; // ray array binary file
  input_types_[11] = "vcl_string"; // name of output file to save the index
  input_types_[12] = "float"; // visibility threshold to declare a ray a sky ray, it's strictly very small if occupied, so could be as small as 0.3f
  input_types_[13] = "float"; // buffer capacity on CPU RAM for the indices to be cached before being written to disc in chunks

  vcl_vector<vcl_string>  output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_create_index_process(bprb_func_process& pro)
{
  using namespace boxm2_create_index_process_globals;

  vcl_size_t local_threads[2]={8,1};
  vcl_size_t global_threads[2]={0,1};  // global threads size is to be determined later

  //sanity check inputs
  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  float transfer_time=0.0f;
  float gpu_time=0.0f;

  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  bocl_device_sptr device = pro.get_input<bocl_device_sptr>(i++);
  vcl_cout << " device: " << device->info().device_name_ << " total memory: " << device->info().total_global_memory_ << '\n'
           << " max allowed work items in a group: " << device->info().max_work_group_size_ << '\n'
           << " max work item sizes in each dimensions: " << device->info().max_work_item_sizes_ << vcl_endl;
  boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(i++);
  vpgl_lvcs lvcs = scene->lvcs();
  boxm2_opencl_cache_sptr  opencl_cache = pro.get_input<boxm2_opencl_cache_sptr>(i++);
  //boxm2_volm_loc_hypotheses_sptr hyp = pro.get_input<boxm2_volm_loc_hypotheses_sptr>(i++);
  vcl_string hyp_file = pro.get_input<vcl_string>(i++);
  boxm2_volm_wr3db_index_params params;
  params.start = pro.get_input<unsigned>(i++);
  params.skip = pro.get_input<unsigned>(i++);
  float elev_dif = pro.get_input<float>(i++);
  params.vmin = pro.get_input<float>(i++);
  params.dmax = pro.get_input<float>(i++);
  params.solid_angle = pro.get_input<float>(i++);
  vcl_string ray_file = pro.get_input<vcl_string>(i++);
  vcl_string index_file = pro.get_input<vcl_string>(i++);
  float vis_thres = pro.get_input<float>(i++);
  float buffer_capacity = pro.get_input<float>(i++);

  volm_spherical_container_sptr sph2 = new volm_spherical_container(params.solid_angle,params.vmin,params.dmax);
  vcl_cout << "number of depth intervals in container: " << sph2->get_depth_offset_map().size()
           << " with solid angle: " << params.solid_angle
           << " vmin: " << params.vmin << " dmax: " << params.dmax << vcl_endl;

  //: read the location hypotheses
  if (!vul_file::exists(hyp_file)) {
    vcl_cerr << "Cannot find: " << hyp_file << "!\n";
    return false;
  }

  volm_loc_hyp hyp(hyp_file);
  vcl_cout << hyp.size() << " hypotheses read from: " << hyp_file << vcl_endl;

  // read spherical shell container
  vsl_b_ifstream ifs(ray_file);
  volm_spherical_shell_container_sptr sph_shell = new volm_spherical_shell_container;
  sph_shell->b_read(ifs);
  ifs.close();

  params.layer_size = (unsigned)sph_shell->get_container_size();
  int layer_size = (int)params.layer_size;

  boxm2_volm_wr3db_index_sptr ind = new boxm2_volm_wr3db_index(params.layer_size, buffer_capacity);
  if (!ind->initialize_write(index_file)) {
    vcl_cerr << "Cannot initialize " << index_file << " for write!\n";
    return false;
  }

  if (!params.write_params_file(index_file)) {
    vcl_cerr << "Cannot write params file for " << index_file << "!\n";
    return false;
  }

  //: adjust dmax if scene has very few blocks
  float dmax = params.dmax;
  if (scene->get_block_ids().size() < 5)
    dmax = (float)(scene->bounding_box().height()+scene->bounding_box().width());

  global_threads[0] = RoundUp(layer_size, (int)local_threads[0]);
  vcl_cout << "layer_size: " << layer_size << ", # of global threads: " << global_threads[0] << vcl_endl;

  //cache size sanity check
  long binCache = (long)(opencl_cache.ptr()->bytes_in_cache());
  vcl_cout<<"Update MBs in cache: "<<binCache/(1024.0*1024.0)<<vcl_endl;

  // create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue( device->context(),
                                                 *(device->device_id()),
                                                 CL_QUEUE_PROFILING_ENABLE,
                                                 &status);
  if (status!=0)
    return false;

  // compile the kernel if not already compiled
  vcl_string identifier=device->device_identifier();
  if (kernels.find(identifier)==kernels.end()) {
    vcl_cout<<"===========Compiling kernels==========="<<vcl_endl;
    vcl_vector<bocl_kernel*> ks;
    compile_kernel(device,ks);
    kernels[identifier]=ks;
  }

  // bit lookup buffer
  cl_uchar lookup_arr[256];
  boxm2_ocl_util::set_bit_lookup(lookup_arr);
  bocl_mem* lookup=new bocl_mem(device->context(), lookup_arr, sizeof(cl_uchar)*256, "bit lookup buffer");
  lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  // create directions buffer
  cl_float* ray_dirs = new cl_float[4*layer_size];
  vcl_vector<vgl_point_3d<double> > cart_points = sph_shell->cart_points();
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
  vcl_cout << " will stop ray casting at distance: " << params.dmax << vcl_endl;

  vcl_map<boxm2_block_id, boxm2_block_metadata>& blocks = scene->blocks();
  vcl_cout << "number of blocks: " << blocks.size() << vcl_endl;
  vcl_cout.flush();
  //: get subblk dimension
  boxm2_block_metadata mdata = scene->get_block_metadata(blocks.begin()->first);
  float subblk_dim = (float)mdata.sub_block_dim_.x();
  vcl_cout << "subblk_dim: " << subblk_dim << vcl_endl;
  bocl_mem*  subblk_dim_mem=new bocl_mem(device->context(), &(subblk_dim), sizeof(float), "sub block dim buffer");
  subblk_dim_mem->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

  vcl_map<boxm2_block_id, vcl_vector<boxm2_block_id> > order_cache;

  boxm2_block_id curr_block;

  //zip through each location hypothesis
  vgl_point_3d<double> h_pt;
  unsigned indexed_cnt = 0;
  while (hyp.get_next(params.start, params.skip, h_pt))
  {
#ifdef DEBUG
    vcl_cout << "Processing hypothesis: " << hi
             << " x: " << hyp->locs_[hi].x() << " y: " << hyp->locs_[hi].y() << " z: " << hyp->locs_[hi].z() << '\n'
             << "Processing hypothesis: " << hyp.current_-skip
             << " x: " << h_pt.x() << " y: " << h_pt.y() << " z: " << h_pt.z() << vcl_endl;
#endif
    if (indexed_cnt%1000 == 0) vcl_cout << indexed_cnt << '.';
    double lx, ly, lz;
    lvcs.global_to_local(h_pt.x(), h_pt.y(), h_pt.z(), vpgl_lvcs::wgs84, lx, ly, lz);
    lz = 2.0*(vcl_ceil(lz/2.0)); // round to next multiple of 2 meters // this is the height in the voxel model
    lz = lz - elev_dif;  // this is where the camera is
    vgl_point_3d<double> local_h_pt_d(lx, ly, lz);

    cl_float loc_arr[4];
    loc_arr[0] = (cl_float)lx; loc_arr[1] = (cl_float)ly; loc_arr[2] = (cl_float)lz;
    loc_arr[3] = 1.0f;
    bocl_mem* hypo_location = new bocl_mem(device->context(), loc_arr, sizeof(cl_float4), "location buffer");
    hypo_location->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

    // Output Arrays
    float* buff = new float[layer_size];
    for (int i=0;i<layer_size;++i) buff[i]=0.0f;
    float* vis_buff = new float[layer_size];
    for (int i=0;i<layer_size;++i) vis_buff[i]=1.0f;
    float* prob_buff = new float[layer_size];
    for (int i=0;i<layer_size;++i) prob_buff[i]=0.0f;
    float* t_infinity_buff = new float[layer_size];
    for (int i=0;i<layer_size;++i) t_infinity_buff[i]=0.0f;

    bocl_mem* exp_depth=new bocl_mem(device->context(),buff,layer_size*sizeof(float),"exp depth buffer");
    exp_depth->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

    bocl_mem* vis=new bocl_mem(device->context(),vis_buff,layer_size*sizeof(float),"visibility buffer");
    vis->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

    bocl_mem* probs=new bocl_mem(device->context(),prob_buff,layer_size*sizeof(float),"vis x omega buffer");
    probs->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

    bocl_mem* t_infinity=new bocl_mem(device->context(),t_infinity_buff,layer_size*sizeof(float),"t infinity buffer");
    t_infinity->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

    // find its block
    vgl_point_3d<double> local;
    if (!scene->block_contains(local_h_pt_d, curr_block, local))
    {
      if (!scene->contains(local_h_pt_d, curr_block, local)) {
        vcl_cerr << " Scene does not contain hypothesis: " << hyp.current_-params.skip
                 << ' ' << local_h_pt_d << " writing empty array for it!\n";
        vcl_vector<unsigned char> values(layer_size, 0);
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
          vcl_cout << "buff is zero after release mem!" << vcl_endl;
        delete [] buff;
        delete [] vis_buff;
        delete [] prob_buff;
        delete [] t_infinity_buff;
        continue;
      }
    }

    vcl_map<boxm2_block_id, vcl_vector<boxm2_block_id> >::iterator ord_iter = order_cache.find(curr_block);
    if (!(ord_iter != order_cache.end())) {
      order_cache[curr_block] =  boxm2_util::order_about_a_block(scene, curr_block, dmax);
      if (order_cache.size() > 100) {// kick the first one
        vcl_map<boxm2_block_id, vcl_vector<boxm2_block_id> >::iterator to_kick = order_cache.begin();
        if (to_kick->first != curr_block)
          order_cache.erase(to_kick);
        else { ++to_kick; order_cache.erase(to_kick); }
      }
    }
    vcl_vector<boxm2_block_id>& vis_blocks = order_cache[curr_block];

    vcl_vector<boxm2_block_id>::iterator blk_iter_inner;

    for (blk_iter_inner = vis_blocks.begin(); blk_iter_inner != vis_blocks.end(); ++blk_iter_inner) {
      boxm2_block_id id_inner = *blk_iter_inner;

      //load tree and alpha
      bocl_kernel* kern =  kernels[identifier][0];

      vul_timer transfer;

      bocl_mem* blk       = opencl_cache->get_block(id_inner);
      bocl_mem* blk_info  = opencl_cache->loaded_block_info();
      bocl_mem* alpha = opencl_cache->get_data<BOXM2_ALPHA>(id_inner);

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
        vcl_cerr << "INDEX KERNEL EXECUTE FAILED!\n";
        return false;
      }
      int status = clFinish(queue);
      if (status != 0) {
        vcl_cerr << "status: " << status << "  INDEX EXECUTE FAILED: " + error_to_string(status) << '\n';
        return false;
      }
      gpu_time += kern->exec_time();

      //clear render kernel args so it can reset em on next execution
      kern->clear_args();

      //remove from device memory unnecessary items
      //opencl_cache->shallow_remove_data(id_inner,boxm2_data_traits<BOXM2_ALPHA>::prefix());
      //opencl_cache->shallow_remove_block(id_inner); // also remove blk_info
      //opencl_cache->clear_cache();

      status = clFinish(queue);
      check_val(status, MEM_FAILURE, "opencl clear cache FAILED: " + error_to_string(status));
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
    vcl_cout << "exp depths after normalization:\n";
    for (unsigned i = 0; i < layer_size; ++i) {
      vcl_cout << buff[i] << " (" << vis_buff[i] << ") ";
    }
    vcl_cout << vcl_endl;
#endif
    // find each depth interval using spherical container
    vcl_vector<unsigned char> values;
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
    vcl_cout << "values array:\n";
    for (unsigned i = 0; i < layer_size; i++) {
      vcl_cout << (int)values[i] << ' ';
    }
    vcl_cout << vcl_endl;
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
      vcl_cout << "buff is zero after release mem!" << vcl_endl;
    delete [] buff;
    delete [] vis_buff;
    delete [] prob_buff;
    delete [] t_infinity_buff;
  }

  delete ray_dir_buffer;
  delete ray_dim_mem;
  delete lookup;
//  delete subblk_dim_mem;

  status = clFinish(queue);
  check_val(status, MEM_FAILURE, "release memory FAILED: " + error_to_string(status));

  delete [] ray_dirs;

  vcl_cout<<"\nGPU Execute time "<<gpu_time<<" ms = " << gpu_time/(1000.0*60.0) << " secs.\n"
          <<"GPU Transfer time "<<transfer_time<<" ms = " << transfer_time/(1000.0*60.0) << " secs." << vcl_endl;
  clReleaseCommandQueue(queue);

  //cache size sanity check
  binCache = (long)(opencl_cache.ptr()->bytes_in_cache());
  vcl_cout<<"At the end of process MBs in cache: "<<binCache/(1024.0*1024.0)<<vcl_endl;

  ind->finalize();
  boxm2_volm_wr3db_index_params::write_size_file(index_file, indexed_cnt);
  return true;
}

namespace boxm2_visualize_index_process_globals
{
  const unsigned n_inputs_ = 6;
  const unsigned n_outputs_ = 0;
}

bool boxm2_visualize_index_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_visualize_index_process_globals;

  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string"; // index file
  input_types_[1] = "float"; // buffer capacity for the index
  input_types_[2] = "unsigned";  // start id of the indices to visualize
  input_types_[3] = "unsigned";  // end id of the indices to visualize
  input_types_[4] = "vcl_string";  // prefix for the output files
  input_types_[5] = "vcl_string"; // ray array binary file

  vcl_vector<vcl_string>  output_types_(n_outputs_);
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_visualize_index_process(bprb_func_process& pro)
{
  using namespace boxm2_visualize_index_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  unsigned i = 0;
  vcl_string index_file = pro.get_input<vcl_string>(i++);
  float buffer_capacity = pro.get_input<float>(i++);
  unsigned si = pro.get_input<unsigned>(i++);
  unsigned ei = pro.get_input<unsigned>(i++);
  vcl_string prefix = pro.get_input<vcl_string>(i++);
  vcl_string ray_file = pro.get_input<vcl_string>(i++);

  vcl_string param_file = vul_file::strip_extension(index_file) + ".params";
  boxm2_volm_wr3db_index_params params;
  if (!params.read_params_file(param_file)) {
    vcl_cerr << "cannot read: " << param_file << '\n';
    return false;
  }
  if (ei == 0) {
    unsigned long eis;
    vcl_string size_file = vul_file::strip_extension(index_file) + ".txt";
    params.read_size_file(size_file, eis);
    vcl_cout << " ei was zero so made it size of the index file: " << eis << vcl_endl;
    ei = (unsigned)eis;
  }

  // read spherical shell container
  vsl_b_ifstream ifs(ray_file);
  volm_spherical_shell_container_sptr sph_shell = new volm_spherical_shell_container;
  sph_shell->b_read(ifs);
  ifs.close();

  unsigned layer_size = (unsigned)sph_shell->get_container_size();
  boxm2_volm_wr3db_index_sptr ind = new boxm2_volm_wr3db_index(layer_size, buffer_capacity);
  if (!ind->initialize_read(index_file)) {
    vcl_cerr << "Cannot initialize index from file: " << index_file << '\n';
    return false;
  }

  vcl_vector<unsigned char> values(layer_size);
  for (unsigned j = 0; j < si; ++j)
    ind->get_next(values);
  for (unsigned j = si; j < ei; ++j) {
    vcl_stringstream str; str << prefix << '_' << j;
    vcl_string temp_name = str.str() + ".vrml";
    ind->get_next(values);
#if 0
    vcl_cout << "j: " << j << " values array:\n";
    for (unsigned i = 0; i < layer_size; i++) {
      vcl_cout << (int)values[i] << ' ';
    }
    vcl_cout << vcl_endl;
#endif
    sph_shell->draw_template(temp_name, values, (unsigned char)254);
    vil_image_view<vil_rgb<vxl_byte> > img;
    sph_shell->panaroma_img(img, values);
    vcl_string img_name = str.str() + ".png";
    vil_save(img, img_name.c_str());
  }
  return true;
}


//////// visualize the index closest to the given lat, lon position
//       assumes that hyp file order and index file order are the same, so uses hyp_id to retrieve index from the binary index file
namespace boxm2_visualize_index_process2_globals
{
  const unsigned n_inputs_ = 7;
  const unsigned n_outputs_ = 0;
}

bool boxm2_visualize_index_process2_cons(bprb_func_process& pro)
{
  using namespace boxm2_visualize_index_process2_globals;

  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string"; // geo index hyp folder
  input_types_[1] = "vcl_string"; // visibility index folder
  input_types_[2] = "vcl_string"; // ray array binary file
  input_types_[3] = "unsigned";  // tile id
  input_types_[4] = "float";  // lat
  input_types_[5] = "float";  // lon
  input_types_[6] = "int"; // 0: visualize depth interval, 1: class labels, 2: orientation labels (in this case, identifier for the data blocks is assumed to "orientation")

  vcl_vector<vcl_string>  output_types_(n_outputs_);
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_visualize_index_process2(bprb_func_process& pro)
{
  using namespace boxm2_visualize_index_process2_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  unsigned i = 0;
  vcl_string geo_index_hyp_folder = pro.get_input<vcl_string>(i++);
  vcl_string visibility_index_folder = pro.get_input<vcl_string>(i++);
  vcl_string ray_file = pro.get_input<vcl_string>(i++);
  unsigned tile_id = pro.get_input<unsigned>(i++);
  float lat = pro.get_input<float>(i++);
  float lon = pro.get_input<float>(i++);
  int data_type = pro.get_input<int>(i++);

  vcl_stringstream file_name_pre; file_name_pre << geo_index_hyp_folder << "geo_index_tile_" << tile_id;
  vcl_cout << "constructing: " << file_name_pre.str() << vcl_endl;
  float min_size;
  volm_geo_index_node_sptr root = volm_geo_index::read_and_construct(file_name_pre.str() + ".txt", min_size);
  volm_geo_index::read_hyps(root, file_name_pre.str());
  unsigned size = volm_geo_index::hypo_size(root);

  if (!size) {
    vcl_cout << " there are no hypos in this tile!.. returning!" << vcl_endl;
    return true;
  }
  else
    vcl_cout << " there are " << size << " hyps in this tile!" << vcl_endl;

  unsigned hyp_id;
  volm_geo_index_node_sptr leaf = volm_geo_index::get_closest(root, lat, lon, hyp_id);
  if (!leaf) {
    vcl_cerr << " the geo index: " << geo_index_hyp_folder <<" do not contain any hyp close to " << lat << ", " << lon << '\n';
    return true;
  }
  vcl_cout << "hyp " << lat << ", " << lon << " is in leaf: " << leaf->extent_ << vcl_endl;
  vcl_cout << "\t closest to hyp: " << leaf->hyps_->locs_[hyp_id].y() << ", " << leaf->hyps_->locs_[hyp_id].x() << vcl_endl;


  vcl_stringstream file_name_pre2; file_name_pre2 << visibility_index_folder << "geo_index_tile_" << tile_id;
  vcl_string index_file;
  if (data_type == 0)
    index_file = leaf->get_index_name(file_name_pre2.str());
  else if (data_type == 1)
    index_file = leaf->get_label_index_name(file_name_pre2.str(), "land");
  else if (data_type == 2)
    index_file = leaf->get_label_index_name(file_name_pre2.str(), "orientation");
  else
    index_file = leaf->get_label_index_name(file_name_pre2.str(), "combined");


  // construct spherical shell container, radius is always 1 cause points will be used to compute ray directions
  //double radius = 1;
  //volm_spherical_shell_container_sptr sph_shell = new volm_spherical_shell_container(radius, params.cap_angle, params.point_angle, params.top_angle, params.bottom_angle);

  vsl_b_ifstream ifs(ray_file);
  volm_spherical_shell_container_sptr sph_shell = new volm_spherical_shell_container;
  sph_shell->b_read(ifs);
  ifs.close();


  ////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////// visualize depth interval index //////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////
  vcl_string param_file;
  vcl_string in_dir = visibility_index_folder + "/*.params";
  for (vul_file_iterator fn = in_dir.c_str(); fn; ++fn) {
    param_file = fn();
    break;
  }
  boxm2_volm_wr3db_index_params params;
  if (!params.read_params_file(param_file)) {
    vcl_cerr << "cannot read: " << param_file << '\n';
    return false;
  }
#if 0
  vcl_string param_file;
  if (data_type < 3) {
    param_file = vul_file::strip_extension(index_file) + ".params";
  }
  else {
    vcl_string dir_name = vul_file::dirname(index_file);
    vcl_string in_dir = dir_name + "/*.params";
    for (vul_file_iterator fn = in_dir.c_str(); fn; ++fn) {
      param_file = fn();
      vcl_cout << "reading from params file: " << param_file << '\n';
      break;
    }
  }
  boxm2_volm_wr3db_index_params params;
  if (!params.read_params_file(param_file)) {
    vcl_cerr << "cannot read: " << param_file << '\n';
    return false;
  }
#endif
#if 0
  vcl_string size_file = vul_file::strip_extension(index_file) + ".txt";
  unsigned long eis;
  params.read_size_file(size_file, eis);
  if ((unsigned long)hyp_id >= eis) {
    vcl_cerr << " the hyp id is: " << hyp_id
             << " which is invalid for the index with: " << eis
             << "; indices read from: " << size_file << '\n';
    return false;
  }
#endif // 0

  boxm2_volm_wr3db_index_sptr ind = new boxm2_volm_wr3db_index(params.layer_size, 1);
  if (!ind->initialize_read(index_file)) {
    vcl_cerr << "Cannot initialize index from file: " << index_file << '\n';
    return false;
  }

  vcl_vector<unsigned char> values(params.layer_size);
  for (unsigned j = 0; j < hyp_id; ++j)
    ind->get_next(values);
  ind->get_next(values);  // this one is the hyp_id'th one

  vcl_string prefix = vul_file::strip_extension(index_file);
  vcl_stringstream str;
  str << prefix << "_hyp_" << leaf->hyps_->locs_[hyp_id].y()
      << '_' << leaf->hyps_->locs_[hyp_id].x() << '_' << leaf->hyps_->locs_[hyp_id].z();

  vcl_string txt_name = str.str() + ".txt";
  vcl_ofstream ofs(txt_name.c_str());
  ofs << params.layer_size << '\n';
  for (unsigned ii = 0; ii < params.layer_size; ii++)
    ofs << (int)values[ii] << '\n';
  ofs.close();

  vcl_string temp_name = str.str() + ".vrml";
  sph_shell->draw_template(temp_name, values, (unsigned char)254);

  vil_image_view<vil_rgb<vxl_byte> > img;
  if (data_type == 0)
   sph_shell->panaroma_img(img, values);
  else if (data_type == 1) // visualize label index
   sph_shell->panaroma_img_class_labels(img, values);
  else if (data_type == 2) // visualize orientation label index
   sph_shell->panaroma_img_orientations(img, values);
  else {
   vil_image_view<vil_rgb<vxl_byte> > img_orientation;
   sph_shell->panaroma_images_from_combined(img_orientation, img, values);
   vcl_stringstream str;
   str << prefix << "_orientation_hyp_" << leaf->hyps_->locs_[hyp_id].y()
       << '_' << leaf->hyps_->locs_[hyp_id].x() << '_' << leaf->hyps_->locs_[hyp_id].z();
   vcl_string img_name = str.str() + ".png";
   vcl_cout << "saving image to: " << img_name << vcl_endl;
   vil_save(img_orientation, img_name.c_str());
  }
  vcl_string img_name;
  if (data_type == 0)
    img_name = str.str() + "_depth.png";
  else if (data_type == 1)
    img_name = str.str() + "_land.png";
  else if (data_type == 2)
    img_name = str.str() + "_orientation.png";
  vcl_cout << "saving image to: " << img_name << vcl_endl;
  vil_save(img, img_name.c_str());

  return true;
}

