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
#include <boxm/ocl/view/boxm_ocl_draw_glbuffer_tableau.h>
#include <boxm/ocl/boxm_render_image_manager.h>
#include <boxm/ocl/boxm_render_ocl_scene_manager.h>

#include <boxm/ocl/boxm_ocl_utils.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_calibration_matrix.h>
#include <vgui/internals/trackball.h>

#if defined(WIN32)
    #include <windows.h>
#endif

void
boxm_ocl_draw_glbuffer_tableau::setup_gl_matrices()
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
boxm_ocl_draw_glbuffer_tableau::boxm_ocl_draw_glbuffer_tableau():
  c_mouse_rotate(vgui_LEFT),
  c_mouse_translate(vgui_RIGHT),
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

    //scene_=0;
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        vcl_cout<< "Error: "<<glewGetErrorString(err)<<vcl_endl;
    }
}
boxm_ocl_draw_glbuffer_tableau::~boxm_ocl_draw_glbuffer_tableau()
{
  
}
bool 
boxm_ocl_draw_glbuffer_tableau::init(boxm_ocl_scene * scene, unsigned ni, unsigned nj, vpgl_perspective_camera<double> * cam)
{
    ni_=ni;
    nj_=nj;
    default_cam_=(*cam);
    cam_=(*cam); //default cam
    scene_=scene;
    vil_image_view<float> expected(ni_,nj_);

    //if(boxm_scene<boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > >* s 
    //    = dynamic_cast<boxm_scene<boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> >>*> (scene.as_pointer()))
    //{
    //    scene_=s;
    //}
    //if(boxm_scene<boct_tree<short, boxm_sample<BOXM_APM_SIMPLE_GREY> > >* s 
    //    = dynamic_cast<boxm_scene<boct_tree<short, boxm_sample<BOXM_APM_SIMPLE_GREY> >>*> (scene.as_pointer()))
    //{
    //    scene_=s;
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
        CL_GL_CONTEXT_KHR, (cl_context_properties) glXGetCurrentContext(),
        CL_WGL_HDC_KHR, (cl_context_properties) glXGetCurrentDC(),
        CL_CONTEXT_PLATFORM, (cl_context_properties) platform_id[0],
        0
    };
#else
    cl_context_properties props[] = 
    {
        CL_GL_CONTEXT_KHR, (cl_context_properties) glXGetCurrentContext(),
        CL_WGL_HDC_KHR, (cl_context_properties) glXGetCurrentDrawable(),
        CL_CONTEXT_PLATFORM, (cl_context_properties) platform_id[0],
        0
    };
