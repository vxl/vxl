// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_render_expected_image_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for rendering the scene.
//
// \author Vishal Jain
// \date Mar 10, 2011

#include <vcl_fstream.h>
#include <vcl_algorithm.h>
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
#include <boxm2/ocl/algo/boxm2_ocl_render_expected_image_function.h>
#include <vul/vul_timer.h>


namespace boxm2_ocl_render_expected_image_process_globals
{
  const unsigned n_inputs_ = 9;
  const unsigned n_outputs_ = 2;
  vcl_size_t lthreads[2]={8,8};

  static vcl_map<vcl_string,vcl_vector<bocl_kernel*> > kernels;
  void compile_kernel(bocl_device_sptr device,vcl_vector<bocl_kernel*> & vec_kernels, vcl_string opts)
  {
    //gather all render sources... seems like a lot for rendering...
    vcl_vector<vcl_string> src_paths;
    vcl_string source_dir = boxm2_ocl_util::ocl_src_root();
    src_paths.push_back(source_dir + "scene_info.cl");
    src_paths.push_back(source_dir + "pixel_conversion.cl");
    src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
    src_paths.push_back(source_dir + "backproject.cl");
    src_paths.push_back(source_dir + "statistics_library_functions.cl");
    src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
    src_paths.push_back(source_dir + "bit/render_bit_scene.cl");
    src_paths.push_back(source_dir + "expected_functor.cl");
    src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");


    vcl_size_t found = opts.find("SHORT");
    if (found!=vcl_string::npos)
    {
      vcl_cout<<"COMPILING SHORT"<<vcl_endl;
      vcl_string options = opts;
      options += "-D RENDER ";
      options += "-D RENDER_MAX -D STEP_CELL=step_cell_render_max(aux_args.mog,aux_args.alpha,data_ptr,d*linfo->block_len,vis,aux_args.expint,aux_args.maxomega)";


      //have kernel construct itself using the context and device
      bocl_kernel * ray_trace_kernel=new bocl_kernel();
      ray_trace_kernel->create_kernel( &device->context(),
                                       device->device_id(),
                                       src_paths,
                                       "render_bit_scene",   //kernel name
                                       options,              //options
                                       "boxm2 opencl render_bit_scene"); //kernel identifier (for error checking)
      vec_kernels.push_back(ray_trace_kernel);
    }
    else
    {
      vcl_string options = opts;
      options += "-D RENDER ";

      options += "-D STEP_CELL=step_cell_render(aux_args.mog,aux_args.alpha,data_ptr,d*linfo->block_len,vis,aux_args.expint)";

      //have kernel construct itself using the context and device
      bocl_kernel * ray_trace_kernel=new bocl_kernel();
      ray_trace_kernel->create_kernel( &device->context(),
                                       device->device_id(),
                                       src_paths,
                                       "render_bit_scene",   //kernel name
                                       options,              //options
                                       "boxm2 opencl render_bit_scene"); //kernel identifier (for error checking)
      vec_kernels.push_back(ray_trace_kernel);

      //create normalize image kernel
      vcl_vector<vcl_string> norm_src_paths;
      norm_src_paths.push_back(source_dir + "pixel_conversion.cl");
      norm_src_paths.push_back(source_dir + "bit/normalize_kernels.cl");
      bocl_kernel * normalize_render_kernel=new bocl_kernel();

      normalize_render_kernel->create_kernel( &device->context(),
                                              device->device_id(),
                                              norm_src_paths,
                                              "normalize_render_kernel",   //kernel name
                                              options,              //options
                                              "normalize render kernel"); //kernel identifier (for error checking)

      vec_kernels.push_back(normalize_render_kernel);
    }
  }
}

bool boxm2_ocl_render_expected_image_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_render_expected_image_process_globals;

  //process takes 1 input
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "boxm2_scene_sptr";
  input_types_[2] = "boxm2_opencl_cache_sptr";
  input_types_[3] = "vpgl_camera_double_sptr";
  input_types_[4] = "unsigned";
  input_types_[5] = "unsigned";
  input_types_[6] = "vcl_string";
  input_types_[7] = "float";                        // near factor ( maximum # of pixels should map to the finest voxel )
  input_types_[8] = "float";                        // far factor ( minimum # of pixels should map to the finest voxel )

  // process has 1 output:
  // output[0]: scene sptr
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";
  output_types_[1] = "vil_image_view_base_sptr";

  bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
  // in case the 7th input is not set
  brdb_value_sptr idx = new brdb_value_t<vcl_string>("");
  brdb_value_sptr tnearfactor   = new brdb_value_t<float>(100000.0f);  //by default update alpha
  brdb_value_sptr tfarfactor   = new brdb_value_t<float>(100000.0f);  //by default update alpha

  pro.set_input(6, idx);
  pro.set_input(7, tnearfactor);
  pro.set_input(8, tfarfactor);
  return good;
}

