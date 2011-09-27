// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_change_detection_process.cxx
//:
// \file
// \brief  A process for change detection
//
// \author Vishal Jain
// \date Mar 10, 2011

#include <bprb/bprb_func_process.h>

#include <vcl_fstream.h>
#include <vcl_algorithm.h>
#include <vcl_sstream.h>
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
#include <boxm2/ocl/algo/boxm2_ocl_render_expected_image_function.h>

namespace boxm2_ocl_change_detection_process_globals
{
  const unsigned n_inputs_     = 8;
  const unsigned n_outputs_    = 2;
  vcl_size_t local_threads [2] = {8,8};
  vcl_size_t global_threads[2] = {8,8};

  //pass two probability threshold (if greater than this, will do nxn on pixel)
  const float PROB_THRESH = .1f; 

  static vcl_map<vcl_string,vcl_vector<bocl_kernel*> > kernels;

  void compile_kernel(bocl_device_sptr device,vcl_vector<bocl_kernel*>& vec_kernels, vcl_string opts)
  {
    //gather all render sources... seems like a lot for rendering...
    //gather all render sources... seems like a lot for rendering...
    vcl_vector<vcl_string> src_paths;
    vcl_string source_dir = boxm2_ocl_util::ocl_src_root();
    src_paths.push_back(source_dir + "scene_info.cl");
    src_paths.push_back(source_dir + "cell_utils.cl");
    src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
    src_paths.push_back(source_dir + "backproject.cl");
    src_paths.push_back(source_dir + "statistics_library_functions.cl");
    src_paths.push_back(source_dir + "expected_functor.cl");
    src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
    src_paths.push_back(source_dir + "bit/change_detection.cl");
    src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");

    //set kernel options
    opts += " -D CHANGE -D DETERMINISTIC ";
    vcl_string options=opts;
    opts += " -D STEP_CELL=step_cell_change_detection_uchar8_w_expected(aux_args.mog,aux_args.alpha,data_ptr,d*linfo->block_len,vis,aux_args.change,aux_args.change_exp,aux_args.intensity,aux_args.intensity_exp) ";

    //have kernel construct itself using the context and device
    bocl_kernel * ray_trace_kernel = new bocl_kernel();
    ray_trace_kernel->create_kernel( &device->context(),
                                     device->device_id(),
                                     src_paths,
                                     "change_detection_bit_scene",   //kernel name
                                     opts,              //options
                                     "boxm2 1x1 ocl change detection"); //kernel identifier (for error checking)
    vec_kernels.push_back(ray_trace_kernel);

    //create nxn kernel
    vcl_stringstream pthresh; 
    pthresh<<" -D PROB_THRESH="<<PROB_THRESH<<"  "; 
    opts += pthresh.str(); 
    bocl_kernel * nxn_kernel = new bocl_kernel(); 
    nxn_kernel->create_kernel( &device->context(),
                                device->device_id(),
                                src_paths,
                                "nxn_change_detection",
                                opts,
                                "boxm2 nxn ocl change detection kernel"); 
    vec_kernels.push_back(nxn_kernel); 
    
    //create normalize image kernel
    vcl_vector<vcl_string> norm_src_paths;
    norm_src_paths.push_back(source_dir + "pixel_conversion.cl");
    norm_src_paths.push_back(source_dir + "bit/normalize_kernels.cl");
    bocl_kernel * normalize_render_kernel=new bocl_kernel();
    normalize_render_kernel->create_kernel( &device->context(),
                                            device->device_id(),
                                            norm_src_paths,
                                            "normalize_change_kernel",   //kernel name
                                            options,              //options
                                            "normalize change detection kernel"); //kernel identifier (for error checking)

    vec_kernels.push_back(normalize_render_kernel);
  }
}

bool boxm2_ocl_change_detection_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_change_detection_process_globals;

  //process takes 1 input
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "boxm2_scene_sptr";
  input_types_[2] = "boxm2_opencl_cache_sptr";
  input_types_[3] = "vpgl_camera_double_sptr";
  input_types_[4] = "vil_image_view_base_sptr";
  input_types_[5] = "vil_image_view_base_sptr";
  input_types_[6] = "int";   //n for nxn ray casting
  input_types_[7] = "vcl_string"; //"raybelief" string for using raybelief

  // process has 1 output:
  // output[0]: scene sptr
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";  //prob of change image
  output_types_[1] = "vil_image_view_base_sptr";  //Red Green change image
  bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);

  //default is 1x1, with no ray belief
  brdb_value_sptr nxn  = new brdb_value_t<int>(1);
  brdb_value_sptr rayb = new brdb_value_t<vcl_string>("");
  pro.set_input(6, nxn);
  pro.set_input(7, rayb); 
  return good; 
}

