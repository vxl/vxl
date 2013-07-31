// This is brl/bseg/bstm/ocl/pro/processes/bstm_ocl_render_expected_image_process.cxx
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
#include <vul/vul_timer.h>
//directory utility
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <bstm/ocl/algo/bstm_ocl_render_expected_image_function.h>


namespace bstm_ocl_render_expected_image_process_globals
{
  const unsigned n_inputs_ = 8;
  const unsigned n_outputs_ = 4;
  vcl_size_t lthreads[2]={8,8};

  static vcl_map<vcl_string,vcl_vector<bocl_kernel*> > kernels;

  void compile_kernel(bocl_device_sptr device,vcl_vector<bocl_kernel*> & vec_kernels, vcl_string opts, bool isViewDep,bool isColor)
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
    if(!isColor) {
      src_paths.push_back(source_dir + "bit/render_bit_scene.cl");
      src_paths.push_back(source_dir + "expected_functor.cl");
    }
    else {
      src_paths.push_back(source_dir + "bit/render_rgb.cl");
    }
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

    if(!isColor) {
      normalize_render_kernel->create_kernel( &device->context(),
                                              device->device_id(),
                                              norm_src_paths,
                                              "normalize_render_kernel",   //kernel name
                                              "-D NORMALIZE_RENDER ",       //options
                                              "normalize_render_kernel"); //kernel identifier (for error checking)
    }
    else
      normalize_render_kernel->create_kernel( &device->context(),
                                              device->device_id(),
                                              norm_src_paths,
                                              "normalize_render_kernel",   //kernel name
                                              "-D NORMALIZE_RENDER ",       //options
                                              "normalize_render_kernel"); //kernel identifier (for error checking)
    vec_kernels.push_back(normalize_render_kernel);
  }
}

bool bstm_ocl_render_expected_image_process_cons(bprb_func_process& pro)
{
  using namespace bstm_ocl_render_expected_image_process_globals;

  //process takes 1 input
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "bstm_scene_sptr";
  input_types_[2] = "bstm_opencl_cache_sptr";
  input_types_[3] = "vpgl_camera_double_sptr";
  input_types_[4] = "unsigned";
  input_types_[5] = "unsigned";
  input_types_[6] = "float"; // time
  input_types_[7] = "bool"; // render label?

  brdb_value_sptr render_label_val    = new brdb_value_t<bool>(false);
  pro.set_input(7, render_label_val);

  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";
  output_types_[1] = "vil_image_view_base_sptr";
  output_types_[2] = "float";
  output_types_[3] = "float";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);

}

