// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_update_with_alt_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for updating the scene when an alternate intensity explaination is available.
//
// \author Daniel Crispell
// \date March 1, 2012

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
#include <vil/vil_save.h>
#include <boxm2/ocl/algo/boxm2_ocl_camera_converter.h>

//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vul/vul_timer.h>
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>

namespace boxm2_ocl_update_with_alt_process_globals
{
  constexpr unsigned n_inputs_ = 11;
  constexpr unsigned n_outputs_ = 0;
  enum {
      UPDATE_SEGLEN = 0,
      UPDATE_PREINF = 1,
      UPDATE_PROC = 2,
      UPDATE_BAYES = 3,
      UPDATE_CELL = 4
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
    src_paths.push_back(source_dir + "bit/update_kernels.cl");
    std::vector<std::string> non_ray_src = std::vector<std::string>(src_paths);
    src_paths.push_back(source_dir + "update_functors.cl");
    src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");

    //compilation options
    std::string options = opts+" -D INTENSITY  ";
    options += " -D DETERMINISTIC ";

    //create all passes
    auto* seg_len = new bocl_kernel();
    std::string seg_opts = options + " -D SEGLEN -D STEP_CELL=step_cell_seglen(aux_args,data_ptr,llid,d) ";
    seg_len->create_kernel(&device->context(),device->device_id(), src_paths, "seg_len_main", seg_opts, "update::seg_len");
    vec_kernels.push_back(seg_len);

    auto* pre_inf = new bocl_kernel();
    std::string pre_opts = options + " -D PREINF -D STEP_CELL=step_cell_preinf(aux_args,data_ptr,llid,d) ";
    pre_inf->create_kernel(&device->context(),device->device_id(), src_paths, "pre_inf_main", pre_opts, "update::pre_inf");
    vec_kernels.push_back(pre_inf);

    //may need DIFF LIST OF SOURCES FOR THIS GUY
    auto* proc_img = new bocl_kernel();
    std::string norm_opts = options + " -D PROC_NORM ";
    proc_img->create_kernel(&device->context(),device->device_id(), non_ray_src, "proc_norm_image", norm_opts, "update::proc_norm_image");
    vec_kernels.push_back(proc_img);

    //push back cast_ray_bit
    auto* bayes_main = new bocl_kernel();
    std::string bayes_opt = options + " -D BAYES -D STEP_CELL=step_cell_bayes(aux_args,data_ptr,llid,d) ";
    bayes_main->create_kernel(&device->context(),device->device_id(), src_paths, "bayes_main", bayes_opt, "update::bayes_main");
    vec_kernels.push_back(bayes_main);

    //may need DIFF LIST OF SOURCES FOR THSI GUY TOO
    auto* update = new bocl_kernel();
    std::string update_opt = options + " -D UPDATE_BIT_SCENE_MAIN ";
    update->create_kernel(&device->context(),device->device_id(), non_ray_src, "update_bit_scene_main", update_opt, "update::update_main");
    vec_kernels.push_back(update);

    return ;
  }

  static std::map<std::string,std::vector<bocl_kernel*> > kernels;
}

bool boxm2_ocl_update_with_alt_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_update_with_alt_process_globals;

  //process takes 11 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "boxm2_scene_sptr";
  input_types_[2] = "boxm2_opencl_cache_sptr";
  input_types_[3] = "vpgl_camera_double_sptr";      //input camera
  input_types_[4] = "vil_image_view_base_sptr";     //input image
  input_types_[5] = "vcl_string";                   //illumination identifier
  input_types_[6] = "vil_image_view_base_sptr";     //mask image view
  input_types_[7] = "bool";                         //do_update_alpha/don't update alpha
  input_types_[8] = "float";                        //variance value? if 0.0 or less, then use variable variance
  input_types_[9] = "vil_image_view_base_sptr";     //alternate explanation prior (per pixel)
  input_types_[10] = "vil_image_view_base_sptr";    //alternate explanation appearance density (per pixel)

  // process has 1 output:
  // output[0]: scene sptr
  std::vector<std::string>  output_types_(n_outputs_);
  bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);

  // default 6 and 7 and 8 inputs
  brdb_value_sptr idx = new brdb_value_t<std::string>("");
  brdb_value_sptr empty_mask = new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<unsigned char>(1,1));
  brdb_value_sptr up_alpha = new brdb_value_t<bool>(true);  //by default update alpha
  brdb_value_sptr def_var = new brdb_value_t<float>(-1.0f);
  pro.set_input(5, idx);
  pro.set_input(6, empty_mask);
  pro.set_input(7, up_alpha);
  pro.set_input(8, def_var);
  return good;
}

