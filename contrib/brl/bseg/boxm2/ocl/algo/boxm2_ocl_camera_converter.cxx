#include "boxm2_ocl_camera_converter.h"
//
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <vcl_where_root_dir.h>
#include <vpgl/algo/vpgl_camera_convert.h>

//Default private variables to null/0
bocl_kernel* boxm2_ocl_camera_converter::persp_to_generic_kernel = nullptr;
std::map<std::string, bocl_kernel*> boxm2_ocl_camera_converter::kernels_;

//takes in an unknown camera (must be vpgl_generic or perspective)
// cam, it's cl_ni, nj, and creates ray image
void boxm2_ocl_camera_converter::compute_ray_image( bocl_device_sptr & device,
                                                    cl_command_queue & queue,
                                                    vpgl_camera_double_sptr & cam,
                                                    unsigned cl_ni,
                                                    unsigned cl_nj,
                                                    bocl_mem_sptr & ray_origins,
                                                    bocl_mem_sptr & ray_directions,
                                                    std::size_t i_min,
                                                    std::size_t j_min,
                                                    bool create_ray_o_d_buffers)
{
  if (cam->type_name() == "vpgl_perspective_camera") {
#ifdef DEBUG
    std::cout<<"Converting perspective cam to generic !!"<<std::endl;
    float convTime =
#endif
      boxm2_ocl_camera_converter::convert_persp_to_generic( device,
                                                            queue,
                                                            (vpgl_perspective_camera<double>*) cam.ptr(),
                                                            ray_origins,
                                                            ray_directions,
                                                            cl_ni, cl_nj,
                                                            i_min, j_min,
                                                            create_ray_o_d_buffers);
#ifdef DEBUG
    std::cout<<"Camera Convert Time: "<<convTime<<" ms"<<std::endl;
#endif
    return;
  }
  else if (cam->type_name() == "vpgl_generic_camera") {
    //set the ray images, and write to buffer
    boxm2_ocl_util::set_generic_camera(cam, (cl_float*) ray_origins->cpu_buffer(), (cl_float*) ray_directions->cpu_buffer(), cl_ni, cl_nj,i_min,j_min);
    if(create_ray_o_d_buffers) {
      ray_origins->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
      ray_directions->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
    }
    else {
      ray_origins->write_to_buffer(queue);
      ray_directions->write_to_buffer(queue);
    }
  }
  else if (cam->type_name() == "vpgl_affine_camera") {
    auto * gen_cam = new vpgl_generic_camera<double> ();
    vpgl_camera_double_sptr gcam = gen_cam;
    vpgl_affine_camera<double> & aff_cam = (*  (vpgl_affine_camera<double>*) cam.ptr());
    vpgl_generic_camera_convert::convert(aff_cam,cl_ni, cl_nj, *gen_cam);
    //set the ray images, and write to buffer

    boxm2_ocl_util::set_generic_camera(gcam, (cl_float*) ray_origins->cpu_buffer(), (cl_float*) ray_directions->cpu_buffer(), cl_ni, cl_nj,i_min,j_min);
    if(create_ray_o_d_buffers) {
      ray_origins->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
      ray_directions->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
    }
    else {
      ray_origins->write_to_buffer(queue);
      ray_directions->write_to_buffer(queue);
    }
  }
  else {
    std::cout<<"Camera type "<<cam->type_name()<<" not supported by boxm2_ocl_camera_converter"<<std::endl;
    ray_origins->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
    ray_directions->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  }
}

//converts persp to generic cam on gpu
float boxm2_ocl_camera_converter::convert_persp_to_generic(bocl_device_sptr & device,
                                                           cl_command_queue & queue,
                                                           vpgl_perspective_camera<double>* pcam,
                                                           bocl_mem_sptr & ray_origins,
                                                           bocl_mem_sptr & ray_directions,
                                                           unsigned cl_ni,
                                                           unsigned cl_nj,
                                                           std::size_t i_min,
                                                           std::size_t j_min,
                                                           bool create_ray_o_d_buffers)
{
    float transfer_time=0.0f;
    float gpu_time=0.0f;

    std::string identifier = device->device_identifier();
    if ( kernels_.find(identifier) == kernels_.end() ) {
      std::cout<<"Compiling conversion kernel (should only happen once)..."<<std::endl;
      persp_to_generic_kernel = boxm2_ocl_camera_converter::compile_persp_to_generic_kernel(device);
      kernels_[identifier] = persp_to_generic_kernel;
    }
    persp_to_generic_kernel = kernels_[identifier];

    //sanity check
    if (pcam->type_name() != "vpgl_perspective_camera") {
      std::cout<<"Cannot convert "<<pcam->type_name()<<" to generic cam!!"<<std::endl;
      return 0.0f;
    }

    //std::cout<<"Converting perspective camera"<<std::endl;

    // set persp cam buffer
    auto *cam_buffer= new cl_float[48];
    boxm2_ocl_util::set_persp_camera(pcam, cam_buffer);
    bocl_mem *  persp_cam=new bocl_mem(device->context(), cam_buffer, 3*sizeof(cl_float16), "persp cam buffer");
    persp_cam->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

    ////create dims buffer
    cl_uint dims[] = {(cl_uint) i_min, (cl_uint) j_min, cl_ni, cl_nj};
    bocl_mem_sptr dims_buff = new bocl_mem(device->context(), dims, sizeof(cl_uint4), "camera dimensions buffer");
    dims_buff->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
    if(create_ray_o_d_buffers){
      ray_origins->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
      ray_directions->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
    }
    //2. set global/local thread size
    std::size_t gThreads[] = {cl_ni,cl_nj};
    std::size_t lThreads[] = {8, 8};


    // set arguments
    persp_to_generic_kernel->set_arg( persp_cam );
    persp_to_generic_kernel->set_arg( ray_origins.ptr() );
    persp_to_generic_kernel->set_arg( ray_directions.ptr() );
    persp_to_generic_kernel->set_arg( dims_buff.ptr());

    //execute kernel
    persp_to_generic_kernel->execute(queue, 2, lThreads, gThreads);
    clFinish(queue);
    ray_origins->read_to_buffer(queue);
    ray_directions->read_to_buffer(queue);
    clFinish(queue);
    gpu_time += persp_to_generic_kernel->exec_time();

    //clear render kernel args so it can reset em on next execution
    persp_to_generic_kernel->clear_args();


    delete persp_cam;

    //delete persp_cam;
    // std::cout<<"Gpu time "<<gpu_time<<" transfer time "<<transfer_time<<std::endl;
    return gpu_time + transfer_time;
}


bocl_kernel* boxm2_ocl_camera_converter::compile_persp_to_generic_kernel(const bocl_device_sptr& device)
{
  //gather all cam convert sources
  std::vector<std::string> src_paths;
  std::string source_dir = boxm2_ocl_util::ocl_src_root();
  src_paths.push_back(source_dir + "scene_info.cl");
  src_paths.push_back(source_dir + "backproject.cl");
  src_paths.push_back(source_dir + "camera_convert.cl");
  std::string options = "";

  //have kernel construct itself using the context and device
  auto* kern = new bocl_kernel();
  kern->create_kernel( &device->context(),
                       device->device_id(),
                       src_paths,
                       "persp_to_generic",   //kernel name
                       options,              //options
                       "boxm2 perspective to generic camera converter kernel"); //kernel identifier (for error checking)
  return kern;
}
