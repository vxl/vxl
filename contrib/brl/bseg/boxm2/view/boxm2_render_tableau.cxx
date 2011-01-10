#include "boxm2_render_tableau.h"
//:
// \file
#include <boxm/ocl/boxm_ocl_utils.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_calibration_matrix.h>
#include <vgui/internals/trackball.h>
#include <vgui/vgui_modifier.h>
#include <vgl/vgl_distance.h>
#include <vcl_sstream.h>


//: Constructor
boxm2_render_tableau::boxm2_render_tableau()
{
  pbuffer_=0;
  ni_=640;
  nj_=480;
}

//: initialize tableau properties
bool boxm2_render_tableau::init(vcl_string scene_file,
                                unsigned ni, 
                                unsigned nj,
                                vpgl_perspective_camera<double> * cam)
{
  //set image dimensions, camera and scene
  ni_ = ni;
  nj_ = nj;
  cam_   = (*cam);
  default_cam_ = (*cam);
  do_init_ocl = true;
  
  //create the scene
  scene_ = new boxm2_scene(scene_file); 
  
  return true;
}




//: Handles tableau events (drawing and keys)
bool boxm2_render_tableau::handle(vgui_event const &e)
{
  //draw handler - called on post_draw()
  if (e.type == vgui_DRAW)
  {
    if (do_init_ocl){
      this->init_clgl();
      do_init_ocl = false;
    }

    //vcl_cout<<"Cam center: "<<cam_.get_camera_center()<<'\n'
    //        <<"stare point: "<<stare_point_<<vcl_endl;
    //vcl_cout<<cam_<<vcl_endl;
    float gpu_time = this->render_frame();
    this->setup_gl_matrices();
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glRasterPos2i(0, 1);
    glPixelZoom(1,-1);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, pbuffer_);
    glDrawPixels(ni_, nj_, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
#if 0
    glPushMatrix();
    vnl_random rand;
    for (unsigned i=0;i<100000;i++)
    {
      glColor3f(rand.drand32(),rand.drand32(),rand.drand32());
      glBegin (GL_LINES);
      glVertex2f (rand.drand32(-1.0,1.0),rand.drand32(-1.0,1.0));
      glVertex2f (rand.drand32(-1.0,1.0), rand.drand32(-1.0,1.0));
      glEnd ();
    }
    glPopMatrix();
#endif // 0

    //calculate and write fps to status
    vcl_stringstream str;
    str<<"rendering at about "<< (1000.0f / gpu_time) <<" fps ";
    status_->write(str.str().c_str());

    return true;
  }

  if (boxm2_cam_tableau::handle(e))
    return true;

  return false;
}

//: calls on ray manager to render frame into the pbuffer_
float boxm2_render_tableau::render_frame()
{
  //boxm_render_bit_scene_manager* ray_mgr = boxm_render_bit_scene_manager::instance();
  cl_int status = clEnqueueAcquireGLObjects( *gpu_pro_->get_queue(), 1,
                                             &render_.image()->buffer() , 0, 0, 0);
  exp_img_->zero_gpu_buffer( *gpu_pro_->get_queue() );

  if (!check_val(status,CL_SUCCESS,"clEnqueueAcquireGLObjects failed. (gl_image)"+error_to_string(status)))
    return false;

  //set inputs
  vcl_cout<<cam_<<vcl_endl;
  vpgl_camera_double_sptr cam = new vpgl_perspective_camera<double>(cam_); 
  brdb_value_sptr brdb_cam = new brdb_value_t<vpgl_camera_double_sptr>(cam); 
  
  //create output image buffer
  vil_image_view_base_sptr expimg = new vil_image_view<float>(ni_, nj_); 
  brdb_value_sptr brdb_expimg = new brdb_value_t<vil_image_view_base_sptr>(expimg); 
  
  //create vis image buffer
  vil_image_view<float>* visimg = new vil_image_view<float>(ni_, nj_); 
  visimg->fill(1.0f); 
  brdb_value_sptr brdb_visimg = new brdb_value_t<vil_image_view_base_sptr>(visimg); 
  
  //create scene brdbvalue pointer
  brdb_value_sptr brdb_scene = new brdb_value_t<boxm2_scene_sptr>(scene_);
  
  vcl_vector<brdb_value_sptr> input; 
  input.push_back(brdb_scene);
  input.push_back(brdb_cam);
  input.push_back(brdb_expimg); 
  input.push_back(brdb_visimg);
  
  //initoutput vector
  vcl_vector<brdb_value_sptr> output; 

  //initialize the GPU render process
  gpu_pro_->run(&render_, input, output);
  gpu_pro_->finish(); 

  status = clEnqueueReleaseGLObjects( *gpu_pro_->get_queue(), 1, &render_.image()->buffer(), 0, 0, 0);
  clFinish( *render_.command_queue() );
  return gpu_pro_->exec_time();
}


