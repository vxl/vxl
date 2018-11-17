// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_batch_compute_normal_albedo_array_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for computing normal_albedo_array parameters based on a set of input images
//
// \author Daniel Crispell
// \date Feb 14, 2012

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>

#include <boxm2/io/boxm2_stream_block_cache.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vul/vul_timer.h>
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>

#include <brad/brad_image_metadata.h>
#include <brad/brad_atmospheric_parameters.h>
#include <brad/brad_illum_util.h>

namespace boxm2_ocl_batch_compute_normal_albedo_array_process_globals
{
   constexpr unsigned n_inputs_ = 6;
   constexpr unsigned n_outputs_ = 0;
   void compile_kernel(const bocl_device_sptr& device,std::vector<bocl_kernel*> & vec_kernels)
   {
      std::vector<std::string> src_paths;
      std::string source_dir = boxm2_ocl_util::ocl_src_root();
      src_paths.push_back(source_dir + "scene_info.cl");
      src_paths.push_back(source_dir + "statistics_library_functions.cl");
      src_paths.push_back(source_dir + "batch/batch_naa_kernels.cl");

      //compilation options

      auto* compute_naa = new bocl_kernel();
      std::string opts = " -D COMPUTE_NAA ";

      compute_naa->create_kernel(&device->context(), device->device_id(), src_paths, "batch_fit_normal_albedo_array", opts, "batch_fit_normal_albedo_array");
      vec_kernels.push_back(compute_naa);
   }
   static std::map<cl_device_id*,std::vector<bocl_kernel*> > kernels;
}

