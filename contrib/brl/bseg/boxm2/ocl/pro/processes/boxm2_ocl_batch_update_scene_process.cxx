// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_batch_update_scene_process.cxx
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

#include <vcl_iomanip.h>
#include <vcl_fstream.h>
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
  const unsigned n_inputs_ =  5;
  const unsigned n_outputs_ = 0;
  enum {
    COMPUTE_ALPHA = 0,
    COMPUTE_MOG = 1
  };

  void compile_kernel(bocl_device_sptr device,vcl_vector<bocl_kernel*> & vec_kernels, vcl_string opts_app)
  {
    vcl_vector<vcl_string> src_paths;
    vcl_string source_dir = boxm2_ocl_util::ocl_src_root();
    src_paths.push_back(source_dir + "scene_info.cl");
    src_paths.push_back(source_dir + "cell_utils.cl");
    src_paths.push_back(source_dir + "statistics_library_functions.cl");
    src_paths.push_back(source_dir + "stat/mem_efficient_weighted_em.cl");
    src_paths.push_back(source_dir + "batch/em_kernels.cl");


    //compilation options
    bocl_kernel* compute_alpha = new bocl_kernel();
    vcl_string opts = " -D COMPUTE_ALPHA " + opts_app;

    compute_alpha->create_kernel(&device->context(), device->device_id(), src_paths, "batch_update_alpha", opts, "batch_update_alpha");
    vec_kernels.push_back(compute_alpha);

    //compilation options
    bocl_kernel* compute_mog = new bocl_kernel();
    vcl_string opts_mog = " -D COMPUTE_MOG_CPU  -D MEM_EFFICIENT_EM " + opts_app;
    compute_mog->create_kernel(&device->context(), device->device_id(), src_paths, "batch_update_mog", opts_mog, "batch_update_mog");
    vec_kernels.push_back(compute_mog);
  }

  template <boxm2_data_type type>
  vcl_map<vcl_string, char*> helper(boxm2_scene_sptr scene, boxm2_block_id id, boxm2_data_traits<type> data_type, vcl_vector<vcl_string> image_ids)
  {
    vcl_map<vcl_string, char*> map;
    for (unsigned j = 0; j < image_ids.size(); j++) {
      vcl_string filename = scene->data_path() + data_type.prefix(image_ids[j]) +"_"+ id.to_string() + ".bin";
      if (vul_file::exists(filename)) {
        char * buffer = new(std::nothrow) char[vul_file::size(filename)];
        if (buffer == 0) vcl_cout<<"Failed to Allocate Memory"<<vcl_endl;
        vcl_ifstream ifs;
        ifs.open(filename.c_str(), vcl_ios::in | vcl_ios::binary);
        if (!ifs) vcl_cerr << "Failed to open file " << filename << '\n';
        ifs.read(buffer, vul_file::size(filename));
        map[image_ids[j]] = buffer;
      }
      else {
        map[image_ids[j]] = 0;
      }
    }
    return map;
  }


  static vcl_map<vcl_string,vcl_vector<bocl_kernel*> > kernels;
}