bool boxm2_ocl_update_with_alt_process(bprb_func_process& pro)
{
  using namespace boxm2_ocl_update_with_alt_process_globals;
  std::size_t local_threads[2]={8,8};
  std::size_t global_threads[2]={8,8};

  //sanity check inputs
  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  float transfer_time=0.0f;
  float gpu_time=0.0f;

  //get the inputs
  unsigned i = 0;
  bocl_device_sptr         device = pro.get_input<bocl_device_sptr>(i++);
  boxm2_scene_sptr         scene = pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_opencl_cache_sptr  opencl_cache = pro.get_input<boxm2_opencl_cache_sptr>(i++);
  vpgl_camera_double_sptr  cam = pro.get_input<vpgl_camera_double_sptr>(i++);
  vil_image_view_base_sptr img = pro.get_input<vil_image_view_base_sptr>(i++);
  std::string               ident = pro.get_input<std::string>(i++);
  vil_image_view_base_sptr mask_sptr = pro.get_input<vil_image_view_base_sptr>(i++);
  bool                     update_alpha = pro.get_input<bool>(i++);
  auto                    mog_var = pro.get_input<float>(i++);
  vil_image_view_base_sptr alt_prior_base = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr alt_density_base = pro.get_input<vil_image_view_base_sptr>(i++);

  // assume alt_prior and alt_density are floating point images
  auto *alt_prior = dynamic_cast<vil_image_view<float>*>(alt_prior_base.ptr());
  if (!alt_prior) {
     std::cerr << "ERROR casting alt_prior image to floating point" << std::endl;
     return false;
  }
  if ((alt_prior->ni() != img->ni()) || (alt_prior->nj() != img->nj())) {
     std::cerr << "ERROR: alt_prior image size does not match input image" << std::endl;
     return false;
  }
  auto *alt_density = dynamic_cast<vil_image_view<float>*>(alt_density_base.ptr());
  if (!alt_density) {
     std::cerr << "ERROR casting alt_density image to floating point" << std::endl;
     return false;
  }
  if ((alt_density->ni() != img->ni()) || (alt_density->nj() != img->nj())) {
     std::cerr << "ERROR: alt_density image size does not match input image" << std::endl;
     return false;
  }
  vil_image_view<float> model_prob(img->ni(), img->nj());
  const float uniform_density = 1.0f;
  for (unsigned int j=0; j<img->nj(); ++j) {
     for (unsigned int i=0; i<img->ni(); ++i) {
        float model_lik = (1.0f - (*alt_prior)(i,j)) * uniform_density;
        float alt_lik = (*alt_prior)(i,j) * (*alt_density)(i,j);
        model_prob(i,j) = model_lik / (model_lik + alt_lik);
     }
  }


  //catch a "null" mask (not really null because that throws an error)
  bool use_mask = false;
  if ( mask_sptr->ni() == img->ni() && mask_sptr->nj() == img->nj() ) {
    std::cout<<"Update using mask."<<std::endl;
    use_mask = true;
  }
  vil_image_view<unsigned char>* mask_map = nullptr;
  if (use_mask) {
    mask_map = dynamic_cast<vil_image_view<unsigned char> *>(mask_sptr.ptr());
    if (!mask_map) {
      std::cout<<"boxm2_update_process:: mask map is not an unsigned char map"<<std::endl;
      return false;
    }
  }

  //cache size sanity check
  std::size_t binCache = opencl_cache.ptr()->bytes_in_cache();
  std::cout<<"Update MBs in cache: "<<binCache/(1024.0*1024.0)<<std::endl;

  //make correct data types are here
  bool foundDataType = false, foundNumObsType = false;
  std::string data_type,num_obs_type,options;
  std::vector<std::string> apps = scene->appearances();
  int appTypeSize;
  for (const auto & app : apps) {
    if ( app == boxm2_data_traits<BOXM2_MOG3_GREY>::prefix() )
    {
      data_type = app;
      foundDataType = true;
      options=" -D MOG_TYPE_8 ";
      appTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_MOG3_GREY>::prefix());
    }
    else if ( app == boxm2_data_traits<BOXM2_MOG3_GREY_16>::prefix() )
    {
      data_type = app;
      foundDataType = true;
      options=" -D MOG_TYPE_16 ";
      appTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_MOG3_GREY_16>::prefix());
    }
    else if ( app == boxm2_data_traits<BOXM2_NUM_OBS>::prefix() )
    {
      num_obs_type = app;
      foundNumObsType = true;
    }
  }
  if (!foundDataType) {
    std::cout<<"BOXM2_OPENCL_UPDATE_PROCESS ERROR: scene doesn't have BOXM2_MOG3_GREY or BOXM2_MOG3_GREY_16 data type"<<std::endl;
    return false;
  }
  if (!foundNumObsType) {
    std::cout<<"BOXM2_OPENCL_UPDATE_PROCESS ERROR: scene doesn't have BOXM2_NUM_OBS type"<<std::endl;
    return false;
  }
  if (ident.size() > 0) {
  data_type += "_" + ident;
    num_obs_type += "_" + ident;
  }

  // create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue( device->context(),
                                                 *(device->device_id()),
                                                 CL_QUEUE_PROFILING_ENABLE,
                                                 &status);
  if (status!=0)
    return false;

  // compile the kernel if not already compiled
  std::string identifier=device->device_identifier()+options;
  if (kernels.find(identifier)==kernels.end()) {
    std::cout<<"===========Compiling kernels==========="<<std::endl;
    std::vector<bocl_kernel*> ks;
    compile_kernel(device,ks,options);
    kernels[identifier]=ks;
  }

  //grab input image, establish cl_ni, cl_nj (so global size is divisible by local size)
  vil_image_view_base_sptr float_img = boxm2_util::prepare_input_image(img, true);
  auto* img_view = static_cast<vil_image_view<float>* >(float_img.ptr());
  auto cl_ni=(unsigned)RoundUp(img_view->ni(),(int)local_threads[0]);
  auto cl_nj=(unsigned)RoundUp(img_view->nj(),(int)local_threads[1]);
  global_threads[0]=cl_ni;
  global_threads[1]=cl_nj;

  //set generic cam
  auto* ray_origins = new cl_float[4*cl_ni*cl_nj];
  auto* ray_directions = new cl_float[4*cl_ni*cl_nj];
  //bocl_mem_sptr ray_o_buff = new bocl_mem(device->context(), ray_origins,   cl_ni*cl_nj * sizeof(cl_float4), "ray_origins buffer");
  //bocl_mem_sptr ray_d_buff = new bocl_mem(device->context(), ray_directions,cl_ni*cl_nj * sizeof(cl_float4), "ray_directions buffer");
  bocl_mem_sptr ray_o_buff = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_origins, "ray_origins buffer");
  bocl_mem_sptr ray_d_buff = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_directions, "ray_directions buffer");
  boxm2_ocl_camera_converter::compute_ray_image( device, queue, cam, cl_ni, cl_nj, ray_o_buff, ray_d_buff);

  //Visibility, Preinf, Norm, and input image buffers
  auto* vis_buff = new float[cl_ni*cl_nj];
  auto* pre_buff = new float[cl_ni*cl_nj];
  auto* norm_buff = new float[cl_ni*cl_nj];
  auto* input_buff=new float[cl_ni*cl_nj];
  unsigned int idx = 0;
  for (unsigned j=0; j<cl_nj; ++j) {
    for (unsigned i=0; i<cl_ni; ++i) {
      if ((i < img->ni()) && (j < img->nj())){
        vis_buff[idx] = model_prob(i,j);
        pre_buff[idx] = (1.0f - model_prob(i,j))*(*alt_density)(i,j);
        norm_buff[idx] = 0.0f;
       }
       else {
         vis_buff[idx] = 0;
         pre_buff[idx] = 0;
         norm_buff[idx] = 0;
       }
       ++idx;
    }
  }

  //determine min/max i and j
  unsigned int min_i=1000000000, max_i=0;
  unsigned int min_j=1000000000, max_j=0;
  if (use_mask)
  {
    for (unsigned int j=0;j<mask_map->nj();++j) {
      for (unsigned int i=0;i<mask_map->ni();++i)
      {
        if ( (*mask_map)(i,j)==0 )
        {
          if (min_i > i) min_i = i;
          if (min_j > j) min_j = j;
          if (max_i < i) max_i = i;
          if (max_j < j) max_j = j;
        }
      }
    }
  }

  //copy input vals into image
  int count=0;
  for (unsigned int j=0;j<cl_nj;++j) {
    for (unsigned int i=0;i<cl_ni;++i) {
      input_buff[count] = 0.0f;
      if ( i<img_view->ni() && j< img_view->nj() )
        input_buff[count] = (*img_view)(i,j);
      ++count;
    }
  }

  //bocl_mem_sptr in_image=new bocl_mem(device->context(),input_buff,cl_ni*cl_nj*sizeof(float),"input image buffer");
  bocl_mem_sptr in_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), input_buff, "input image buffer");
  in_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  //bocl_mem_sptr vis_image=new bocl_mem(device->context(),vis_buff,cl_ni*cl_nj*sizeof(float),"vis image buffer");
  bocl_mem_sptr vis_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), vis_buff, "vis image buffer");
  vis_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  //bocl_mem_sptr pre_image=new bocl_mem(device->context(),pre_buff,cl_ni*cl_nj*sizeof(float),"pre image buffer");
  bocl_mem_sptr pre_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), pre_buff, "pre image buffer");
  pre_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  //bocl_mem_sptr norm_image=new bocl_mem(device->context(),norm_buff,cl_ni*cl_nj*sizeof(float),"pre image buffer");
  bocl_mem_sptr norm_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), norm_buff, "norm image buffer");
  norm_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // Image Dimensions
  int img_dim_buff[4];
  img_dim_buff[0] = 0;
  img_dim_buff[1] = 0;
  img_dim_buff[2] = img_view->ni();
  img_dim_buff[3] = img_view->nj();