bool boxm2_ocl_batch_compute_normal_albedo_array_process_cons(bprb_func_process& pro)
{
   using namespace boxm2_ocl_batch_compute_normal_albedo_array_process_globals;

   //process takes 5 inputs, no output
   // 0) opencl device
   // 1) boxm2 scene
   // 2) opencl cache
   // 3) name of text file containing list of image ids
   // 4) name of text file containing list of image_metadata files
   // 5) name of text file containing list of atmospheric_parameters files
   std::vector<std::string> input_types_(n_inputs_);
   input_types_[0] = "bocl_device_sptr";
   input_types_[1] = "boxm2_scene_sptr";
   input_types_[2] = "boxm2_opencl_cache_sptr";
   input_types_[3] = "vcl_string";        // filename: list of image ids
   input_types_[4] = "vcl_string";        // filename: list of image metadata files
   input_types_[5] = "vcl_string";        // filename: list of atmospheric parameters files

   std::vector<std::string>  output_types_(n_outputs_);

   return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_ocl_batch_compute_normal_albedo_array_process(bprb_func_process& pro)
{
   using namespace boxm2_ocl_batch_compute_normal_albedo_array_process_globals;

   if ( pro.n_inputs() < n_inputs_ ) {
      std::cout << pro.name() << ": The number of inputs should be " << n_inputs_<< std::endl;
      return false;
   }
   //get the inputs
   bocl_device_sptr device = pro.get_input<bocl_device_sptr>(0);
   boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(1);
   boxm2_opencl_cache_sptr opencl_cache= pro.get_input<boxm2_opencl_cache_sptr>(2);
   std::string id_list_fname =  pro.get_input<std::string>(3);
   std::string md_list_fname =  pro.get_input<std::string>(4);
   std::string atm_list_fname = pro.get_input<std::string>(5);

   // load metadata and atmopsheric_parameters
   std::vector<brad_image_metadata> metadata;
   std::vector<brad_atmospheric_parameters> atm_params;
   std::vector<std::string> image_ids;

   std::ifstream id_list_ifs(id_list_fname.c_str());
   if (!id_list_ifs.good()) {
      std::cerr << "ERROR reading: " << id_list_fname << '\n';
      return false;
   }
   while (!id_list_ifs.eof()) {
      std::string img_id;
      id_list_ifs >> img_id;
      if (img_id.length() == 0)
         continue;
      image_ids.push_back(img_id);
   }
   id_list_ifs.close();

   std::ifstream md_list_ifs(md_list_fname.c_str());
   if (!md_list_ifs.good()) {
      std::cerr << "ERROR reading: " << md_list_fname << '\n';
      return false;
   }
   while (!md_list_ifs.eof()) {
      std::string filename;
      md_list_ifs >> filename;
      if (filename.length() == 0)
         continue;
      std::cout << "metadata filename = <" << filename << '>' <<  std::endl;
      brad_image_metadata md;
      std::ifstream md_ifs(filename.c_str());
      if (!md_ifs.good()) {
         std::cerr << "ERROR reading image_metadata file: " << filename << '\n';
         return false;
      }
      md_ifs >> md;
      metadata.push_back(md);
   }
   md_list_ifs.close();

   std::ifstream atm_list_ifs(atm_list_fname.c_str());
   if (!atm_list_ifs.good()) {
      std::cerr << "ERROR reading: " << atm_list_fname << '\n';
      return false;
   }
   while (!atm_list_ifs.eof()) {
      std::string filename;
      atm_list_ifs >> filename;
      if (filename.length() == 0)
         continue;
      std::cout << "atmospheric_params filename = <" << filename << '>' <<  std::endl;
      brad_atmospheric_parameters atm;
      std::ifstream atm_ifs(filename.c_str());
      if (!atm_ifs.good()) {
         std::cerr << "ERROR reading atmospheric_parameters file: " << filename << '\n';
         return false;
      }
      atm_ifs >> atm;
      atm_params.push_back(atm);
   }
   atm_list_ifs.close();

   // sanity check
   unsigned int num_images = image_ids.size();
   if (atm_params.size() != num_images) {
      std::cerr << "ERROR: atmospheric params and image id list are different length" << '\n';
      return false;
   }
   if (metadata.size() != num_images) {
      std::cerr << "ERROR: metadata and image id list are different length" << '\n';
      return false;
   }

   // create scale and offset buffers
   // hard-coded variances
   const double reflectance_var = boxm2_normal_albedo_array_constants::sigma_albedo * boxm2_normal_albedo_array_constants::sigma_albedo;
   const double airlight_var = boxm2_normal_albedo_array_constants::sigma_airlight * boxm2_normal_albedo_array_constants::sigma_airlight;
   const double optical_depth_var = boxm2_normal_albedo_array_constants::sigma_optical_depth * boxm2_normal_albedo_array_constants::sigma_optical_depth;
   const double skylight_var = boxm2_normal_albedo_array_constants::sigma_skylight * boxm2_normal_albedo_array_constants::sigma_skylight;

   // get normal directions
   std::vector<vgl_vector_3d<double> > normals = boxm2_normal_albedo_array::get_normals();
   unsigned int num_normals = normals.size();
   // opencl code depends on 16 normal directions
   if (num_normals != 16) {
      std::cerr << "ERROR: boxm2_ocl_batch_compute_normal_albedo_array_process: expecting 16 normals, got " << num_normals << '\n';
      return false;
   }
   // compute offsets and scales for linear radiance model
   auto* radiance_scales_buff = new float[num_normals * num_images];
   auto* radiance_offsets_buff = new float[num_normals * num_images];
   auto* radiance_var_scales_buff = new float[num_normals * num_images];
   auto* radiance_var_offsets_buff = new float[num_normals * num_images];

   for (unsigned int m=0; m<num_images; ++m) {
      for (unsigned int n=0; n<num_normals; ++n) {
         unsigned int idx = m*num_normals + n;
         // compute offsets as radiance of surface with 0 reflectance
         double radiance_offset = brad_expected_radiance_chavez(0.0, normals[n], metadata[m], atm_params[m]);
         radiance_offsets_buff[idx] = radiance_offset;
         // use perfect reflector to compute radiance scale
         double radiance = brad_expected_radiance_chavez(1.0, normals[n], metadata[m], atm_params[m]);
         radiance_scales_buff[idx] = radiance - radiance_offset;
         // compute offset of radiance variance
         double var_offset = brad_radiance_variance_chavez(0.0, normals[n], metadata[m], atm_params[m], reflectance_var, optical_depth_var, skylight_var, airlight_var);
         radiance_var_offsets_buff[idx] = var_offset;
         // compute scale
         double var = brad_radiance_variance_chavez(1.0, normals[n], metadata[m], atm_params[m], reflectance_var, optical_depth_var, skylight_var, airlight_var);
         radiance_var_scales_buff[idx] = var - var_offset;
      }
   }

  bocl_mem_sptr radiance_scales = new bocl_mem(device->context(), radiance_scales_buff, sizeof(float)*num_normals*num_images,"radiance scales buffer");
  radiance_scales->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  bocl_mem_sptr radiance_offsets = new bocl_mem(device->context(), radiance_offsets_buff, sizeof(float)*num_normals*num_images,"radiance offsets buffer");
  radiance_offsets->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  bocl_mem_sptr radiance_var_scales = new bocl_mem(device->context(), radiance_var_scales_buff, sizeof(float)*num_normals*num_images,"radiance variance scales buffer");
  radiance_var_scales->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  bocl_mem_sptr radiance_var_offsets = new bocl_mem(device->context(), radiance_var_offsets_buff, sizeof(float)*num_normals*num_images,"radiance variance offsets buffer");
  radiance_var_offsets->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  cl_int num_images_buff = num_images;
  bocl_mem_sptr num_images_ocl = new bocl_mem(device->context(), &num_images_buff, sizeof(cl_int), "num images buffer");
  num_images_ocl->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  std::vector<std::string> type_names;
  type_names.emplace_back("aux0");
  type_names.emplace_back("aux1");
  type_names.emplace_back("aux2");
  type_names.emplace_back("aux3");

  boxm2_cache_sptr cpu_cache = opencl_cache->get_cpu_cache();

   // create a command queue.
   int status=0;
   cl_command_queue queue = clCreateCommandQueue(device->context(),
                                                 *(device->device_id()),
                                                 CL_QUEUE_PROFILING_ENABLE,&status);
   if (status!=0) return false;
   // compile the kernel
   if (kernels.find((device->device_id()))==kernels.end())
   {
      std::cout<<"===========Compiling kernels==========="<<std::endl;
      std::vector<bocl_kernel*> ks;
      compile_kernel(device,ks);
      kernels[(device->device_id())]=ks;
   }
   // create all buffers

   vul_timer timer;
   timer.mark();

   boxm2_stream_block_cache str_blk_cache(scene, type_names, image_ids);

   std::vector<boxm2_block_id> block_ids = scene->get_block_ids();
   std::vector<boxm2_block_id>::iterator id;
   for (id = block_ids.begin(); id != block_ids.end(); ++id)
   {
      /* bocl_mem* blk = */ opencl_cache->get_block(scene,*id);
      bocl_mem* blk_info = opencl_cache->loaded_block_info();
      bocl_mem* alpha = opencl_cache->get_data<BOXM2_ALPHA>(scene,*id,0,true);
      int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
      auto* info_buffer = (boxm2_scene_info*) blk_info->cpu_buffer();
      info_buffer->data_buffer_length = (int) (alpha->num_bytes()/alphaTypeSize);

      bocl_kernel * kern = kernels[(device->device_id())][0];
      //boxm2_block_metadata block_mdata = scene->get_block_metadata(*id);
      str_blk_cache.init(*id);

      std::size_t num_cells = str_blk_cache.block_size_in_bytes_["aux0"]/ sizeof(float);

      auto num_cells_buff = (cl_int)num_cells;
      bocl_mem_sptr num_cells_ocl = new bocl_mem(device->context(), &num_cells_buff, sizeof(cl_int), "num cells buffer");
      num_cells_ocl->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

      std::string data_type = boxm2_data_traits<BOXM2_NORMAL_ALBEDO_ARRAY>::prefix();
      int appTypeSize = (int)boxm2_data_info::datasize(data_type);
      bocl_mem* naa_apm = opencl_cache->get_data(scene,*id,data_type,num_cells*appTypeSize, true);

      boxm2_data_base * data_type0 = str_blk_cache.data_types_["aux0"];
      bocl_mem_sptr bocl_data_type0 = opencl_cache->alloc_mem(data_type0->buffer_length(), data_type0->data_buffer(),"bocl data type0");
      if (!bocl_data_type0->create_buffer(CL_MEM_USE_HOST_PTR,queue))
         std::cout<<"Aux0 buffer was not created"<<std::endl;

      boxm2_data_base * data_type1 = str_blk_cache.data_types_["aux1"];
      bocl_mem_sptr bocl_data_type1 = opencl_cache->alloc_mem(data_type1->buffer_length(), data_type1->data_buffer(), "bocl data type1");
      if (!bocl_data_type1->create_buffer(CL_MEM_USE_HOST_PTR,queue))
         std::cout<<"Aux1 buffer was not created"<<std::endl;

      boxm2_data_base * data_type2 = str_blk_cache.data_types_["aux2"];
      bocl_mem_sptr bocl_data_type2 = opencl_cache->alloc_mem(data_type2->buffer_length(), data_type2->data_buffer(), "bocl data type2");
      if (!bocl_data_type2->create_buffer(CL_MEM_USE_HOST_PTR,queue))
         std::cout<<"Aux2 buffer was not created"<<std::endl;

      boxm2_data_base * data_type3 = str_blk_cache.data_types_["aux3"];
      bocl_mem_sptr bocl_data_type3 = opencl_cache->alloc_mem(data_type3->buffer_length(), data_type3->data_buffer(), "bocl data type3");
      if (!bocl_data_type3->create_buffer(CL_MEM_USE_HOST_PTR,queue))
         std::cout<<"Aux3 buffer was not created"<<std::endl;

      std::size_t lThreads[] = {8, 8};
      std::size_t gThreads[] = {num_cells*8,8};

      kern->set_arg(bocl_data_type0.ptr());
      kern->set_arg(bocl_data_type1.ptr());
      kern->set_arg(bocl_data_type2.ptr());
      kern->set_arg(bocl_data_type3.ptr());
      kern->set_arg(naa_apm);
      kern->set_arg(radiance_scales.ptr());
      kern->set_arg(radiance_offsets.ptr());
      kern->set_arg(radiance_var_scales.ptr());
      kern->set_arg(radiance_var_offsets.ptr());
      kern->set_arg(num_cells_ocl.ptr());
      kern->set_arg(num_images_ocl.ptr());
      kern->set_local_arg(sizeof(cl_float)*num_images);
      kern->set_local_arg(sizeof(cl_float)*num_images);
      kern->set_local_arg(sizeof(cl_float)*num_images);
      kern->set_local_arg(sizeof(cl_float16)*lThreads[0]*lThreads[1]);

      kern->execute(queue, 2, lThreads, gThreads);

      clFinish(queue);

      std::cout<<"Time taken "<< kern->exec_time()<<std::endl;

      //clear kernel args so it can reset em on next execution
      kern->clear_args();
      naa_apm->read_to_buffer(queue);
      clFinish(queue);
      std::cout << "read appearance model to buffer" << std::endl;
      //cpu_cache->remove_data_base( *id, boxm2_data_traits<BOXM2_NORMAL_ALBEDO_ARRAY>::prefix() );
      str_blk_cache.clear();

      opencl_cache->unref_mem(bocl_data_type0.ptr());
      opencl_cache->unref_mem(bocl_data_type1.ptr());
      opencl_cache->unref_mem(bocl_data_type2.ptr());
      opencl_cache->unref_mem(bocl_data_type3.ptr());
  }
  clReleaseCommandQueue(queue);

  delete[] radiance_scales_buff;
  delete[] radiance_offsets_buff;
  delete[] radiance_var_scales_buff;
  delete[] radiance_var_offsets_buff;


  std::cout << "Finished Ocl NAA in " << timer.all() << " ms" << std::endl;
  return true;
}
