// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_update_alpha_naa_process.cxx
//:
// \file
// \brief  A process for updating alpha, based on pre-calculated normal_albedo_array appearance model
//
// \author Daniel Crispell
// \date 16 Dec 2011

#include <bprb/bprb_func_process.h>

#include <vcl_fstream.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boxm2/boxm2_util.h>
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>
#include <boxm2/ocl/algo/boxm2_ocl_camera_converter.h>

#include <brad/brad_illum_util.h>
#include <brad/brad_image_metadata.h>
#include <brad/brad_atmospheric_parameters.h>

//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vul/vul_timer.h>
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>

namespace boxm2_ocl_update_alpha_naa_process_globals
{
  const unsigned n_inputs_  = 9;
  const unsigned n_outputs_ = 0;
  enum {
      UPDATE_SEGLEN = 0,
      UPDATE_PREINF = 1,
      UPDATE_PROC   = 2,
      UPDATE_BAYES  = 3,
      UPDATE_CELL   = 4
  };

  bool compile_kernel(bocl_device_sptr device,vcl_vector<bocl_kernel*> & vec_kernels,vcl_string opts)
  {
    //gather all render sources... seems like a lot for rendering...
    vcl_vector<vcl_string> src_paths;
    vcl_string source_dir = boxm2_ocl_util::ocl_src_root();
    src_paths.push_back(source_dir + "scene_info.cl");
    src_paths.push_back(source_dir + "cell_utils.cl");
    src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
    src_paths.push_back(source_dir + "backproject.cl");
    src_paths.push_back(source_dir + "statistics_library_functions.cl");
    src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
    src_paths.push_back(source_dir + "bit/update_kernels.cl");
    src_paths.push_back(source_dir + "bit/update_naa_kernels.cl");
    vcl_vector<vcl_string> non_ray_src = vcl_vector<vcl_string>(src_paths);
    src_paths.push_back(source_dir + "update_functors.cl");
    src_paths.push_back(source_dir + "update_naa_functors.cl");
    src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");

    //compilation options
    vcl_string options = opts + " -D INTENSITY  ";
    options += " -D DETERMINISTIC ";

    //create all passes
    bocl_kernel* seg_len = new bocl_kernel();
    vcl_string seg_opts = options + "-D SEGLEN -D STEP_CELL=step_cell_seglen(aux_args,data_ptr,llid,d) ";
    if (!seg_len->create_kernel(&device->context(),device->device_id(), src_paths, "seg_len_main", seg_opts, "update::seg_len")) {
      vcl_cerr << "ERROR compiling kernel\n";
      return false;
    }
    vec_kernels.push_back(seg_len);

    bocl_kernel* pre_inf = new bocl_kernel();
    vcl_string pre_opts = options + " -D PREINF_NAA -D STEP_CELL=step_cell_preinf_naa(aux_args,data_ptr,llid,d) ";
    if (!pre_inf->create_kernel(&device->context(),device->device_id(), src_paths, "pre_inf_naa_main", pre_opts, "update::pre_inf_naa")) {
      vcl_cerr << "ERROR compiling kernel\n";
      return false;
    }
    vec_kernels.push_back(pre_inf);


    bocl_kernel* proc_img = new bocl_kernel();
    vcl_string proc_opt = options + " -D PROC_NORM_NAA ";
    if (!proc_img->create_kernel(&device->context(),device->device_id(), non_ray_src, "proc_norm_image", proc_opt, "update::proc_norm_image")) {
      vcl_cerr << "ERROR compling kernel\n";
      return false;
    }
    vec_kernels.push_back(proc_img);


    bocl_kernel* bayes_main = new bocl_kernel();
    vcl_string bayes_opt = options + " -D BAYES_NAA -D STEP_CELL=step_cell_bayes_naa(aux_args,data_ptr,llid,d) ";
    if (!bayes_main->create_kernel(&device->context(),device->device_id(), src_paths, "bayes_main_naa", bayes_opt, "update::bayes_naa_main")) {
      vcl_cerr << "ERROR compiling kernel\n";
      return false;
    }
    vec_kernels.push_back(bayes_main);

    bocl_kernel* update = new bocl_kernel();
    vcl_string update_opt = options + " -D UPDATE_ALPHA_ONLY ";
    if (!update->create_kernel(&device->context(),device->device_id(), non_ray_src, "update_bit_scene_main_alpha_only", update_opt, "update::update_main_alpha_only")) {
      vcl_cerr << "ERROR compiling kernel\n";
      return false;
    }
    vec_kernels.push_back(update);

    return true;
  }

