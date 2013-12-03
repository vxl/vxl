// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_render_expected_depth_region_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for rendering depth map of a scene.
//
// \author Vishal Jain
// \date Mar 10, 2011

#include <vcl_fstream.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <vil/vil_image_view.h>
//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <vul/vul_timer.h>
#include <boxm2/boxm2_util.h>

#include <boxm2/ocl/algo/boxm2_ocl_camera_converter.h>
#include <bvgl/algo/bvgl_2d_geo_index.h>

namespace boxm2_ocl_render_expected_depth_region_process_globals
{
  const unsigned n_inputs_ = 10;
  const unsigned n_outputs_ = 3;
  vcl_size_t local_threads[2]={8,8};
  void compile_kernel(bocl_device_sptr device,vcl_vector<bocl_kernel*> & vec_kernels)
  {
    //gather all render sources... seems like a lot for rendering...
    vcl_vector<vcl_string> src_paths;
    vcl_string source_dir = boxm2_ocl_util::ocl_src_root();
    src_paths.push_back(source_dir + "scene_info.cl");
    src_paths.push_back(source_dir + "pixel_conversion.cl");
    src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
    src_paths.push_back(source_dir + "backproject.cl");
    src_paths.push_back(source_dir + "statistics_library_functions.cl");
    src_paths.push_back(source_dir + "expected_functor.cl");
    src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
    src_paths.push_back(source_dir + "bit/render_bit_scene.cl");
    src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");

    //set kernel options
    vcl_string options = " -D RENDER_DEPTH ";
    options +=  "-D DETERMINISTIC";
    options += " -D STEP_CELL=step_cell_render_depth2(tblock,linfo->block_len,aux_args.alpha,data_ptr,d*linfo->block_len,aux_args.vis,aux_args.expdepth,aux_args.expdepthsqr,aux_args.probsum,aux_args.t)";
    //options += "  -D STEP_CELL=step_cell_render_depth2(tblock,aux_args.alpha,data_ptr,d*linfo->block_len,aux_args.vis,aux_args.expdepth,aux_args.expdepthsqr,aux_args.probsum,aux_args.t)";
    //have kernel construct itself using the context and device
    bocl_kernel * ray_trace_kernel=new bocl_kernel();

    ray_trace_kernel->create_kernel( &device->context(),
                                     device->device_id(),
                                     src_paths,
                                     "render_depth",   //kernel name
                                     options,              //options
                                     "boxm2 opencl render depth image"); //kernel identifier (for error checking)
    vec_kernels.push_back(ray_trace_kernel);

    //create normalize image kernel
    vcl_vector<vcl_string> norm_src_paths;
    norm_src_paths.push_back(source_dir + "scene_info.cl");

    norm_src_paths.push_back(source_dir + "pixel_conversion.cl");
    norm_src_paths.push_back(source_dir + "bit/normalize_kernels.cl");
    bocl_kernel * normalize_render_kernel=new bocl_kernel();

    normalize_render_kernel->create_kernel( &device->context(),
                                            device->device_id(),
                                            norm_src_paths,
                                            "normalize_render_depth_kernel",   //kernel name
                                            options,              //options
                                            "normalize render depth kernel"); //kernel identifier (for error checking)


    vec_kernels.push_back(normalize_render_kernel);
  }
  static vcl_map<vcl_string,vcl_vector<bocl_kernel*> > kernels;
}

