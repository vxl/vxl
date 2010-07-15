#ifdef UNIX
    #if defined(__APPLE__) || defined(MACOSX)
        #include <OpenGL/OpenGL.h>
    #else
        #include <GL/glew.h>
        #include <GL/glx.h>
    #endif
#endif

#if defined(WIN32)
    #include <GL/glew.h>
    #include <windows.h>
#endif
#if defined (__APPLE__) || defined(MACOSX)
  #define GL_SHARING_EXTENSION "cl_APPLE_gl_sharing"
#else
  #define GL_SHARING_EXTENSION "cl_khr_gl_sharing"
#endif
#include <boxm/ocl/view/boxm_ocl_draw_glbuffer_tableau.h>
#include <boxm/ocl/boxm_render_image_manager.h>
#include <boxm/ocl/boxm_render_ocl_scene_manager.h>
#include <boxm/ocl/boxm_update_ocl_scene_manager.h>

#include <boxm/ocl/boxm_ocl_utils.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_calibration_matrix.h>
#include <vgui/internals/trackball.h>
#include <vgui/vgui_modifier.h>
#include <vgl/vgl_distance.h>



//: Constructor 
boxm_ocl_draw_glbuffer_tableau::boxm_ocl_draw_glbuffer_tableau():
                                  c_mouse_rotate(vgui_LEFT),
                                  c_mouse_translate(vgui_LEFT, vgui_CTRL),
                                  c_mouse_zoom(vgui_MIDDLE),
                                  default_cam_(),cam_()
{
    pbuffer_=0;
    ni_=640;
    nj_=480;

    trackball(token.quat , 0.0, 0.0, 0.0, 0.0);
    token.scale = 1.0;
    token.trans[0] = 0;
    token.trans[1] = 0;
    token.trans[2] = -10;
    home = token;
   
    stare_point_ = vgl_homg_point_3d<double>(0.0,0.0,0.0);
}

//: Destructor 
boxm_ocl_draw_glbuffer_tableau::~boxm_ocl_draw_glbuffer_tableau()
{ }

//: initialize tableau properties
bool boxm_ocl_draw_glbuffer_tableau::init(boxm_ocl_scene * scene, 
                          unsigned ni, unsigned nj, 
                          vpgl_perspective_camera<double> * cam)
{
  //set image dimensions, camera and scene 
  ni_=ni;
  nj_=nj;
  default_cam_=(*cam);
  cam_=(*cam); //default cam
  scene_=scene;
  
  do_init_ocl = true;
  return true;
}

bool boxm_ocl_draw_glbuffer_tableau::init_ocl(){
  
  GLenum err = glewInit();
  if (GLEW_OK != err)
  {
      /* Problem: glewInit failed, something is seriously wrong. */
      vcl_cout<< "Error: "<<glewGetErrorString(err)<<vcl_endl;
  }
  
  vil_image_view<float> expected(ni_,nj_);

  //initialize the render manager     
  boxm_render_ocl_scene_manager* ray_mgr
      = boxm_render_ocl_scene_manager::instance();
  int status=0;
  cl_platform_id platform_id[1];
  status = clGetPlatformIDs (1, platform_id, NULL);
  if (status!=CL_SUCCESS) {
      vcl_cout<<error_to_string(status);
      return 0;
  }  
  
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
  ray_mgr->context_ = clCreateContext(props, 1, &ray_mgr->devices()[0], NULL, NULL, &status);

//  initialize ray trace using the input camera  
  int bundle_dim = 8;  
  ray_mgr->set_bundle_ni(bundle_dim);  
  ray_mgr->set_bundle_nj(bundle_dim);
  ray_mgr->init_ray_trace(scene_, &cam_, expected);
  bool good=true;  good = good && ray_mgr->set_scene_data()
      && ray_mgr->set_all_blocks() 
      && ray_mgr->set_scene_data_buffers()
      && ray_mgr->set_tree_buffers();

  // run the raytracing
  good = good && ray_mgr->set_persp_camera(&cam_)
      && ray_mgr->set_persp_camera_buffers()
      && ray_mgr->set_input_image()
      && ray_mgr->set_input_image_buffers()
      && ray_mgr->set_image_dims_buffers();
      
  // delete old buffer
  if (pbuffer_) {
      clReleaseMemObject(ray_mgr->image_buf_);
      glDeleteBuffers(1, &pbuffer_);
  }

  //generate glBuffer, and bind to ray_mgr->image_gl_buf_
  glGenBuffers(1, &pbuffer_);
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbuffer_);
  glBufferData(GL_PIXEL_UNPACK_BUFFER, ray_mgr->wni() * ray_mgr->wnj() * sizeof(GLubyte) * 4, 0, GL_STREAM_DRAW);
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

  //create OpenCL buffer from GL PBO, and set kernel and arguments
  ray_mgr->image_gl_buf_ = clCreateFromGLBuffer(ray_mgr->context(),
                                                CL_MEM_WRITE_ONLY, 
                                                pbuffer_, 
                                                &status);
  ray_mgr->set_kernel();
  ray_mgr->set_args();
  ray_mgr->set_commandqueue();
  ray_mgr->set_workspace();  
  
}

//: sets up viewport and GL Modes 
void boxm_ocl_draw_glbuffer_tableau::setup_gl_matrices()
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