bool boxm2_ocl_render_expected_image_process(bprb_func_process& pro)
{
  using namespace boxm2_ocl_render_expected_image_process_globals;

  vul_timer rtime;
  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  bocl_device_sptr device= pro.get_input<bocl_device_sptr>(i++);
  boxm2_scene_sptr scene =pro.get_input<boxm2_scene_sptr>(i++);

  boxm2_opencl_cache_sptr opencl_cache= pro.get_input<boxm2_opencl_cache_sptr>(i++);
  vpgl_camera_double_sptr cam= pro.get_input<vpgl_camera_double_sptr>(i++);
  unsigned ni=pro.get_input<unsigned>(i++);
  unsigned nj=pro.get_input<unsigned>(i++);
  vcl_string ident = pro.get_input<vcl_string>(i++);
  float   nearfactor   = pro.get_input<float>(i++);
  float   farfactor    = pro.get_input<float>(i++);
  bool foundDataType = false;
  vcl_string data_type,options;
  vcl_vector<vcl_string> apps = scene->appearances();

  int apptypesize = 0;
  for (unsigned int i=0; i<apps.size(); ++i) {
    if ( apps[i] == boxm2_data_traits<BOXM2_MOG3_GREY>::prefix() )
    {
      data_type = apps[i];
      foundDataType = true;
      options="-D MOG_TYPE_8 ";
      apptypesize = boxm2_data_traits<BOXM2_MOG3_GREY>::datasize();
    }
    else if ( apps[i] == boxm2_data_traits<BOXM2_MOG3_GREY_16>::prefix() )
    {
      data_type = apps[i];
      foundDataType = true;
      options="-D MOG_TYPE_16 ";
      apptypesize = boxm2_data_traits<BOXM2_MOG3_GREY_16>::datasize();
    }
    else if ( apps[i] == boxm2_data_traits<BOXM2_FLOAT8>::prefix() )
    {
      data_type = apps[i];
      foundDataType = true;
      options="-D FLOAT8 ";
      apptypesize = boxm2_data_traits<BOXM2_FLOAT8>::datasize();
    }
    else if ( apps[i] == boxm2_data_traits<BOXM2_LABEL_SHORT>::prefix() )
    {
      data_type = apps[i];
      foundDataType = true;
      options="-D SHORT ";
      apptypesize = boxm2_data_traits<BOXM2_LABEL_SHORT>::datasize();
    }
  }
  if (!foundDataType) {
    vcl_cout<<"BOXM2_OCL_RENDER_PROCESS ERROR: scene doesn't have BOXM2_MOG3_GREY or BOXM2_MOG3_GREY_16 data type"<<vcl_endl;
    return false;
  }
  if (ident.size() > 0) {
    data_type += "_" + ident;
  }

//: create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue(device->context(),*(device->device_id()),
                                                CL_QUEUE_PROFILING_ENABLE,&status);
  if (status!=0) return false;
  vcl_string identifier=device->device_identifier()+options;

  // compile the kernel
  if (kernels.find(identifier)==kernels.end())
  {
    vcl_cout<<"===========Compiling kernels==========="<<vcl_endl;
    vcl_vector<bocl_kernel*> ks;
    compile_kernel(device,ks,options);
    kernels[identifier]=ks;
  }

  unsigned cl_ni=RoundUp(ni,lthreads[0]);
  unsigned cl_nj=RoundUp(nj,lthreads[1]);
  float* buff = new float[cl_ni*cl_nj];
  for (unsigned i=0;i<cl_ni*cl_nj;i++) buff[i]=0.0f;

  bocl_mem_sptr exp_image=opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), buff,"exp image buffer");
  exp_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  int img_dim_buff[4];
  img_dim_buff[0] = 0;   img_dim_buff[2] = ni;
  img_dim_buff[1] = 0;   img_dim_buff[3] = nj;
  bocl_mem_sptr exp_img_dim=new bocl_mem(device->context(), img_dim_buff, sizeof(int)*4, "image dims");
  exp_img_dim->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // visibility image
  float* vis_buff = new float[cl_ni*cl_nj];
  vcl_fill(vis_buff, vis_buff + cl_ni*cl_nj, 1.0f);
  bocl_mem_sptr vis_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), vis_buff,"vis image buffer");
  vis_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
   float* max_omega_buff = new float[cl_ni*cl_nj];
  vcl_fill(max_omega_buff, max_omega_buff + cl_ni*cl_nj, 0.0f);
  bocl_mem_sptr max_omega_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), max_omega_buff,"vis image buffer");
  max_omega_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  float tnearfar[2] = { 0.0f, 1000000} ;

  if(cam->type_name() == "vpgl_perspective_camera")
  {
      
      float f  = ((vpgl_perspective_camera<double> *)cam.ptr())->get_calibration().focal_length()*((vpgl_perspective_camera<double> *)cam.ptr())->get_calibration().x_scale();
      vcl_cout<<"Focal Length " << f<<vcl_endl;
      tnearfar[0] = f* scene->finest_resolution()/nearfactor ;
      tnearfar[1] = f* scene->finest_resolution()*farfactor ;

      vcl_cout<<"Near and Far Clipping planes "<<tnearfar[0]<<" "<<tnearfar[1]<<vcl_endl;
  }
  bocl_mem_sptr tnearfar_mem_ptr = opencl_cache->alloc_mem(2*sizeof(float), tnearfar, "tnearfar  buffer");
  tnearfar_mem_ptr->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);


  // run expected image function
  render_expected_image(scene, device, opencl_cache, queue,
                        cam, exp_image, vis_image, max_omega_image,exp_img_dim,
                        data_type, kernels[identifier][0], lthreads, cl_ni, cl_nj,apptypesize,tnearfar_mem_ptr);
  // normalize
  if (kernels[identifier].size()>1)
  {
    vcl_size_t gThreads[] = {cl_ni,cl_nj};
    bocl_kernel* normalize_kern = kernels[identifier][1];
    normalize_kern->set_arg( exp_image.ptr() );
    normalize_kern->set_arg( vis_image.ptr() );
    normalize_kern->set_arg( exp_img_dim.ptr());
    normalize_kern->execute( queue, 2, lthreads, gThreads);
    clFinish(queue);

    //clear render kernel args so it can reset em on next execution
    normalize_kern->clear_args();
  }

  // read out expected image
  exp_image->read_to_buffer(queue);
  vis_image->read_to_buffer(queue);