  static vcl_map<vcl_string,vcl_vector<bocl_kernel*> > kernels;
}

bool boxm2_ocl_update_alpha_naa_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_update_alpha_naa_process_globals;

  //process takes 9 inputs
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "boxm2_scene_sptr";
  input_types_[2] = "boxm2_opencl_cache_sptr";
  input_types_[3] = "vpgl_camera_double_sptr";      //input camera
  input_types_[4] = "vil_image_view_base_sptr";     //input image
  input_types_[5] = "brad_image_metadata_sptr";     // image metadata
  input_types_[6] = "brad_atmospheric_parameters_sptr";  // atmospheric parameters
  input_types_[7] = "vil_image_view_base_sptr";     // alternate explaination prior
  input_types_[8] = "vil_image_view_base_sptr";     // alternate explaination density

  // process has 1 output:
  // output[0]: scene sptr
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);

  return good;
}

bool boxm2_ocl_update_alpha_naa_process(bprb_func_process& pro)
{
   // variances
  const double reflectance_var = boxm2_normal_albedo_array_constants::sigma_albedo * boxm2_normal_albedo_array_constants::sigma_albedo;
  const double airlight_var = boxm2_normal_albedo_array_constants::sigma_airlight * boxm2_normal_albedo_array_constants::sigma_airlight;
  const double optical_depth_var = boxm2_normal_albedo_array_constants::sigma_optical_depth * boxm2_normal_albedo_array_constants::sigma_optical_depth;
  const double skylight_var = boxm2_normal_albedo_array_constants::sigma_skylight * boxm2_normal_albedo_array_constants::sigma_skylight;

  using namespace boxm2_ocl_update_alpha_naa_process_globals;
  vcl_size_t local_threads[2]={8,8};
  vcl_size_t global_threads[2]={8,8};

  //sanity check inputs
  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  float transfer_time=0.0f;
  float gpu_time=0.0f;

  //get the inputs
  bocl_device_sptr         device       = pro.get_input<bocl_device_sptr>(0);
  boxm2_scene_sptr         scene        = pro.get_input<boxm2_scene_sptr>(1);
  boxm2_opencl_cache_sptr  opencl_cache = pro.get_input<boxm2_opencl_cache_sptr>(2);
  vpgl_camera_double_sptr  cam          = pro.get_input<vpgl_camera_double_sptr>(3);
  vil_image_view_base_sptr img          = pro.get_input<vil_image_view_base_sptr>(4);
  brad_image_metadata_sptr metadata     = pro.get_input<brad_image_metadata_sptr>(5);
  brad_atmospheric_parameters_sptr atm_params = pro.get_input<brad_atmospheric_parameters_sptr>(6);
  vil_image_view_base_sptr alt_prior_base    = pro.get_input<vil_image_view_base_sptr>(7);
  vil_image_view_base_sptr alt_density_base   = pro.get_input<vil_image_view_base_sptr>(8);


  if ( alt_prior_base->ni() != img->ni() || alt_prior_base->nj() != img->nj() ) {
    vcl_cerr << "ERROR: alt_prior and image sizes do not match! exiting without update.\n";
    return false;
  }
  if ( alt_density_base->ni() != img->ni() || alt_density_base->nj() != img->nj() ) {
    vcl_cerr << "ERROR: alt_density and image sizes do not match! exiting without update.\n";
    return false;
  }

  vil_image_view<float> *alt_prior = dynamic_cast<vil_image_view<float>*>(alt_prior_base.ptr());
  if (!alt_prior) {
    vcl_cerr << "ERROR casting alt_prior to vil_image_view<float>\n";
    return false;
  }
  vil_image_view<float> *alt_density = dynamic_cast<vil_image_view<float>*>(alt_density_base.ptr());
  if (!alt_density) {
    vcl_cerr << "ERROR casting alt_density to vil_image_view<float>\n";
    return false;
  }

  // get normal directions
  vcl_vector<vgl_vector_3d<double> > normals = boxm2_normal_albedo_array::get_normals();
  unsigned int num_normals = normals.size();
  // opencl code depends on there being exactly 16 normal directions - do sanity check here
  if (num_normals != 16) {
    vcl_cerr << "ERROR: boxm2_ocl_update_alpha_naa_process: num_normals = " << num_normals << ".  Expected 16\n";
    return false;
  }

   double deg2rad = vnl_math::pi_over_180;
   double sun_az = metadata->sun_azimuth_ * deg2rad;
   double sun_el = metadata->sun_elevation_ * deg2rad;
   vgl_vector_3d<double> sun_dir(vcl_sin(sun_az)*vcl_cos(sun_el),
                                 vcl_cos(sun_az)*vcl_cos(sun_el),
                                 vcl_sin(sun_el));

  // buffers for holding radiance scales per normal
  float* radiance_scales_buff = new float[num_normals];
  float* radiance_offsets_buff = new float[num_normals];
  float* radiance_var_scales_buff = new float[num_normals];
  float* radiance_var_offsets_buff = new float[num_normals];

  // compute offsets and scales for linear radiance model
  for (unsigned n=0; n < num_normals; ++n) {
     // compute offsets as radiance of surface with 0 reflectance
     double offset = brad_expected_radiance_chavez(0.0, normals[n], *metadata, *atm_params);
     radiance_offsets_buff[n] = offset;
     // use perfect reflector to compute radiance scale
     double radiance = brad_expected_radiance_chavez(1.0, normals[n], *metadata, *atm_params);
     radiance_scales_buff[n] = radiance - offset;
     // compute offset of radiance variance
     double var_offset = brad_radiance_variance_chavez(0.0, normals[n], *metadata, *atm_params, reflectance_var, optical_depth_var, skylight_var, airlight_var);
     radiance_var_offsets_buff[n] = var_offset;
     // compute scale
     double var = brad_radiance_variance_chavez(1.0, normals[n], *metadata, *atm_params, reflectance_var, optical_depth_var, skylight_var, airlight_var);
     radiance_var_scales_buff[n] = var - var_offset;
     vcl_cout << "---- normal = " << normals[n] << '\n'
              << "radiance scale = " << radiance << " offset = " << offset << '\n'
              << "radiance var scale = " << var << " variance offset = " << var_offset << vcl_endl;
  }

  //cache size sanity check
  long binCache = opencl_cache.ptr()->bytes_in_cache();
  vcl_cout<<"Update MBs in cache: "<<binCache/(1024.0*1024.0)<<vcl_endl;

  //make sure correct data types are here
  bool found_appearance = false, found_num_obs = false;
  vcl_string data_type, num_obs_type, options;
  vcl_vector<vcl_string> apps = scene->appearances();
  int appTypeSize;
  for (unsigned int i=0; i<apps.size(); ++i) {
    if ( apps[i] == boxm2_data_traits<BOXM2_NORMAL_ALBEDO_ARRAY>::prefix() )
    {
      data_type = apps[i];
      found_appearance = true;
      options=" -D NAA_APPEARANCE ";
      appTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_NORMAL_ALBEDO_ARRAY>::prefix());
    }
    else if ( apps[i] == boxm2_data_traits<BOXM2_NUM_OBS>::prefix() )
    {
      num_obs_type = apps[i];
      found_num_obs = true;
    }
  }
  if (!found_appearance) {
    vcl_cout<<"BOXM2_OPENCL_UPDATE_ALPHA_NAA_PROCESS ERROR: scene doesn't have BOXM2_NORMAL_ALBEDO_ARRAY data type"<<vcl_endl;
    return false;
  }
  if (!found_num_obs) {
    vcl_cout<<"BOXM2_OPENCL_UPDATE_ALPHA_NAA_PROCESS ERROR: scene doesn't have BOXM2_NUM_OBS type"<<vcl_endl;
    return false;
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
  vcl_string identifier=device->device_identifier()+options;
  if (kernels.find(identifier)==kernels.end()) {
    vcl_cout << "===========Compiling kernels===========" << vcl_endl;
    vcl_vector<bocl_kernel*> ks;
    if (!compile_kernel(device,ks,options)) {
      vcl_cerr << "ERROR compiling kernels\n";
      return false;
    }
    kernels[identifier]=ks;
  }

  //grab input image, establish cl_ni, cl_nj (so global size is divisible by local size)
  vil_image_view_base_sptr float_img = boxm2_util::prepare_input_image(img, true);
  vil_image_view<float>* img_view = static_cast<vil_image_view<float>* >(float_img.ptr());
  unsigned cl_ni=(unsigned)RoundUp(img_view->ni(),(int)local_threads[0]);
  unsigned cl_nj=(unsigned)RoundUp(img_view->nj(),(int)local_threads[1]);
  global_threads[0]=cl_ni;
  global_threads[1]=cl_nj;

  //set generic cam
  cl_float* ray_origins    = new cl_float[4*cl_ni*cl_nj];
  cl_float* ray_directions = new cl_float[4*cl_ni*cl_nj];
  bocl_mem_sptr ray_o_buff = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_origins,"ray_origins buffer");
  bocl_mem_sptr ray_d_buff = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_directions,"ray_directions buffer");
  boxm2_ocl_camera_converter::compute_ray_image( device, queue, cam, cl_ni, cl_nj, ray_o_buff, ray_d_buff);

  // Visibility, Preinf, Norm, and input image buffers
  float* vis_buff = new float[cl_ni*cl_nj];
  float* pre_buff = new float[cl_ni*cl_nj];
  float* norm_buff = new float[cl_ni*cl_nj];
  float* input_buff=new float[cl_ni*cl_nj];
  {
    int count = 0;
    for (unsigned int j=0; j<cl_nj; ++j) {
      for (unsigned int i=0; i<cl_ni; ++i) {
        if ((i < img->ni()) && (j < img->nj())) {
          vis_buff[count] = 1.0f - (*alt_prior)(i,j);
          pre_buff[count] = (*alt_prior)(i,j) * (*alt_density)(i,j);
          norm_buff[count] = 0.0f;
        }
        else {
          vis_buff[count] = 1.0f;
          pre_buff[count] = 0.0f;
          norm_buff[count] = 0.0f;
        }
        ++count;
      }
    }
  }

  // copy input vals into image
  int count = 0;
  for (unsigned int j=0; j<cl_nj; ++j) {
    for (unsigned int i=0; i<cl_ni; ++i) {
      input_buff[count] = 0.0f;
      if ( i<img_view->ni() && j< img_view->nj() )
        input_buff[count] = (*img_view)(i,j);
      ++count;
    }
  }

  bocl_mem_sptr in_image=opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float),input_buff,"input image buffer");
  in_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  bocl_mem_sptr vis_image=opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float),vis_buff,"vis image buffer");
  vis_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  bocl_mem_sptr pre_image=opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float),pre_buff,"pre image buffer");
  pre_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  bocl_mem_sptr norm_image=opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float),norm_buff,"norm image buffer");
  norm_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  bocl_mem_sptr radiance_scales = new bocl_mem(device->context(), radiance_scales_buff, sizeof(float)*num_normals,"radiance scales buffer");
  radiance_scales->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  bocl_mem_sptr radiance_offsets = new bocl_mem(device->context(), radiance_offsets_buff, sizeof(float)*num_normals,"radiance offsets buffer");
  radiance_offsets->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  bocl_mem_sptr radiance_var_scales = new bocl_mem(device->context(), radiance_var_scales_buff, sizeof(float)*num_normals,"radiance variance scales buffer");
  radiance_var_scales->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  bocl_mem_sptr radiance_var_offsets = new bocl_mem(device->context(), radiance_var_offsets_buff, sizeof(float)*num_normals,"radiance variance offsets buffer");
  radiance_var_offsets->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

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
    vcl_cout<<"ROI "<<min_i<<' '<<min_j<<' '<<max_i<<' '<<max_j<<vcl_endl;
  }