bool boxm2_ocl_render_expected_depth_region_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_render_expected_depth_region_process_globals;

  //process takes 1 input
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "boxm2_scene_sptr";
  input_types_[2] = "boxm2_opencl_cache_sptr";
  input_types_[3] = "vpgl_camera_double_sptr";
  input_types_[4] = "double";
  input_types_[5] = "double";
  input_types_[6] = "double";
  input_types_[7] = "double";
  input_types_[8] = "unsigned";
  input_types_[9] = "unsigned";


  // process has 1 output:
  // output[0]: scene sptr
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";
  output_types_[1] = "vil_image_view_base_sptr";
  output_types_[2] = "vil_image_view_base_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_ocl_render_expected_depth_region_process(bprb_func_process& pro)
{
  using namespace boxm2_ocl_render_expected_depth_region_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  float transfer_time=0.0f;
  float gpu_time=0.0f;
  //get the inputs
  unsigned i = 0;
  bocl_device_sptr device= pro.get_input<bocl_device_sptr>(i++);
  boxm2_scene_sptr scene =pro.get_input<boxm2_scene_sptr>(i++);

  boxm2_opencl_cache_sptr opencl_cache= pro.get_input<boxm2_opencl_cache_sptr>(i++);
  vpgl_camera_double_sptr cam= pro.get_input<vpgl_camera_double_sptr>(i++);
  double   lat  = pro.get_input<double>(i++);
  double   lon  = pro.get_input<double>(i++);
  double   elev = pro.get_input<double>(i++);
  double radius = pro.get_input<double>(i++);
  unsigned   ni = pro.get_input<unsigned>(i++);
  unsigned   nj = pro.get_input<unsigned>(i++);

  vcl_string identifier=device->device_identifier();
  
  vcl_cerr << " --------------------- inside process --------------------------------------" << vcl_endl;
  vcl_cerr << " lon = " << lon << ", lat = " << lat << ", elev = " << elev << vcl_endl;
  vcl_cerr << " ni = " << ni << ", nj = " << nj << vcl_endl;
  vcl_cerr << " device = " << identifier << vcl_endl;

  // create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue(device->context(),
                                                *(device->device_id()),
                                                CL_QUEUE_PROFILING_ENABLE,
                                                &status);
  if (status!=0)
    return false;

  // compile the kernel
  if (kernels.find(identifier)==kernels.end())
  {
    //vcl_cout<<"===========Compiling kernels==========="<<vcl_endl;
    vcl_vector<bocl_kernel*> ks;
    compile_kernel(device,ks);
    kernels[identifier]=ks;
  }

#if 0
  // create all buffers
  cl_float cam_buffer[48];
  boxm2_ocl_util::set_persp_camera(cam, cam_buffer);
  bocl_mem_sptr persp_cam=new bocl_mem(device->context(), cam_buffer, 3*sizeof(cl_float16), "persp cam buffer");
  persp_cam->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
