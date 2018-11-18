// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_batch_update_scene_process.cxx
#include <iomanip>
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief This process updates the scene (alphas and mogs) given the expectations from all the images.
//  Namely, the process expects to find
//  the output of boxm2_ocl_compute_expectation_per_view_process for each image. The updates to the alpha and mog are done separately,
//  in two optimizations. The alpha computation is done through gradient descent (newton's method). The mog computation is done through
//  expectation-maximization of weighted samples. These computations correspond to performing a single M-step in EM style batch updating.
//
//  Important: MAX_OBS_PER_CELL must be bigger than the defined number in weighted_em.cl
//
// \author Ali Osman Ulusoy
// \date Jun 12, 2012

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>

#include <boxm2/io/boxm2_stream_block_cache.h>
#include <boxm2/cpp/algo/boxm2_data_serial_iterator.h>

#include <boxm2/ocl/boxm2_ocl_util.h>
//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vul/vul_timer.h>
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <boxm2/boxm2_util.h>

namespace boxm2_ocl_batch_update_scene_process_globals
{
  constexpr unsigned n_inputs_ = 5;
  constexpr unsigned n_outputs_ = 0;
  enum {
    COMPUTE_ALPHA = 0,
    COMPUTE_MOG = 1
  };

  void compile_kernel(const bocl_device_sptr& device,std::vector<bocl_kernel*> & vec_kernels, const std::string& opts_app)
  {
    std::vector<std::string> src_paths;
    std::string source_dir = boxm2_ocl_util::ocl_src_root();
    src_paths.push_back(source_dir + "scene_info.cl");
    src_paths.push_back(source_dir + "cell_utils.cl");
    src_paths.push_back(source_dir + "statistics_library_functions.cl");
    src_paths.push_back(source_dir + "stat/mem_efficient_weighted_em.cl");
    src_paths.push_back(source_dir + "batch/em_kernels.cl");


    //compilation options
    auto* compute_alpha = new bocl_kernel();
    std::string opts = " -D COMPUTE_ALPHA " + opts_app;

    compute_alpha->create_kernel(&device->context(), device->device_id(), src_paths, "batch_update_alpha", opts, "batch_update_alpha");
    vec_kernels.push_back(compute_alpha);

    //compilation options
    auto* compute_mog = new bocl_kernel();
    std::string opts_mog = " -D COMPUTE_MOG_CPU  -D MEM_EFFICIENT_EM " + opts_app;
    compute_mog->create_kernel(&device->context(), device->device_id(), src_paths, "batch_update_mog", opts_mog, "batch_update_mog");
    vec_kernels.push_back(compute_mog);
  }

  template <boxm2_data_type type>
  std::map<std::string, char*> helper(const boxm2_scene_sptr& scene, const boxm2_block_id& id, boxm2_data_traits<type> data_type, std::vector<std::string> image_ids)
  {
    std::map<std::string, char*> map;
    for (auto & image_id : image_ids) {
      std::string filename = scene->data_path() + data_type.prefix(image_id) +"_"+ id.to_string() + ".bin";
      if (vul_file::exists(filename)) {
        char * buffer = new(std::nothrow) char[vul_file::size(filename)];
        if (buffer == nullptr) std::cout<<"Failed to Allocate Memory"<<std::endl;
        std::ifstream ifs;
        ifs.open(filename.c_str(), std::ios::in | std::ios::binary);
        if (!ifs) std::cerr << "Failed to open file " << filename << '\n';
        ifs.read(buffer, vul_file::size(filename));
        map[image_id] = buffer;
      }
      else {
        map[image_id] = nullptr;
      }
    }
    return map;
  }


  static std::map<std::string,std::vector<bocl_kernel*> > kernels;
}

