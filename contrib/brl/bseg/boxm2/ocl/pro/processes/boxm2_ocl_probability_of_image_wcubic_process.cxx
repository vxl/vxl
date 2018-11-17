// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_probability_of_image_wcubic_process.cxx
//:
// \file
// \brief  A process for change detection
//
// \author Vishal Jain
// \date Mar 10, 2011

#include <fstream>
#include <iostream>
#include <algorithm>
#include <bprb/bprb_func_process.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boxm2/boxm2_util.h>
#include <vil/vil_image_view.h>
//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vul/vul_timer.h>
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>

#include <boxm2/ocl/algo/boxm2_ocl_camera_converter.h>

namespace boxm2_ocl_probability_of_image_wcubic_process_globals
{
  constexpr unsigned n_inputs_ = 7;
  constexpr unsigned n_outputs_ = 1;
  std::size_t lthreads[2]={8,8};

  static std::map<std::string,std::vector<bocl_kernel*> > kernels;

  void compile_kernel(const bocl_device_sptr& device,std::vector<bocl_kernel*> & vec_kernels, std::string opts)
  {
    //gather all render sources... seems like a lot for rendering...
  //gather all render sources... seems like a lot for rendering...
  std::vector<std::string> src_paths;
  std::string source_dir = boxm2_ocl_util::ocl_src_root();
  src_paths.push_back(source_dir + "scene_info.cl");
  src_paths.push_back(source_dir + "cell_utils.cl");
  src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
  src_paths.push_back(source_dir + "backproject.cl");
  src_paths.push_back(source_dir + "statistics_library_functions.cl");
  src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
  src_paths.push_back(source_dir + "bit/compute_probability_of_image.cl");
  src_paths.push_back(source_dir + "update_cubic_functors.cl");
  src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");

  //set kernel options
  opts += " -D PROB_CUBIC_IMAGE  ";
  std::string options=opts;

  opts += " -D STEP_CELL=step_cell_cubic_compute_probability_of_intensity(aux_args,data_ptr,d*linfo->block_len,vis,aux_args.prob_image) ";

    //have kernel construct itself using the context and device
    auto * ray_trace_kernel=new bocl_kernel();

    ray_trace_kernel->create_kernel( &device->context(),
                                     device->device_id(),
                                     src_paths,
                                     "cubic_compute_probability_of_image",   //kernel name
                                     opts,              //options
                                     "boxm2 ocl probability computation"); //kernel identifier (for error checking)
    vec_kernels.push_back(ray_trace_kernel);
    //create normalize image kernel
    std::vector<std::string> norm_src_paths;
    norm_src_paths.push_back(source_dir + "pixel_conversion.cl");
    norm_src_paths.push_back(source_dir + "bit/normalize_kernels.cl");
    auto * normalize_render_kernel=new bocl_kernel();

    normalize_render_kernel->create_kernel( &device->context(),
                                            device->device_id(),
                                            norm_src_paths,
                                            "normalize_probability_image_kernel",   //kernel name
                                            "-D PROB_IMAGE",              //options
                                            "normalize probability image kernel"); //kernel identifier (for error checking)

    vec_kernels.push_back(normalize_render_kernel);
  }
}

