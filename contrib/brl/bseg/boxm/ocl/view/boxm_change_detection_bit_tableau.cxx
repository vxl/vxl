#include <boxm/ocl/view/boxm_change_detection_bit_tableau.h>
//:
// \file


#include <boxm/ocl/boxm_update_bit_scene_manager.h>
#include <boxm/ocl/boxm_ocl_utils.h>
#include <vpgl/vpgl_calibration_matrix.h>
#include <vgui/internals/trackball.h>
#include <vgl/vgl_distance.h>

//image load
#include <vil/vil_convert.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vgui/vgui_dialog.h>
#include <vil/vil_save.h>
//: Constructor
boxm_change_detection_bit_tableau::boxm_change_detection_bit_tableau()
: pbuffer_(0), ni_(640), nj_(480), curr_frame_(68), do_update_(true)
{
}

//: Destructor
boxm_change_detection_bit_tableau::~boxm_change_detection_bit_tableau()
{
  vcl_cout<<"CHANGE DETECTION TABLEAU DESTROYED"<<vcl_endl;
}

//: initialize tableau properties
bool boxm_change_detection_bit_tableau::init(boxm_ocl_bit_scene * scene,
                                             unsigned ni, unsigned nj,
                                             vcl_vector<vcl_string> cam_files,
                                             vcl_vector<vcl_string> img_files,
                                             vcl_vector<vcl_string> exp_img_files)
{
  //set image dimensions, camera and scene
  ni_ = ni;
  nj_ = nj;
  scene_ = scene;

  count_=0;
  curr_count_=0;
  ////directory of cameras
  cam_files_ = cam_files;
  img_files_ = img_files;
  exp_img_files_ = exp_img_files;

  ////initialize OCL stuff
  do_init_ocl_ = true;
  toggle_old_  = true;
  return true;
}

//: initializes ocl_related stuff
// (must be called AFTER window is popped up so QT
// can get the correct context )
bool boxm_change_detection_bit_tableau::init_ocl()
{
  //initialize GLEW
  GLenum err = glewInit();
  if (GLEW_OK != err)
  {
    /* Problem: glewInit failed, something is seriously wrong. */
    vcl_cout<< "Error: "<<glewGetErrorString(err)<<vcl_endl;
  }

  //initialize the render manager
  boxm_update_bit_scene_manager* updt_mgr
      = boxm_update_bit_scene_manager::instance();
  int status=0;
  cl_platform_id platform_id[1];
  status = clGetPlatformIDs (1, platform_id, NULL);
  if (status!=CL_SUCCESS) {
    vcl_cout<<error_to_string(status)<<vcl_endl;
    return 0;
  }

  cl_context compute_context;
#ifdef WIN32
  cl_context_properties props[] =
  {
    CL_GL_CONTEXT_KHR, (cl_context_properties) wglGetCurrentContext(),
    CL_WGL_HDC_KHR, (cl_context_properties) wglGetCurrentDC(),
    CL_CONTEXT_PLATFORM, (cl_context_properties) platform_id[0],
    0
  };
  //create OpenCL context with display properties determined above
  compute_context = clCreateContext(props, 1, &updt_mgr->devices()[0], NULL, NULL, &status);
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
  compute_context = clCreateContext(props, 0, 0, NULL, NULL, &status);
#else
  cl_context_properties props[] =
  {
    CL_GL_CONTEXT_KHR, (cl_context_properties)glXGetCurrentContext(),
    CL_GLX_DISPLAY_KHR, (cl_context_properties)glXGetCurrentDisplay(),
    CL_CONTEXT_PLATFORM, (cl_context_properties) platform_id[0],
    0
  };
  compute_context = clCreateContext(props, 1, &updt_mgr->devices()[0], NULL, NULL, &status);
#endif

  if (status!=CL_SUCCESS) {
    vcl_cout<<"Error: Failed to create a compute CL/GL context!" << error_to_string(status) <<vcl_endl;
    return 0;
  }

  //set OpenCL context with display properties determined above
  updt_mgr->context_ = compute_context;

  // Set 2d workspace, scene, norm data and then all buffers
  int bundle_dim = 8;
  updt_mgr->set_bundle_ni(bundle_dim);
  updt_mgr->set_bundle_nj(bundle_dim);
  updt_mgr->init_scene(scene_, ni_,nj_);  //THIS SETS WNI AND WNJ used below


  ///initialize update
  //need to set ray trace
  //delete old buffer
  if (pbuffer_) {
    clReleaseMemObject(updt_mgr->image_buf_);
    glDeleteBuffers(1, &pbuffer_);
  }

  //generate glBuffer, and bind to ocl_mgr->image_gl_buf_
  glGenBuffers(1, &pbuffer_);
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbuffer_);
  glBufferData(GL_PIXEL_UNPACK_BUFFER, updt_mgr->wni() * updt_mgr->wnj() * sizeof(GLubyte) * 4, 0, GL_STREAM_DRAW);
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

  //create OpenCL buffer from GL PBO, and set kernel and arguments
  updt_mgr->image_gl_buf_ = clCreateFromGLBuffer(updt_mgr->context(),
                                                 CL_MEM_WRITE_ONLY,
                                                 pbuffer_,
                                                 &status);

  //Finally create input image and persp cam buffers, set args and
  updt_mgr->setup_online_processing();
  curr_frame_=0;

  return true;
}


