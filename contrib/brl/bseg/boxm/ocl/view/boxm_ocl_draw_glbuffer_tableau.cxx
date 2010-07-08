#ifdef UNIX
    #if defined(__APPLE__) || defined(MACOSX)
        #include <OpenGL/OpenGL.h>
    #else
        #include <GL/glx.h>
    #endif
#endif


#if defined (__APPLE__) || defined(MACOSX)
  #define GL_SHARING_EXTENSION "cl_APPLE_gl_sharing"
#else
  #define GL_SHARING_EXTENSION "cl_khr_gl_sharing"
  // GLEW and GLUT includes
  #include <GL/glew.h>
  
#endif
#include <boxm/ocl/view/boxm_ocl_draw_glbuffer_tableau.h>
#include <boxm/ocl/boxm_render_image_manager.h>
#include <boxm/ocl/boxm_ocl_utils.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_calibration_matrix.h>

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
boxm_ocl_draw_glbuffer_tableau::boxm_ocl_draw_glbuffer_tableau()
{
    pbuffer_=0;
    ni_=640;
    nj_=480;
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
boxm_ocl_draw_glbuffer_tableau::init(boxm_scene_base_sptr scene, unsigned ni, unsigned nj, vpgl_perspective_camera<double> * cam)
{
    ni_=ni;
    nj_=nj;
    cam_=cam; //default cam
    vil_image_view<float> expected(ni_,nj_);
    if(boxm_scene<boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > >* s 
        = dynamic_cast<boxm_scene<boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> >>*> (scene.as_pointer()))
    {
        scene_=s;
    }
    if(boxm_scene<boct_tree<short, boxm_sample<BOXM_APM_SIMPLE_GREY> > >* s 
        = dynamic_cast<boxm_scene<boct_tree<short, boxm_sample<BOXM_APM_SIMPLE_GREY> >>*> (scene.as_pointer()))
    {
        scene_=s;
        boxm_render_image_manager<boxm_sample<BOXM_APM_SIMPLE_GREY> >* ray_mgr
            = boxm_render_image_manager<boxm_sample<BOXM_APM_SIMPLE_GREY> >::instance();
        int status=0;
        cl_platform_id platform_id[1];
        status = clGetPlatformIDs (1, platform_id, NULL);
        if (status!=CL_SUCCESS) {
            vcl_cout<<error_to_string(status);
            return 0;
        }  
        cl_context_properties props[] =
        {
            CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(),
            CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(),
            CL_CONTEXT_PLATFORM, (cl_context_properties)platform_id[0],
            0
        };
        ray_mgr->context_ = clCreateContext(props, 1, &ray_mgr->devices()[0], NULL, NULL, &status);
        int bundle_dim=8;  ray_mgr->set_bundle_ni(bundle_dim);  ray_mgr->set_bundle_nj(bundle_dim);
        
        ray_mgr->init_ray_trace(s, cam_, expected);
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
    }

    return true;

}
bool
boxm_ocl_draw_glbuffer_tableau::handle(vgui_event const &e)
{

  if (e.type == vgui_DRAW)
  {

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
  }


  if (e.type == vgui_BUTTON_DOWN && e.button == vgui_LEFT )
  {

      vpgl_calibration_matrix<double> cmatrix=cam_->get_calibration();
      double f=cmatrix.focal_length();

      if(f>0.05)
          cmatrix.set_focal_length(f-0.05);
      else
          cmatrix.set_focal_length(1.0);
      cam_->set_calibration(cmatrix);
      this->post_redraw();
  }
 if (e.type == vgui_BUTTON_DOWN && e.button == vgui_RIGHT )
  {

      vpgl_calibration_matrix<double> cmatrix=cam_->get_calibration();
      double f=cmatrix.focal_length();

      if(f>2.0)
          cmatrix.set_focal_length(f+0.05);
      else
          cmatrix.set_focal_length(1.0);
      cam_->set_calibration(cmatrix);
      this->post_redraw();
  }
   return false;
}


bool
boxm_ocl_draw_glbuffer_tableau::render_frame()
{
    if(boxm_scene<boct_tree<short, boxm_sample<BOXM_APM_SIMPLE_GREY> > >* scene 
        = dynamic_cast<boxm_scene<boct_tree<short, boxm_sample<BOXM_APM_SIMPLE_GREY> >>*> (scene_.as_pointer()))
    {


        boxm_render_image_manager<boxm_sample<BOXM_APM_SIMPLE_GREY> >* ray_mgr = boxm_render_image_manager<boxm_sample<BOXM_APM_SIMPLE_GREY> >::instance();
        cl_int status= clEnqueueAcquireGLObjects(ray_mgr->command_queue_, 1, &ray_mgr->image_gl_buf_ , 0, 0, 0);
        ray_mgr->set_persp_camera(cam_);
        ray_mgr->write_persp_camera_buffers();
        ray_mgr->run();

        status=clEnqueueReleaseGLObjects(ray_mgr->command_queue_, 1, &ray_mgr->image_gl_buf_ , 0, 0, 0);
        clFinish( ray_mgr->command_queue_ );

        return true;
    }
    else 
    {
        vcl_cout<<"Undefined tree type "<<vcl_endl;
        return false;
    }
}
