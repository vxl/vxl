#include "boxm2_change_detection_tableau.h"
//:
// \file
#include <boxm/ocl/boxm_ocl_utils.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_calibration_matrix.h>
#include <vgui/internals/trackball.h>
#include <vgui/vgui_modifier.h>
#include <vgui/vgui_event.h>
#include <vgl/vgl_distance.h>
#include <vcl_sstream.h>


//: Constructor
boxm2_change_detection_tableau::boxm2_change_detection_tableau()
{
  pbuffer_=0;
  ni_=640;
  nj_=480;
  do_change_=false;
}

//: initialize tableau properties
bool boxm2_change_detection_tableau::init(vcl_string scene_file,
                                          unsigned ni,
                                          unsigned nj,
                                          vpgl_perspective_camera<double> * cam,
                                          vcl_vector<vcl_string> cam_files,
                                          vcl_vector<vcl_string> img_files)
{
  //set image dimensions, camera and scene
  ni_ = ni;
  nj_ = nj;
  do_init_ocl = true;
  cam_files_ = cam_files;
  img_files_ = img_files;

  //create the scene
  scene_ = new boxm2_scene(scene_file);

  return true;
}


//: Handles tableau events (drawing and keys)
bool boxm2_change_detection_tableau::handle(vgui_event const &e)
{
  //draw handler - called on post_draw()
  if (e.type == vgui_DRAW)
  {
    if (do_init_ocl) {
      this->init_clgl();
      do_init_ocl = false;
    }

    this->change_frame();
    this->setup_gl_matrices();
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glRasterPos2i(0, 1);
    glPixelZoom(1,-1);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, pbuffer_);
    glDrawPixels(ni_, nj_, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
  }
  else if (e.type == vgui_KEY_PRESS && e.key == vgui_key('n')) {
    vcl_cout<<"Change Detection"<<vcl_endl;
    this->post_redraw();
  }

  return false;
}
//: updates given a random frame
float boxm2_change_detection_tableau::change_frame()
{
  change_count_++;
  cl_int status = clEnqueueAcquireGLObjects( *gpu_pro_->get_queue(), 1,
                                             &change_.image()->buffer() , 0, 0, 0);
  cd_img_->zero_gpu_buffer( *gpu_pro_->get_queue() );
  if (!check_val(status,CL_SUCCESS,"clEnqueueAcquireGLObjects failed. (gl_image)"+error_to_string(status)))
    return false;

  //pickup a random frame
  int curr_frame = change_count_%cam_files_.size();
  vcl_cout<<"Cam "<<cam_files_[curr_frame]<<'\n'
          <<"Image "<<img_files_[curr_frame]<<vcl_endl;

  //build the camera from file
  vcl_ifstream ifs(cam_files_[curr_frame].c_str());
  vpgl_perspective_camera<double>* pcam = new vpgl_perspective_camera<double>;
  if (!ifs.is_open()) {
      vcl_cerr << "Failed to open file " << cam_files_[curr_frame] << '\n';
      return -1;
  }
  ifs >> *pcam;
  vpgl_camera_double_sptr cam_sptr(pcam);
  brdb_value_sptr brdb_cam = new brdb_value_t<vpgl_camera_double_sptr>(cam_sptr);

  //load image from file
  vil_image_view_base_sptr loaded_image = vil_load(img_files_[curr_frame].c_str());
  vil_image_view<float>* floatimg = new vil_image_view<float>(loaded_image->ni(), loaded_image->nj(), 1);
  if (vil_image_view<vxl_byte> *img_byte = dynamic_cast<vil_image_view<vxl_byte>*>(loaded_image.ptr()))
    vil_convert_stretch_range_limited(*img_byte, *floatimg, vxl_byte(0), vxl_byte(255), 0.0f, 1.0f);
  else {
    vcl_cerr << "Failed to load image " << img_files_[curr_frame] << '\n';
    return -1;
  }
  //create input image buffer
  vil_image_view_base_sptr floatimg_sptr(floatimg);
  brdb_value_sptr brdb_inimg = new brdb_value_t<vil_image_view_base_sptr>(floatimg_sptr);

  //create out exp image buffer
  vil_image_view<unsigned int>* expimg = new vil_image_view<unsigned int>(ni_, nj_);
  expimg->fill(0);
  brdb_value_sptr brdb_expimg = new brdb_value_t<vil_image_view_base_sptr>(expimg);

  //create out cd image buffer
  vil_image_view_base_sptr cdimg = new vil_image_view<float>(ni_, nj_);
  brdb_value_sptr brdb_cdimg = new brdb_value_t<vil_image_view_base_sptr>(cdimg);

  //create vis image buffer
  vil_image_view<float>* visimg = new vil_image_view<float>(ni_, nj_);
  visimg->fill(1.0f);
  brdb_value_sptr brdb_visimg = new brdb_value_t<vil_image_view_base_sptr>(visimg);

  //create generic scene
  brdb_value_sptr brdb_scene = new brdb_value_t<boxm2_scene_sptr>(scene_);
  vcl_vector<brdb_value_sptr> input;
  input.push_back(brdb_scene);
  input.push_back(brdb_cam);
  input.push_back(brdb_expimg);
  input.push_back(brdb_visimg);
  //initoutput vector
  vcl_vector<brdb_value_sptr> output;

  //initialize the GPU render process
  gpu_pro_->run(&change_render_, input, output);
  //gpu_pro_->finish();

  //create vis image buffer
  vil_image_view<float>* visimg1 = new vil_image_view<float>(ni_, nj_);
  visimg1->fill(1.0f);
  brdb_value_sptr brdb_visimg1 = new brdb_value_t<vil_image_view_base_sptr>(visimg1);

  //set inputs
  vcl_vector<brdb_value_sptr> input1;
  input1.push_back(brdb_scene);
  input1.push_back(brdb_cam);
  input1.push_back(brdb_inimg);
  input1.push_back(brdb_expimg);
  input1.push_back(brdb_cdimg);
  input1.push_back(brdb_visimg1);

  //execute gpu_update
  gpu_pro_->run(&change_, input1, output);

  status = clEnqueueReleaseGLObjects( *gpu_pro_->get_queue(), 1, &change_.image()->buffer(), 0, 0, 0);
    vil_image_view<vxl_byte> byte_img(ni_, nj_);
  for (unsigned int i=0; i<ni_; ++i)
    for (unsigned int j=0; j<nj_; ++j)
      byte_img(i,j) =  (unsigned char)( vcl_floor((*visimg1)(i,j)*255.0f) );   //just grab the first byte (all foura r the same)
  vil_save( byte_img, "f:/test.png");
  clFinish( *change_.command_queue() );
  return gpu_pro_->exec_time();
}


