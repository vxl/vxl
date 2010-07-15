#ifdef UNIX
    #if defined(__APPLE__) || defined(MACOSX)
        #include <OpenGL/OpenGL.h>
    #else
        #include <GL/glew.h>
        #include <GL/glx.h>
    #endif
#endif


#if defined (__APPLE__) || defined(MACOSX)
  #define GL_SHARING_EXTENSION "cl_APPLE_gl_sharing"
#else
  #define GL_SHARING_EXTENSION "cl_khr_gl_sharing"
#endif
#include <boxm/ocl/view/boxm_ocl_update_tableau.h>
#include <boxm/ocl/boxm_render_image_manager.h>
#include <boxm/ocl/boxm_render_ocl_scene_manager.h>
#include <boxm/ocl/boxm_update_ocl_scene_manager.h>

#include <boxm/ocl/boxm_ocl_utils.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_calibration_matrix.h>
#include <vgui/internals/trackball.h>
#include <vgl/vgl_distance.h>

//image load 
#include <vpgl/vpgl_perspective_camera.h>
#include <vil/vil_convert.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>


#if defined(WIN32)
    #include <windows.h>
#endif

//: Constructor 
boxm_ocl_update_tableau::boxm_ocl_update_tableau():
                                  c_mouse_rotate(vgui_LEFT),
                                  c_mouse_translate(vgui_LEFT, vgui_CTRL),
                                  c_mouse_zoom(vgui_MIDDLE),
                                  default_cam_(),cam_()
{
    pbuffer_=0;
    ni_=640;
    nj_=480;
    curr_frame_ = 0;
    stare_point_ = vgl_homg_point_3d<double>(0.0,0.0,0.0);
    do_update_ = true;
}

//: Destructor 
boxm_ocl_update_tableau::~boxm_ocl_update_tableau()
{ 
  //boxm_update_ocl_scene_manager* updt_mgr = 
    //boxm_update_ocl_scene_manager::instance();
  //updt_mgr->finish_online_processing();
  //updt_mgr->clean_update();
  //scene_->save();
}

//: initialize tableau properties
bool boxm_ocl_update_tableau::init(boxm_ocl_scene * scene, 
                                  unsigned ni, unsigned nj, 
                                  vpgl_perspective_camera<double> * cam,
                                  vcl_vector<vcl_string> cam_files,
                                  vcl_vector<vcl_string> img_files)
{
  //set image dimensions, camera and scene 
  ni_ = ni;
  nj_ = nj;
  default_cam_ = (*cam);
  cam_ = (*cam); //default cam
  scene_ = scene;
  vil_image_view<float> expected(ni_,nj_);
  
  //directory of cameras
  cam_files_ = cam_files;
  img_files_ = img_files;
  
  //initialize OCL stuff
  do_init_ocl_ = true;
  return true;
}


//-------------- OpenCL methods (render and update) ------------------//

//: calls on update manager to update model 
bool boxm_ocl_update_tableau::update_model() {
  vcl_cout<<"UPDATING MODEL!!!"<<vcl_endl;
  
  //make sure you get a valid frame... 
  if(curr_frame_ >= cam_files_.size()) curr_frame_ = 0;
  vcl_cout<<"Cam "<<cam_files_[curr_frame_]
          <<" Image "<<img_files_[curr_frame_]<<vcl_endl;

  //load up the update manager instance 
  boxm_update_ocl_scene_manager* updt_mgr = boxm_update_ocl_scene_manager::instance();
   
  //build the camera from file 
  vcl_ifstream ifs(cam_files_[curr_frame_].c_str());
  vpgl_perspective_camera<double>* pcam = new vpgl_perspective_camera<double>;
  if (!ifs.is_open()) {
      vcl_cerr << "Failed to open file " << cam_files_[curr_frame_] << vcl_endl;
      return -1;
  }
  ifs >> *pcam;
   
  //load image from file 
  vil_image_view_base_sptr loaded_image = vil_load(img_files_[curr_frame_].c_str());
  vil_image_view<float> floatimg(loaded_image->ni(), loaded_image->nj(), 1);
  if (vil_image_view<vxl_byte> *img_byte
        = dynamic_cast<vil_image_view<vxl_byte>*>(loaded_image.ptr()))
    vil_convert_stretch_range_limited(*img_byte ,floatimg, vxl_byte(0), vxl_byte(255), 0.0f, 1.0f);
  else {
    vcl_cerr << "Failed to load image " << img_files_[curr_frame_] << vcl_endl;
    return -1;
  }
  
  //run the opencl update business 
  updt_mgr->set_input_image(floatimg);
  updt_mgr->write_image_buffer();
  updt_mgr->set_persp_camera(pcam);
  updt_mgr->write_persp_camera_buffers();
  updt_mgr->online_processing();
  return true;
}