bool boxm2_ocl_batch_update_scene_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_batch_update_scene_process_globals;

  //process takes 6 inputs, no output
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "boxm2_scene_sptr";
  input_types_[2] = "boxm2_opencl_cache_sptr";
  input_types_[3] = "unsigned";         //: number of images
  input_types_[4] = "vcl_string";       //: identifiers name file

  vcl_vector<vcl_string>  output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_ocl_batch_update_scene_process(bprb_func_process& pro)
{
  using namespace boxm2_ocl_batch_update_scene_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cout << pro.name() << ": The number of inputs should be " << n_inputs_<< vcl_endl;
    return false;
  }
  float mog_transfer_time=0.0f;
  float alpha_transfer_time=0.0f;
  float mog_kernel_time=0.0f;
  float alpha_kernel_time=0.0f;

  float gpu_time=0.0f;
  //get the inputs
  unsigned i = 0;
  bocl_device_sptr device             = pro.get_input<bocl_device_sptr>(i++);
  boxm2_scene_sptr scene              = pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_opencl_cache_sptr opencl_cache= pro.get_input<boxm2_opencl_cache_sptr>(i++);
  unsigned int num_imgs               = pro.get_input<unsigned>(i++);
  vcl_string identifier_filename      = pro.get_input<vcl_string>(i++);

  bool foundDataType = false;
  vcl_string data_type,num_obs_type,options;
  vcl_vector<vcl_string> apps = scene->appearances();
  int appTypeSize;
  for (unsigned int i=0; i<apps.size(); ++i) {
    if ( apps[i] == boxm2_data_traits<BOXM2_MOG3_GREY>::prefix() )
    {
      data_type = apps[i];
      foundDataType = true;
      options=" -D MOG_TYPE_8 ";
      appTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_MOG3_GREY>::prefix());
    }
  }
  if (!foundDataType) {
    vcl_cout<<"BOXM2_OCL_BATCH_UPDATE_SCENE_PROCESS ERROR: scene doesn't have BOXM2_MOG3_GREY data type"<<vcl_endl;
    return false;
  }


  boxm2_cache_sptr cache = opencl_cache->get_cpu_cache();
  //: Read data types and identifier file names.
  vcl_ifstream ifs(identifier_filename.c_str());
  if (!ifs.good()) {
      vcl_cerr << "error opening file " <<identifier_filename << '\n';
      return false;
  }
  vcl_vector<vcl_string> image_ids;
  unsigned int n_images = 0;
  ifs >> n_images;
  for (unsigned int i=0; i<n_images; ++i) {
    vcl_string img_id;
    ifs >> img_id;
    image_ids.push_back(img_id);
  }
  ifs.close();

  vcl_vector<vcl_string> type_names_mog_update;
  type_names_mog_update.push_back("boxm2_data_index");
  type_names_mog_update.push_back("boxm2_num_obs_single_int");

  // create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue(device->context(),
                                                *(device->device_id()),
                                                CL_QUEUE_PROFILING_ENABLE,&status);
  if (status!=0) return false;

  vcl_string identifier=device->device_identifier()+options;
  // compile the kernel if not already compiled
  if (kernels.find(identifier)==kernels.end())
  {
    vcl_cout<<"===========Compiling kernels==========="<<vcl_endl;
    vcl_vector<bocl_kernel*> ks;
    compile_kernel(device,ks,options);
    kernels[identifier]=ks;
  }

  bocl_mem_sptr num_imgs_mem = new bocl_mem(device->context(), &num_imgs, sizeof(cl_int), "Number of Images");
  num_imgs_mem->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  vcl_vector<boxm2_block_id> block_ids = scene->get_block_ids();
  vcl_vector<boxm2_block_id>::iterator id;
  for (id = block_ids.begin(); id != block_ids.end(); ++id)
  {
      vcl_cout << "Processing " << *id << vcl_endl;
      bocl_mem* blk       = opencl_cache->get_block(*id);
      bocl_mem* blk_info  = opencl_cache->loaded_block_info();
      bocl_mem* alpha     = opencl_cache->get_data<BOXM2_ALPHA>(*id,0,false);
      boxm2_scene_info* info_buffer = (boxm2_scene_info*) blk_info->cpu_buffer();
      int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
      info_buffer->data_buffer_length = (int) (alpha->num_bytes()/alphaTypeSize);
      blk_info->write_to_buffer((queue));

      bocl_mem* mog       = opencl_cache->get_data(*id,data_type, alpha->num_bytes()/alphaTypeSize*appTypeSize,false);

      boxm2_stream_block_cache str_blk_cache(scene, type_names_mog_update, image_ids);

      boxm2_block_metadata mdata = scene->get_block_metadata(*id);
      str_blk_cache.init(*id);

      unsigned int datasize = str_blk_cache.block_size_in_bytes_["boxm2_data_index"]/ sizeof(unsigned int);

      //get data indices from str cache
      boxm2_data_base * data_type0 = str_blk_cache.data_types_["boxm2_data_index"];
      unsigned int* indices  = static_cast<unsigned int *> ((void*)data_type0->data_buffer() );

      //get nobs from str cache
      boxm2_data_base * data_type1 = str_blk_cache.data_types_["boxm2_num_obs_single_int"];
      unsigned int* nobs  = static_cast<unsigned int *> ((void*)data_type1->data_buffer() );

      //read exp, obs from files
      boxm2_data_traits<BOXM2_EXPECTATION> exp_type;
      boxm2_data_traits<BOXM2_PIXEL> pixel_type;
      boxm2_data_traits<BOXM2_AUX2> pre_exp_type;
      boxm2_data_traits<BOXM2_AUX3> seglen_type;
      vcl_map<vcl_string, char*> expectations = helper(scene, *id, exp_type, image_ids);
      vcl_map<vcl_string, char*> pixels = helper(scene, *id, pixel_type, image_ids);
      vcl_map<vcl_string, char*> pre_expectations = helper(scene, *id, pre_exp_type, image_ids);
      vcl_map<vcl_string, char*> seglens = helper(scene, *id, seglen_type, image_ids);


      vcl_vector<unsigned char> obs_all;
      vcl_vector<float> exp_all;
      vcl_vector<float> seglen_all;

      cl_float* weighted_pre_exp_sum = new cl_float[datasize];
      cl_float* pre_exp_sum = new cl_float[datasize];
      cl_float* exp_sum = new cl_float[datasize];
      cl_float* seglen_sum = new cl_float[datasize];

      cl_uint* sampleIndex = new cl_uint[datasize];
      cl_uint  currIdx     = 0;

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
          unsigned int index  = indices[datasize*k+s];
          unsigned int obs_num  = nobs[datasize*k+s];
          if (obs_num == 0)
            continue;

          boxm2_data_traits<BOXM2_EXPECTATION>::datatype* exp_imgK = (boxm2_data_traits<BOXM2_EXPECTATION>::datatype*) (expectations[image_ids[k]]);
          boxm2_data_traits<BOXM2_PIXEL>::datatype* obs_imgK = (boxm2_data_traits<BOXM2_PIXEL>::datatype*) (pixels[image_ids[k]]);
          boxm2_data_traits<BOXM2_AUX2>::datatype* pre_exp_imgK = (boxm2_data_traits<BOXM2_AUX2>::datatype*) (pre_expectations[image_ids[k]]);
          boxm2_data_traits<BOXM2_AUX3>::datatype* seglen_imgK = (boxm2_data_traits<BOXM2_AUX3>::datatype*) (seglens[image_ids[k]]);

          if (exp_imgK == 0 || obs_imgK == 0) {
            vcl_cerr << "ERROR!!!! exp/obs arrays empty..." << '\n';
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
          max_obs_per_cell  = obs_per_cell;
      }
      vcl_cout << "Max obs per cell: " << max_obs_per_cell << vcl_endl;


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
      unsigned int num_samp = (unsigned int) obs_all.size();
      vcl_cout << "Total num samples: " << num_samp << vcl_endl;
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
            vcl_cout<<"expectations buffer was not created"<<vcl_endl;
          if (!bocl_data_type2->create_buffer(CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,queue))
            vcl_cout<<"seglen buffer was not created"<<vcl_endl;


          vcl_cout << opencl_cache->bytes_in_cache() << " bytes in ocl cache for alpha update..." << vcl_endl;

          alpha_transfer_time += (float) transfer.all();


          //Determine local and global number of threads
          vcl_size_t lThreads[] = {32,1};
          vcl_size_t gThreads[] = {RoundUp(datasize,lThreads[0]), 1};

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
          vcl_cout<<"Time taken for alpha update: "<< kern->exec_time()<<vcl_endl;
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
            vcl_cout<<"boxm2_mog2_rgb buffer was not created"<<vcl_endl;
          if (!bocl_data_type2->create_buffer(CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,queue))
            vcl_cout<<"float16_exp buffer was not created"<<vcl_endl;

          int niterTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_NUM_OBS>::prefix());
          bocl_mem* num_iter   = opencl_cache->get_data(*id, boxm2_data_traits<BOXM2_NUM_OBS>::prefix(),alpha->num_bytes()/alphaTypeSize*niterTypeSize,false);


          vcl_cout << opencl_cache->bytes_in_cache() << " bytes in ocl cache for mog update..." << vcl_endl;

          mog_transfer_time += (float) transfer.all();

          //Determine local and global number of threads
          vcl_size_t lThreads[] = {64,1};
          vcl_size_t gThreads[] = {RoundUp(datasize,lThreads[0]), 1};

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
          vcl_cout<<"Time taken for mog update: "<< kern->exec_time()<<vcl_endl;
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

  vcl_cout << "Alpha transfer time: " << alpha_transfer_time << '\n'
           << "Mog transfer time: " << mog_transfer_time << '\n'
           << "Alpha kernel time: " << alpha_kernel_time << '\n'
           << "Mog kernel time: " << mog_kernel_time << vcl_endl;
  clReleaseCommandQueue(queue);

  vcl_cout << vcl_endl << "EM M-STEP DONE." << vcl_endl;

  return true;
}


