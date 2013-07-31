// This is brl/bseg/bstm/ocl/pro/processes/bstm_ocl_render_gl_expected_image_process.cxx
//:
// \file


#include <bprb/bprb_func_process.h>

#include <vcl_fstream.h>
#include <vcl_algorithm.h>
#include <bstm/ocl/bstm_opencl_cache.h>
#include <bstm/bstm_scene.h>
#include <bstm/bstm_block.h>
#include <bstm/bstm_data_base.h>
#include <bstm/bstm_util.h>
#include <bstm/ocl/bstm_ocl_util.h>
//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <bstm/ocl/algo/bstm_ocl_render_expected_image_function.h>


namespace bstm_ocl_render_gl_expected_image_process_globals
{
  const unsigned n_inputs_ = 10 ;
  const unsigned n_outputs_ = 1;
  vcl_size_t lthreads[2]={8,8};

  static vcl_map<vcl_string,vcl_vector<bocl_kernel*> > kernels;

  void compile_kernel(bocl_device_sptr device,vcl_vector<bocl_kernel*> & vec_kernels, vcl_string opts, bool isViewDep)
  {
    //gather all render sources... seems like a lot for rendering...
    vcl_vector<vcl_string> src_paths;
    vcl_string source_dir = vcl_string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/bstm/ocl/cl/";
    src_paths.push_back(source_dir + "scene_info.cl");
    src_paths.push_back(source_dir + "pixel_conversion.cl");
    src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
    src_paths.push_back(source_dir + "bit/time_tree_library_functions.cl");
    src_paths.push_back(source_dir + "statistics_library_functions.cl");
    src_paths.push_back(source_dir + "view_dep_app_helper_functions.cl");
    src_paths.push_back(source_dir + "backproject.cl");
    src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
    src_paths.push_back(source_dir + "bit/render_bit_scene.cl");
    src_paths.push_back(source_dir + "expected_functor.cl");
    src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");

    //set kernel options
    vcl_string options = opts;
    if(!isViewDep)
      options += " -D RENDER_LAMBERT -D RENDER_MOG ";
    else
      options += " -D RENDER_VIEW_DEP ";

    options += " -D STEP_CELL=step_cell_render(aux_args,data_ptr_tt,d*linfo->block_len)";

    //have kernel construct itself using the context and device
    bocl_kernel * ray_trace_kernel=new bocl_kernel();

    vcl_cout << "Compiling with options: " << options << vcl_endl;
    ray_trace_kernel->create_kernel( &device->context(),
                                     device->device_id(),
                                     src_paths,
                                     "render_bit_scene",   //kernel name
                                     options,              //options
                                     "bstm opencl render"); //kernel identifier (for error checking)
    vec_kernels.push_back(ray_trace_kernel);
    //create normalize image kernel
    vcl_vector<vcl_string> norm_src_paths;
    norm_src_paths.push_back(source_dir + "pixel_conversion.cl");
    norm_src_paths.push_back(source_dir + "bit/normalize_kernels.cl");
    bocl_kernel * normalize_render_kernel=new bocl_kernel();

    normalize_render_kernel->create_kernel( &device->context(),
                                            device->device_id(),
                                            norm_src_paths,
                                            "normalize_render_kernel_rgb_gl",   //kernel name
                                            "-D NORMALIZE_RENDER_GL ",       //options
                                            "normalize render kernel rgb gl"); //kernel identifier (for error checking)

    vec_kernels.push_back(normalize_render_kernel);
  }
}

bool bstm_ocl_render_gl_expected_image_process_cons(bprb_func_process& pro)
{
  using namespace bstm_ocl_render_gl_expected_image_process_globals;

  //process takes 1 input
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "bstm_scene_sptr";
  input_types_[2] = "bstm_opencl_cache_sptr";
  input_types_[3] = "vpgl_camera_double_sptr";
  input_types_[4] = "unsigned";
  input_types_[5] = "unsigned";
  input_types_[6] = "bocl_mem_sptr"; // exp image buffer;
  input_types_[7] = "bocl_mem_sptr"; // exp image dimensions buffer;
  input_types_[8] = "float"; // time
  input_types_[9] = "bool"; // render label?

  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0] = "float";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);

}