#endif

  unsigned cl_ni=RoundUp(ni,local_threads[0]);
  unsigned cl_nj=RoundUp(nj,local_threads[1]);
  float* buff = new float[cl_ni*cl_nj];
  for (unsigned i=0;i<cl_ni*cl_nj;i++) buff[i]=0.0f;
  float* var_buff = new float[cl_ni*cl_nj];
  for (unsigned i=0;i<cl_ni*cl_nj;i++) var_buff[i]=0.0f;
  float* vis_buff = new float[cl_ni*cl_nj];
  for (unsigned i=0;i<cl_ni*cl_nj;i++) vis_buff[i]=1.0f;
  float* prob_buff = new float[cl_ni*cl_nj];
  for (unsigned i=0;i<cl_ni*cl_nj;i++) prob_buff[i]=0.0f;
  float* t_infinity_buff = new float[cl_ni*cl_nj];
  for (unsigned i=0;i<cl_ni*cl_nj;i++) t_infinity_buff[i]=0.0f;

  bocl_mem_sptr exp_image=new bocl_mem(device->context(),buff,cl_ni*cl_nj*sizeof(float),"exp image buffer");
  exp_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  bocl_mem_sptr var_image=new bocl_mem(device->context(),var_buff,cl_ni*cl_nj*sizeof(float),"var image buffer");
  var_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  bocl_mem_sptr vis_image=new bocl_mem(device->context(),vis_buff,cl_ni*cl_nj*sizeof(float),"vis image buffer");
  vis_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  bocl_mem_sptr prob_image=new bocl_mem(device->context(),prob_buff,cl_ni*cl_nj*sizeof(float),"vis x omega image buffer");
  prob_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  bocl_mem_sptr t_infinity=new bocl_mem(device->context(),t_infinity_buff,cl_ni*cl_nj*sizeof(float),"t infinity buffer");
  t_infinity->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  //set generic cam
  cl_float* ray_origins = new cl_float[4*cl_ni*cl_nj];
  cl_float* ray_directions = new cl_float[4*cl_ni*cl_nj];
  bocl_mem_sptr ray_o_buff = new bocl_mem(device->context(), ray_origins, cl_ni*cl_nj * sizeof(cl_float4) , "ray_origins buffer");
  bocl_mem_sptr ray_d_buff = new bocl_mem(device->context(), ray_directions,  cl_ni*cl_nj * sizeof(cl_float4), "ray_directions buffer");
  boxm2_ocl_camera_converter::compute_ray_image( device, queue, cam, cl_ni, cl_nj, ray_o_buff, ray_d_buff);

  // Image Dimensions
  int img_dim_buff[4];
  img_dim_buff[0] = 0;
  img_dim_buff[1] = 0;
  img_dim_buff[2] = ni;
  img_dim_buff[3] = nj;
  bocl_mem_sptr exp_img_dim=new bocl_mem(device->context(), img_dim_buff, sizeof(int)*4, "image dims");
  exp_img_dim->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // Output Array
  float output_arr[100];
  for (int i=0; i<100; ++i) output_arr[i] = 0.0f;
  bocl_mem_sptr  cl_output=new bocl_mem(device->context(), output_arr, sizeof(float)*100, "output buffer");
  cl_output->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // bit lookup buffer
  cl_uchar lookup_arr[256];
  boxm2_ocl_util::set_bit_lookup(lookup_arr);
  bocl_mem_sptr lookup=new bocl_mem(device->context(), lookup_arr, sizeof(cl_uchar)*256, "bit lookup buffer");
  lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  //2. set workgroup size
  vcl_size_t lThreads[] = {8, 8};
  vcl_size_t gThreads[] = {cl_ni,cl_nj};
  float subblk_dim = 0.0;
  // set arguments

  // locate the block where current location is in using bvgl quadtree index
  vgl_box_3d<double> scene_bbox = scene->bounding_box();
  vcl_cout << " scene bounding box = " << scene_bbox << vcl_endl;
  // set the leaf size 4 times larger than the block size
  vgl_box_3d<double> blk_bbox = scene->blocks().begin()->second.bbox();
  float min_size = 4*(blk_bbox.max_x() - blk_bbox.min_x());
  vgl_box_2d<double> scene_bbox_2d(scene_bbox.min_x(), scene_bbox.max_x(), scene_bbox.min_y(), scene_bbox.max_y());
  bvgl_2d_geo_index_node_sptr blk_id_tree_2d = bvgl_2d_geo_index::construct_tree<vcl_vector<boxm2_block_id> >(scene_bbox_2d, min_size);

  // clear the contents
  vcl_vector<bvgl_2d_geo_index_node_sptr> leaves_all;
  bvgl_2d_geo_index::get_leaves(blk_id_tree_2d, leaves_all);
  for (unsigned i = 0; i < leaves_all.size(); i++) {
    bvgl_2d_geo_index_node<vcl_vector<boxm2_block_id> >* leaf_ptr = 
      dynamic_cast<bvgl_2d_geo_index_node<vcl_vector<boxm2_block_id> >* >(leaves_all[i].ptr());
    leaf_ptr->contents_.clear();
  }
  vcl_cout << " 2D geo_index has root bounding box: " << blk_id_tree_2d->extent_
           << " and its leaf has size: " << leaves_all[0]->extent_
           << vcl_endl;
  // fill in the contents
  vcl_map<boxm2_block_id, boxm2_block_metadata> blks = scene->blocks();
  for (vcl_map<boxm2_block_id, boxm2_block_metadata>::iterator mit = blks.begin(); mit != blks.end(); ++mit) {
    boxm2_block_id curr_blk_id = mit->first;
    vgl_box_2d<double> curr_blk_bbox_2d(mit->second.bbox().min_x(), mit->second.bbox().max_x(), mit->second.bbox().min_y(), mit->second.bbox().max_y());
    vcl_vector<bvgl_2d_geo_index_node_sptr> leaves;
    bvgl_2d_geo_index::get_leaves(blk_id_tree_2d, leaves, curr_blk_bbox_2d);
    if (leaves.empty())
      continue;
    for (unsigned l_idx = 0; l_idx < leaves.size(); l_idx++) {
      bvgl_2d_geo_index_node<vcl_vector<boxm2_block_id> >* leaf_ptr = 
        dynamic_cast<bvgl_2d_geo_index_node<vcl_vector<boxm2_block_id> >* >(leaves[l_idx].ptr());
      leaf_ptr->contents_.push_back(curr_blk_id);
    }
  }

  // locate the block for current location
  vpgl_lvcs lvcs = scene->lvcs();
  double lx, ly, lz;
  lvcs.global_to_local(lon, lat, elev, vpgl_lvcs::wgs84, lx, ly, lz);
  vgl_point_3d<double> local_h_pt_d(lx, ly, lz);
  bvgl_2d_geo_index_node_sptr curr_leaf = 0;
  boxm2_block_id curr_block;
  bvgl_2d_geo_index::get_leaf(blk_id_tree_2d, curr_leaf, vgl_point_2d<double>(local_h_pt_d.x(), local_h_pt_d.y()));
  if (curr_leaf) {
    vcl_cout << " leaf " << curr_leaf->extent_ << " contains location: " << lon << " lat: " << lat << " elev: " << elev << " ( " << local_h_pt_d << vcl_endl;
    bvgl_2d_geo_index_node<vcl_vector<boxm2_block_id> >* curr_leaf_ptr =
      dynamic_cast<bvgl_2d_geo_index_node<vcl_vector<boxm2_block_id> >* >(curr_leaf.ptr());
    bool found_blk = false;
    for (vcl_vector<boxm2_block_id>::iterator vit = curr_leaf_ptr->contents_.begin(); vit != curr_leaf_ptr->contents_.end(); ++vit) {
      vgl_box_3d<double> curr_blk_bbox = scene->blocks()[*vit].bbox();
      if (curr_blk_bbox.contains(local_h_pt_d)) {
        curr_block = *vit;
        found_blk = true;
        break;
      }
    }
    if (!found_blk) {
      vcl_cout << pro.name() << ": Scene does not contain location -- lon: " << lon << " lat: " << lat << " elev: " << elev
               << " ( " << local_h_pt_d << "), writing empty array for it!\n";
      vil_image_view<float>* exp_img_out=new vil_image_view<float>(ni,nj);
      vil_image_view<float>* exp_var_out=new vil_image_view<float>(ni,nj);
      vil_image_view<float>* vis_out=new vil_image_view<float>(ni,nj);

      exp_img_out->fill(0.5);
      exp_var_out->fill(0.5);
      vis_out->fill(0.5);
      return false;
    }
  }
  else {
    vcl_cout << pro.name() << ": Scene does not contain location -- lon: " << lon << " lat: " << lat << " elev: " << elev
               << " ( " << local_h_pt_d << "), writing empty array for it!\n";
    vil_image_view<float>* exp_img_out=new vil_image_view<float>(ni,nj);
    vil_image_view<float>* exp_var_out=new vil_image_view<float>(ni,nj);
    vil_image_view<float>* vis_out=new vil_image_view<float>(ni,nj);
    exp_img_out->fill(0.5);
    exp_var_out->fill(0.5);
    vis_out->fill(0.5);
    return false;
  }