//: calls on ray manager to render frame into the pbuffer_ 
bool boxm_ocl_update_tableau::render_frame() {
    vcl_cout<<"RENDERING FRAME!"<<vcl_endl;
    boxm_update_ocl_scene_manager* ocl_mgr = boxm_update_ocl_scene_manager::instance();
    cl_int status = clEnqueueAcquireGLObjects(ocl_mgr->command_queue_, 1, 
                                              &ocl_mgr->image_gl_buf_ , 0, 0, 0);
    if (!ocl_mgr->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (input_image)"+error_to_string(status)))
        return false;

    ocl_mgr->set_persp_camera(&cam_);
    ocl_mgr->write_persp_camera_buffers();
    ocl_mgr->rendering();
    status = clEnqueueReleaseGLObjects(ocl_mgr->command_queue_, 1, &ocl_mgr->image_gl_buf_ , 0, 0, 0);
    clFinish( ocl_mgr->command_queue_ );
    return true;
}



//: initializes ocl_related stuff 
// (must be called AFTER window is popped up so QT 
// can get the correct context )
bool boxm_ocl_update_tableau::init_ocl() {
  
  //only ever do this method once. 
  do_init_ocl_ = false;
  
  //initialize GLEW 
  GLenum err = glewInit();
  if (GLEW_OK != err)
  {
    /* Problem: glewInit failed, something is seriously wrong. */
    vcl_cout<< "Error: "<<glewGetErrorString(err)<<vcl_endl;
  }
  
  //initialize the render manager     
  boxm_update_ocl_scene_manager* ocl_mgr = boxm_update_ocl_scene_manager::instance();
  int status=0;
  cl_platform_id platform_id[1];
  status = clGetPlatformIDs (1, platform_id, NULL);
  if (status!=CL_SUCCESS) {
      vcl_cout<<error_to_string(status)<<vcl_endl;
      return 0;
  }  
  
  //get context properties 
#ifdef WIN32
  cl_context_properties props[] =
  {
      CL_GL_CONTEXT_KHR, (cl_context_properties) wglGetCurrentContext(),
      CL_WGL_HDC_KHR, (cl_context_properties) wglGetCurrentDC(),
      CL_CONTEXT_PLATFORM, (cl_context_properties) platform_id[0],
      0
  };
#else
  cl_context_properties props[] = 
  {
      CL_GL_CONTEXT_KHR, (cl_context_properties)glXGetCurrentContext(), 
      CL_GLX_DISPLAY_KHR, (cl_context_properties)glXGetCurrentDisplay(), 
      CL_CONTEXT_PLATFORM, (cl_context_properties) platform_id[0], 
      0
  };
#endif

  //create OpenCL context with display properties determined above 
  ocl_mgr->context_ = clCreateContext(props, 1, &ocl_mgr->devices()[0], NULL, NULL, &status);

  ///initialize update  
  vil_image_view<float> expected(ni_,nj_);
  int bundle_dim = 8;  
  ocl_mgr->set_bundle_ni(bundle_dim);  
  ocl_mgr->set_bundle_nj(bundle_dim);
  ocl_mgr->init_update(scene_, &cam_, expected);
  if (!ocl_mgr->setup_norm_data(true, 0.5f, 0.25f))
    return -1;   
  ocl_mgr->setup_online_processing();
  ocl_mgr->online_processing();

  
  //need to set ray trace 
  //delete old buffer
  if (pbuffer_) {
      clReleaseMemObject(ocl_mgr->image_buf_);
      glDeleteBuffers(1, &pbuffer_);
  }

  //generate glBuffer, and bind to ocl_mgr->image_gl_buf_
  glGenBuffers(1, &pbuffer_);
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbuffer_);
  glBufferData(GL_PIXEL_UNPACK_BUFFER, ocl_mgr->wni() * ocl_mgr->wnj() * sizeof(GLubyte) * 4, 0, GL_STREAM_DRAW);
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

  //create OpenCL buffer from GL PBO, and set kernel and arguments
  ocl_mgr->image_gl_buf_ = clCreateFromGLBuffer(ocl_mgr->context(),
                                                CL_MEM_WRITE_ONLY, 
                                                pbuffer_, 
                                                &status);
  return true;
}