bool bstm_ocl_render_gl_expected_image_process(bprb_func_process& pro)
{
  using namespace bstm_ocl_render_gl_expected_image_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  bocl_device_sptr device= pro.get_input<bocl_device_sptr>(i++);
  bstm_scene_sptr scene =pro.get_input<bstm_scene_sptr>(i++);

  bstm_opencl_cache_sptr opencl_cache= pro.get_input<bstm_opencl_cache_sptr>(i++);
  vpgl_camera_double_sptr cam= pro.get_input<vpgl_camera_double_sptr>(i++);
  unsigned ni=pro.get_input<unsigned>(i++);
  unsigned nj=pro.get_input<unsigned>(i++);
  bocl_mem_sptr exp_image =pro.get_input<bocl_mem_sptr>(i++);
  bocl_mem_sptr exp_img_dim =pro.get_input<bocl_mem_sptr>(i++);
  float time = pro.get_input<float>(i++);
  bool render_label = pro.get_input<bool>(i++);




  //get scene data type and appTypeSize
  vcl_string data_type,label_data_type;
  int apptypesize,label_apptypesize;
  vcl_vector<vcl_string> valid_types;
  valid_types.push_back(bstm_data_traits<BSTM_MOG6_VIEW>::prefix());
  valid_types.push_back(bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::prefix());
  valid_types.push_back(bstm_data_traits<BSTM_MOG3_GREY>::prefix());
  valid_types.push_back(bstm_data_traits<BSTM_GAUSS_RGB>::prefix());
  valid_types.push_back(bstm_data_traits<BSTM_GAUSS_RGB_VIEW_COMPACT>::prefix());
  if ( !bstm_util::verify_appearance( *scene, valid_types, data_type, apptypesize ) ) {
    vcl_cout<<"bstm_ocl_render_gl_expected_image_process ERROR: scene doesn't have BSTM_MOG3_GREY or BSTM_GAUSS_RGB data type"<<vcl_endl;
    return false;
  }

  bool isViewDep = (data_type == bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::prefix()) ||
                    (data_type == bstm_data_traits<BSTM_MOG6_VIEW>::prefix()) ||
                    (data_type == bstm_data_traits<BSTM_GAUSS_RGB_VIEW_COMPACT>::prefix());

  //get initial options (MOG TYPE)
  vcl_string options = bstm_ocl_util::mog_options(data_type);

  vcl_cout << data_type << " " << options << " " << apptypesize <<vcl_endl;
  //get scene label data type and appTypeSize if any.
  vcl_vector<vcl_string> valid_label_types;
  valid_label_types.push_back(bstm_data_traits<BSTM_LABEL>::prefix());
  bool foundLabelDataType = bstm_util::verify_appearance( *scene, valid_label_types, label_data_type, label_apptypesize );
#ifdef DEBUG
  if ( !foundLabelDataType )
    vcl_cout<<"Scene doesn't have BSTM_LABEL label type...rendering without it..."<<vcl_endl;
  else
    vcl_cout<<"Scene has " << label_data_type << " type...rendering with it..."<<vcl_endl;
#endif
  //get options for teh label
  options += bstm_ocl_util::label_options(label_data_type);

  //: create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue(device->context(),*(device->device_id()), CL_QUEUE_PROFILING_ENABLE,&status);
  if (status!=0) return false;
  vcl_string identifier=device->device_identifier()+options;

  // compile the kernel
  if (kernels.find(identifier)==kernels.end())
  {
    vcl_cout<<"===========Compiling kernels==========="<<vcl_endl;
    vcl_vector<bocl_kernel*> ks;
    compile_kernel(device,ks,options, isViewDep);
    kernels[identifier]=ks;
  }

  unsigned cl_ni=RoundUp(ni,lthreads[0]);
  unsigned cl_nj=RoundUp(nj,lthreads[1]);

  float* buff = new float[4*cl_ni*cl_nj];
  vcl_fill(buff, buff + 4*cl_ni*cl_nj, 0.0f);
  bocl_mem_sptr exp_color = new bocl_mem(device->context(), buff, 4*cl_ni*cl_nj*sizeof(float), "color im buffer (float4) buffer");
  exp_color->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // visibility image
  float* vis_buff = new float[cl_ni*cl_nj];
  vcl_fill(vis_buff, vis_buff + cl_ni*cl_nj, 1.0f);
  bocl_mem_sptr vis_image = new bocl_mem(device->context(), vis_buff, cl_ni*cl_nj*sizeof(float),"vis image buffer");
  vis_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);


  // run expected image function
  float render_time;

  if(!foundLabelDataType || !render_label)
    render_time = render_expected_image( scene, device, opencl_cache, queue,
                                      cam, exp_color, vis_image, exp_img_dim,
                                      data_type, kernels[identifier][0], lthreads, cl_ni, cl_nj, apptypesize,time);
  else
    render_time = render_expected_image( scene, device, opencl_cache, queue,
                                          cam, exp_color, vis_image, exp_img_dim,
                                          data_type, kernels[identifier][0], lthreads, cl_ni, cl_nj, apptypesize,time, label_data_type, label_apptypesize, render_label);



  // normalize
  {
    vcl_size_t gThreads[] = {cl_ni,cl_nj};
    bocl_kernel* normalize_kern= kernels[identifier][1];
    normalize_kern->set_arg( exp_color.ptr() );
    normalize_kern->set_arg( vis_image.ptr() );
    normalize_kern->set_arg( exp_img_dim.ptr());
    normalize_kern->set_arg( exp_image.ptr() );
    normalize_kern->execute( queue, 2, lthreads, gThreads);
    clFinish(queue);

    //clear render kernel args so it can reset em on next execution
    normalize_kern->clear_args();
    render_time += normalize_kern->exec_time();
  }



  // read out expected image
  clReleaseCommandQueue(queue);

  delete[] vis_buff;
  delete[] buff;

  //opencl_cache->free_mem(exp_color.ptr());
  //opencl_cache->free_mem(vis_image.ptr());

  //store render time
  int argIdx = 0;
  pro.set_output_val<float>(argIdx, render_time);
  return true;
}