//-------------- OpenCL methods (render and update) ------------------//
//: calls on update manager to change_detection
bool boxm_change_detection_bit_tableau::change_detection()
{
#ifdef DEBUG
  vcl_cout<<"UPDATING MODEL!!!"<<vcl_endl;
#endif
  count_++; curr_count_++;

  //make sure you get a valid frame...
  if (curr_frame_ >= (int)cam_files_.size())
    curr_frame_ = 0;

  vcl_cout<<"Cam "<<cam_files_[curr_frame_]
          <<"Image "<<img_files_[curr_frame_]<<vcl_endl;

  //load up the update manager instance
  boxm_update_bit_scene_manager* updt_mgr = boxm_update_bit_scene_manager::instance();

  //build the camera from file
  vcl_ifstream ifs(cam_files_[curr_frame_].c_str());
  vpgl_perspective_camera<double>* pcam = new vpgl_perspective_camera<double>;
  if (!ifs.is_open()) {
    vcl_cerr << "Failed to open file " << cam_files_[curr_frame_] << '\n';
    return -1;
  }
  ifs >> *pcam;

  //load image from file
  vil_image_view_base_sptr loaded_image = vil_load(img_files_[curr_frame_].c_str());
  vil_image_view_base_sptr loaded_exp_image = vil_load(exp_img_files_[curr_frame_].c_str());
  vil_image_view<float> floatimg(loaded_image->ni(), loaded_image->nj(), 1);
  vil_image_view<float> exp_floatimg(loaded_exp_image->ni(), loaded_exp_image->nj(), 1);
  if (vil_image_view<vxl_byte> *img_byte = dynamic_cast<vil_image_view<vxl_byte>*>(loaded_image.ptr()))
  {
    vil_convert_stretch_range_limited(*img_byte ,floatimg, vxl_byte(0), vxl_byte(255), 0.0f, 1.0f);
  }
  else {
    vcl_cerr << "Failed to load image " << img_files_[curr_frame_] << '\n';
    return -1;
  }
  if (vil_image_view<vxl_byte> *exp_img_byte = dynamic_cast<vil_image_view<vxl_byte>*>(loaded_exp_image.ptr()))
  {
    vil_convert_stretch_range_limited(*exp_img_byte ,exp_floatimg, vxl_byte(0), vxl_byte(255), 0.0f, 1.0f);
  }
  else {
    vcl_cerr << "Failed to load image " << exp_img_files_[curr_frame_] << '\n';
    return -1;
  }
  //curr_frame_++ ;
  cl_int status = clEnqueueAcquireGLObjects(updt_mgr->command_queue_, 1,
                                            &updt_mgr->image_gl_buf_ , 0, 0, 0);
  if (!updt_mgr->check_val(status,CL_SUCCESS,"tableau::clEnqueueAcquiredGLObjects failed (render_frame)"+error_to_string(status)))
    return false;

  //run the opencl update business (MAKE THIS JUST ONE METHOD, NOT FIVE CALLS)
  updt_mgr->set_input_image(floatimg,exp_floatimg);
  updt_mgr->write_image_buffer();
  updt_mgr->set_persp_camera(pcam);
  updt_mgr->write_persp_camera_buffers();
  if (toggle_old_)
    updt_mgr->change_detection_old();
  else
    updt_mgr->change_detection();

  status = clEnqueueReleaseGLObjects(updt_mgr->command_queue_, 1, &updt_mgr->image_gl_buf_ , 0, 0, 0);
  clFinish( updt_mgr->command_queue_ );

  return true;
}

//: calls on ray manager to render frame into the pbuffer_
//--------------------- Event Handlers -------------------------------//

//: Handles tableau events (drawing and keys)
bool boxm_change_detection_bit_tableau::handle(vgui_event const &e)
{
  //draw handler - called on post_draw()
  if (e.type == vgui_DRAW) {
    if (do_init_ocl_) {
      this->init_ocl();
      do_init_ocl_ = false;
    }

#ifdef DEBUG
    vcl_cout<<"redrawing\n"
            <<"Cam center: "<<cam_.get_camera_center()<<'\n'
            <<"stare point: "<<stare_point_<<vcl_endl;
#endif
    this->change_detection();
    this->setup_gl_matrices();
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glRasterPos2i(0, 1);
    glPixelZoom(1,-1);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, pbuffer_);
    glDrawPixels(ni_, nj_, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);

    //update status
    vcl_stringstream str;
    if (toggle_old_)
      str<<"Old Change Detection: frame "<<curr_frame_;
    else
      str<<"Change Detection: frame "<<curr_frame_;
    status_->write(str.str().c_str());
    return true;
  }
  //handle update command - keyboard press U
  else if (e.type == vgui_KEY_PRESS && e.key == vgui_key('v')) {
    vcl_cout<<"Going Backward"<<vcl_endl;
    //do_update_ = true;
    curr_frame_--;
    this->post_redraw();
  }
  else if (e.type == vgui_KEY_PRESS && e.key == vgui_key('c')) {
    vcl_cout<<"Going Forward"<<vcl_endl;
    //do_update_ = true;
    curr_frame_++;
    this->post_redraw();
  }

  else if (e.type == vgui_KEY_PRESS && e.key == vgui_key('t')) {
    vcl_cout<<"Continuing update"<<vcl_endl;
    toggle_old_ = !toggle_old_;
    this->post_redraw();
  }
  else if (e.type == vgui_KEY_PRESS && e.key == vgui_key('r')) {
    vcl_cout<<"Continuing update"<<vcl_endl;
    curr_frame_=0;
    this->post_redraw();
  }
  //otherwise trigger cam handling events
  return vgui_tableau::handle(e);
}

//: sets up viewport and GL Modes
void boxm_change_detection_bit_tableau::setup_gl_matrices()
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
