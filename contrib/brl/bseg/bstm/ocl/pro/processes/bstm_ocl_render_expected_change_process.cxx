// This is brl/bseg/bstm/ocl/pro/processes/bstm_ocl_render_expected_change_process.cxx
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
#include <vil/algo/vil_median.h>


namespace bstm_ocl_render_expected_change_process_globals
{
  const unsigned n_inputs_ = 7;
  const unsigned n_outputs_ = 1;
  vcl_size_t lthreads[2]={8,8};

  static vcl_map<vcl_string,vcl_vector<bocl_kernel*> > kernels;

  void compile_kernel(bocl_device_sptr device,vcl_vector<bocl_kernel*> & vec_kernels, vcl_string opts)
  {
    //gather all render sources... seems like a lot for rendering...
    vcl_vector<vcl_string> src_paths;
    vcl_string source_dir = vcl_string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/bstm/ocl/cl/";
    src_paths.push_back(source_dir + "scene_info.cl");
    src_paths.push_back(source_dir + "pixel_conversion.cl");
    src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
    src_paths.push_back(source_dir + "bit/time_tree_library_functions.cl");
    src_paths.push_back(source_dir + "statistics_library_functions.cl");
    src_paths.push_back(source_dir + "backproject.cl");
    src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
    src_paths.push_back(source_dir + "bit/render_bit_scene.cl");
    src_paths.push_back(source_dir + "expected_functor.cl");
    src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");

    //set kernel options
    vcl_string options = opts;


    options += "-D RENDER_LAMBERT -D RENDER_CHANGE -D STEP_CELL=step_cell_render_change(aux_args,data_ptr_tt,d*linfo->block_len)";

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

  }
}

bool bstm_ocl_render_expected_change_process_cons(bprb_func_process& pro)
{
  using namespace bstm_ocl_render_expected_change_process_globals;

  //process takes 1 input
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "bstm_scene_sptr";
  input_types_[2] = "bstm_opencl_cache_sptr";
  input_types_[3] = "vpgl_camera_double_sptr";
  input_types_[4] = "unsigned";
  input_types_[5] = "unsigned";
  input_types_[6] = "float"; // time


  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);

}

bool bstm_ocl_render_expected_change_process(bprb_func_process& pro)
{
  using namespace bstm_ocl_render_expected_change_process_globals;

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

  //get scene data type and appTypeSize
  vcl_string data_type,label_data_type;
  int apptypesize,label_apptypesize;
  vcl_vector<vcl_string> valid_types;

  data_type = bstm_data_traits<BSTM_CHANGE>::prefix();
  apptypesize = bstm_data_traits<BSTM_CHANGE>::datasize();

  vcl_string options = bstm_ocl_util::mog_options(data_type);

  //: create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue(device->context(),*(device->device_id()), CL_QUEUE_PROFILING_ENABLE,&status);
  if (status!=0) return false;
  vcl_string identifier=device->device_identifier()+options;

  // compile the kernel
  if (kernels.find(identifier)==kernels.end())
  {
    vcl_cout<<"===========Compiling kernels==========="<<vcl_endl;
    vcl_vector<bocl_kernel*> ks;
    compile_kernel(device,ks,options);
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

  render_time = render_expected_image( scene, device, opencl_cache, queue,
                                    cam, exp_image, vis_image, exp_img_dim,
                                    data_type, kernels[identifier][0], lthreads, cl_ni, cl_nj, apptypesize,time);



//  {
//    vcl_size_t gThreads[] = {cl_ni,cl_nj};
//    bocl_kernel* normalize_kern= kernels[identifier][1];
//    normalize_kern->set_arg( exp_image.ptr() );
//    normalize_kern->set_arg( vis_image.ptr() );
//    normalize_kern->set_arg( exp_img_dim.ptr());
//    normalize_kern->execute( queue, 2, lthreads, gThreads);
//    clFinish(queue);
//
//    //clear render kernel args so it can reset em on next execution
//    normalize_kern->clear_args();
//    render_time += normalize_kern->exec_time();
//  }


  float all_time = rtime.all();
  vcl_cout<<"Total Render time: "<<render_time <<" ms"<<vcl_endl;

  vis_image->read_to_buffer(queue);
  exp_image->read_to_buffer(queue);


  vil_image_view< float >* exp_img_out = new vil_image_view< float >(ni,nj);
  int numFloats = 4;
  int count = 0;
  for (unsigned c=0;c<nj;++c) {
    for (unsigned r=0;r<ni;++r,count+=numFloats) {
      (*exp_img_out)(r,c) =  (1-buff[count]) ;
    }
  }


  int medfilt_halfsize = 2;
  vil_image_view<float>* out_img = new vil_image_view< float >(ni,nj);
  out_img->fill(0);

  vcl_vector<int> strel_vec_i, strel_vec_j;
  for (int i=-medfilt_halfsize; i <= medfilt_halfsize; ++i)
    for (int j=-medfilt_halfsize; j <= medfilt_halfsize; ++j) {
      strel_vec_i.push_back(i);
      strel_vec_j.push_back(j); }

  vil_structuring_element strel(strel_vec_i,strel_vec_j);
  vil_median(*exp_img_out,*out_img,strel);


  delete [] vis_buff;
  delete [] buff;
  delete exp_img_out;

  // read out expected image
  clReleaseCommandQueue(queue);

  //store render time
  int argIdx = 0;
  pro.set_output_val<vil_image_view_base_sptr>(argIdx++, out_img);
  return true;
}