bool boxm2_ocl_change_detection_process(bprb_func_process& pro)
{
  using namespace boxm2_ocl_change_detection_process_globals;
  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  float transfer_time=0.0f;
  float gpu_time=0.0f;
  
  //get the inputs
  unsigned i = 0;
  bocl_device_sptr          device        = pro.get_input<bocl_device_sptr>(i++);
  boxm2_scene_sptr          scene         = pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_opencl_cache_sptr   opencl_cache  = pro.get_input<boxm2_opencl_cache_sptr>(i++);
  vpgl_camera_double_sptr   cam           = pro.get_input<vpgl_camera_double_sptr>(i++);
  vil_image_view_base_sptr  img           = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr  exp_img       = pro.get_input<vil_image_view_base_sptr>(i++);
  int                       n             = pro.get_input<unsigned>(i++);                 //nxn 
  vcl_string                norm_type     = pro.get_input<vcl_string>(i++); 
  

  unsigned ni=img->ni();
  unsigned nj=img->nj();
  bool foundDataType = false, foundNumObsType = false;
  vcl_string data_type,num_obs_type,options;
  vcl_vector<vcl_string> apps = scene->appearances();
  for (unsigned int i=0; i<apps.size(); ++i) {
    if ( apps[i] == boxm2_data_traits<BOXM2_MOG3_GREY>::prefix() )
    {
      data_type = apps[i];
      foundDataType = true;
      options=" -D MOG_TYPE_8 ";
    }
    else if ( apps[i] == boxm2_data_traits<BOXM2_MOG3_GREY_16>::prefix() )
    {
      data_type = apps[i];
      foundDataType = true;
      options=" -D MOG_TYPE_16 ";
    }
    else if ( apps[i] == boxm2_data_traits<BOXM2_NUM_OBS>::prefix() )
    {
      num_obs_type = apps[i];
      foundNumObsType = true;
    }
  }
  if (!foundDataType) {
    vcl_cout<<"boxm2_ocl_change_detection_process ERROR: scene doesn't have BOXM2_MOG3_GREY or BOXM2_MOG3_GREY_16 data type"<<vcl_endl;
    return false;
  }
  if (!foundNumObsType) {
    vcl_cout<<"boxm2_ocl_change_detection_process ERROR: scene doesn't have BOXM2_NUM_OBS type"<<vcl_endl;
    return false;
  }

  //: create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue(device->context(),*(device->device_id()),
                                                CL_QUEUE_PROFILING_ENABLE,&status);
  if (status!=0) 
    return false;

  // compile the kernel
  vcl_string identifier=device->device_identifier()+options;
  if (kernels.find(identifier)==kernels.end())
  {
    vcl_cout<<"===========Compiling kernels==========="<<vcl_endl;
    vcl_vector<bocl_kernel*> ks;
    compile_kernel(device,ks,options);
    kernels[identifier]=ks;
  }

  //prepare cam buffers
  cl_float cam_buffer[48];
  boxm2_ocl_util::set_ocl_camera(cam, cam_buffer);
  bocl_mem_sptr persp_cam=new bocl_mem(device->context(), cam_buffer, 3*sizeof(cl_float16), "persp cam buffer");
  persp_cam->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  //prepare input images
  vil_image_view_base_sptr float_img     = boxm2_ocl_util::prepare_input_image(img, true); //true for force gray scale
  vil_image_view_base_sptr float_exp_img = boxm2_ocl_util::prepare_input_image(exp_img);
  vil_image_view<float>*   img_view      = static_cast<vil_image_view<float>* >(float_img.ptr());
  vil_image_view<float>*   exp_img_view  = static_cast<vil_image_view<float>* >(float_exp_img.ptr());

  //prepare workspace size
  unsigned cl_ni    = RoundUp(img_view->ni(),local_threads[0]);
  unsigned cl_nj    = RoundUp(img_view->nj(),local_threads[1]);
  global_threads[0] = cl_ni;
  global_threads[1] = cl_nj;

  //prepare image buffers (cpu)
  float* vis_buff               = new float[cl_ni*cl_nj];
  float* exp_image_buff         = new float[cl_ni*cl_nj];
  float* change_image_buff      = new float[cl_ni*cl_nj];
  float* change_exp_image_buff  = new float[cl_ni*cl_nj];
  float* input_buff             = new float[cl_ni*cl_nj];
  for (unsigned i=0;i<cl_ni*cl_nj;i++) {
    vis_buff[i]=1.0f;
    change_image_buff[i]=0.0f;
    change_exp_image_buff[i]=0.0f;
  }
  int count=0;
  for (unsigned int j=0;j<cl_nj;++j) {
    for (unsigned int i=0;i<cl_ni;++i) {
      input_buff[count] = 0.0f;
      exp_image_buff[count]=0.0;
      if (i<img_view->ni() && j< img_view->nj()) {
          input_buff[count]=(*img_view)(i,j);
          exp_image_buff[count]=(*exp_img_view)(i,j);
      }
      ++count;
    }
  }
  
  //prepare image buffers (GPU)
  bocl_mem_sptr in_image=new bocl_mem(device->context(),input_buff,cl_ni*cl_nj*sizeof(float),"input image buffer");
  in_image->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  bocl_mem_sptr exp_image=new bocl_mem(device->context(),exp_image_buff,cl_ni*cl_nj*sizeof(float),"expected image buffer");
  exp_image->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  bocl_mem_sptr change_image=new bocl_mem(device->context(),change_image_buff,cl_ni*cl_nj*sizeof(float),"change image buffer");
  change_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  bocl_mem_sptr change_exp_image=new bocl_mem(device->context(),change_exp_image_buff,cl_ni*cl_nj*sizeof(float),"change exp image buffer");
  change_exp_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

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
  for (int i=0; i<100; ++i) output_arr[i] = 0.0f;
  bocl_mem_sptr  cl_output=new bocl_mem(device->context(), output_arr, sizeof(float)*100, "output buffer");
  cl_output->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // bit lookup buffer
  cl_uchar lookup_arr[256];
  boxm2_ocl_util::set_bit_lookup(lookup_arr);
  bocl_mem_sptr lookup=new bocl_mem(device->context(), lookup_arr, sizeof(cl_uchar)*256, "bit lookup buffer");
  lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
  
  //----------------------------------------------------------------------------
  // STEP ONE: Do 1x1 Change Detection Pass
  //----------------------------------------------------------------------------
  //For each ID in the visibility order, grab that block
  vcl_vector<boxm2_block_id> vis_order = scene->get_vis_blocks( (vpgl_perspective_camera<double>*) cam.ptr());
  vcl_vector<boxm2_block_id>::iterator id;
  for (id = vis_order.begin(); id != vis_order.end(); ++id)
  {
    //choose correct render kernel
    boxm2_block_metadata mdata = scene->get_block_metadata(*id);
    bocl_kernel* kern =  kernels[identifier][0];

    //write the image values to the buffer
    vul_timer transfer;
    bocl_mem* blk       = opencl_cache->get_block(*id);
    bocl_mem* blk_info  = opencl_cache->loaded_block_info();
    bocl_mem* alpha     = opencl_cache->get_data<BOXM2_ALPHA>(*id);
    bocl_mem* mog       = opencl_cache->get_data(*id,data_type);
    transfer_time += (float) transfer.all();

    ////3. SET args
    kern->set_arg( blk_info );
    kern->set_arg( blk );
    kern->set_arg( alpha );
    kern->set_arg( mog );
    kern->set_arg( persp_cam.ptr() );
    kern->set_arg( in_image.ptr() );
    kern->set_arg( exp_image.ptr() );
    kern->set_arg( change_image.ptr() );
    kern->set_arg( change_exp_image.ptr() );
    kern->set_arg( img_dim.ptr());
    kern->set_arg( cl_output.ptr() );
    kern->set_arg( lookup.ptr() );
    kern->set_arg( vis_image.ptr() );

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
  change_image->read_to_buffer(queue);
  change_exp_image->read_to_buffer(queue);
  vis_image->read_to_buffer(queue);
    
  //----------------------------------------------------------------------------
  // STEP TWO: If adaptive do second pass
  //----------------------------------------------------------------------------
  if( n > 1 )
  {
    //do some bookkeeping before second pass
    float* prob_change_buff = new float[cl_ni*cl_nj]; 
    count=0;
    for (unsigned int j=0; j<cl_nj; ++j) {
      for (unsigned int i=0; i<cl_ni; ++i) {
        float change             = change_image_buff[count]; 
        float vis                = vis_buff[count]; 
        float prob_change        = 1.0f / (1.0f + change+vis); 
        
        //store prob change, initialize max_change
        prob_change_buff[count] = prob_change; 

        //reset those that are high enough
        if(prob_change > PROB_THRESH) {
          change_image_buff[count]     = 0.0f; 
          change_exp_image_buff[count] = 0.0f; 
        }
        
        //store pass one vis buff, used later in normalize
        ++count;
      }
    }
    //set the two change images back up
    change_image->write_to_buffer(queue);
    change_exp_image->write_to_buffer(queue); 

    //set prob_change cl buffer
    bocl_mem_sptr prob_change = new bocl_mem(device->context(),prob_change_buff,cl_ni*cl_nj*sizeof(float),"pass one prob change buffer");
    prob_change->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

    //run CD for each pixel in a nxn neighborhood
    int half = n/2; 
    for(int oi=-half; oi<=half; ++oi) 
    {
      for(int oj=-half; oj<=half; ++oj) 
      {
        int oi_buff[1] = {oi}; 
        int oj_buff[1] = {oj}; 
        bocl_mem_sptr oi_mem = new bocl_mem(device->context(),oi_buff, sizeof(int),"offset i buffer");
        bocl_mem_sptr oj_mem = new bocl_mem(device->context(),oj_buff, sizeof(int),"offset j buffer");
        oi_mem->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
        oj_mem->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

        //create gpu buff for p2_vis
        float* p2_vis_buff = new float[cl_nj*cl_ni]; 
        vcl_fill(p2_vis_buff, p2_vis_buff+cl_nj*cl_ni, 1.0f); 
        bocl_mem_sptr p2_vis      = new bocl_mem(device->context(),p2_vis_buff, cl_ni*cl_nj*sizeof(float),"pass one visibility buffer");
        p2_vis->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
        
        //create nxn change buffers
        float* nxn_change_buff       = new float[cl_ni*cl_nj];
        float* nxn_change_exp_buff   = new float[cl_ni*cl_nj];
        vcl_fill(nxn_change_buff, nxn_change_buff+cl_nj*cl_ni, 0.0f); 
        vcl_fill(nxn_change_exp_buff, nxn_change_exp_buff+cl_nj*cl_ni, 0.0f); 
        bocl_mem_sptr nxn_change     = new bocl_mem(device->context(),nxn_change_buff, cl_ni*cl_nj*sizeof(float),"pass two change buffer");
        bocl_mem_sptr nxn_change_exp = new bocl_mem(device->context(),nxn_change_exp_buff, cl_ni*cl_nj*sizeof(float),"pass two change exp buffer");
        nxn_change->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
        nxn_change_exp->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

        //run change detection per block
        for (id = vis_order.begin(); id != vis_order.end(); ++id)
        {
          //choose correct render kernel
          boxm2_block_metadata mdata = scene->get_block_metadata(*id);
          bocl_kernel*         kern  = kernels[identifier][1];

          //write the image values to the buffer
          vul_timer transfer;
          bocl_mem* blk       = opencl_cache->get_block(*id);
          bocl_mem* blk_info  = opencl_cache->loaded_block_info();
          bocl_mem* alpha     = opencl_cache->get_data<BOXM2_ALPHA>(*id);
          bocl_mem* mog       = opencl_cache->get_data(*id,data_type);
          transfer_time      += (float) transfer.all();

          ////3. SET args
          kern->set_arg( blk_info );
          kern->set_arg( blk );
          kern->set_arg( alpha );
          kern->set_arg( mog );
          kern->set_arg( oi_mem.ptr() );    //offset i
          kern->set_arg( oj_mem.ptr() );    //offset j
          kern->set_arg( persp_cam.ptr() );
          kern->set_arg( in_image.ptr() );
          kern->set_arg( exp_image.ptr() );
          kern->set_arg( nxn_change.ptr() );        //pass two change image
          kern->set_arg( nxn_change_exp.ptr() );    //pass two change image exp
          kern->set_arg( prob_change.ptr() );         //pass one probability of change (for adaptive-ness)
          kern->set_arg( img_dim.ptr());
          kern->set_arg( cl_output.ptr() );
          kern->set_arg( lookup.ptr() );
          kern->set_arg( p2_vis.ptr() );
          
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
        nxn_change->read_to_buffer(queue); 
        nxn_change_exp->read_to_buffer(queue); 
        
        //set max change image
        for(int i=0; i<cl_ni*cl_nj; ++i) {
          //if 1x1 change was high enough...
          if(prob_change_buff[i] > PROB_THRESH) {
            
            if(nxn_change_buff[i] > change_image_buff[i])
              change_image_buff[i] = nxn_change_buff[i]; 
              
            if(nxn_change_exp_buff[i] > change_exp_image_buff[i])
              change_exp_image_buff[i] = nxn_change_exp_buff[i]; 
          }
        }
        
        //clean up this pass
        delete [] p2_vis_buff; 
        delete [] nxn_change_buff; 
        delete [] nxn_change_exp_buff;
      }
    }
    
    //clean up adaptive image buffers
    delete [] prob_change_buff; 
    
    //write to change buffer 
    change_image->write_to_buffer(queue); 
    change_exp_image->write_to_buffer(queue); 
  }
  
  
  //----------------------------------------------------------------------------
  // STEP THREE: Do normalize pass on change image
  //----------------------------------------------------------------------------
  bocl_kernel* normalize_change_kernel =  kernels[identifier][2];
  {
    // Image Dimensions
    int rbelief_buff[1] = {0};
    if(norm_type=="raybelief") 
      rbelief_buff[0] = 1; 
    bocl_mem_sptr rbelief = new bocl_mem(device->context(), rbelief_buff, sizeof(int), "rbelief buffer");
    rbelief->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
    
    normalize_change_kernel->set_arg( change_image.ptr() );
    normalize_change_kernel->set_arg( change_exp_image.ptr() );
    normalize_change_kernel->set_arg( vis_image.ptr() );
    normalize_change_kernel->set_arg( img_dim.ptr());
    normalize_change_kernel->set_arg( rbelief.ptr());
    normalize_change_kernel->execute( queue, 2, local_threads, global_threads);
    clFinish(queue);
    gpu_time += normalize_change_kernel->exec_time();

    //clear render kernel args so it can reset em on next execution
    normalize_change_kernel->clear_args();
  }
  // read out expected image
  change_image->read_to_buffer(queue);
  vcl_cout<<"Change Detection GPU Time: " << gpu_time << " ms" << vcl_endl;
  
  //----------------------------------------------------------------------------
  //prep output images
  //----------------------------------------------------------------------------
  //store change image
  vil_image_view<float>* change_img_out = new vil_image_view<float>(ni,nj);
  for (unsigned c=0;c<nj;c++)
    for (unsigned r=0;r<ni;r++)
      (*change_img_out)(r,c) = change_image_buff[c*cl_ni+r];
  vil_save(*change_img_out, "change_test.tiff");
      
  //store rgb change image
  vil_image_view_base_sptr rgb_sptr =  new vil_image_view<vxl_byte>(ni,nj, 4); 
  vil_image_view<vxl_byte>* change_rgb_out = (vil_image_view<vxl_byte>*) rgb_sptr.ptr(); 
  for (unsigned c=0; c<nj; c++) {
    for (unsigned r=0; r<ni; r++) {
      (*change_rgb_out)(r,c,0) = (vxl_byte) ( change_image_buff[c*cl_ni+r] * 255.0f); 
      (*change_rgb_out)(r,c,1) = (vxl_byte) ( (*img_view)(r,c) * 255.0f/8.0f ); 
      (*change_rgb_out)(r,c,2) = (vxl_byte) 0; 
      (*change_rgb_out)(r,c,3) = (vxl_byte) 255; 
    }
  }

  //cleanup the image buffers
  delete [] change_image_buff;
  delete [] change_exp_image_buff;
  delete [] vis_buff;
  delete [] exp_image_buff;
  delete [] input_buff;
  
  clReleaseCommandQueue(queue);
  i=0;
  // store scene smaprt pointer
  pro.set_output_val<vil_image_view_base_sptr>(i++, change_img_out);
  pro.set_output_val<vil_image_view_base_sptr>(i++, rgb_sptr);
  return true;
}