bool boxm2_ocl_probability_of_image_wcubic_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_probability_of_image_wcubic_process_globals;

  //process takes 1 input
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "boxm2_scene_sptr";
  input_types_[2] = "boxm2_opencl_cache_sptr";
  input_types_[3] = "vpgl_camera_double_sptr";
  input_types_[4] = "vil_image_view_base_sptr";
  input_types_[5] = "vcl_string";  // model identifier
  input_types_[6] = "vcl_string";  // aux identifier

  // process has 1 output:
  // output[0]: scene sptr
  std::vector<std::string>  output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";

  brdb_value_sptr idx = new brdb_value_t<std::string>("");
  pro.set_input(5, idx);
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_ocl_probability_of_image_wcubic_process(bprb_func_process& pro)
{
  using namespace boxm2_ocl_probability_of_image_wcubic_process_globals;
  std::size_t local_threads[2]={8,8};
  std::size_t global_threads[2]={8,8};

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
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
  vil_image_view_base_sptr img =pro.get_input<vil_image_view_base_sptr>(i++);
  std::string data_identifier =pro.get_input<std::string>(i++);
  std::string image_identifier =pro.get_input<std::string>(i++);
  unsigned ni=img->ni();
  unsigned nj=img->nj();
  bool foundDataType = false;
  std::string data_type,options;
  std::vector<std::string> apps = scene->appearances();
  for (const auto & app : apps) {
    if ( app == boxm2_data_traits<BOXM2_FLOAT8>::prefix() )
    {
      data_type = app;
      foundDataType = true;
      options=" -D FLOAT8 ";
    }
  }
  if (!foundDataType) {
    std::cout<<"boxm2_ocl_probability_of_image_wcubic_process ERROR: scene doesn't have BOXM2_MOG3_GREY or BOXM2_MOG3_GREY_16 data type"<<std::endl;
    return false;
  }

  if (data_identifier!="")
      data_type = data_type+"_"+data_identifier;

//: create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue(device->context(),*(device->device_id()),
                                                CL_QUEUE_PROFILING_ENABLE,&status);
  if (status!=0) return false;

  std::string identifier=device->device_identifier()+options;
  // compile the kernel
  if (kernels.find(identifier)==kernels.end())
  {
    std::cout<<"===========Compiling kernels==========="<<std::endl;
    std::vector<bocl_kernel*> ks;
    compile_kernel(device,ks,options);
    kernels[identifier]=ks;
  }

  //set generic cam

  vil_image_view_base_sptr float_img=boxm2_util::prepare_input_image(img);
  auto* img_view = static_cast<vil_image_view<float>* >(float_img.ptr());

  unsigned cl_ni=RoundUp(img_view->ni(),local_threads[0]);
  unsigned cl_nj=RoundUp(img_view->nj(),local_threads[1]);

  auto* ray_origins = new cl_float[4*cl_ni*cl_nj];
  auto* ray_directions = new cl_float[4*cl_ni*cl_nj];
  bocl_mem_sptr ray_o_buff = new bocl_mem(device->context(), ray_origins, cl_ni*cl_nj * sizeof(cl_float4) , "ray_origins buffer");
  bocl_mem_sptr ray_d_buff = new bocl_mem(device->context(), ray_directions,  cl_ni*cl_nj * sizeof(cl_float4), "ray_directions buffer");
  boxm2_ocl_camera_converter::compute_ray_image( device, queue, cam, cl_ni, cl_nj, ray_o_buff, ray_d_buff);

  global_threads[0]=cl_ni;
  global_threads[1]=cl_nj;
  auto* vis_buff = new float[cl_ni*cl_nj];
  auto* prob_image_buff = new float[cl_ni*cl_nj];
  for (unsigned i=0;i<cl_ni*cl_nj;i++)
  {
    vis_buff[i]=1.0f;
    prob_image_buff[i]=0.0f;
  }

  bocl_mem_sptr prob_image=new bocl_mem(device->context(),prob_image_buff,cl_ni*cl_nj*sizeof(float),"expected image buffer");
  prob_image->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  bocl_mem_sptr vis_image=new bocl_mem(device->context(),vis_buff,cl_ni*cl_nj*sizeof(float),"vis image buffer");
  vis_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // Image Dimensions
  int img_dim_buff[4];
  img_dim_buff[0] = 0;
  img_dim_buff[1] = 0;
  img_dim_buff[2] = img_view->ni();
  img_dim_buff[3] = img_view->nj();
  bocl_mem_sptr img_dim=new bocl_mem(device->context(), img_dim_buff, sizeof(int)*4, "image dims");
  img_dim->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // Output Array
  float output_arr[100];
  for (float & i : output_arr) i = 0.0f;
  bocl_mem_sptr  cl_output=new bocl_mem(device->context(), output_arr, sizeof(float)*100, "output buffer");
  cl_output->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // bit lookup buffer
  cl_uchar lookup_arr[256];
  boxm2_ocl_util::set_bit_lookup(lookup_arr);
  bocl_mem_sptr lookup=new bocl_mem(device->context(), lookup_arr, sizeof(cl_uchar)*256, "bit lookup buffer");
  lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  //For each ID in the visibility order, grab that block
  std::vector<boxm2_block_id> vis_order = scene->get_vis_blocks(  (vpgl_perspective_camera<double>*) cam.ptr());
  std::vector<boxm2_block_id>::iterator id;
  for (id = vis_order.begin(); id != vis_order.end(); ++id)
  {
    //choose correct render kernel
    boxm2_block_metadata mdata = scene->get_block_metadata(*id);
    bocl_kernel* kern =  kernels[identifier][0];
    //write the image values to the buffer
    vul_timer transfer;
    bocl_mem* blk = opencl_cache->get_block(scene,*id);
    bocl_mem* blk_info = opencl_cache->loaded_block_info();
    bocl_mem* alpha = opencl_cache->get_data<BOXM2_ALPHA>(scene,*id);
    int mogTypeSize = (int) boxm2_data_info::datasize(data_type);
    auto* info_buffer = (boxm2_scene_info*) blk_info->cpu_buffer();
    int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
    info_buffer->data_buffer_length = (int) (alpha->num_bytes()/alphaTypeSize);
    blk_info->write_to_buffer((queue));

    bocl_mem* mog = opencl_cache->get_data(scene,*id,data_type,alpha->num_bytes()/alphaTypeSize*mogTypeSize,false);    //info_buffer->data_buffer_length*boxm2_data_info::datasize(data_type));
    std::string aux0_datatype = boxm2_data_traits<BOXM2_AUX0>::prefix(image_identifier);
    bocl_mem* aux0 = opencl_cache->get_data(scene,*id,aux0_datatype,alpha->num_bytes(),false);
    std::string aux1_datatype = boxm2_data_traits<BOXM2_AUX1>::prefix(image_identifier);
    bocl_mem* aux1 = opencl_cache->get_data(scene,*id,aux1_datatype,alpha->num_bytes(),false);

    transfer_time += (float) transfer.all();

    ////3. SET args
    kern->set_arg( blk_info );
    kern->set_arg( blk );
    kern->set_arg( alpha );
    kern->set_arg( mog );
    kern->set_arg( aux0 );
    kern->set_arg( aux1 );
    kern->set_arg( ray_o_buff.ptr() );
    kern->set_arg( ray_d_buff.ptr() );
    kern->set_arg( prob_image.ptr() );
    kern->set_arg( vis_image.ptr() );
    kern->set_arg( img_dim.ptr());
    kern->set_arg( cl_output.ptr() );
    kern->set_arg( lookup.ptr() );

    //local tree , cumsum buffer, imindex buffer
    kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_uchar16) );
    kern->set_local_arg( local_threads[0]*local_threads[1]*10*sizeof(cl_uchar) );
    kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_int) );

    //execute kernel
    kern->execute(queue, 2, local_threads, global_threads);
    clFinish(queue);
    gpu_time += kern->exec_time();

    //clear render kernel args so it can reset em on next execution
    kern->clear_args();
  }