//: private helper method to init_clgl stuff (gpu processor)
bool boxm2_change_detection_tableau::init_clgl()
{
  //get relevant blocks
  vcl_cout<<"Data Path: "<<scene_->data_path()<<vcl_endl;
  cache_ = new boxm2_nn_cache(scene_.ptr());

  //initialize gpu pro / manager
  gpu_pro_ = boxm2_opencl_processor::instance();
  gpu_pro_->context_ = create_clgl_context();
  gpu_pro_->set_scene(scene_.ptr());
  gpu_pro_->set_cpu_cache(cache_);
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
  cd_img_ = new bocl_mem(gpu_pro_->context(), /*(void*) pbuffer_*/ NULL, ni_*nj_*sizeof(GLubyte)*4, "exp image (gl) buffer");
  cd_img_->set_gl_buffer(clgl_buffer_);

  //initialize the GPU render process
  //initialize the GPU change detection process
  change_.init_kernel(&gpu_pro_->context(), &gpu_pro_->devices()[0]);
  change_.set_image(cd_img_);

  change_render_.init_kernel(&gpu_pro_->context(), &gpu_pro_->devices()[0]);

  return true;
}

//: private helper method to create clgl context using cl_context properties
cl_context boxm2_change_detection_tableau::create_clgl_context()
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
  cl_device_id device = gpu_pro_->devices()[0];
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
  cl_device_id device = gpu_pro_->devices()[0];
  ComputeContext = clCreateContext(props, 1, &device, NULL, NULL, &status);
#endif

  if (status!=CL_SUCCESS) {
    vcl_cout<<"Error: Failed to create a compute CL/GL context!" << error_to_string(status) <<vcl_endl;
    return 0;
  }
  return ComputeContext;
}


//: sets up viewport and GL Modes
void boxm2_change_detection_tableau::setup_gl_matrices()
{
  GLint vp[4]; // x,y,w,h
  glGetIntegerv(GL_VIEWPORT, vp);
  glViewport(0, 0, vp[2], vp[3]);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, 1.0, 0.0, 1.0, 0.0, 1.0);
}
