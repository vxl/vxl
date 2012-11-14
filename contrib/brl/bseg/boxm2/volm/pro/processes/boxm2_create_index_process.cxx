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
//#include <boxm2/volm/boxm2_volm_locations.h>
//#include <bbas/volm/volm_spherical_container.h>
#include <bbas/volm/volm_spherical_shell_container.h>
#include <bbas/volm/volm_spherical_shell_container_sptr.h>
#include <bbas/volm/volm_loc_hyp.h>
#include <vul/vul_timer.h>
#include <vul/vul_file.h>
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
  const unsigned n_inputs_ = 17;
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
    vcl_string options = " -D COMPINDEX ";
    options += " -D STEP_CELL=step_cell_compute_index(tblock,aux_args.alpha,data_ptr,d*linfo->block_len,aux_args.vis,aux_args.expdepth,aux_args.expdepthsqr,aux_args.probsum,aux_args.t)";

    bocl_kernel* compute_index = new bocl_kernel();

    compute_index->create_kernel(&device->context(),device->device_id(), src_paths, "compute_loc_index", options, "compute_loc_index");
    vec_kernels.push_back(compute_index);

    //create normalize image kernel
    bocl_kernel * norm_kernel=new bocl_kernel();
    if (!norm_kernel) {
      vcl_cout << " cannot allocate kernel object!\n" << vcl_endl; vcl_cout.flush(); }
    else
      vcl_cout << " got a new kernel object!\n";

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
  input_types_[10] = "float"; // cap angle to create the spherical shell container  -- 180 for full sphere, 90 for half sphere
  input_types_[11] = "float"; // point angle to create the spherical shell container
  input_types_[12] = "float"; // top angle to remove from shp shell
  input_types_[13] = "float"; // bottom angle to remove from sph sheel
  input_types_[14] = "vcl_string"; // name of output file to save the index
  input_types_[15] = "float"; // visibility threshold to declare a ray a sky ray, it's strictly very small if occupied, so could be as small as 0.3f
  input_types_[16] = "float"; // buffer capacity on CPU RAM for the indices to be cached before being written to disc in chunks

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
           << " max work item sizes in each dimensions: " << device->info().max_work_item_sizes_ << '\n';
  boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(i++);
  vpgl_lvcs lvcs = scene->lvcs();
  boxm2_opencl_cache_sptr  opencl_cache = pro.get_input<boxm2_opencl_cache_sptr>(i++);
  //boxm2_volm_loc_hypotheses_sptr hyp = pro.get_input<boxm2_volm_loc_hypotheses_sptr>(i++);
  vcl_string hyp_file = pro.get_input<vcl_string>(i++);
  unsigned start = pro.get_input<unsigned>(i++);
  unsigned skip = pro.get_input<unsigned>(i++);
  float elev_dif = pro.get_input<float>(i++);
  float vmin = pro.get_input<float>(i++);
  float dmax = pro.get_input<float>(i++);
  float solid_angle = pro.get_input<float>(i++);
  float cap_angle = pro.get_input<float>(i++);
  float point_angle = pro.get_input<float>(i++);
  float top_angle = pro.get_input<float>(i++);
  float bottom_angle = pro.get_input<float>(i++);
  vcl_string index_file = pro.get_input<vcl_string>(i++);
  float vis_thres = pro.get_input<float>(i++);
  float buffer_capacity = pro.get_input<float>(i++);

  volm_spherical_container_sptr sph2 = new volm_spherical_container(solid_angle,vmin,dmax);
  vcl_cout << "number of voxels in container: " << sph2->get_voxels().size() << vcl_endl;

  //: read the location hypotheses
  if (!vul_file::exists(hyp_file)) {
    vcl_cerr << "Cannot find: " << hyp_file << "!\n";
    return false;
  }

  volm_loc_hyp hyp(hyp_file);
  vcl_cout << hyp.size() << " hypotheses read from: " << hyp_file << vcl_endl;

  // construct spherical shell container, radius is always 1 cause points will be used to compute ray directions
  double radius = 1;
  volm_spherical_shell_container_sptr sph_shell = new volm_spherical_shell_container(radius, cap_angle, point_angle, top_angle, bottom_angle);
  int layer_size = (int)(sph_shell->get_container_size());
  boxm2_volm_wr3db_index_sptr ind = new boxm2_volm_wr3db_index(layer_size, buffer_capacity);
  ind->initialize_write(index_file);

  global_threads[0] = RoundUp(layer_size, (int)local_threads[0]);
  vcl_cout << "layer_size: " << layer_size << ", # of global threads: " << global_threads[0] << '\n';
  unsigned int thread_cnt = (unsigned)global_threads[0];

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
  vcl_vector<vgl_point_3d<double> >& cart_points = sph_shell->cart_points();
  int cnt = 0;
  for (int i = 0; i < layer_size; i++) {
    ray_dirs[4*i  ] = (cl_float)cart_points[i].x();
    ray_dirs[4*i+1] = (cl_float)cart_points[i].y();
    ray_dirs[4*i+2] = (cl_float)cart_points[i].z();
    ray_dirs[4*i+3] = 0.0f;
  }

  bocl_mem* ray_dir_buffer = new bocl_mem(device->context(), ray_dirs, sizeof(cl_float4)*layer_size, "ray directions buffer");
  ray_dir_buffer->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  bocl_mem* ray_dim_mem = new bocl_mem(device->context(), &(layer_size), sizeof(int), "ray directions size");
  ray_dim_mem->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

  vcl_map<boxm2_block_id, boxm2_block_metadata>& blocks = scene->blocks();
  vcl_cout << "number of blocks: " << blocks.size() << vcl_endl;
  vcl_cout.flush();
  //: get subblk dimension
  boxm2_block_metadata mdata = scene->get_block_metadata(blocks.begin()->first);
  float subblk_dim = (float)mdata.sub_block_dim_.x();
  bocl_mem*  subblk_dim_mem=new bocl_mem(device->context(), &(subblk_dim), sizeof(float), "sub block dim buffer");
  subblk_dim_mem->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

  vcl_map<boxm2_block_id, vcl_vector<boxm2_block_id> > order_cache;

  boxm2_block_id curr_block;

  //zip through each location hypothesis
  vgl_point_3d<float> h_pt;
  unsigned indexed_cnt = 0;
  while (hyp.get_next(start, skip, h_pt))
  {
    //vcl_cout << "Processing hypothesis: " << hi << " x: " << hyp->locs_[hi].x() << " y: " << hyp->locs_[hi].y() << " z: " << hyp->locs_[hi].z() << vcl_endl;
    vcl_cout << "Processing hypothesis: " << hyp.current_-skip << " x: " << h_pt.x() << " y: " << h_pt.y() << " z: " << h_pt.z() << vcl_endl;
    double lx, ly, lz;
    lvcs.global_to_local(h_pt.x(), h_pt.y(), h_pt.z(), vpgl_lvcs::wgs84, lx, ly, lz);
    vcl_cout << "   in local coords: " << lx << ' ' << ly << ' ' << lz << vcl_endl;
    lz = lz - elev_dif;
    vcl_cout << "   after subtracting elev dif (" << elev_dif << "): " << lx << ' ' << ly << ' ' << lz << vcl_endl;
    vgl_point_3d<double> local_h_pt_d(lx, ly, lz);

    cl_float loc_arr[4];
    loc_arr[0] = (cl_float)lx; loc_arr[1] = (cl_float)ly; loc_arr[2] = (cl_float)lz;
    loc_arr[3] = 1.0f;
    bocl_mem* hypo_location = new bocl_mem(device->context(), loc_arr, sizeof(cl_float4), "location buffer");
    hypo_location->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

    // Output Arrays
    float* buff = new float[layer_size];
    for (int i=0;i<layer_size;i++) buff[i]=0.0f;
    float* vis_buff = new float[layer_size];
    for (int i=0;i<layer_size;i++) vis_buff[i]=1.0f;
    float* prob_buff = new float[layer_size];
    for (int i=0;i<layer_size;i++) prob_buff[i]=0.0f;
    float* t_infinity_buff = new float[layer_size];
    for (int i=0;i<layer_size;i++) t_infinity_buff[i]=0.0f;

    bocl_mem* exp_depth=new bocl_mem(device->context(),buff,layer_size*sizeof(float),"exp depth buffer");
    exp_depth->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

    bocl_mem* vis=new bocl_mem(device->context(),vis_buff,layer_size*sizeof(float),"visibility buffer");
    vis->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

    bocl_mem* probs=new bocl_mem(device->context(),prob_buff,layer_size*sizeof(float),"vis x omega buffer");
    probs->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

    bocl_mem* t_infinity=new bocl_mem(device->context(),t_infinity_buff,layer_size*sizeof(float),"t infinity buffer");
    t_infinity->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

#if 0
    //zip through each block
    vcl_map<boxm2_block_id, boxm2_block_metadata>::iterator blk_iter_inner;
    for (blk_iter_inner = blocks.begin(); blk_iter_inner != blocks.end(); ++blk_iter_inner) { ... }
#endif
    vgl_point_3d<double> local;
    vul_timer t;
    t.mark();
    if (!scene->block_contains(local_h_pt_d, curr_block, local))
    {
      if (!scene->contains(local_h_pt_d, curr_block, local)) {
        vcl_cerr << " Scene does not contain hypothesis: " << hyp.current_-skip << ' ' << local_h_pt_d << " writing empty array for it!\n";
        vcl_vector<unsigned char> values(layer_size, 0);
        ind->add_to_index(values);
        indexed_cnt++;
        // release the device and host memories
        delete exp_depth;  // calls release_memory() which enqueues a mem delete event, call clFinish to make sure it is executed
        delete vis;
        delete probs;
        delete t_infinity;
        delete hypo_location;
        status = clFinish(queue);
        check_val(status, MEM_FAILURE, "release memory FAILED: " + error_to_string(status));
        if (!buff)
          vcl_cout << "buff is zero after release mem!\n"; vcl_cout.flush();
        delete [] buff;
        delete [] vis_buff;
        delete [] prob_buff;
        delete [] t_infinity_buff;
        continue;
      }
    }
    vcl_cout << "Total time taken = " << t.user()/1000.0 << " secs.\n";

    t.mark();
    vcl_map<boxm2_block_id, vcl_vector<boxm2_block_id> >::iterator ord_iter = order_cache.find(curr_block);
    if (!(ord_iter != order_cache.end())) {
      order_cache[curr_block] =  boxm2_util::order_about_a_block(scene, curr_block, dmax);
      if (order_cache.size() > 100) {// kick the first one
        vcl_map<boxm2_block_id, vcl_vector<boxm2_block_id> >::iterator to_kick = order_cache.begin();
        if (to_kick->first != curr_block)
          order_cache.erase(to_kick);
        else { to_kick++; order_cache.erase(to_kick); }
      }
    }
    vcl_vector<boxm2_block_id>& vis_blocks = order_cache[curr_block];

    vcl_cout << "Total time taken = " << t.user()/1000.0 << " secs.\n"
             << "number of visible blocks: " << vis_blocks.size() << vcl_endl;
    vcl_vector<boxm2_block_id>::iterator blk_iter_inner;

    for (blk_iter_inner = vis_blocks.begin(); blk_iter_inner != vis_blocks.end(); ++blk_iter_inner) {
      boxm2_block_id id_inner = *blk_iter_inner;
      //boxm2_block_id id_inner = blk_iter_inner->first;

      //load tree and alpha
      bocl_kernel* kern =  kernels[identifier][0];

      vul_timer transfer;

      bocl_mem* blk       = opencl_cache->get_block(id_inner);
      bocl_mem* blk_info  = opencl_cache->loaded_block_info();
      //bocl_mem* alpha     = opencl_cache->get_data<BOXM2_ALPHA>(blk_iter_inner->first,0,false);
      bocl_mem* alpha = opencl_cache->get_data<BOXM2_ALPHA>(id_inner);

      transfer_time += (float) transfer.all();

      kern->set_arg( blk_info );
      kern->set_arg( ray_dir_buffer);
      kern->set_arg( ray_dim_mem);
      kern->set_arg( blk );
      kern->set_arg( lookup  );
      kern->set_arg( alpha  );
      kern->set_arg( hypo_location );
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
        vcl_cerr << "status: " << status << "  INDEX EXECUTE FAILED: " + error_to_string(status) << vcl_endl;
        return false;
      }
      gpu_time += kern->exec_time();

      //clear render kernel args so it can reset em on next execution
      kern->clear_args();

      //remove from device memory unnecessary items
      opencl_cache->shallow_remove_data(id_inner,boxm2_data_traits<BOXM2_ALPHA>::prefix());
      opencl_cache->shallow_remove_block(id_inner); // also remove blk_info
      //opencl_cache->clear_cache();

      status = clFinish(queue);
      check_val(status, MEM_FAILURE, "opencl clear cache FAILED: " + error_to_string(status));
    }
    if (vis_blocks.size() != 0)  // normalize
    {
      bocl_kernel* normalize_kern= kernels[identifier][1];
      normalize_kern->set_arg( exp_depth );
      normalize_kern->set_arg( probs );
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
    for (unsigned i = 0; i < layer_size; i++) {
      vcl_cout << buff[i] << " (" << vis_buff[i] << ") ";
    }
    vcl_cout << vcl_endl;
#endif
    // find each depth interval using spherical container
    vcl_vector<unsigned char> values;
    for (int i = 0; i < layer_size; i++) {
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
    sph_shell->draw_template("./test.vrml", values, 254);
#endif
    // add to index
    ind->add_to_index(values);
    indexed_cnt++;

    // release the device and host memories
    delete exp_depth;  // calls release_memory() which enqueues a mem delete event, call clFinish to make sure it is executed
    delete vis;
    delete probs;
    delete t_infinity;
    delete hypo_location;

    status = clFinish(queue);
    check_val(status, MEM_FAILURE, "release memory FAILED: " + error_to_string(status));

    if (!buff)
      vcl_cout << "buff is zero after release mem!\n"; vcl_cout.flush();
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

  vcl_cout<<"Gpu time "<<gpu_time<<" transfer time "<<transfer_time<< " micro seconds." << vcl_endl;
  clReleaseCommandQueue(queue);

  //cache size sanity check
  binCache = (long)(opencl_cache.ptr()->bytes_in_cache());
  vcl_cout<<"At the end of process MBs in cache: "<<binCache/(1024.0*1024.0)<<vcl_endl;

  ind->finalize();
  vcl_string index_size_file = vul_file::strip_extension(index_file) + ".txt";
  vcl_ofstream ofs(index_size_file.c_str());
  ofs << indexed_cnt << '\n';
  ofs.close();

  return true;
}


namespace boxm2_partition_hypotheses_process_globals
{
  const unsigned n_inputs_ = 4;
  const unsigned n_outputs_ = 0;
}

bool boxm2_partition_hypotheses_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_partition_hypotheses_process_globals;

  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string"; // scene xml file
  input_types_[1] = "vcl_string"; // binary hypotheses file with lat, lon, elev positions to generate indices for
  input_types_[2] = "float"; // elevation difference to adjust local heights, some scenes need a height adjustment according to their resolution
  input_types_[3] = "vcl_string"; // postfix of name of output file to save the hyp file, ".bin" will be added to save hyps and ".txt" will be added to save size
  vcl_vector<vcl_string>  output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_partition_hypotheses_process(bprb_func_process& pro)
{
  using namespace boxm2_partition_hypotheses_process_globals;

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
  vcl_string scene_file = pro.get_input<vcl_string>(i++);
  boxm2_scene_sptr scene = new boxm2_scene(scene_file);
  vpgl_lvcs lvcs = scene->lvcs();
  vcl_string hyp_file = pro.get_input<vcl_string>(i++);
  float elev_dif = pro.get_input<float>(i++);
  vcl_string out_file = pro.get_input<vcl_string>(i++);

  //: read the location hypotheses
  if (!vul_file::exists(hyp_file)) {
    vcl_cerr << "Cannot find: " << hyp_file << "!\n";
    return false;
  }

  volm_loc_hyp hyp(hyp_file);
  vcl_cout << hyp.size() << " hypotheses read from: " << hyp_file << vcl_endl;
  vcl_cout.flush();
  volm_loc_hyp hyp2;  // empty one
  boxm2_block_id curr_block;

  for (unsigned hi = 0; hi < hyp.size(); hi++)
  {
    vgl_point_3d<float> h_pt;
    if (!hyp.get_next(h_pt)) {
      vcl_cerr << "!!Problem retrieving hyp: " << hi << " from file: " << hyp_file << vcl_endl;
      return false;
    }
    double lx, ly, lz;
    lvcs.global_to_local(h_pt.x(), h_pt.y(), h_pt.z(), vpgl_lvcs::wgs84, lx, ly, lz);
    lz = lz - elev_dif;
    vgl_point_3d<double> local_h_pt_d(lx, ly, lz);

    vgl_point_3d<double> local;
    if (!scene->contains(local_h_pt_d, curr_block, local))
      continue;

    // bool add(float lat, float lon, float elev);  // longitude is x (east) / latitude is y (north) // elev is z
    hyp2.add(h_pt.y(), h_pt.x(), h_pt.z());
  }
  hyp2.write_hypotheses(out_file+".bin");
  vcl_string out_txt(out_file+".txt");
  vcl_ofstream ofs(out_txt.c_str());
  ofs << hyp2.size() << vcl_endl;
  vcl_cout << hyp2.size() << " hyps written to " << out_file+".bin" << vcl_endl;
  ofs.close();

  return true;
}


namespace boxm2_hypotheses_kml_process_globals
{
  const unsigned n_inputs_ = 5;
  const unsigned n_outputs_ = 0;
}

bool boxm2_hypotheses_kml_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_hypotheses_kml_process_globals;

  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string"; // binary hypotheses file with lat, lon, elev positions to generate indices for
  input_types_[1] = "unsigned"; // start
  input_types_[2] = "unsigned"; // skip
  input_types_[3] = "float"; // size of boxes to draw in arcseconds, e.g. 0.01
  input_types_[4] = "vcl_string"; // output kml file
  vcl_vector<vcl_string>  output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_hypotheses_kml_process(bprb_func_process& pro)
{
  using namespace boxm2_hypotheses_kml_process_globals;

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
  vcl_string hyp_file = pro.get_input<vcl_string>(i++);
  unsigned start = pro.get_input<unsigned>(i++);
  unsigned skip = pro.get_input<unsigned>(i++);
  float b_size = pro.get_input<float>(i++);
  vcl_string out_file = pro.get_input<vcl_string>(i++);

  //: read the location hypotheses
  if (!vul_file::exists(hyp_file)) {
    vcl_cerr << "Cannot find: " << hyp_file << "!\n";
    return false;
  }
  vcl_ofstream ofs(out_file.c_str());
  bkml_write::open_document(ofs);

  volm_loc_hyp hyp(hyp_file);
  vcl_cout << hyp.size() << " hypotheses read from: " << hyp_file << vcl_endl;

  vgl_point_3d<float> h_pt;
  unsigned cnt = 0;
  while (hyp.get_next(start, skip, h_pt))
  {
    vcl_cout.precision(6);
    vcl_cout << h_pt.y() << ' ' << h_pt.x() << vcl_endl;
    vnl_double_2 ll; ll[0] = h_pt.y(); ll[1] = h_pt.x();  // longitude is x (east) / latitude is y (north) // elev is z
    vnl_double_2 lr; lr[0] = h_pt.y(); lr[1] = h_pt.x()+b_size;
    vnl_double_2 ur; ur[0] = h_pt.y()+b_size; ur[1] = h_pt.x()+b_size;
    vnl_double_2 ul; ul[0] = h_pt.y()+b_size; ul[1] = h_pt.x();

    vcl_stringstream box_id; box_id << hyp.current_-1;
    vcl_string desc = "desc";
    bkml_write::write_box(ofs, box_id.str(), desc, ul, ur, ll, lr);
    cnt++;
    if (cnt > 10)
      break;
  }
  vcl_cout << cnt << " boxes are written to the output " << out_file << vcl_endl;
  bkml_write::close_document(ofs);
  ofs.close();

  return true;
}


namespace boxm2_visualize_index_process_globals
{
  const unsigned n_inputs_ = 9;
  const unsigned n_outputs_ = 0;
}

bool boxm2_visualize_index_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_visualize_index_process_globals;

  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string"; // index file
  input_types_[1] = "float"; // cap angle to construct index
  input_types_[2] = "float"; // point angle
  input_types_[3] = "float"; // top angle
  input_types_[4] = "float"; // bottom angle
  input_types_[5] = "float"; // buffer capacity for the index
  input_types_[6] = "unsigned";  // start id of the indices to visualize
  input_types_[7] = "unsigned";  // end id of the indices to visualize
  input_types_[8] = "vcl_string";  // prefix for the output files

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
  float cap_angle = pro.get_input<float>(i++);
  float point_angle = pro.get_input<float>(i++);
  float top_angle = pro.get_input<float>(i++);
  float bottom_angle = pro.get_input<float>(i++);
  float buffer_capacity = pro.get_input<float>(i++);
  unsigned si = pro.get_input<unsigned>(i++);
  unsigned ei = pro.get_input<unsigned>(i++);
  vcl_string prefix = pro.get_input<vcl_string>(i++);

  // construct spherical shell container, radius is always 1 cause points will be used to compute ray directions
  double radius = 1;
  volm_spherical_shell_container_sptr sph_shell = new volm_spherical_shell_container(radius, cap_angle, point_angle, top_angle, bottom_angle);
  int layer_size = (int)(sph_shell->get_container_size());
  boxm2_volm_wr3db_index_sptr ind = new boxm2_volm_wr3db_index(layer_size, buffer_capacity);
  ind->initialize_read(index_file);

  vcl_vector<unsigned char> values(layer_size);
  for (unsigned j = 0; j < si; j++)
    ind->get_next(values);
  for (unsigned j = si; j < ei; j++) {
    vcl_stringstream str; str << prefix << '_' << j;
    vcl_string temp_name = str.str() + ".vrml";
    ind->get_next(values);
    sph_shell->draw_template(temp_name, values, (unsigned char)254);
    vil_image_view<vxl_byte> img;
    sph_shell->panaroma_img(img, values);
    vcl_string img_name = str.str() + ".png";
    vil_save(img, img_name.c_str());
  }
  return true;
}

