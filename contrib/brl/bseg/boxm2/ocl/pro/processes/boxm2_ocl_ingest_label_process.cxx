// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_ingest_label_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for ingesting a label image in a 3-d scene.
//
// \author Ozge C. Ozcanli 
// \date Jan 18, 2013

#include <vcl_fstream.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>
//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vul/vul_timer.h>
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>

namespace boxm2_ocl_ingest_label_process_globals
{
  const unsigned n_inputs_  = 8;
  //const unsigned n_inputs_  = 7;
  const unsigned n_outputs_ = 0;
  vcl_size_t local_threads[2]={8,8};
  void compile_kernel(bocl_device_sptr device,vcl_vector<bocl_kernel*> & vec_kernels, vcl_string options)
  {
    //gather all render sources... seems like a lot for rendering...
    vcl_vector<vcl_string> src_paths;
    vcl_string source_dir = boxm2_ocl_util::ocl_src_root();
    src_paths.push_back(source_dir + "scene_info.cl");
    src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
    src_paths.push_back(source_dir + "backproject.cl");
    src_paths.push_back(source_dir + "bit/ingest_label_map.cl");
    src_paths.push_back(source_dir + "update_functors.cl");
    src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");

    //set kernel options
    options += " -D INGEST_LABEL_MAP -D SHORT ";
    options += " -D STEP_CELL=step_cell_ingest_label_map(aux_args,data_ptr)";

    //have kernel construct itself using the context and device
    bocl_kernel * ray_trace_kernel=new bocl_kernel();


    ray_trace_kernel->create_kernel( &device->context(),
                                     device->device_id(),
                                     src_paths,
                                     "ingest_label_map",   //kernel name
                                     options,               //options
                                     "boxm2 opencl ingest label map"); 
    vec_kernels.push_back(ray_trace_kernel);
  }
  static vcl_map<vcl_string,vcl_vector<bocl_kernel*> > kernels;
}

