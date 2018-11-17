// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_update_aux_per_view_process.cxx
//:
// \file
// \brief  A process for updating the scene.
//
// \author Vishal Jain
// \date Mar 25, 2011

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
#include <boxm2/ocl/algo/boxm2_ocl_camera_converter.h>
#include <vil/vil_convert.h>

//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vul/vul_timer.h>
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>

namespace boxm2_ocl_update_aux_per_view_process_globals
{
  constexpr unsigned n_inputs_ = 7;
  constexpr unsigned n_outputs_ = 0;
  enum {
    UPDATE_AUX_LEN_INT_VIS = 0,
    CONVERT_AUX_INT_FLOAT = 1
  };

  void compile_kernel(const bocl_device_sptr& device,std::vector<bocl_kernel*> & vec_kernels,const std::string& opts)
  {
    //gather all render sources... seems like a lot for rendering...
    std::vector<std::string> src_paths;
    std::string source_dir = boxm2_ocl_util::ocl_src_root();
    src_paths.push_back(source_dir + "scene_info.cl");
    src_paths.push_back(source_dir + "cell_utils.cl");
    src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
    src_paths.push_back(source_dir + "backproject.cl");
    src_paths.push_back(source_dir + "statistics_library_functions.cl");
    src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
    src_paths.push_back(source_dir + "bit/batch_update_kernels.cl");

    //convert aux buffer int values to float (just divide by SEGLENFACTOR
    auto* convert_aux_int_float = new bocl_kernel();
    convert_aux_int_float->create_kernel(&device->context(),device->device_id(), src_paths, "convert_aux_int_to_float", opts+" -D CONVERT_AUX ", "batch_update::convert_aux_int_to_float");

    src_paths.push_back(source_dir + "batch_update_functors.cl");
    src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");

    //push back cast_ray_bit
    auto* aux_len_int_vis = new bocl_kernel();
    std::string aux_opt = opts + " -D AUX_LEN_INT_VIS -D STEP_CELL=step_cell_aux_len_int_vis(aux_args,data_ptr,llid,d) ";
    aux_len_int_vis->create_kernel(&device->context(),device->device_id(), src_paths, "aux_len_int_vis_main", aux_opt, "batch_update::aux_len_int_vis_main");
    vec_kernels.push_back(aux_len_int_vis);

    ////may need DIFF LIST OF SOURCES FOR THSI GUY TOO
    vec_kernels.push_back(convert_aux_int_float);

    return ;
  }


  static std::map<std::string,std::vector<bocl_kernel*> > kernels;
}

bool boxm2_ocl_update_aux_per_view_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_update_aux_per_view_process_globals;

  //process takes 1 input
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "boxm2_scene_sptr";
  input_types_[2] = "boxm2_opencl_cache_sptr";
  input_types_[3] = "vpgl_camera_double_sptr";
  input_types_[4] = "vil_image_view_base_sptr";
  input_types_[5] = "vcl_string";
  input_types_[6] = "vil_image_view_base_sptr";

  // process has 1 output:
  // output[0]: scene sptr
  std::vector<std::string>  output_types_(n_outputs_);

  bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
  // in case the 6th input is not set
  brdb_value_sptr idx5 = new brdb_value_t<std::string>("");
  pro.set_input(5, idx5);
  // in case the 7th input is not set
  brdb_value_sptr idx6 = new brdb_value_t<vil_image_view_base_sptr>();
  pro.set_input(6, idx6);
  return good;
}