#if 0
  if (min_i<max_i && min_j<max_j)
  {
    img_dim_buff[0] = min_i;
    img_dim_buff[1] = min_j;
    img_dim_buff[2] = max_i;
    img_dim_buff[3] = max_j;
    std::cout<<"ROI "<<min_i<<' '<<min_j<<' '<<max_i<<' '<<max_j<<std::endl;
  }
#endif // 0

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

  // app density used for proc_norm_image
  float app_buffer[4]={1.0,0.0,0.0,0.0};
  bocl_mem_sptr app_density = new bocl_mem(device->context(), app_buffer, sizeof(cl_float4), "app density buffer");
  app_density->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // set arguments
  std::vector<boxm2_block_id> vis_order = scene->get_vis_blocks(cam);
  std::vector<boxm2_block_id>::iterator id;
  for (unsigned int i=0; i<kernels[identifier].size(); ++i)
  {
    if ( i == UPDATE_PROC ) {
      bocl_kernel * proc_kern=kernels[identifier][i];

      proc_kern->set_arg( norm_image.ptr() );
      proc_kern->set_arg( vis_image.ptr() );
      proc_kern->set_arg( pre_image.ptr());
      proc_kern->set_arg( img_dim.ptr() );

      //execute kernel
      proc_kern->execute( queue, 2, local_threads, global_threads);
      int status = clFinish(queue);
      if (!check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status)))
        return false;
      proc_kern->clear_args();
      norm_image->read_to_buffer(queue);

      // reset vis and pre buffers
      vis_image->read_to_buffer(queue);
      pre_image->read_to_buffer(queue);

      unsigned int idx = 0;
      for (unsigned j=0; j<cl_nj; ++j) {
        for (unsigned i=0; i<cl_ni; ++i) {
          if ((i < img->ni()) && (j < img->nj())){
            vis_buff[idx] = model_prob(i,j);
            pre_buff[idx] = (1.0f - model_prob(i,j))*(*alt_density)(i,j);
          }
          else {
            vis_buff[idx] = 0;
            pre_buff[idx] = 0;
          }
          ++idx;
        }
      }
      vis_image->write_to_buffer(queue);
      pre_image->write_to_buffer(queue);
      clFinish(queue);

      continue;
    }

    //set masked values
    vis_image->read_to_buffer(queue);
    if (use_mask)
    {
      int count = 0;
      for (unsigned int j=0;j<cl_nj;++j) {
        for (unsigned int i=0;i<cl_ni;++i) {
          if ( i<mask_map->ni() && j<mask_map->nj() ) {
            if ( (*mask_map)(i,j)>0 ) {
              input_buff[count] = -1.0f;
              vis_buff  [count] = -1.0f;
            }
          }
          ++count;
        }
      }
      in_image->write_to_buffer(queue);
      vis_image->write_to_buffer(queue);
      clFinish(queue);
    }

    for (id = vis_order.begin(); id != vis_order.end(); ++id)
    {
      //choose correct render kernel
      boxm2_block_metadata mdata = scene->get_block_metadata(*id);
      bocl_kernel* kern =  kernels[identifier][i];

      //write the image values to the buffer
      vul_timer transfer;
      bocl_mem* blk = opencl_cache->get_block(scene,*id);
      bocl_mem* blk_info = opencl_cache->loaded_block_info();
      bocl_mem* alpha = opencl_cache->get_data<BOXM2_ALPHA>(scene,*id,0,false);
      auto* info_buffer = (boxm2_scene_info*) blk_info->cpu_buffer();
      int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
      info_buffer->data_buffer_length = (int) (alpha->num_bytes()/alphaTypeSize);
      blk_info->write_to_buffer((queue));

      int nobsTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_NUM_OBS>::prefix());
      // data type string may contain an identifier so determine the buffer size
      bocl_mem* mog = opencl_cache->get_data(scene,*id,data_type,alpha->num_bytes()/alphaTypeSize*appTypeSize,false);    //info_buffer->data_buffer_length*boxm2_data_info::datasize(data_type));
      bocl_mem* num_obs = opencl_cache->get_data(scene,*id,num_obs_type,alpha->num_bytes()/alphaTypeSize*nobsTypeSize,false);//,info_buffer->data_buffer_length*boxm2_data_info::datasize(num_obs_type));

      //grab an appropriately sized AUX data buffer
      int auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX0>::prefix());
      bocl_mem *aux0 = opencl_cache->get_data<BOXM2_AUX0>(scene,*id, info_buffer->data_buffer_length*auxTypeSize);
      auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX1>::prefix());
      bocl_mem *aux1 = opencl_cache->get_data<BOXM2_AUX1>(scene,*id, info_buffer->data_buffer_length*auxTypeSize);

      transfer_time += (float) transfer.all();
      if (i==UPDATE_SEGLEN)
      {
        aux0->zero_gpu_buffer(queue);
        aux1->zero_gpu_buffer(queue);
        kern->set_arg( blk_info );
        kern->set_arg( blk );
        kern->set_arg( alpha );
        kern->set_arg( aux0 );
        kern->set_arg( aux1 );
        kern->set_arg( lookup.ptr() );

        // kern->set_arg( persp_cam.ptr() );
        kern->set_arg( ray_o_buff.ptr() );
        kern->set_arg( ray_d_buff.ptr() );

        kern->set_arg( img_dim.ptr() );
        kern->set_arg( in_image.ptr() );
        kern->set_arg( cl_output.ptr() );
        kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_uchar16) );//local tree,
        kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_uchar4) ); //ray bundle,
        kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_int) );    //cell pointers,
        kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_float4) ); //cached aux,
        kern->set_local_arg( local_threads[0]*local_threads[1]*10*sizeof(cl_uchar) ); //cumsum buffer, imindex buffer

        //execute kernel
        kern->execute(queue, 2, local_threads, global_threads);
        int status = clFinish(queue);
        check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status));
        gpu_time += kern->exec_time();

        //clear render kernel args so it can reset em on next execution
        kern->clear_args();

        aux0->read_to_buffer(queue);
        aux1->read_to_buffer(queue);
      }
      else if (i==UPDATE_PREINF)
      {
        kern->set_arg( blk_info );
        kern->set_arg( blk );
        kern->set_arg( alpha );
        kern->set_arg( mog );
        kern->set_arg( num_obs );
        kern->set_arg( aux0 );
        kern->set_arg( aux1 );
        kern->set_arg( lookup.ptr() );
        kern->set_arg( ray_o_buff.ptr() );
        kern->set_arg( ray_d_buff.ptr() );

        kern->set_arg( img_dim.ptr() );
        kern->set_arg( vis_image.ptr() );
        kern->set_arg( pre_image.ptr() );
        kern->set_arg( cl_output.ptr() );
        kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_uchar16) );//local tree,
        kern->set_local_arg( local_threads[0]*local_threads[1]*10*sizeof(cl_uchar) ); //cumsum buffer, imindex buffer
        //execute kernel
        kern->execute(queue, 2, local_threads, global_threads);
        int status = clFinish(queue);
        check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status));
        gpu_time += kern->exec_time();

        //clear render kernel args so it can reset em on next execution
        kern->clear_args();

        //write info to disk
      }
      else if (i==UPDATE_BAYES)
      {
        auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX2>::prefix());
        bocl_mem *aux2 = opencl_cache->get_data<BOXM2_AUX2>(scene,*id, info_buffer->data_buffer_length*auxTypeSize);
        aux2->zero_gpu_buffer(queue);
        auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX3>::prefix());
        bocl_mem *aux3 = opencl_cache->get_data<BOXM2_AUX3>(scene,*id, info_buffer->data_buffer_length*auxTypeSize);
        aux3->zero_gpu_buffer(queue);

        kern->set_arg( blk_info );
        kern->set_arg( blk );
        kern->set_arg( alpha );
        kern->set_arg( mog );
        kern->set_arg( num_obs );
        kern->set_arg( aux0 );
        kern->set_arg( aux1 );
        kern->set_arg( aux2 );
        kern->set_arg( aux3 );
        kern->set_arg( lookup.ptr() );
        kern->set_arg( ray_o_buff.ptr() );
        kern->set_arg( ray_d_buff.ptr() );

        kern->set_arg( img_dim.ptr() );
        kern->set_arg( vis_image.ptr() );
        kern->set_arg( pre_image.ptr() );
        kern->set_arg( norm_image.ptr() );
        kern->set_arg( cl_output.ptr() );
        kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_uchar16) );//local tree,
        kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_short2) ); //ray bundle,
        kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_int) );    //cell pointers,
        kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_float) ); //cached aux,
        kern->set_local_arg( local_threads[0]*local_threads[1]*10*sizeof(cl_uchar) ); //cumsum buffer, imindex buffer
                //execute kernel
        kern->execute(queue, 2, local_threads, global_threads);
        int status = clFinish(queue);
        check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status));
        gpu_time += kern->exec_time();

        //clear render kernel args so it can reset em on next execution
        kern->clear_args();
        aux2->read_to_buffer(queue);
        aux3->read_to_buffer(queue);
      }
      else if (i==UPDATE_CELL)
      {
        auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX2>::prefix());
        bocl_mem *aux2 = opencl_cache->get_data<BOXM2_AUX2>(scene,*id, info_buffer->data_buffer_length*auxTypeSize);

        auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX3>::prefix());
        bocl_mem *aux3 = opencl_cache->get_data<BOXM2_AUX3>(scene,*id, info_buffer->data_buffer_length*auxTypeSize);

        // update_alpha boolean buffer
        cl_int up_alpha[1];
        up_alpha[0] = update_alpha ? 1 : 0;
        bocl_mem_sptr up_alpha_mem = new bocl_mem(device->context(), up_alpha, sizeof(up_alpha), "update alpha bool buffer");
        up_alpha_mem->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

        //mog variance, if 0.0f or less, then var will be learned
        bocl_mem_sptr mog_var_mem = new bocl_mem(device->context(), &mog_var, sizeof(mog_var), "update gauss variance");
        mog_var_mem->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

        local_threads[0] = 64;
        local_threads[1] = 1 ;
        global_threads[0]=RoundUp(info_buffer->data_buffer_length,local_threads[0]);
        global_threads[1]=1;

        kern->set_arg( blk_info );
        kern->set_arg( alpha );
        kern->set_arg( mog );
        kern->set_arg( num_obs );
        kern->set_arg( aux0 );
        kern->set_arg( aux1 );
        kern->set_arg( aux2 );
        kern->set_arg( aux3 );
        kern->set_arg( up_alpha_mem.ptr() );
        kern->set_arg( mog_var_mem.ptr() );
        kern->set_arg( cl_output.ptr() );

        //execute kernel
        kern->execute(queue, 2, local_threads, global_threads);
        int status = clFinish(queue);
        check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status));
        gpu_time += kern->exec_time();

        //clear render kernel args so it can reset em on next execution
        kern->clear_args();

        //write info to disk
        alpha->read_to_buffer(queue);
        mog->read_to_buffer(queue);
        num_obs->read_to_buffer(queue);
      }

      //read image out to buffer (from gpu)
      in_image->read_to_buffer(queue);
      vis_image->read_to_buffer(queue);
      pre_image->read_to_buffer(queue);
      cl_output->read_to_buffer(queue);
      clFinish(queue);
    }
  }

  ///debugging save vis, pre, norm images
