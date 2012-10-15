// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_render_expected_height_map_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for rendering height map of a scene.
//
// \author Vishal Jain
// \date Mar 30, 2011

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
#include <vul/vul_timer.h>
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>

namespace boxm2_ocl_render_expected_height_map_process_globals
{
  const unsigned n_inputs_  = 3;
  const unsigned n_outputs_ = 6;
  vcl_size_t local_threads[2]={8,8};
  void compile_kernel(bocl_device_sptr device,vcl_vector<bocl_kernel*> & vec_kernels, vcl_string options)
  {
    //gather all render sources... seems like a lot for rendering...
    vcl_vector<vcl_string> src_paths;
    vcl_string source_dir = boxm2_ocl_util::ocl_src_root();
    src_paths.push_back(source_dir + "scene_info.cl");
    src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
    src_paths.push_back(source_dir + "backproject.cl");
    src_paths.push_back(source_dir + "statistics_library_functions.cl");
    src_paths.push_back(source_dir + "expected_functor.cl");
    src_paths.push_back(source_dir + "bit/render_height_map.cl");
    src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");

    //set kernel options
    options += " -D RENDER_HEIGHT_MAP";
    options += " -D DETERMINISTIC";
    options += " -D STEP_CELL=step_cell_render_depth2(tblock,aux_args.alpha,aux_args.mog,data_ptr,d*linfo->block_len,vis,aux_args.expdepth,aux_args.expdepthsqr,aux_args.probsum,aux_args.expint)";
    //step_cell_render(aux_args.mog,aux_args.alpha,data_ptr,d,aux_args.vis,aux_args.expint)";

    //have kernel construct itself using the context and device
    bocl_kernel * ray_trace_kernel=new bocl_kernel();

    ray_trace_kernel->create_kernel( &device->context(),
                                     device->device_id(),
                                     src_paths,
                                     "render_height_map",   //kernel name
                                     options,              //options
                                     "boxm2 opencl render height image"); //kernel identifier (for error checking)
    vec_kernels.push_back(ray_trace_kernel);

    //create normalize image kernel
    vcl_vector<vcl_string> norm_src_paths;
    norm_src_paths.push_back(source_dir + "pixel_conversion.cl");
    norm_src_paths.push_back(source_dir + "bit/normalize_kernels.cl");
    bocl_kernel * normalize_render_kernel=new bocl_kernel();
    vcl_string norm_options = " -D RENDER_DEPTH ";
    normalize_render_kernel->create_kernel( &device->context(),
                                            device->device_id(),
                                            norm_src_paths,
                                            "normalize_render_depth_kernel",   //kernel name
                                            norm_options,              //options
                                            "normalize render depth kernel"); //kernel identifier (for error checking)

    vec_kernels.push_back(normalize_render_kernel);
  }
  static vcl_map<vcl_string,vcl_vector<bocl_kernel*> > kernels;
}