//: Handles tableau events (drawing and keys) 
bool boxm_ocl_draw_glbuffer_tableau::handle(vgui_event const &e)
{
  //draw handler - called on post_draw()
  if (e.type == vgui_DRAW)
  {
      if(do_init_ocl){
        this->init_ocl();
        do_init_ocl = false;
      }
    
      //vcl_cout<<"Quat ["<<token.quat[0]<<","<<token.quat[1]<<","<<token.quat[2]<<","<<token.quat[3]<<"]"<<vcl_endl;
      //vcl_cout<<"trans ["<<token.trans[0]<<","<<token.trans[1]<<","<<token.trans[2]<<"]"<<vcl_endl;
      //vcl_cout<<"Fov "<<token.fov<<vcl_endl;
      vcl_cout<<"Cam center: "<<cam_.get_camera_center()<<vcl_endl
              <<"stare point: "<<stare_point_<<vcl_endl;
      vcl_cout<<cam_<<vcl_endl;
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
  if (e.type == vgui_RESHAPE)
  {
      this->setup_gl_matrices();
      this->post_redraw();
  }
  
  //handle key presses - these mostly just rearrange the camera
  if (e.type == vgui_KEY_PRESS) 
  {
    if(e.key == vgui_key_CTRL('r')) {
        vcl_cout<<"resetting to initial camera view"<<vcl_endl;
        cam_.set_camera_center(default_cam_.get_camera_center());
        cam_.set_rotation(default_cam_.get_rotation());
        token = this->home;
        lastpos = this->home;
    }
    else if(e.key == vgui_key_CTRL('x')) {
        vcl_cout<<"looking down X axis at the origin"<<vcl_endl;
        cam_.set_camera_center(vgl_point_3d<double>(2,0,0));
        cam_.look_at(vgl_homg_point_3d<double>(0,0,0));
    }
    else if(e.key == vgui_key_CTRL('y')) {
        vcl_cout<<"looking down Y axis at the origin"<<vcl_endl;
        cam_.set_camera_center(vgl_point_3d<double>(0,2,0));
        cam_.look_at(vgl_homg_point_3d<double>(0,0,0));
    }
    else if(e.key == vgui_key_CTRL('z')) { 
        vcl_cout<<"looking down Z axis at the origin"<<vcl_endl;
        cam_.set_camera_center(vgl_point_3d<double>(0,0,2));
        cam_.look_at(vgl_homg_point_3d<double>(0,0,0));
    }
    this->post_redraw();
  }
  
  event = e;
  if (vgui_drag_mixin::handle(e))
    return true;

  if (vgui_tableau::handle(e))
    return true;

  return false;
}

//: calls on ray manager to render frame into the pbuffer_ 
bool boxm_ocl_draw_glbuffer_tableau::render_frame()
{
    boxm_render_ocl_scene_manager* ray_mgr = boxm_render_ocl_scene_manager::instance();
    cl_int status = clEnqueueAcquireGLObjects(ray_mgr->command_queue_, 1, 
                                             &ray_mgr->image_gl_buf_ , 0, 0, 0);
    if (!ray_mgr->check_val(status,CL_SUCCESS,"clEnqueueAcquireGLObjects failed. (gl_image)"+error_to_string(status)))
        return false;

    ray_mgr->set_persp_camera(&cam_);
    ray_mgr->write_persp_camera_buffers();
    ray_mgr->run();
    status = clEnqueueReleaseGLObjects(ray_mgr->command_queue_, 1, &ray_mgr->image_gl_buf_ , 0, 0, 0);
    clFinish( ray_mgr->command_queue_ );
    return true;
}

//: called on all mouse down events on tableau 
bool boxm_ocl_draw_glbuffer_tableau::mouse_down(int x, int y, vgui_button /*button*/, vgui_modifier /*modifier*/)
{  
  if (c_mouse_rotate(event) || c_mouse_translate(event) || c_mouse_zoom(event)) {
    beginx = x;
    beginy = y;
    lastpos = this->token;
    last = event;
    return true;
  }
  return false;
}

//: called on mouse movement while mousedown is true
bool boxm_ocl_draw_glbuffer_tableau::mouse_drag(int x, int y, vgui_button button, vgui_modifier modifier)
{
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

    vcl_cout<<cam_;
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

    //dy determines change in theta
    

    // changed to vcl_pow(5,dy) to vcl_pow(5.0,dy)
    // the first version is ambiguous when overloads exist for vcl_pow
    double scalefactor = vcl_pow(5.0, dy);
    this->token.scale = static_cast<float>(/*lastpos.scale* scalefactor */ dy/10.0f);
    cam_.set_camera_center(cam_.get_camera_center()+token.scale*cam_.principal_axis());
    vcl_cout<<cam_;


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
    
    vcl_cout<<"Translating: begin ("<<beginx<<","<<beginy<<") -> ("<<x<<","<<y<<")"<<vcl_endl;

    double scale = .01;

    vgl_point_3d<double> p0(beginx/width, beginy/height, 0.0);
    vgl_point_3d<double> p1(x/width, y/height, 0.0);
    
    vcl_cout<<"cam matrix: "<<cam_.get_matrix()<<vcl_endl;
    vgl_point_3d<double> wp0 = cam_.get_rotation().inverse()*(p0-cam_.get_translation());
    vgl_point_3d<double> wp1 = cam_.get_rotation().inverse()*(p1-cam_.get_translation());
    vgl_vector_3d<double> tVec = scale*(wp0-wp1);

    
    vcl_cout<<"  begin world point:"<<wp0<<vcl_endl;
    vcl_cout<<"  end world point  :"<<wp1<<vcl_endl;
    
    vgl_point_3d<double> cam_center;
    cam_.set_camera_center(cam_center + tVec);
    
    //this->token.trans[0] = static_cast<float>(lastpos.trans[0] - dx * 20);
    //this->token.trans[1] = static_cast<float>(lastpos.trans[1] - dy * 20);

    this->post_redraw();
    return true;
  }
  return false;
}

bool boxm_ocl_draw_glbuffer_tableau::mouse_up(int x, int y, vgui_button button, vgui_modifier modifier)
{
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