bool boxm2_ocl_update_aux_per_view_process(bprb_func_process& pro)
{
  using namespace boxm2_ocl_update_aux_per_view_process_globals;
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
  bocl_device_sptr device = pro.get_input<bocl_device_sptr>(i++);
  boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_opencl_cache_sptr opencl_cache = pro.get_input<boxm2_opencl_cache_sptr>(i++);
  vpgl_camera_double_sptr cam = pro.get_input<vpgl_camera_double_sptr>(i++);
  vil_image_view_base_sptr img = pro.get_input<vil_image_view_base_sptr>(i++);
  std::string suffix = pro.get_input<std::string>(i++);
  vil_image_view_base_sptr mask_base = pro.get_input<vil_image_view_base_sptr>(i++);

  vil_image_view<float> mask(img->ni(),img->nj());
  if (mask_base){
    if (auto *mask_byte = dynamic_cast<vil_image_view<vxl_byte>*>(mask_base.ptr())) {
      vil_convert_stretch_range_limited(*mask_byte, mask, (vxl_byte)0, (vxl_byte)255, 0.0f, 1.0f);
    }
    else if (auto *mask_float = dynamic_cast<vil_image_view<float>*>(mask_base.ptr())) {
      // clamp values at (0,1)
      vil_convert_stretch_range_limited(*mask_float,mask, 0.0f, 1.0f, 0.0f, 1.0f);
    }
    else {
      std::cerr << "ERROR: unknown mask pixel type\n";
      return false;
    }
  }
  else {
    // no mask specified: use all pixels
    mask.fill(1.0f);
  }

  long binCache = opencl_cache.ptr()->bytes_in_cache();
  std::cout<<"Update MBs in cache: "<<binCache/(1024.0*1024.0)<<std::endl;

  bool foundDataType = false;
  std::string data_type,num_obs_type,options;
  std::vector<std::string> apps = scene->appearances();
  for (const auto & app : apps) {
    if ( app == boxm2_data_traits<BOXM2_MOG3_GREY>::prefix() )
    {
      data_type = app;
      foundDataType = true;
      options=" -D MOG_TYPE_8 ";
      // boxm2_data_info::datasize(boxm2_data_traits<BOXM2_MOG3_GREY>::prefix());
    }
    else if ( app == boxm2_data_traits<BOXM2_MOG3_GREY_16>::prefix() )
    {
      data_type = app;
      foundDataType = true;
      options=" -D MOG_TYPE_16 ";
      // boxm2_data_info::datasize(boxm2_data_traits<BOXM2_MOG3_GREY_16>::prefix());
    }
    else if ( app == boxm2_data_traits<BOXM2_NORMAL_ALBEDO_ARRAY>::prefix() )
    {
      data_type = app;
      foundDataType = true;
      options=" -D BOXM2_NORMAL_ALBEDO_ARRAY ";
      // boxm2_data_info::datasize(boxm2_data_traits<BOXM2_NORMAL_ALBEDO_ARRAY>::prefix());
    }
  }
  if (!foundDataType) {
    std::cout<<"BOXM2_OCL_UPDATE_AUX_PER_VIEW_PROCESS ERROR: scene doesn't have BOXM2_MOG3_GREY or BOXM2_MOG3_GREY_16 data type"<<std::endl;
    return false;
  }

  // create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue(device->context(),*(device->device_id()),CL_QUEUE_PROFILING_ENABLE,&status);
  if (status!=0) return false;

  std::string identifier=device->device_identifier()+options;
  // compile the kernel if not already compiled
  if (kernels.find(identifier)==kernels.end())
  {
    std::cout<<"===========Compiling kernels==========="<<std::endl;
    std::vector<bocl_kernel*> ks;
    compile_kernel(device,ks,"");
    kernels[identifier]=ks;
  }

  //grab input image, establish cl_ni, cl_nj (so global size is divisible by local size)
  vil_image_view_base_sptr float_img=boxm2_util::prepare_input_image(img);
  auto* img_view = static_cast<vil_image_view<float>* >(float_img.ptr());
  auto cl_ni=(unsigned)RoundUp(img_view->ni(),(int)local_threads[0]);
  auto cl_nj=(unsigned)RoundUp(img_view->nj(),(int)local_threads[1]);
  global_threads[0]=cl_ni;
  global_threads[1]=cl_nj;

  //set generic cam
  auto* ray_origins = new cl_float[4*cl_ni*cl_nj];
  auto* ray_directions = new cl_float[4*cl_ni*cl_nj];
  std::cout << "allocating ray_o_buff: ni = " << cl_ni << ", nj = " << cl_nj << "  size = " << cl_ni*cl_nj*sizeof(cl_float4) << std::endl;
  bocl_mem_sptr ray_o_buff = opencl_cache->alloc_mem( cl_ni*cl_nj * sizeof(cl_float4) , ray_origins,"ray_origins buffer");
  std::cout << "allocating ray_d_buff: ni = " << cl_ni << ", nj = " << cl_nj << std::endl;
  bocl_mem_sptr ray_d_buff = opencl_cache->alloc_mem(cl_ni*cl_nj * sizeof(cl_float4), ray_directions,"ray_directions buffer");
  boxm2_ocl_camera_converter::compute_ray_image( device, queue, cam, cl_ni, cl_nj, ray_o_buff, ray_d_buff);

  //Visibility, Preinf, Norm, and input image buffers
  auto* vis_buff = new float[cl_ni*cl_nj];

  auto* input_buff=new float[cl_ni*cl_nj];

  int count=0;
  for (unsigned int j=0;j<cl_nj;++j)
    for (unsigned int i=0;i<cl_ni;++i)
    {
      input_buff[count] = 0.0f;
      vis_buff[i] = 1.0f;
      if (i<img_view->ni() && j< img_view->nj()) {
        input_buff[count]=(*img_view)(i,j);
        vis_buff[count] = mask(i,j);
      }
      ++count;
    }
  std::cout << "allocating input_buff: ni = " << cl_ni << ", nj = " << cl_nj << std::endl;
  bocl_mem_sptr in_image=opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float),input_buff,"input image buffer");
  in_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  std::cout << "allocating vis_buff: ni = " << cl_ni << ", nj = " << cl_nj << std::endl;
  bocl_mem_sptr vis_image=opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float),vis_buff,"vis image buffer");
  vis_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  std::cout << "Done allocating images" << std::endl;

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


  // set arguments
  std::vector<boxm2_block_id> vis_order = scene->get_vis_blocks(cam);
  std::vector<boxm2_block_id>::iterator id;

  for (id = vis_order.begin(); id != vis_order.end(); ++id)
  {
    //write the image values to the buffer
    vul_timer transfer;
    bocl_mem* blk = opencl_cache->get_block(scene,*id);
    std::cout<<(*id)<<' '<<opencl_cache->bytes_in_cache()<<std::endl;
    bocl_mem* blk_info = opencl_cache->loaded_block_info();
    bocl_mem* alpha = opencl_cache->get_data<BOXM2_ALPHA>(scene,*id,0,false);
    auto* info_buffer = (boxm2_scene_info*) blk_info->cpu_buffer();
    int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
    info_buffer->data_buffer_length = (int) (alpha->num_bytes()/alphaTypeSize);
    blk_info->write_to_buffer((queue));

    //grab an appropriately sized AUX data buffer
    int auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX0>::prefix());
    bocl_mem *aux0 = opencl_cache->get_data(scene,*id, boxm2_data_traits<BOXM2_AUX0>::prefix(suffix),info_buffer->data_buffer_length*auxTypeSize,false);
    auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX1>::prefix());
    bocl_mem *aux1 = opencl_cache->get_data(scene,*id, boxm2_data_traits<BOXM2_AUX1>::prefix(suffix),info_buffer->data_buffer_length*auxTypeSize,false);
    auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX2>::prefix());
    bocl_mem *aux2 = opencl_cache->get_data(scene,*id,boxm2_data_traits<BOXM2_AUX2>::prefix(suffix), info_buffer->data_buffer_length*auxTypeSize,false);
    auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX3>::prefix());
    bocl_mem *aux3 = opencl_cache->get_data(scene,*id,boxm2_data_traits<BOXM2_AUX3>::prefix(suffix), info_buffer->data_buffer_length*auxTypeSize,false);

    // transfer_time += (float) transfer.all();
    for (unsigned int i=0; i<kernels[identifier].size(); ++i)
    {
      bocl_kernel* kern =  kernels[identifier][i];

      if (i==UPDATE_AUX_LEN_INT_VIS)
      {
        local_threads[0] = 8;
        local_threads[1] = 8 ;
        global_threads[0]=cl_ni;
        global_threads[1]=cl_nj;
        aux0->zero_gpu_buffer(queue);
        aux1->zero_gpu_buffer(queue);
        aux2->zero_gpu_buffer(queue);
        aux3->zero_gpu_buffer(queue);

        kern->set_arg( blk_info );
        kern->set_arg( blk );
        kern->set_arg( alpha );
        kern->set_arg( aux0 );
        kern->set_arg( aux1 );
        kern->set_arg( aux2 );
        kern->set_arg( aux3 );
        kern->set_arg( lookup.ptr() );
        kern->set_arg( ray_o_buff.ptr() );
        kern->set_arg( ray_d_buff.ptr() );

        kern->set_arg( img_dim.ptr() );
        kern->set_arg( vis_image.ptr() );
        kern->set_arg( in_image.ptr() );
        kern->set_arg( cl_output.ptr() );
        kern->set_local_arg( local_threads[0]*local_threads[1]   *sizeof(cl_uchar16) );//local tree,
        kern->set_local_arg( local_threads[0]*local_threads[1]   *sizeof(cl_short2) ); //ray bundle,
        kern->set_local_arg( local_threads[0]*local_threads[1]   *sizeof(cl_int) );    //cell pointers,
        kern->set_local_arg( local_threads[0]*local_threads[1]   *sizeof(cl_float) ); //cached aux,
        kern->set_local_arg( local_threads[0]*local_threads[1]*10*sizeof(cl_uchar) ); //cumsum buffer, imindex buffer
        //execute kernel
        kern->execute(queue, 2, local_threads, global_threads);
        int status = clFinish(queue);
        check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status));
        gpu_time += kern->exec_time();
        kern->clear_args();
      }
      else if (i==CONVERT_AUX_INT_FLOAT)
      {
        local_threads[0] = 64;
        local_threads[1] = 1 ;
        global_threads[0]=RoundUp(info_buffer->data_buffer_length,local_threads[0]);
        global_threads[1]=1;

        kern->set_arg( blk_info );
        kern->set_arg( aux0 );
        kern->set_arg( aux1 );
        kern->set_arg( aux2 );
        kern->set_arg( aux3 );

        //execute kernel
        kern->execute(queue, 2, local_threads, global_threads);
        int status = clFinish(queue);
        check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status));
        gpu_time += kern->exec_time();

        //clear render kernel args so it can reset em on next execution
        kern->clear_args();

        //write info to disk
        aux0->read_to_buffer(queue);
        aux1->read_to_buffer(queue);
        aux2->read_to_buffer(queue);
        aux3->read_to_buffer(queue);
      }
      clFinish(queue);
    }
    opencl_cache->deep_remove_data(scene,*id,boxm2_data_traits<BOXM2_AUX0>::prefix(suffix), true);
    opencl_cache->deep_remove_data(scene,*id,boxm2_data_traits<BOXM2_AUX1>::prefix(suffix), true);
    opencl_cache->deep_remove_data(scene,*id,boxm2_data_traits<BOXM2_AUX2>::prefix(suffix), true);
    opencl_cache->deep_remove_data(scene,*id,boxm2_data_traits<BOXM2_AUX3>::prefix(suffix), true);
  }

  std::cout<<"Gpu time "<<gpu_time<<" transfer time "<<transfer_time<<std::endl;
  opencl_cache->unref_mem(vis_image.ptr());
  opencl_cache->unref_mem(in_image.ptr());
  opencl_cache->unref_mem(ray_d_buff.ptr());
  opencl_cache->unref_mem(ray_o_buff.ptr());

  delete [] vis_buff;
  delete [] input_buff;
  delete [] ray_origins;
  delete [] ray_directions;

  clReleaseCommandQueue(queue);
  return true;
}