#if 0
  int idx = 0;
  vil_image_view<float> vis_view(cl_ni,cl_nj);
  vil_image_view<float> norm_view(cl_ni,cl_nj);
  vil_image_view<float> pre_view(cl_ni,cl_nj);
  for (unsigned c=0;c<cl_nj;++c) {
    for (unsigned r=0;r<cl_ni;++r) {
      vis_view(r,c) = vis_buff[idx];
      norm_view(r,c) = norm_buff[idx];
      pre_view(r,c) = pre_buff[idx];
      idx++;
    }
  }
  vil_save( vis_view, "vis_debug.tiff");
  vil_save( norm_view, "norm_debug.tiff");
  vil_save( pre_view, "pre_debug.tiff");
#endif

  delete [] vis_buff;
  delete [] pre_buff;
  delete [] norm_buff;
  delete [] input_buff;
  delete [] ray_origins;
  delete [] ray_directions;
  opencl_cache->unref_mem(in_image.ptr());
  opencl_cache->unref_mem(vis_image.ptr());
  opencl_cache->unref_mem(pre_image.ptr());
  opencl_cache->unref_mem(norm_image.ptr());
  opencl_cache->unref_mem(ray_o_buff.ptr());
  opencl_cache->unref_mem(ray_d_buff.ptr());

  std::cout<<"Gpu time "<<gpu_time<<" transfer time "<<transfer_time<<std::endl;
  clReleaseCommandQueue(queue);
  return true;
}