//: sets up viewport and GL Modes 
void boxm_ocl_update_tableau::setup_gl_matrices() {
  GLint vp[4]; // x,y,w,h
  glGetIntegerv(GL_VIEWPORT, vp);
  glViewport(0, 0, vp[2], vp[3]);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, 1.0, 0.0, 1.0, 0.0, 1.0);
}

//--------------------- Event Handlers -------------------------------//


//: Handles tableau events (drawing and keys) 
bool boxm_ocl_update_tableau::handle(vgui_event const &e) {
  //draw handler - called on post_draw()
  if (e.type == vgui_DRAW) {
    if(do_init_ocl_) this->init_ocl();
  
    vcl_cout<<"redrawing"<<vcl_endl;
    do_update_ = false;
    //vcl_cout<<"Quat ["<<token.quat[0]<<","<<token.quat[1]<<","<<token.quat[2]<<","<<token.quat[3]<<"]"<<vcl_endl;
    //vcl_cout<<"trans ["<<token.trans[0]<<","<<token.trans[1]<<","<<token.trans[2]<<"]"<<vcl_endl;
    //vcl_cout<<"Fov "<<token.fov<<vcl_endl;
    //vcl_cout<<"Cam center: "<<cam_.get_camera_center()<<vcl_endl
            //<<"stare point: "<<stare_point_<<vcl_endl;
    //vcl_cout<<cam_<<vcl_endl;
    this->render_frame();
    this->setup_gl_matrices();
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glRasterPos2i(0, 1);
    glPixelZoom(1,-1);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, pbuffer_);
    glDrawPixels(ni_, nj_, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
    return true;
  }
  else if (e.type == vgui_RESHAPE)
  {
    //this->setup_gl_matrices();
    this->post_redraw();
  }
  //handle key presses - these mostly just rearrange the camera
  else if (e.type == vgui_KEY_PRESS) 
  {
    vcl_cout<<"Key pressed: "<<vcl_endl;
    if(e.key == vgui_key('u')) {
      vcl_cout<<"Continuing update"<<vcl_endl;
      do_update_ = true;
      this->post_idle_request();
    }
    else if(e.key == vgui_key('r')) {
      vcl_cout<<"resetting to initial camera view"<<vcl_endl;
      cam_.set_camera_center(default_cam_.get_camera_center());
      cam_.set_rotation(default_cam_.get_rotation());
      this->post_redraw();
    }
    else if(e.key == vgui_key('x')) {
      vcl_cout<<"looking down X axis at the origin"<<vcl_endl;
      cam_.set_camera_center(vgl_point_3d<double>(2,0,0));
      cam_.look_at(vgl_homg_point_3d<double>(0,0,0));
      this->post_redraw();
    }
    else if(e.key == vgui_key('y')) {
      vcl_cout<<"looking down Y axis at the origin"<<vcl_endl;
      cam_.set_camera_center(vgl_point_3d<double>(0,2,0));
      cam_.look_at(vgl_homg_point_3d<double>(0,0,0));
      this->post_redraw();
    }
    else if(e.key == vgui_key('z')) { 
      vcl_cout<<"looking down Z axis at the origin"<<vcl_endl;
      cam_.set_camera_center(vgl_point_3d<double>(0,0,2));
      cam_.look_at(vgl_homg_point_3d<double>(0,0,0));
      this->post_redraw();
    }
  }
  //HANDLE idle events - do model updating
  else if (e.type == vgui_IDLE) 
  {
    if(do_update_) {
      vcl_cout<<"Idling - i will be updating scene "<<vcl_endl;
      this->update_model();
      return true;
    }
    else {
      vcl_cout<<"done idling "<<vcl_endl;
      return false;
    }
  }
  else if (vgui_drag_mixin::handle(e)) {
    ///vcl_cout<<"Drag mix in handling it"<<vcl_endl;
    return true;
  }
  else if (vgui_tableau::handle(e)) {
    //vcl_cout<<"Vgui tableau handling it"<<vcl_endl;
    return true;
  }
  return false;
}