#if 1
  // normalize
  bocl_kernel* normalize_prob_image_kernel =  kernels[identifier][1];
  {
    normalize_prob_image_kernel->set_arg( prob_image.ptr() );
    normalize_prob_image_kernel->set_arg( vis_image.ptr() );
    normalize_prob_image_kernel->set_arg( img_dim.ptr());
    normalize_prob_image_kernel->execute( queue, 2, local_threads, global_threads);
    clFinish(queue);
    gpu_time += normalize_prob_image_kernel->exec_time();

    //clear render kernel args so it can reset em on next execution
    normalize_prob_image_kernel->clear_args();
  }
#endif
  // read out expected image
  prob_image->read_to_buffer(queue);
  vis_image->read_to_buffer(queue);
   clFinish(queue);
  auto* prob_img_out=new vil_image_view<float>(ni,nj);

  for (unsigned c=0;c<nj;c++)
    for (unsigned r=0;r<ni;r++)
      (*prob_img_out)(r,c)=prob_image_buff[c*cl_ni+r];

  delete [] prob_image_buff;
  delete [] vis_buff;

  clReleaseCommandQueue(queue);
  i=0;
  // store scene smaprt pointer
  pro.set_output_val<vil_image_view_base_sptr>(i++, prob_img_out);
  return true;
}