#if 1 //output a float image by default
  vil_image_view<float>* exp_img_out=new vil_image_view<float>(ni,nj);
  for (unsigned c=0;c<nj;c++)
    for (unsigned r=0;r<ni;r++)
      (*exp_img_out)(r,c)=buff[c*cl_ni+r];
#else //option to output a byte image (For easier saving)
  vil_image_view<vxl_byte>* exp_img_out=new vil_image_view<vxl_byte>(ni,nj);
    for (unsigned c=0;c<nj;c++)
      for (unsigned r=0;r<ni;r++)
        (*exp_img_out)(r,c)= (vxl_byte) (buff[c*cl_ni+r] * 255.0f);
#endif
  vil_image_view<float>* vis_img_out=new vil_image_view<float>(ni,nj);
  for (unsigned c=0;c<nj;c++)
    for (unsigned r=0;r<ni;r++)
      (*vis_img_out)(r,c)=vis_buff[c*cl_ni+r];

  vcl_cout<<"Total Render time: "<<rtime.all()<<" ms"<<vcl_endl;
  delete [] vis_buff;
  delete [] buff;
  delete [] max_omega_buff;
  opencl_cache->unref_mem(vis_image.ptr());
  opencl_cache->unref_mem(exp_image.ptr());
  opencl_cache->unref_mem(max_omega_image.ptr());
    opencl_cache->unref_mem(tnearfar_mem_ptr.ptr());
  clReleaseCommandQueue(queue);
  i=0;
  // store scene smaprt pointer
  pro.set_output_val<vil_image_view_base_sptr>(i++, exp_img_out);
  pro.set_output_val<vil_image_view_base_sptr>(i++, vis_img_out);
  return true;
}