//: private helper method to init_clgl stuff (gpu processor)
bool boxm2_render_tableau::init_clgl()
{
  //get relevant blocks
  vcl_cout<<"Data Path: "<<scene_->data_path()<<vcl_endl; 
  boxm2_nn_cache* dcache = new boxm2_nn_cache(scene_.ptr());

  //initialize gpu pro / manager
  gpu_pro_ = boxm2_opencl_processor::instance();
  gpu_pro_->context_ = create_clgl_context(); 
  gpu_pro_->set_scene(scene_.ptr()); 
  gpu_pro_->set_cpu_cache(dcache); 
  gpu_pro_->init();
 
  // delete old buffer
  if (pbuffer_) {
      clReleaseMemObject(clgl_buffer_);
      glDeleteBuffers(1, &pbuffer_);
  }

  //generate glBuffer, and bind to ray_mgr->image_gl_buf_
  glGenBuffers(1, &pbuffer_);
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbuffer_);
  glBufferData(GL_PIXEL_UNPACK_BUFFER, ni_*nj_*sizeof(GLubyte)*4, 0, GL_STREAM_DRAW);
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

  //create OpenCL buffer from GL PBO, and set kernel and arguments
  int status = 0;
  clgl_buffer_ = clCreateFromGLBuffer(gpu_pro_->context(),
                                      CL_MEM_WRITE_ONLY,
                                      pbuffer_,
                                      &status);
  exp_img_ = new bocl_mem(gpu_pro_->context(), /*(void*) pbuffer_*/ NULL, ni_*nj_*sizeof(GLubyte)*4, "exp image (gl) buffer");
  exp_img_->set_gl_buffer(clgl_buffer_);  
                                      
  //initialize the GPU render process
  render_.init_kernel(&gpu_pro_->context(), &gpu_pro_->devices()[0]); 
  render_.set_image(exp_img_); 
  vcl_cout<<"RENDER IMAGE SET"<<vcl_endl;
                                    
  return true;
}


//: private helper method to create clgl context using cl_context properties
cl_context boxm2_render_tableau::create_clgl_context()
{
  //init glew
  GLenum err = glewInit();
  if (GLEW_OK != err)
    vcl_cout<< "GlewInit Error: "<<glewGetErrorString(err)<<vcl_endl;    // Problem: glewInit failed, something is seriously wrong.
  
  //initialize the render manager
  int status=0;
  cl_platform_id platform_id[1];
  status = clGetPlatformIDs (1, platform_id, NULL);
  if (status!=CL_SUCCESS) {
    vcl_cout<<error_to_string(status);
    return 0;
  }
  cl_device_id device = gpu_pro_->devices()[0]; 
  
  ////create OpenCL context
  cl_context ComputeContext;
#ifdef WIN32
  cl_context_properties props[] =
  {
    CL_GL_CONTEXT_KHR, (cl_context_properties) wglGetCurrentContext(),
    CL_WGL_HDC_KHR, (cl_context_properties) wglGetCurrentDC(),
    CL_CONTEXT_PLATFORM, (cl_context_properties) platform_id[0],
    0
  };
  //create OpenCL context with display properties determined above
  ComputeContext = clCreateContext(props, 1, &device, NULL, NULL, &status);
#elif defined(__APPLE__) || defined(MACOSX)
  CGLContextObj kCGLContext = CGLGetCurrentContext();
  CGLShareGroupObj kCGLShareGroup = CGLGetShareGroup(kCGLContext);

  cl_context_properties props[] = {
    CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE, (cl_context_properties)kCGLShareGroup,
    CL_CONTEXT_PLATFORM, (cl_context_properties) platform_id[0],
    0
  };
  //create a CL context from a CGL share group - no GPU devices must be passed,
  //all CL compliant devices in the CGL share group will be used to create the context. more info in cl_gl_ext.h
  ComputeContext = clCreateContext(props, 0, 0, NULL, NULL, &status);
#else
  cl_context_properties props[] =
  {
      CL_GL_CONTEXT_KHR, (cl_context_properties)glXGetCurrentContext(),
      CL_GLX_DISPLAY_KHR, (cl_context_properties)glXGetCurrentDisplay(),
      CL_CONTEXT_PLATFORM, (cl_context_properties) platform_id[0],
      0
  };
  ComputeContext = clCreateContext(props, 1, &device, NULL, NULL, &status);
#endif

  if (status!=CL_SUCCESS) {
    vcl_cout<<"Error: Failed to create a compute CL/GL context!" << error_to_string(status) <<vcl_endl;
    return 0;
  }
  return ComputeContext;
}