#if 0
  //vgl_point_3d<double> local_h_pt_d(cent_x, cent_y, cent_z);
  vgl_point_3d<double> local;
  boxm2_block_id curr_block;

  if (!scene->block_contains(local_h_pt_d, curr_block, local)) {
    if (!scene->contains(local_h_pt_d, curr_block, local)) {
      vcl_cerr << " Scene does not contain location -- lon: " << lon << " lat: " << lat << " elev: " << elev
               << " ( " << local_h_pt_d << "), writing empty array for it!\n";
      vil_image_view<float>* exp_img_out=new vil_image_view<float>(ni,nj);
      vil_image_view<float>* exp_var_out=new vil_image_view<float>(ni,nj);
      vil_image_view<float>* vis_out=new vil_image_view<float>(ni,nj);

      exp_img_out->fill(0.5);
      exp_var_out->fill(0.5);
      vis_out->fill(0.5);
      return false;
    }
  }
#endif

  vcl_cout << " Scene has location -- lon: " << lon << " lat: " << lat << " elev: " << elev << " in box " << curr_block << vcl_endl;
  // get the block that are within radius dmax centered from curr_block
  vcl_vector<boxm2_block_id> vis_order = boxm2_util::order_about_a_block(scene, curr_block, radius);
  unsigned temp_cnt = 0;
  vcl_cout << vis_order.size() << " are loaded for the rendering... " << vcl_endl;
  vcl_vector<boxm2_block_id>::iterator id;
  for (id = vis_order.begin(); id != vis_order.end(); ++id)
  {
    vcl_cout << "cnt = " << temp_cnt++ << " vis_order = " << (*id) << vcl_endl;
    //choose correct render kernel
    boxm2_block_metadata mdata = scene->get_block_metadata(*id);
    bocl_kernel* kern =  kernels[identifier][0];

    //write the image values to the buffer
    vul_timer transfer;
    bocl_mem* blk           = opencl_cache->get_block(*id);
    bocl_mem* alpha         = opencl_cache->get_data<BOXM2_ALPHA>(*id);
    bocl_mem * blk_info     = opencl_cache->loaded_block_info();
    transfer_time          += (float) transfer.all();
    subblk_dim              = mdata.sub_block_dim_.x();
    ////3. SET args
    kern->set_arg( blk_info );
    kern->set_arg( blk );
    kern->set_arg( alpha );
    //kern->set_arg( persp_cam.ptr() );
    kern->set_arg( ray_o_buff.ptr() );
    kern->set_arg( ray_d_buff.ptr() );
    kern->set_arg( exp_image.ptr() );
    kern->set_arg( var_image.ptr() );
    kern->set_arg( exp_img_dim.ptr());
    kern->set_arg( cl_output.ptr() );
    kern->set_arg( lookup.ptr() );
    kern->set_arg( vis_image.ptr() );
    kern->set_arg( prob_image.ptr() );
    kern->set_arg( t_infinity.ptr() );

    //local tree , cumsum buffer, imindex buffer
    kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_uchar16) );
    kern->set_local_arg( local_threads[0]*local_threads[1]*10*sizeof(cl_uchar) );
    kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_int) );

    //execute kernel
    kern->execute(queue, 2, lThreads, gThreads);
    clFinish(queue);
    gpu_time += kern->exec_time();

    cl_output->read_to_buffer(queue);

    // clear render kernel args so it can reset em on next execution
    kern->clear_args();
  }

  bocl_mem_sptr  subblk_dim_mem=new bocl_mem(device->context(), &(subblk_dim), sizeof(float), "sub block dim buffer");
  subblk_dim_mem->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );
  // normalize
  {
    bocl_kernel* normalize_kern= kernels[identifier][1];
    normalize_kern->set_arg( exp_image.ptr() );
    normalize_kern->set_arg( var_image.ptr() );
    normalize_kern->set_arg( vis_image.ptr() );
    normalize_kern->set_arg( exp_img_dim.ptr());
    normalize_kern->set_arg( t_infinity.ptr());
    normalize_kern->set_arg( subblk_dim_mem.ptr());
    normalize_kern->execute( queue, 2, local_threads, gThreads);
    clFinish(queue);
    gpu_time += normalize_kern->exec_time();

    //clear render kernel args so it can reset em on next execution
    normalize_kern->clear_args();
    exp_image->read_to_buffer(queue);
    var_image->read_to_buffer(queue);
    vis_image->read_to_buffer(queue);
  }


  clReleaseCommandQueue(queue);
  i=0;

  vil_image_view<float>* exp_img_out=new vil_image_view<float>(ni,nj);
  vil_image_view<float>* exp_var_out=new vil_image_view<float>(ni,nj);
  vil_image_view<float>* vis_out=new vil_image_view<float>(ni,nj);

  for (unsigned c=0;c<nj;c++)
    for (unsigned r=0;r<ni;r++)
    {
      (*exp_img_out)(r,c)=buff[c*cl_ni+r];
      (*exp_var_out)(r,c)=var_buff[c*cl_ni+r];
      (*vis_out)(r,c)=vis_buff[c*cl_ni+r];
    }

  // store scene smaprt pointer
  pro.set_output_val<vil_image_view_base_sptr>(i++, exp_img_out);
  pro.set_output_val<vil_image_view_base_sptr>(i++, exp_var_out);
  pro.set_output_val<vil_image_view_base_sptr>(i++, vis_out);
  return true;
}