bool boxm2_ocl_ingest_label_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_ingest_label_process_globals;

  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "boxm2_scene_sptr";
  input_types_[2] = "boxm2_opencl_cache_sptr";
  input_types_[3] = "vil_image_view_base_sptr";
  input_types_[4] = "vil_image_view_base_sptr";
  input_types_[5] = "vil_image_view_base_sptr";
  input_types_[6] = "vil_image_view_base_sptr";
  input_types_[7] = "vcl_string";  // the identifier of the output label_short data type, e.g. "combined"

  // process has no outputs
  vcl_vector<vcl_string> output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_ocl_ingest_label_process(bprb_func_process& pro)
{
  using namespace boxm2_ocl_ingest_label_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  float transfer_time=0.0f;
  float gpu_time=0.0f;
  //get the inputs
  unsigned i = 0;
  bocl_device_sptr device= pro.get_input<bocl_device_sptr>(i++);
  boxm2_scene_sptr scene =pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_opencl_cache_sptr opencl_cache= pro.get_input<boxm2_opencl_cache_sptr>(i++);

  vil_image_view_base_sptr x_img = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr y_img = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr z_img = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr label_img = pro.get_input<vil_image_view_base_sptr>(i++);
  vcl_string out_ident = pro.get_input<vcl_string>(i++);  // identifier for the output label data blocks

  unsigned int ni     = z_img->ni();
  unsigned int nj     = z_img->nj();

  unsigned int cl_ni  = RoundUp(z_img->ni(),8);
  unsigned int cl_nj  = RoundUp(z_img->nj(),8);
  vcl_cout << "casting images of size: " << z_img->ni() << " x " << z_img->nj() << " rounded up size: " << cl_ni << " x " << cl_nj << vcl_endl;

  vil_image_view<float> * z_img_float = dynamic_cast<vil_image_view<float> * > (z_img.ptr());
  vil_image_view<float> * x_img_float = dynamic_cast<vil_image_view<float> * > (x_img.ptr());
  vil_image_view<float> * y_img_float = dynamic_cast<vil_image_view<float> * > (y_img.ptr());
  if (label_img->pixel_format() != VIL_PIXEL_FORMAT_BYTE) {
    vcl_cerr << " format of label image is not vxl_byte!!\n";
    return false;
  }
  vcl_cout << "ingesting label img..\n"; vcl_cout.flush();

  vil_image_view<vxl_byte> * label_img_byte = dynamic_cast<vil_image_view<vxl_byte> * > (label_img.ptr());

  // form the ray buffer
  cl_float* ray_origins    = new float[4*cl_ni*cl_nj];
  cl_uchar* labels        = new unsigned char[cl_ni*cl_nj];

  int count=0;
  for (unsigned int j=0;j<cl_nj;++j) {
    for (unsigned int i=0;i<cl_ni;++i) {
      if ( i < ni && j < nj )
      {
        ray_origins[count+0] = (*x_img_float)(i,j);
        ray_origins[count+1] = (*y_img_float)(i,j);
        ray_origins[count+2] = (*z_img_float)(i,j);
        ray_origins[count+3] = 0.0;
      }
      count += 4;
    }
  }
  count=0;
  for (unsigned int j=0;j<cl_nj;++j) {
    for (unsigned int i=0;i<cl_ni;++i) {
      if ( i < ni && j < nj )
        labels[count] = (cl_uchar)(*label_img_byte)(i,j);
      count++;
    }
  }

  bocl_mem* ray_o_buff = new bocl_mem(device->context(),
                                          ray_origins,
                                          cl_ni*cl_nj * sizeof(cl_float4),
                                          "ray_origins buffer");
  ray_o_buff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  bocl_mem* label_buff = new bocl_mem(device->context(),
                                        labels,
                                        cl_ni*cl_nj * sizeof(cl_uchar),
                                        "label image buffer");
  label_buff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  vcl_string data_type = boxm2_data_traits<BOXM2_LABEL_SHORT>::prefix();

  vcl_string out_data_type = data_type + "_" + out_ident;
  // read orientation data blocks
  data_type += "_orientation";
  vcl_cout << " will read data blocks with type: " << data_type << vcl_endl;
  vcl_cout << " will ingest and write data blocks with type: " << out_data_type << " as output!\n";
  

  size_t apptypesize = boxm2_data_traits<BOXM2_LABEL_SHORT>::datasize();
  size_t alphaTypeSize = boxm2_data_traits<BOXM2_ALPHA>::datasize();


  //get x and y size from scene
  vcl_vector<boxm2_block_id> vis_order = scene->get_block_ids();
  vcl_vector<boxm2_block_id>::iterator id;
  // create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue(device->context(),
                                                *(device->device_id()),
                                                CL_QUEUE_PROFILING_ENABLE,&status);
  if (status!=0)
    return false;

  // compile the kernel
  vcl_string identifier=device->device_identifier();

  if (kernels.find(identifier)==kernels.end())
  {
    vcl_cout<<"===========Compiling kernels==========="<<vcl_endl;
    vcl_vector<bocl_kernel*> ks;
    compile_kernel(device,ks,"");
    kernels[identifier]=ks;
  }
  // Image Dimensions
  int img_dim_buff[4];
  img_dim_buff[0] = 0;
  img_dim_buff[1] = 0;
  img_dim_buff[2] = ni;
  img_dim_buff[3] = nj;
  bocl_mem_sptr exp_img_dim=new bocl_mem(device->context(), img_dim_buff, sizeof(int)*4, "image dims");
  exp_img_dim->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);


  // bit lookup buffer
  cl_uchar lookup_arr[256];
  boxm2_ocl_util::set_bit_lookup(lookup_arr);
  bocl_mem_sptr lookup=new bocl_mem(device->context(), lookup_arr, sizeof(cl_uchar)*256, "bit lookup buffer");
  lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  //2. set workgroup size
  vcl_size_t lThreads[] = {8, 8};
  vcl_size_t gThreads[] = {cl_ni,cl_nj};

  vcl_cout<<"Ingesting Label Img"<<vcl_endl;
  // set arguments

  for (id = vis_order.begin(); id != vis_order.end(); ++id)
  {
    vcl_cout<<"Block # "<<*id<<vcl_endl;
    //choose correct render kernel
    boxm2_block_metadata mdata = scene->get_block_metadata(*id);
    bocl_kernel* kern =  kernels[identifier][0];

    //write the image values to the buffer
    vul_timer transfer;
    bocl_mem * blk           = opencl_cache->get_block(*id);
    
    // to go back to ingesting labels as is with the input data_type, uncomment below and comment the next get_data, also see below
    //// get alpha to get size, TODO: Fix This!
    //bocl_mem * alpha         = opencl_cache->get_data<BOXM2_ALPHA>(*id);
    //bocl_mem * label_data    = opencl_cache->get_data(*id,data_type,alpha->num_bytes()/alphaTypeSize*apptypesize,true);
    bocl_mem* label_data       = opencl_cache->get_data(*id,data_type,0,true);
    
    bocl_mem * blk_info      = opencl_cache->loaded_block_info();
    transfer_time           += (float) transfer.all();

    ////3. SET args
    kern->set_arg(blk_info );
    kern->set_arg(exp_img_dim.ptr() );
    kern->set_arg(ray_o_buff );
    kern->set_arg(blk );
    kern->set_arg(label_data );
    kern->set_arg(label_buff );
    kern->set_arg(lookup.ptr() );
    //local tree , cumsum buffer, imindex buffer
    kern->set_local_arg( lThreads[0]*lThreads[1]*sizeof(cl_uchar16) );
    kern->set_local_arg( lThreads[0]*lThreads[1]*10*sizeof(cl_uchar) );
    kern->set_local_arg( lThreads[0]*lThreads[1]*sizeof(cl_int) );
    vcl_cout<<"Setting arguments"<<vcl_endl;

    //execute kernel
    kern->execute(queue, 2, lThreads, gThreads);
    clFinish(queue);
    gpu_time += kern->exec_time();
    vcl_cout<<" Time "<<gpu_time<<vcl_endl;
    
    // to go back to ingesting labels as is with the input data_type, uncomment below and comment deep_replace, also change ingest_label_map.cl accordingly
    //label_data->read_to_buffer(queue);
    opencl_cache->deep_replace_data(*id, out_data_type, label_data, false); // deep replace to output type where read_to_buffer is called
    
    //clear render kernel args so it can reset em on next execution
    kern->clear_args();

    //opencl_cache->clear_cache();
    //opencl_cache->finish_queue();
    clFinish(queue);
  }
#ifdef DEBUG
  vil_image_view<float> test(ni,nj);
  count = 0;
  for (unsigned int j=0;j<cl_nj;++j) {
    for (unsigned int i=0;i<cl_ni;++i) {
      if ( i < ni && j < nj )
      {
        test(i,j) = outimg[count];
      }
      ++count;
    }
  }
  vil_save(test,"f:/test.tiff");
#endif
  clReleaseCommandQueue(queue);
  
  delete ray_o_buff;
  delete label_buff;
  
  delete [] ray_origins;
  delete [] labels;
  return true;
}