#endif // 0

  bocl_mem_sptr img_dim = new bocl_mem(device->context(), img_dim_buff, sizeof(int)*4, "image dims");
  img_dim->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

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

  // app density used for proc_norm_image
  float background_density_buffer[1];
  // compute expected value of background density over normal directions
  double expected_dL_drho = 0.0;
  for (unsigned int n=0; n<num_normals; ++n) {
     expected_dL_drho += radiance_scales_buff[n] / num_normals;
  }
  *background_density_buffer = 1.0 / expected_dL_drho;

  bocl_mem_sptr background_density = new bocl_mem(device->context(), background_density_buffer, sizeof(cl_float), "background appearance density buffer");
  background_density->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  //vcl_cout << "######  Entering stage loop: bytes in cache = " << opencl_cache->bytes_in_cache() << vcl_endl;

  // set arguments
  vcl_vector<boxm2_block_id> vis_order = scene->get_vis_blocks(cam);
  vcl_vector<boxm2_block_id>::iterator id;

  for (unsigned int i=0; i<kernels[identifier].size(); ++i)
  {
    vcl_cout << "kernel i = " << i << vcl_endl;

    if ( i == UPDATE_PROC ) {
      bocl_kernel *proc_kern = kernels[identifier][i];

      proc_kern->set_arg( norm_image.ptr() );
      proc_kern->set_arg( vis_image.ptr() );
      proc_kern->set_arg( pre_image.ptr());
      proc_kern->set_arg( img_dim.ptr() );
      proc_kern->set_arg( background_density.ptr() );

      //execute kernel
      proc_kern->execute( queue, 2, local_threads, global_threads);
      int status = clFinish(queue);
      check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status));
      proc_kern->clear_args();
      norm_image->read_to_buffer(queue);

      //set masked values
      vis_image->read_to_buffer(queue);
      pre_image->read_to_buffer(queue);
      {
          int count = 0;
          for (unsigned int j=0; j<cl_nj; ++j) {
            for (unsigned int i=0; i<cl_ni; ++i) {
              if ((i < img->ni()) && (j < img->nj())) {
                vis_buff[count] = 1.0f - (*alt_prior)(i,j);
                pre_buff[count] = (*alt_prior)(i,j) * (*alt_density)(i,j);
              }
              else {
                vis_buff[count] = 1.0f;
                pre_buff[count] = 0.0f;
              }
              ++count;
            }
          }
      }
      vis_image->write_to_buffer(queue);
      pre_image->write_to_buffer(queue);
      clFinish(queue);

      continue;
    }

    for (id = vis_order.begin(); id != vis_order.end(); ++id)
    {
      //vcl_cout << "### vis loop begin: bytes in cache = " << opencl_cache->bytes_in_cache() << vcl_endl;
      //choose correct render kernel
      boxm2_block_metadata mdata = scene->get_block_metadata(*id);
      bocl_kernel* kern =  kernels[identifier][i];

      //write the image values to the buffer
      vul_timer transfer;
      bocl_mem* blk       = opencl_cache->get_block(*id);
      bocl_mem* blk_info  = opencl_cache->loaded_block_info();
      bocl_mem* alpha     = opencl_cache->get_data<BOXM2_ALPHA>(*id,0,false);

      //vcl_cout << "  loaded block and alpha: bytes in cache = " << opencl_cache->bytes_in_cache() << vcl_endl;
      boxm2_scene_info* info_buffer = (boxm2_scene_info*) blk_info->cpu_buffer();
      int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
      info_buffer->data_buffer_length = (int) (alpha->num_bytes()/alphaTypeSize);
      blk_info->write_to_buffer((queue));

      //int nobsTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_NUM_OBS>::prefix());
      // data type string may contain an identifier so determine the buffer size
      unsigned int num_cells = alpha->num_bytes()/alphaTypeSize;

      //bocl_mem* num_obs   = opencl_cache->get_data(*id,num_obs_type,num_cells*nobsTypeSize, true);

      //grab an appropriately sized AUX data buffer
      int auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX0>::prefix());
      bocl_mem *aux0   = opencl_cache->get_data<BOXM2_AUX0>(*id, info_buffer->data_buffer_length*auxTypeSize);
      auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX1>::prefix());
      bocl_mem *aux1   = opencl_cache->get_data<BOXM2_AUX1>(*id, info_buffer->data_buffer_length*auxTypeSize);
      //vcl_cout << " loaded aux0, aux1: bytes in cache = " << opencl_cache->bytes_in_cache() << vcl_endl;

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
        bocl_mem* naa_apm   = opencl_cache->get_data(*id,data_type,num_cells*appTypeSize, true);

        kern->set_arg( blk_info );
        kern->set_arg( blk );
        kern->set_arg( alpha );
        kern->set_arg( radiance_scales.ptr() );
        kern->set_arg( radiance_offsets.ptr() );
        kern->set_arg( radiance_var_scales.ptr() );
        kern->set_arg( radiance_var_offsets.ptr() );
        kern->set_arg( naa_apm );
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
        bocl_mem* naa_apm   = opencl_cache->get_data(*id,data_type,num_cells*appTypeSize, true);

        //vcl_cout << "loaded naa bytes in cache = " << opencl_cache->bytes_in_cache() << vcl_endl;

        auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX2>::prefix());
        bocl_mem *aux2   = opencl_cache->get_data<BOXM2_AUX2>(*id, info_buffer->data_buffer_length*auxTypeSize);
        aux2->zero_gpu_buffer(queue);
        auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX3>::prefix());
        bocl_mem *aux3   = opencl_cache->get_data<BOXM2_AUX3>(*id, info_buffer->data_buffer_length*auxTypeSize);

        //vcl_cout << "loaded aux2 and aux3 : bytes in cache = " << opencl_cache->bytes_in_cache() << vcl_endl;
        aux3->zero_gpu_buffer(queue);

        kern->set_arg( blk_info );
        kern->set_arg( blk );
        kern->set_arg( alpha );
        kern->set_arg( radiance_scales.ptr() );
        kern->set_arg( radiance_offsets.ptr() );
        kern->set_arg( radiance_var_scales.ptr() );
        kern->set_arg( radiance_var_offsets.ptr() );
        kern->set_arg( naa_apm );
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

        //clear render kernel args so it can reset 'm on next execution
        //vcl_cout << "executed bayes update: bytes in cache = " << opencl_cache->bytes_in_cache() << vcl_endl;
        kern->clear_args();
        aux2->read_to_buffer(queue);
        aux3->read_to_buffer(queue);
        //vcl_cout << " read aux2 and aux3. bytes in cache = " << opencl_cache->bytes_in_cache() << vcl_endl;
      }
      else if (i==UPDATE_CELL)
      {
        auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX2>::prefix());
        bocl_mem *aux2   = opencl_cache->get_data<BOXM2_AUX2>(*id, info_buffer->data_buffer_length*auxTypeSize);

        auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX3>::prefix());
        bocl_mem *aux3   = opencl_cache->get_data<BOXM2_AUX3>(*id, info_buffer->data_buffer_length*auxTypeSize);

        local_threads[0] = 64;
        local_threads[1] = 1 ;
        global_threads[0]=RoundUp(info_buffer->data_buffer_length,local_threads[0]);
        global_threads[1]=1;
        kern->set_arg( blk_info );
        kern->set_arg( alpha );
        kern->set_arg( aux0 );
        kern->set_arg( aux1 );
        kern->set_arg( aux2 );
        kern->set_arg( aux3 );
        //kern->set_arg( cl_output.ptr() );
        //execute kernel
        kern->execute(queue, 2, local_threads, global_threads);
        //vcl_cout << "execute returned" << vcl_endl;
        int status = clFinish(queue);
        //vcl_cout << "clFinish returned" << vcl_endl;
        check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status));
        gpu_time += kern->exec_time();

        //clear render kernel args so it can reset em on next execution
        kern->clear_args();

        alpha->read_to_buffer(queue);

        clFinish(queue);
      }
      //vcl_cout << "bytes in cache = " << opencl_cache->bytes_in_cache() << vcl_endl;
      //read image out to buffer (from gpu)

      in_image->read_to_buffer(queue);
      vis_image->read_to_buffer(queue);
      pre_image->read_to_buffer(queue);
      cl_output->read_to_buffer(queue);
      clFinish(queue);
//#define DEBUG
#ifdef DEBUG
      ///debugging save vis, pre, norm images
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
    }
    vcl_cout << "kernel " << i << " complete" << vcl_endl;
  }

  opencl_cache->unref_mem(ray_o_buff.ptr());
  opencl_cache->unref_mem(ray_d_buff.ptr());
  opencl_cache->unref_mem(in_image.ptr());
  opencl_cache->unref_mem(pre_image.ptr());
  opencl_cache->unref_mem(vis_image.ptr());
  opencl_cache->unref_mem(norm_image.ptr());

  delete [] vis_buff;
  delete [] pre_buff;
  delete [] norm_buff;
  delete [] input_buff;
  delete [] ray_origins;
  delete [] ray_directions;

  delete [] radiance_scales_buff;
  delete [] radiance_offsets_buff;
  delete [] radiance_var_scales_buff;
  delete [] radiance_var_offsets_buff;

  vcl_cout<<"Gpu time "<<gpu_time<<" transfer time "<<transfer_time<<vcl_endl;
  clReleaseCommandQueue(queue);
  return true;
}
