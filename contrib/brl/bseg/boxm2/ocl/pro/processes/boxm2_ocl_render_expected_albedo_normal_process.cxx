// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_render_expected_albedo_normal_process.cxx
//:
// \file
// \brief  A process for rendering the scene using the normalized_albedo_array appearance model.
//
// \author Daniel Crispell
// \date Dec 19, 2011

#include <bprb/bprb_func_process.h>

#include <vcl_fstream.h>
#include <vcl_algorithm.h>
#include <vcl_vector.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <vil/vil_image_view.h>
#include <vgl/vgl_vector_3d.h>

//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <boxm2/ocl/algo/boxm2_ocl_render_expected_image_function.h>
#include <vul/vul_timer.h>


namespace boxm2_ocl_render_expected_albedo_normal_process_globals
{
  const unsigned n_inputs_ = 6;
  const unsigned n_outputs_ = 3;
  vcl_size_t lthreads[2]={8,8};

  static vcl_map<vcl_string,vcl_vector<bocl_kernel*> > kernels;
  bool compile_kernel(bocl_device_sptr device,vcl_vector<bocl_kernel*> & vec_kernels, vcl_string opts)
  {
    //gather all render sources... seems like a lot for rendering...
    vcl_vector<vcl_string> src_paths;
    vcl_string source_dir = boxm2_ocl_util::ocl_src_root();
    src_paths.push_back(source_dir + "scene_info.cl");
    src_paths.push_back(source_dir + "pixel_conversion.cl");
    src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
    src_paths.push_back(source_dir + "backproject.cl");
    src_paths.push_back(source_dir + "statistics_library_functions.cl");
    src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
    src_paths.push_back(source_dir + "bit/render_bit_scene.cl");
    src_paths.push_back(source_dir + "expected_functor.cl");
    src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");

    //set kernel options
    vcl_string options = opts + " -D RENDER_ALBEDO_NORMAL ";
    options += " -D DETERMINISTIC ";
    options += " -D STEP_CELL=step_cell_render_albedo_normal(aux_args,data_ptr,d*linfo->block_len,vis)";

    //have kernel construct itself using the context and device
    bocl_kernel * ray_trace_kernel=new bocl_kernel();

    if (!ray_trace_kernel->create_kernel( &device->context(),
                                          device->device_id(),
                                          src_paths,
                                          "render_albedo_normal",   //kernel name
                                          options,              //options
                                          "boxm2 opencl render_albedo_normal")) { //kernel identifier (for error checking)
       vcl_cerr << "create_kernel (render kernel) returned error.\n";
       return false;
    }
    vec_kernels.push_back(ray_trace_kernel);

    return true;
  }
}

bool boxm2_ocl_render_expected_albedo_normal_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_render_expected_albedo_normal_process_globals;

  //process takes 6 inputs
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "boxm2_scene_sptr";
  input_types_[2] = "boxm2_opencl_cache_sptr";
  input_types_[3] = "vpgl_camera_double_sptr";
  input_types_[4] = "unsigned";
  input_types_[5] = "unsigned";

  // process has 3 outputs:
  // output[0]: expected albedo image
  // output[1]: normal image
  // output[2]: visibility image
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";
  output_types_[1] = "vil_image_view_base_sptr";
  output_types_[2] = "vil_image_view_base_sptr";

  bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);

  return good;
}