bool boxm2_ocl_render_expected_height_map_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_render_expected_height_map_process_globals;

  //process takes 1 input
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "boxm2_scene_sptr";
  input_types_[2] = "boxm2_opencl_cache_sptr";

  // process has 1 output:
  // output[0]: scene sptr
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";
  output_types_[1] = "vil_image_view_base_sptr";
  output_types_[2] = "vil_image_view_base_sptr";
  output_types_[3] = "vil_image_view_base_sptr";
  output_types_[4] = "vil_image_view_base_sptr";
  output_types_[5] = "vil_image_view_base_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_ocl_render_expected_height_map_process(bprb_func_process& pro)
{
  using namespace boxm2_ocl_render_expected_height_map_process_globals;

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
  vgl_box_3d<double> bbox=scene->bounding_box();

  //find data type for rendering appearance model
  bool foundDataType = false;
  vcl_string mog_type,options;
  vcl_vector<vcl_string> apps = scene->appearances();
  for (unsigned int i=0; i<apps.size(); ++i) {
    if ( apps[i] == boxm2_data_traits<BOXM2_MOG3_GREY>::prefix() ) {
      mog_type = apps[i];
      foundDataType = true;
      options=" -D MOG_TYPE_8";
    }
    else if ( apps[i] == boxm2_data_traits<BOXM2_MOG3_GREY_16>::prefix() ) {
      mog_type = apps[i];
      foundDataType = true;
      options=" -D MOG_TYPE_16";
    }
    else if ( apps[i] == boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix() )  {
      mog_type = apps[i];
      foundDataType = true;
      options=" -D MOG_TYPE_8";
    }
    else if ( apps[i] == boxm2_data_traits<BOXM2_FLOAT8>::prefix() )  {
      mog_type = apps[i]+"_cubic_model";
      foundDataType = true;
      options=" -D FLOAT8";
    }
  }
  if (!foundDataType) {
    vcl_cout<<"BOXM2_OCL_RENDER_PROCESS ERROR: scene doesn't have BOXM2_MOG3_GREY or BOXM2_MOG3_GREY_16 data type"<<vcl_endl;
    return false;
  }

  //get x and y size from scene
  vcl_vector<boxm2_block_id> vis_order = scene->get_block_ids();// (vpgl_perspective_camera<double>*) cam.ptr());
  vcl_vector<boxm2_block_id>::iterator id;
  float xint=0.0f;
  float yint=0.0f;
  for (id = vis_order.begin(); id != vis_order.end(); ++id)
  {
    boxm2_block_metadata mdata=scene->get_block_metadata(*id);
    float num_octree_cells=vcl_pow(2.0f,(float)mdata.max_level_-1);
    xint=mdata.sub_block_dim_.x()/num_octree_cells;
    yint=mdata.sub_block_dim_.y()/num_octree_cells;
  }
  unsigned int ni=(unsigned int)vcl_ceil(bbox.width()/xint);
  unsigned int nj=(unsigned int)vcl_ceil(bbox.height()/yint);
  vcl_cout<<"Size of the image "<<ni<<','<<nj<<vcl_endl;
  float z= bbox.max_z();
  vcl_string identifier=device->device_identifier();

  //: create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue(device->context(),*(device->device_id()),
                                                CL_QUEUE_PROFILING_ENABLE,&status);
  if (status!=0)return false;

  // compile the kernel
  if (kernels.find(identifier)==kernels.end())
  {
    vcl_cout<<"===========Compiling kernels==========="<<vcl_endl;
    vcl_vector<bocl_kernel*> ks;
    compile_kernel(device,ks,options);
    kernels[identifier]=ks;
  }
  bocl_mem_sptr z_mem=new bocl_mem(device->context(),&z,sizeof(float),"constant ray_oz");
  z_mem->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
  bocl_mem_sptr xint_mem=new bocl_mem(device->context(),&xint,sizeof(float),"x interval");
  xint_mem->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
  bocl_mem_sptr yint_mem=new bocl_mem(device->context(),&yint,sizeof(float),"y interval");
  yint_mem->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  float scene_origin[4];
  scene_origin[0]=bbox.min_x();
  scene_origin[1]=bbox.min_y();
  scene_origin[2]=bbox.min_z();
  scene_origin[3]=1.0;

  bocl_mem_sptr scene_origin_mem=new bocl_mem(device->context(), scene_origin, sizeof(float)*4, "scene origin");
  scene_origin_mem->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

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
  float* app_buff = new float[cl_ni*cl_nj];
  for (unsigned i=0;i<cl_ni*cl_nj;i++) app_buff[i]=0.0f;

  //new bocl_mem(device->context(),buff,cl_ni*cl_nj*sizeof(float),"height (z) buffer ");
  bocl_mem_sptr exp_image=opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float),buff,"height (z) buffer ");
  exp_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  //new bocl_mem(device->context(),var_buff,cl_ni*cl_nj*sizeof(float),"var image buffer");
  bocl_mem_sptr var_image=opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float),var_buff, "var image buffer");
  var_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  //new bocl_mem(device->context(),vis_buff,cl_ni*cl_nj*sizeof(float),"vis image buffer");
  bocl_mem_sptr vis_image=opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float),vis_buff, "vis image buffer");
  vis_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  //new bocl_mem(device->context(),prob_buff,cl_ni*cl_nj*sizeof(float),"vis x omega image buffer");
  bocl_mem_sptr prob_image=opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float),prob_buff, "vis x omega image buffer");
  prob_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  //new bocl_mem(device->context(),app_buff,cl_ni*cl_nj*sizeof(float),"exp image buffer");
  bocl_mem_sptr app_image=opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float),app_buff, "app image buffer");
  app_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // Image Dimensions
  unsigned int img_dim_buff[4] = {0,0,ni,nj};
  bocl_mem_sptr exp_img_dim=new bocl_mem(device->context(), img_dim_buff, sizeof(unsigned int)*4, "image dims");
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

  // set arguments
  for (id = vis_order.begin(); id != vis_order.end(); ++id)
  {
    //choose correct render kernel
    boxm2_block_metadata mdata = scene->get_block_metadata(*id);
    bocl_kernel* kern =  kernels[identifier][0];

    //write the image values to the buffer
    vul_timer transfer;
    bocl_mem* blk           = opencl_cache->get_block(*id);
    bocl_mem* alpha         = opencl_cache->get_data<BOXM2_ALPHA>(*id);
    // bocl_mem* mog        = opencl_cache->get_data(*id, mog_type);
    bocl_mem* mog           = opencl_cache->get_data(*id,mog_type,alpha->num_bytes()*8,true);
    bocl_mem * blk_info     = opencl_cache->loaded_block_info();
    transfer_time          += (float) transfer.all();

    ////3. SET args
    kern->set_arg( blk_info );
    kern->set_arg( z_mem.ptr());
    kern->set_arg( xint_mem.ptr());
    kern->set_arg( yint_mem.ptr());
    kern->set_arg( scene_origin_mem.ptr());
    kern->set_arg( blk );
    kern->set_arg( alpha );
    kern->set_arg( mog );
    kern->set_arg( app_image.ptr() );
    kern->set_arg( exp_image.ptr() );
    kern->set_arg( var_image.ptr() );
    kern->set_arg( exp_img_dim.ptr());
    kern->set_arg( cl_output.ptr() );
    kern->set_arg( lookup.ptr() );
    kern->set_arg( vis_image.ptr() );
    kern->set_arg( prob_image.ptr() );

    //local tree , cumsum buffer, imindex buffer
    kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_uchar16) );
    kern->set_local_arg( local_threads[0]*local_threads[1]*10*sizeof(cl_uchar) );
    kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_int) );

    //execute kernel
    kern->execute(queue, 2, lThreads, gThreads);
    clFinish(queue);
    gpu_time += kern->exec_time();

    cl_output->read_to_buffer(queue);

    //clear render kernel args so it can reset em on next execution
    kern->clear_args();
  }
  // normalize
  {
#if 0
    bocl_kernel* normalize_kern= kernels[identifier][1];
    normalize_kern->set_arg( exp_image.ptr() );
    normalize_kern->set_arg( var_image.ptr() );
    normalize_kern->set_arg( prob_image.ptr() );
    normalize_kern->set_arg( exp_img_dim.ptr());
    normalize_kern->execute( queue, 2, local_threads, gThreads);
    clFinish(queue);
    gpu_time += normalize_kern->exec_time();

    //clear render kernel args so it can reset em on next execution
    normalize_kern->clear_args();
#endif
    exp_image->read_to_buffer(queue);
    var_image->read_to_buffer(queue);
    vis_image->read_to_buffer(queue);
    prob_image->read_to_buffer(queue);
    app_image->read_to_buffer(queue);
  }

  clReleaseCommandQueue(queue);
  i=0;
  vil_image_view<float>* exp_img_out=new vil_image_view<float>(ni,nj);
  vil_image_view<float>* exp_var_out=new vil_image_view<float>(ni,nj);
  vil_image_view<float>* xcoord_img=new vil_image_view<float>(ni,nj);
  vil_image_view<float>* ycoord_img=new vil_image_view<float>(ni,nj);
  vil_image_view<float>* prob_img=new vil_image_view<float>(ni,nj);
  vil_image_view<float>* app_img=new vil_image_view<float>(ni,nj);

  for (unsigned c=0;c<nj;++c)
    for (unsigned r=0;r<ni;++r)
    {
      (*exp_img_out)(r,c) = z-buff[c*cl_ni+r];
      (*exp_var_out)(r,c) = var_buff[c*cl_ni+r];
      (*xcoord_img)(r,c)  = r*xint+scene_origin[0];
      (*ycoord_img)(r,c)  = c*yint+scene_origin[1];
      (*prob_img)(r,c)    = prob_buff[c*cl_ni+r];
      (*app_img)(r,c)     = app_buff[c*cl_ni+r];
    }
  // store scene smaprt pointer
  pro.set_output_val<vil_image_view_base_sptr>(i++, exp_img_out);
  pro.set_output_val<vil_image_view_base_sptr>(i++, exp_var_out);
  pro.set_output_val<vil_image_view_base_sptr>(i++, xcoord_img);
  pro.set_output_val<vil_image_view_base_sptr>(i++, ycoord_img);
  pro.set_output_val<vil_image_view_base_sptr>(i++, prob_img);
  pro.set_output_val<vil_image_view_base_sptr>(i++, app_img);
  delete[] buff;
  delete[] var_buff;
  delete[] vis_buff;
  delete[] prob_buff;
  delete[] app_buff;
  opencl_cache->unref_mem(exp_image.ptr());
  opencl_cache->unref_mem(var_image.ptr());
  opencl_cache->unref_mem(vis_image.ptr());
  opencl_cache->unref_mem(prob_image.ptr());
  opencl_cache->unref_mem(app_image.ptr());
  return true;
}