//: called on all mouse down events on tableau 
bool boxm_ocl_update_tableau::mouse_down(int x, int y, vgui_button /*button*/, vgui_modifier /*modifier*/) {  
  if (c_mouse_rotate(event) || c_mouse_translate(event) || c_mouse_zoom(event)) {
    do_update_ = false;
    beginx = x;
    beginy = y;
    last = event;
    return true;
  }
  return false;
}

//: called on mouse movement while mousedown is true
bool boxm_ocl_update_tableau::mouse_drag(int x, int y, vgui_button button, vgui_modifier modifier) {
  // SPINNING
  if (c_mouse_rotate(button, modifier))
  {
#ifdef DEBUG
    vcl_cerr << "vgui_viewer3D_tableau::mouse_drag: left\n";
#endif

    GLdouble vp[4];
    glGetDoublev(GL_VIEWPORT, vp); // ok
    float width  = (float)vp[2];
    float height = (float)vp[3];

    // get mouse deltas 
    double dx = (beginx - x) / width;
    double dy = (beginy - y) / height;
      
    //cam location in spherical coordinates (this should be relative to the stare point
    vgl_point_3d<double> cam_center = cam_.get_camera_center();
    vgl_point_3d<double> origin(0.0f,0.0f,0.0f);
    double rad = vgl_distance<double>(cam_center, origin);
    double theta = vcl_acos(cam_center.z()/rad);
    double phi = vcl_atan2(cam_center.y(), cam_center.x());
    
    //update theta by a function of dy 
    double angleScale = .01;
    double newTheta = theta - dy * angleScale;
    double newPhi = phi + dx * angleScale;
    vgl_point_3d<double> newCenter(rad * vcl_sin(newTheta) * vcl_cos(newPhi), 
                                   rad * vcl_sin(newTheta) * vcl_sin(newPhi),
                                   rad * vcl_cos(newTheta));
    cam_.set_camera_center(newCenter);
    cam_.look_at(stare_point_);      

    //vcl_cout<<cam_;
    this->post_redraw();
    return true;
  }

  // ZOOMING
  if (c_mouse_zoom(button, modifier))
  {
#ifdef DEBUG
    vcl_cerr << "vgui_viewer3D_tableau::mouse_drag: middle\n";
#endif

    GLdouble vp[4];
    glGetDoublev(GL_VIEWPORT, vp); // ok
    double width = vp[2];
    double height = vp[3];
    double dx = (beginx - x) / width;
    double dy = (beginy - y) / height;

    // changed to vcl_pow(5,dy) to vcl_pow(5.0,dy)
    // the first version is ambiguous when overloads exist for vcl_pow
    double scale = dy/10.0f;
    cam_.set_camera_center(cam_.get_camera_center() + scale*cam_.principal_axis());
    //vcl_cout<<cam_;
    this->post_redraw();
    return true;
  }

  // TRANSLATION
  if (c_mouse_translate(button, modifier)) {
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp); // ok
    double width = (double)vp[2];
    double height = (double)vp[3];

    double dx = (beginx - x) / width;
    double dy = (beginy - y) / height;

    //get film plane, and translate 
    //cam_;

//    this->token.trans[0] = static_cast<float>(lastpos.trans[0] - dx * 20);
//    this->token.trans[1] = static_cast<float>(lastpos.trans[1] - dy * 20);

    this->post_redraw();
    return true;
  }
  return false;
}

bool boxm_ocl_update_tableau::mouse_up(int x, int y, vgui_button button, vgui_modifier modifier) {
  do_update_ = true;

  // SPINNING
  if (c_mouse_rotate(button, modifier))
  {
#ifdef DEBUG
    vcl_cerr << "vgui_viewer3D_tableau::mouse_up: left\n";
#endif

    GLdouble vp[4];
    glGetDoublev(GL_VIEWPORT, vp); // ok
    double width = vp[2];
    double height = vp[3];

    double wscale = 2.0 / width;
    double hscale = 2.0 / height;
    float delta_r[4];
    trackball(delta_r,
              static_cast<float>(wscale*beginx - 1), static_cast<float>(hscale*beginy - 1),
              static_cast<float>(wscale*x - 1), static_cast<float>(hscale*y - 1));

  }
  
  return false;
}