bool boxm2_ocl_render_expected_albedo_normal_process(bprb_func_process& pro)
{
  using namespace boxm2_ocl_render_expected_albedo_normal_process_globals;

  vul_timer rtime;
  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  //get the inputs
  bocl_device_sptr device= pro.get_input<bocl_device_sptr>(0);
  boxm2_scene_sptr scene =pro.get_input<boxm2_scene_sptr>(1);

  boxm2_opencl_cache_sptr opencl_cache= pro.get_input<boxm2_opencl_cache_sptr>(2);
  vpgl_camera_double_sptr cam= pro.get_input<vpgl_camera_double_sptr>(3);
  unsigned ni=pro.get_input<unsigned>(4);
  unsigned nj=pro.get_input<unsigned>(5);

  bool found_appearance = false;
  vcl_string data_type,options;
  vcl_vector<vcl_string> apps = scene->appearances();

  for (unsigned int i=0; i<apps.size(); ++i) {
    if ( apps[i] == boxm2_data_traits<BOXM2_NORMAL_ALBEDO_ARRAY>::prefix() )
    {
      data_type = apps[i];
      found_appearance = true;
      // boxm2_data_traits<BOXM2_NORMAL_ALBEDO_ARRAY>::datasize();
    }
  }
  if (!found_appearance) {
    vcl_cout<<"BOXM2_OCL_RENDER_IMAGE_NAA_PROCESS ERROR: scene doesn't have BOXM2_NORMAL_ALBEDO_ARRAY data type" << vcl_endl;
    return false;
  }

  //: create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue(device->context(),*(device->device_id()),
                                                CL_QUEUE_PROFILING_ENABLE,&status);
  if (status!=0) return false;
  vcl_string identifier=device->device_identifier()+options;

  // compile the kernel
  if (kernels.find(identifier)==kernels.end())
  {
    vcl_cout<<"===========Compiling kernels==========="<<vcl_endl;
    vcl_vector<bocl_kernel*> ks;
    if (!compile_kernel(device,ks,options)){
      vcl_cerr << "ERROR: compile_kernel returned false.\n";
      return false;
    }
    kernels[identifier]=ks;
  }

  unsigned cl_ni=RoundUp(ni,lthreads[0]);
  unsigned cl_nj=RoundUp(nj,lthreads[1]);
  float* exp_buff = new float[4*cl_ni*cl_nj];
  vcl_fill(exp_buff, exp_buff + 4*cl_ni*cl_nj, 0.0f);

  bocl_mem_sptr exp_albedo_normal = opencl_cache->alloc_mem(4*cl_ni*cl_nj*sizeof(float), exp_buff,"exp normal albedo buffer");

  exp_albedo_normal->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  int img_dim_buff[4];
  img_dim_buff[0] = 0;   img_dim_buff[2] = ni;
  img_dim_buff[1] = 0;   img_dim_buff[3] = nj;
  bocl_mem_sptr exp_img_dim=new bocl_mem(device->context(), img_dim_buff, sizeof(int)*4, "image dims");
  exp_img_dim->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // visibility image
  float* vis_buff = new float[cl_ni*cl_nj];
  vcl_fill(vis_buff, vis_buff + cl_ni*cl_nj, 1.0f);
  bocl_mem_sptr vis_image=opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float),vis_buff,"vis image buffer");
  vis_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // run expected image function
  render_expected_albedo_normal(scene, device, opencl_cache, queue,
                                cam, exp_albedo_normal, vis_image, exp_img_dim,
                                kernels[identifier][0], lthreads, cl_ni, cl_nj);

  // read out expected image
  exp_albedo_normal->read_to_buffer(queue);
  vis_image->read_to_buffer(queue);

  vil_image_view<float>* exp_albedo_out=new vil_image_view<float>(ni,nj);
  for (unsigned r=0;r<nj;r++)
    for (unsigned c=0;c<ni;c++)
      (*exp_albedo_out)(c,r)= exp_buff[4*(r*cl_ni+c)];

  vil_image_view<float>* exp_normal_out = new vil_image_view<float>(ni,nj,3);
  for (unsigned r=0;r<nj;r++)
    for (unsigned c=0;c<ni;c++) {
      (*exp_normal_out)(c,r,0)=exp_buff[4*(r*cl_ni+c)+1];
      (*exp_normal_out)(c,r,1)=exp_buff[4*(r*cl_ni+c)+2];
      (*exp_normal_out)(c,r,2)=exp_buff[4*(r*cl_ni+c)+3];
    }

  vil_image_view<float>* mask_out=new vil_image_view<float>(ni,nj);
  for (unsigned r=0;r<nj;r++)
    for (unsigned c=0;c<ni;c++)
      (*mask_out)(c,r)= 1.0f - vis_buff[r*cl_ni+c];

  // normalize images
  const float mask_thresh = 0.1;
  for (unsigned int j=0; j<nj; ++j) {
     for (unsigned int i=0; i<ni; ++i) {
        float &mask_val = (*mask_out)(i,j);
        float &expected_albedo = (*exp_albedo_out)(i,j);
        vgl_vector_3d<double> expected_normal(0.0, 0.0, 0.0);
        if (mask_val > mask_thresh) {
          expected_albedo /= mask_val;
          expected_normal = vgl_vector_3d<double>((*exp_normal_out)(i,j,0),(*exp_normal_out)(i,j,1),(*exp_normal_out)(i,j,2));
          if (expected_normal.z() < -1e-3){
             vcl_cout << "ERROR: expected_normal = " << expected_normal << vcl_endl;
          }
          else {
             normalize(expected_normal);
          }
        }
        else {
           expected_albedo = 0.0f;
           mask_val = 0.0f;
        }
        (*exp_normal_out)(i,j,0) = expected_normal.x();
        (*exp_normal_out)(i,j,1) = expected_normal.y();
        (*exp_normal_out)(i,j,2) = expected_normal.z();
     }
  }

  vcl_cout<<"Total Render time: "<<rtime.all()<<" ms"<<vcl_endl;

  opencl_cache->unref_mem(exp_albedo_normal.ptr());
  opencl_cache->unref_mem(vis_image.ptr());

  delete [] vis_buff;
  delete [] exp_buff;
  clReleaseCommandQueue(queue);

  // store scene smaprt pointer
  pro.set_output_val<vil_image_view_base_sptr>(0, exp_albedo_out);
  pro.set_output_val<vil_image_view_base_sptr>(1, exp_normal_out);
  pro.set_output_val<vil_image_view_base_sptr>(2, mask_out);
  return true;
}