#endif
    ray_mgr->context_ = clCreateContext(props, 1, &ray_mgr->devices()[0], NULL, NULL, &status);
    int bundle_dim=8;  ray_mgr->set_bundle_ni(bundle_dim);  ray_mgr->set_bundle_nj(bundle_dim);

    ray_mgr->init_ray_trace(scene_, &cam_, expected);
    bool good=true;
    good=good && ray_mgr->set_scene_data()
        && ray_mgr->set_all_blocks() 
        && ray_mgr->set_scene_data_buffers()
        && ray_mgr->set_tree_buffers();

    // run the raytracing
    good=good && ray_mgr->set_persp_camera(cam)
        && ray_mgr->set_persp_camera_buffers()
        && ray_mgr->set_input_image()
        && ray_mgr->set_input_image_buffers()
        && ray_mgr->set_image_dims_buffers();
    if (pbuffer_) {
        // delete old buffer
        clReleaseMemObject(ray_mgr->image_buf_);
        glDeleteBuffers(1, &pbuffer_);
    }



    glGenBuffers(1, &pbuffer_);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbuffer_);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, ray_mgr->wni() * ray_mgr->wnj() * sizeof(GLubyte) * 4, 0, GL_STREAM_DRAW);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    //// create OpenCL buffer from GL PBO
    ray_mgr->image_gl_buf_ = clCreateFromGLBuffer(ray_mgr->context(),CL_MEM_WRITE_ONLY, pbuffer_, &status);
    vcl_cout<<error_to_string(status)<<vcl_endl;
    ray_mgr->set_kernel();
    ray_mgr->set_args();
    ray_mgr->set_commandqueue();
    ray_mgr->set_workspace();
    //}

    return true;

}
bool
boxm_ocl_draw_glbuffer_tableau::handle(vgui_event const &e)
{

  if (e.type == vgui_DRAW)
  {

      vcl_cout<<"Quat ["<<token.quat[0]<<","<<token.quat[1]<<","<<token.quat[2]<<","<<token.quat[3]<<"]"<<vcl_endl;
      //vcl_cout<<"Scale "<<token.scale<<vcl_endl;
      vcl_cout<<"trans ["<<token.trans[0]<<","<<token.trans[1]<<","<<token.trans[2]<<"]"<<vcl_endl;
      vcl_cout<<"Fov "<<token.fov<<vcl_endl;


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
  
  event = e;
  if (vgui_drag_mixin::handle(e))
      return true;

  if (vgui_tableau::handle(e))
    return true;


   return false;
}


bool
boxm_ocl_draw_glbuffer_tableau::render_frame()
{

    boxm_render_ocl_scene_manager* ray_mgr = boxm_render_ocl_scene_manager::instance();
    cl_int status= clEnqueueAcquireGLObjects(ray_mgr->command_queue_, 1, &ray_mgr->image_gl_buf_ , 0, 0, 0);
    ray_mgr->set_persp_camera(&cam_);
    ray_mgr->write_persp_camera_buffers();
    ray_mgr->run();

    status=clEnqueueReleaseGLObjects(ray_mgr->command_queue_, 1, &ray_mgr->image_gl_buf_ , 0, 0, 0);
    clFinish( ray_mgr->command_queue_ );

    return true;
}


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

    float wscale = 2.0f / width;
    float hscale = 2.0f / height;
    float delta_r[4];
    trackball(delta_r,
              (wscale*beginx - 1)/2.0, (hscale*beginy - 1)/2.0,
              (wscale*x - 1)/2.0, (hscale*y - 1)/2.0);
    add_quats(delta_r, lastpos.quat, this->token.quat);

    prevx = x;
    prevx = y;
    vnl_quaternion<double> q(token.quat[0],token.quat[1],token.quat[2],token.quat[3]);
    vgl_rotation_3d<double> r(q*default_cam_.get_rotation().as_quaternion());
    cam_.set_rotation(r);

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
    double scalefactor = vcl_pow(5.0, dy);
    this->token.scale = static_cast<float>(/*lastpos.scale* scalefactor */ dy/10.0f);
    cam_.set_camera_center(cam_.get_camera_center()+token.scale*cam_.principal_axis());

    // changed to vcl_pow(5,dy) to vcl_pow(5.0,dy)
    // the first version is ambiguous when overloads exist for vcl_pow
    //double zoomfactor = vcl_pow(5.0,dx);
    //this->token.fov = static_cast<float>(lastpos.fov * zoomfactor);

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

    this->token.trans[0] = static_cast<float>(lastpos.trans[0] - dx * 20);
    this->token.trans[1] = static_cast<float>(lastpos.trans[1] - dy * 20);

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

    //if (beginx != x && beginy != y)
    //{
    //  this->spinning = true;
    //  double delay = event.secs_since(last);

    //  delete spin_data;
    //  spin_data = new vgui_viewer3D_tableau_spin;
    //  spin_data->viewer = this;
    //  spin_data->delay = delay;
    //  for (int i=0; i<4; ++i)
    //    spin_data->delta_r[i] = delta_r[i];

    //  // Fl::add_timeout(delay,spin_callback,(void*)spin_data);

    //  return true;
    //}
  }
  return false;
}