bool bstm_ocl_render_expected_image_process(bprb_func_process& pro)
{
  using namespace bstm_ocl_render_expected_image_process_globals;

  vul_timer rtime;
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
    vcl_cout<<"bstm_ocl_render_expected_image_process ERROR: scene doesn't have correct appearance model data type"<<vcl_endl;
    return false;
  }


  bool isViewDep = (data_type == bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::prefix()) ||
                   (data_type == bstm_data_traits<BSTM_MOG6_VIEW>::prefix()) ||
                   (data_type == bstm_data_traits<BSTM_GAUSS_RGB_VIEW_COMPACT>::prefix() );

  bool isColor = ( data_type == bstm_data_traits<BSTM_GAUSS_RGB>::prefix() );

  //get initial options (MOG TYPE)
  vcl_string options = bstm_ocl_util::mog_options(data_type);

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
  cl_command_queue queue = clCreateCommandQueue(device->context(),*(device->device_id()),
                                                CL_QUEUE_PROFILING_ENABLE,&status);
  if (status!=0) return false;
  vcl_string identifier=device->device_identifier()+options;

  // compile the kernel
  if (kernels.find(identifier)==kernels.end())
  {
    vcl_cout<<"===========Compiling kernels==========="<<vcl_endl;
    vcl_vector<bocl_kernel*> ks;
    compile_kernel(device,ks,options, isViewDep,isColor);
    kernels[identifier]=ks;
  }

  unsigned cl_ni=RoundUp(ni,lthreads[0]);
  unsigned cl_nj=RoundUp(nj,lthreads[1]);

  float* buff = new float[4*cl_ni*cl_nj];
  vcl_fill(buff, buff + 4*cl_ni*cl_nj, 0.0f);
  bocl_mem_sptr exp_image = new bocl_mem(device->context(), buff ,  4*cl_ni*cl_nj*sizeof(float), "exp image buffer");
  exp_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  int img_dim_buff[4];
  img_dim_buff[0] = 0;   img_dim_buff[2] = ni;
  img_dim_buff[1] = 0;   img_dim_buff[3] = nj;
  bocl_mem_sptr exp_img_dim=new bocl_mem(device->context(), img_dim_buff, sizeof(int)*4, "image dims");
  exp_img_dim->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // visibility image
  float* vis_buff = new float[cl_ni*cl_nj];
  vcl_fill(vis_buff, vis_buff + cl_ni*cl_nj, 1.0f);
  bocl_mem_sptr vis_image = new bocl_mem(device->context(), vis_buff, cl_ni*cl_nj*sizeof(float), "vis image buffer");
  vis_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);


  // run expected image function
  float render_time;
  //vcl_cout << "Render label:" << render_label << " found label data type: " << foundLabelDataType << vcl_endl;
  if(!foundLabelDataType || !render_label)
    render_time = render_expected_image( scene, device, opencl_cache, queue,
                                      cam, exp_image, vis_image, exp_img_dim,
                                      data_type, kernels[identifier][0], lthreads, cl_ni, cl_nj, apptypesize,time);
  else
    render_time = render_expected_image( scene, device, opencl_cache, queue,
                                      cam, exp_image, vis_image, exp_img_dim,
                                      data_type, kernels[identifier][0], lthreads, cl_ni, cl_nj, apptypesize,time, label_data_type, label_apptypesize, render_label);
  // normalize


  {
    vcl_size_t gThreads[] = {cl_ni,cl_nj};
    bocl_kernel* normalize_kern= kernels[identifier][1];
    normalize_kern->set_arg( exp_image.ptr() );
    normalize_kern->set_arg( vis_image.ptr() );
    normalize_kern->set_arg( exp_img_dim.ptr());
    normalize_kern->execute( queue, 2, lthreads, gThreads);
    clFinish(queue);

    //clear render kernel args so it can reset em on next execution
    normalize_kern->clear_args();
    render_time += normalize_kern->exec_time();
  }


  float all_time = rtime.all();
  vcl_cout<<"Total Render time: "<<render_time <<" ms"<<vcl_endl;

  vis_image->read_to_buffer(queue);
  exp_image->read_to_buffer(queue);


  vil_image_view<vil_rgba<vxl_byte> >* exp_img_out = new vil_image_view<vil_rgba<vxl_byte> >(ni,nj);
  int numFloats = 4;
  int count = 0;
  for (unsigned c=0;c<nj;++c) {
    for (unsigned r=0;r<ni;++r,count+=numFloats) {
      (*exp_img_out)(r,c) =
      vil_rgba<vxl_byte> ( (vxl_byte) (buff[count]*255.0f),
                           (vxl_byte) (buff[count+1]*255.0f),
                           (vxl_byte) (buff[count+2]*255.0f),
                           (vxl_byte) 255 );
    }
  }

  vil_image_view<float>* vis_img_out=new vil_image_view<float>(ni,nj);
  for (unsigned c=0;c<nj;c++)
    for (unsigned r=0;r<ni;r++)
      (*vis_img_out)(r,c)=vis_buff[c*cl_ni+r];


  delete [] vis_buff;
  delete [] buff;

  // read out expected image
  clReleaseCommandQueue(queue);

  //store render time
  int argIdx = 0;
  pro.set_output_val<vil_image_view_base_sptr>(argIdx++, exp_img_out);
  pro.set_output_val<vil_image_view_base_sptr>(argIdx++, vis_img_out);
  pro.set_output_val<float>(argIdx++, render_time);
  pro.set_output_val<float>(argIdx++, all_time );
  return true;
}