bool boxm2_ocl_batch_update_scene_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_batch_update_scene_process_globals;

  //process takes 6 inputs, no output
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "boxm2_scene_sptr";
  input_types_[2] = "boxm2_opencl_cache_sptr";
  input_types_[3] = "unsigned";         //: number of images
  input_types_[4] = "vcl_string";       //: identifiers name file

  std::vector<std::string>  output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_ocl_batch_update_scene_process(bprb_func_process& pro)
{
  using namespace boxm2_ocl_batch_update_scene_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The number of inputs should be " << n_inputs_<< std::endl;
    return false;
  }
  float mog_transfer_time=0.0f;
  float alpha_transfer_time=0.0f;
  float mog_kernel_time=0.0f;
  float alpha_kernel_time=0.0f;

  float gpu_time=0.0f;
  //get the inputs
  unsigned i = 0;
  bocl_device_sptr device = pro.get_input<bocl_device_sptr>(i++);
  boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_opencl_cache_sptr opencl_cache= pro.get_input<boxm2_opencl_cache_sptr>(i++);
  auto num_imgs = pro.get_input<unsigned>(i++);
  std::string identifier_filename = pro.get_input<std::string>(i++);

  bool foundDataType = false;
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
  }
  if (!foundDataType) {
    std::cout<<"BOXM2_OCL_BATCH_UPDATE_SCENE_PROCESS ERROR: scene doesn't have BOXM2_MOG3_GREY data type"<<std::endl;
    return false;
  }


  boxm2_cache_sptr cache = opencl_cache->get_cpu_cache();
  //: Read data types and identifier file names.
  std::ifstream ifs(identifier_filename.c_str());
  if (!ifs.good()) {
      std::cerr << "error opening file " <<identifier_filename << '\n';
      return false;
  }
  std::vector<std::string> image_ids;
  unsigned int n_images = 0;
  ifs >> n_images;
  for (unsigned int i=0; i<n_images; ++i) {
    std::string img_id;
    ifs >> img_id;
    image_ids.push_back(img_id);
  }
  ifs.close();

  std::vector<std::string> type_names_mog_update;
  type_names_mog_update.emplace_back("boxm2_data_index");
  type_names_mog_update.emplace_back("boxm2_num_obs_single_int");

  // create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue(device->context(),
                                                *(device->device_id()),
                                                CL_QUEUE_PROFILING_ENABLE,&status);
  if (status!=0) return false;

  std::string identifier=device->device_identifier()+options;
  // compile the kernel if not already compiled
  if (kernels.find(identifier)==kernels.end())
  {
    std::cout<<"===========Compiling kernels==========="<<std::endl;
    std::vector<bocl_kernel*> ks;
    compile_kernel(device,ks,options);
    kernels[identifier]=ks;
  }

  bocl_mem_sptr num_imgs_mem = new bocl_mem(device->context(), &num_imgs, sizeof(cl_int), "Number of Images");
  num_imgs_mem->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  std::vector<boxm2_block_id> block_ids = scene->get_block_ids();
  std::vector<boxm2_block_id>::iterator id;
  for (id = block_ids.begin(); id != block_ids.end(); ++id)
  {
      std::cout << "Processing " << *id << std::endl;
      bocl_mem* blk = opencl_cache->get_block(scene,*id);
      bocl_mem* blk_info = opencl_cache->loaded_block_info();
      bocl_mem* alpha = opencl_cache->get_data<BOXM2_ALPHA>(scene,*id,0,false);
      auto* info_buffer = (boxm2_scene_info*) blk_info->cpu_buffer();
      int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
      info_buffer->data_buffer_length = (int) (alpha->num_bytes()/alphaTypeSize);
      blk_info->write_to_buffer((queue));

      bocl_mem* mog = opencl_cache->get_data(scene,*id,data_type, alpha->num_bytes()/alphaTypeSize*appTypeSize,false);

      boxm2_stream_block_cache str_blk_cache(scene, type_names_mog_update, image_ids);

      boxm2_block_metadata mdata = scene->get_block_metadata(*id);
      str_blk_cache.init(*id);

      unsigned int datasize = str_blk_cache.block_size_in_bytes_["boxm2_data_index"]/ sizeof(unsigned int);

      //get data indices from str cache
      boxm2_data_base * data_type0 = str_blk_cache.data_types_["boxm2_data_index"];
      auto* indices = static_cast<unsigned int *> ((void*)data_type0->data_buffer() );

      //get nobs from str cache
      boxm2_data_base * data_type1 = str_blk_cache.data_types_["boxm2_num_obs_single_int"];
      auto* nobs = static_cast<unsigned int *> ((void*)data_type1->data_buffer() );

      //read exp, obs from files
      boxm2_data_traits<BOXM2_EXPECTATION> exp_type;
      boxm2_data_traits<BOXM2_PIXEL> pixel_type;
      boxm2_data_traits<BOXM2_AUX2> pre_exp_type;
      boxm2_data_traits<BOXM2_AUX3> seglen_type;
      std::map<std::string, char*> expectations = helper(scene, *id, exp_type, image_ids);
      std::map<std::string, char*> pixels = helper(scene, *id, pixel_type, image_ids);
      std::map<std::string, char*> pre_expectations = helper(scene, *id, pre_exp_type, image_ids);
      std::map<std::string, char*> seglens = helper(scene, *id, seglen_type, image_ids);


      std::vector<unsigned char> obs_all;
      std::vector<float> exp_all;
      std::vector<float> seglen_all;

      auto* weighted_pre_exp_sum = new cl_float[datasize];
      auto* pre_exp_sum = new cl_float[datasize];
      auto* exp_sum = new cl_float[datasize];
      auto* seglen_sum = new cl_float[datasize];

      auto* sampleIndex = new cl_uint[datasize];
      cl_uint  currIdx = 0;

      //snap over each voxel
      unsigned max_obs_per_cell = 0;
      for (unsigned s = 0; s < datasize; ++s)
      {
        sampleIndex[s] = currIdx;

        weighted_pre_exp_sum[s] = 0; //init pre_exp_sum
        pre_exp_sum[s] = 0; //init pre_exp_sum
        exp_sum[s] = 0; //init pre_exp_sum
        seglen_sum[s] = 0; //init pre_exp_sum

        unsigned obs_per_cell = 0;
        for (unsigned short k = 0; k < num_imgs; ++k)
        {
          unsigned int index = indices[datasize*k+s];
          unsigned int obs_num = nobs[datasize*k+s];
          if (obs_num == 0)
            continue;

          auto* exp_imgK = (boxm2_data_traits<BOXM2_EXPECTATION>::datatype*) (expectations[image_ids[k]]);
          auto* obs_imgK = (boxm2_data_traits<BOXM2_PIXEL>::datatype*) (pixels[image_ids[k]]);
          auto* pre_exp_imgK = (boxm2_data_traits<BOXM2_AUX2>::datatype*) (pre_expectations[image_ids[k]]);
          auto* seglen_imgK = (boxm2_data_traits<BOXM2_AUX3>::datatype*) (seglens[image_ids[k]]);

          if (exp_imgK == nullptr || obs_imgK == nullptr) {
            std::cerr << "ERROR!!!! exp/obs arrays empty..." << '\n';
            continue;
          }

          for (unsigned int l = 0; l < obs_num; l++)
          {
            if (exp_imgK[index+l] >= 0 && seglen_imgK[index+l] > 1.0e-07) //add (exp,obs) only if worthwhile
            {
              exp_all.push_back(exp_imgK[index+l] );
              obs_all.push_back(obs_imgK[index+l] );
              seglen_all.push_back(seglen_imgK[index+l] );

              weighted_pre_exp_sum[s] += pre_exp_imgK[index+l] * seglen_imgK[index+l]; //weighted sum of pre_exp for each cell
              pre_exp_sum[s] += pre_exp_imgK[index+l];
              seglen_sum[s] += seglen_imgK[index+l];
              exp_sum[s] += exp_imgK[index+l];

              currIdx++;
              obs_per_cell++;
            }
          }
        }

        if (max_obs_per_cell < obs_per_cell)
          max_obs_per_cell = obs_per_cell;
      }
      std::cout << "Max obs per cell: " << max_obs_per_cell << std::endl;


      //delete expectations and pixels
      for (unsigned short k = 0; k < num_imgs; k++)
      {
        if (expectations[image_ids[k]]) {
          delete[] expectations[image_ids[k]];
          delete[] pixels[image_ids[k]];
          delete[] pre_expectations[image_ids[k]];
          delete[] seglens[image_ids[k]];
        }
      }
      //clear stream cache
      str_blk_cache.clear();


      //create data size related buffers...
      auto num_samp = (unsigned int) obs_all.size();
      std::cout << "Total num samples: " << num_samp << std::endl;
      bocl_mem_sptr num_samples = new bocl_mem(device->context(), &num_samp, sizeof(cl_uint), "total number of samples buffer");
      num_samples->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

      bocl_mem_sptr index_mem = new bocl_mem(device->context(), sampleIndex, sizeof(cl_uint)*datasize, "sample index buffer");
      index_mem->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

      bocl_mem_sptr weighted_pre_exp_sum_mem = new bocl_mem(device->context(), weighted_pre_exp_sum, sizeof(cl_float)*datasize, "weighted pre expectation sum buffer");
      weighted_pre_exp_sum_mem->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

      bocl_mem_sptr pre_exp_sum_mem = new bocl_mem(device->context(), pre_exp_sum, sizeof(cl_float)*datasize, "pre expectation sum buffer");
      pre_exp_sum_mem->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

      bocl_mem_sptr exp_sum_mem = new bocl_mem(device->context(), exp_sum, sizeof(cl_float)*datasize, "expectation sum buffer");
      exp_sum_mem->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

      bocl_mem_sptr seglen_sum_mem = new bocl_mem(device->context(), seglen_sum, sizeof(cl_float)*datasize, "seglen sum buffer");
      seglen_sum_mem->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

      bocl_mem_sptr  datasize_mem = new bocl_mem(device->context(), &datasize, sizeof(cl_int), "Data Size");
      datasize_mem->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

      for (unsigned int i=0; i<kernels[identifier].size(); i++)
      {
        bocl_kernel* kern =  kernels[identifier][i];

        if (i == COMPUTE_ALPHA)
        {
          vul_timer transfer;

          //allocate mem on gpu.
          bocl_mem_sptr bocl_data_type0 = opencl_cache->alloc_mem(exp_all.size() * sizeof(float), &exp_all[0], "expectations");
          bocl_mem_sptr bocl_data_type2 = opencl_cache->alloc_mem(seglen_all.size() * sizeof(float), &seglen_all[0], "seglens");


          if (!bocl_data_type0->create_buffer(CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,queue))
            std::cout<<"expectations buffer was not created"<<std::endl;
          if (!bocl_data_type2->create_buffer(CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,queue))
            std::cout<<"seglen buffer was not created"<<std::endl;


          std::cout << opencl_cache->bytes_in_cache() << " bytes in ocl cache for alpha update..." << std::endl;

          alpha_transfer_time += (float) transfer.all();


          //Determine local and global number of threads
          std::size_t lThreads[] = {32,1};
          std::size_t gThreads[] = {RoundUp(datasize,lThreads[0]), 1};

          //Set AUX args
          kern->set_arg( blk_info );
          kern->set_arg(bocl_data_type0.ptr());
          kern->set_arg(bocl_data_type2.ptr());

          kern->set_arg(num_imgs_mem.ptr());
          kern->set_arg(datasize_mem.ptr());
          kern->set_arg(weighted_pre_exp_sum_mem.ptr());
          kern->set_arg(pre_exp_sum_mem.ptr());
          kern->set_arg(exp_sum_mem.ptr());
          kern->set_arg(seglen_sum_mem.ptr());
          kern->set_arg(alpha);
          kern->set_arg(index_mem.ptr());
          kern->set_arg(num_samples.ptr());
          kern->execute(queue, 2, lThreads, gThreads);

          int status = clFinish(queue);
          check_val(status, MEM_FAILURE, "UPDATE ALPHA EXECUTE FAILED: " + error_to_string(status));
          std::cout<<"Time taken for alpha update: "<< kern->exec_time()<<std::endl;
          alpha_kernel_time += (float) kern->exec_time();
          kern->clear_args();


          alpha->read_to_buffer(queue); //read back alpha
          weighted_pre_exp_sum_mem->read_to_buffer(queue);
          //clear aux data
          opencl_cache->unref_mem(bocl_data_type0.ptr());
          opencl_cache->unref_mem(bocl_data_type2.ptr());
        }
        else if (i == COMPUTE_MOG)
        {
          vul_timer transfer;

          //allocate mem on gpu.
          bocl_mem_sptr bocl_data_type0 = opencl_cache->alloc_mem(obs_all.size() * sizeof(unsigned char), &obs_all[0], "boxm2_mog2_rgb");
          bocl_mem_sptr bocl_data_type2 = opencl_cache->alloc_mem(exp_all.size() * sizeof(float), &exp_all[0], "float16_exp");

          if (!bocl_data_type0->create_buffer(CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,queue))
            std::cout<<"boxm2_mog2_rgb buffer was not created"<<std::endl;
          if (!bocl_data_type2->create_buffer(CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,queue))
            std::cout<<"float16_exp buffer was not created"<<std::endl;

          int niterTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_NUM_OBS>::prefix());
          bocl_mem* num_iter = opencl_cache->get_data(scene,*id, boxm2_data_traits<BOXM2_NUM_OBS>::prefix(),alpha->num_bytes()/alphaTypeSize*niterTypeSize,false);


          std::cout << opencl_cache->bytes_in_cache() << " bytes in ocl cache for mog update..." << std::endl;

          mog_transfer_time += (float) transfer.all();

          //Determine local and global number of threads
          std::size_t lThreads[] = {64,1};
          std::size_t gThreads[] = {RoundUp(datasize,lThreads[0]), 1};

          //Set AUX args
          kern->set_arg( blk_info );
          kern->set_arg(bocl_data_type0.ptr());
          kern->set_arg(bocl_data_type2.ptr());

          kern->set_arg(num_imgs_mem.ptr());
          kern->set_arg(datasize_mem.ptr());
          kern->set_arg(mog);
          kern->set_arg(num_iter);
          kern->set_arg(index_mem.ptr());
          kern->set_arg(num_samples.ptr());
          kern->execute(queue, 2, lThreads, gThreads);

          int status = clFinish(queue);
          check_val(status, MEM_FAILURE, "UPDATE MOG EXECUTE FAILED: " + error_to_string(status));
          std::cout<<"Time taken for mog update: "<< kern->exec_time()<<std::endl;
          mog_kernel_time += (float) kern->exec_time();
          kern->clear_args();

          mog->read_to_buffer(queue);   //read back mog
          num_iter->read_to_buffer(queue);   //read back nobs

          //clear aux data
          opencl_cache->unref_mem(bocl_data_type0.ptr());
          opencl_cache->unref_mem(bocl_data_type2.ptr());
        }
      }
      delete[] sampleIndex;
      delete[] weighted_pre_exp_sum;
      delete[] seglen_sum;
      delete[] pre_exp_sum;
      delete[] exp_sum;
  }

  std::cout << "Alpha transfer time: " << alpha_transfer_time << '\n'
           << "Mog transfer time: " << mog_transfer_time << '\n'
           << "Alpha kernel time: " << alpha_kernel_time << '\n'
           << "Mog kernel time: " << mog_kernel_time << std::endl;
  clReleaseCommandQueue(queue);

  std::cout << std::endl << "EM M-STEP DONE." << std::endl;

  return true;
}
