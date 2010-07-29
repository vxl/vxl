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

#include <boxm/ocl/view/boxm_ocl_update_tableau.h>
//:
// \file
#if 0
#include <boxm/ocl/boxm_render_image_manager.h>
#include <boxm/ocl/boxm_render_ocl_scene_manager.h>
#endif
#include <boxm/ocl/boxm_update_ocl_scene_manager.h>

#include <boxm/ocl/boxm_ocl_utils.h>
#include <vpgl/vpgl_calibration_matrix.h>
#include <vgui/internals/trackball.h>
#include <vgl/vgl_distance.h>

//image load
#include <vil/vil_convert.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>


//: Constructor
boxm_ocl_update_tableau::boxm_ocl_update_tableau()
{
    pbuffer_=0;
    ni_=640;
    nj_=480;
    curr_frame_ = 0;
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

  count_=0;
  ////directory of cameras
  cam_files_ = cam_files;
  img_files_ = img_files;

  ////initialize OCL stuff
  do_init_ocl_ = true;
  return true;
}

//: initializes ocl_related stuff
// (must be called AFTER window is popped up so QT
// can get the correct context )
bool boxm_ocl_update_tableau::init_ocl()
{
  //initialize GLEW
  GLenum err = glewInit();
  if (GLEW_OK != err)
  {
    /* Problem: glewInit failed, something is seriously wrong. */
    vcl_cout<< "Error: "<<glewGetErrorString(err)<<vcl_endl;
  }

  //initialize the render manager
  boxm_update_ocl_scene_manager* updt_mgr
      = boxm_update_ocl_scene_manager::instance();
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
  updt_mgr->context_ = clCreateContext(props, 1, &updt_mgr->devices()[0], NULL, NULL, &status);

  vcl_cout<<error_to_string(status);
  ///initialize update

  vil_image_view<float> expected(ni_,nj_);
  int bundle_dim = 8;
  updt_mgr->set_bundle_ni(bundle_dim);
  updt_mgr->set_bundle_nj(bundle_dim);
  updt_mgr->init_update(scene_, &cam_, expected);
  if (!updt_mgr->setup_norm_data(true, 0.5f, 0.25f))
    return -1;
  updt_mgr->setup_online_processing();

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
  return true;
}


//-------------- OpenCL methods (render and update) ------------------//
bool boxm_ocl_update_tableau::refine_model()
{
  vcl_cout<<"REFINING MODEL!!!"<<vcl_endl;
  boxm_update_ocl_scene_manager* updt_mgr = boxm_update_ocl_scene_manager::instance();
  return updt_mgr->refine();
}
//: calls on update manager to update model
bool boxm_ocl_update_tableau::update_model()
{
  vcl_cout<<"UPDATING MODEL!!!"<<vcl_endl;

  //make sure you get a valid frame...
  if (curr_frame_ >= cam_files_.size()) curr_frame_ = 0;

  curr_frame_=rand.lrand32(0,cam_files_.size()-1);

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
  if (vil_image_view<vxl_byte> *img_byte = dynamic_cast<vil_image_view<vxl_byte>*>(loaded_image.ptr()))
  {
    vil_convert_stretch_range_limited(*img_byte ,floatimg, vxl_byte(0), vxl_byte(255), 0.0f, 1.0f);
  }
  else {
    vcl_cerr << "Failed to load image " << img_files_[curr_frame_] << vcl_endl;
    return -1;
  }

  curr_frame_++ ;

  //run the opencl update business
  updt_mgr->set_input_image(floatimg);
  updt_mgr->write_image_buffer();
  updt_mgr->set_persp_camera(pcam);
  updt_mgr->write_persp_camera_buffers();
  updt_mgr->online_processing();

  return true;
}

//: calls on ray manager to render frame into the pbuffer_
bool boxm_ocl_update_tableau::render_frame()
{
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


//--------------------- Event Handlers -------------------------------//

//: Handles tableau events (drawing and keys)
bool boxm_ocl_update_tableau::handle(vgui_event const &e)
{
  //draw handler - called on post_draw()
  if (e.type == vgui_DRAW) {
    if (do_init_ocl_) {
      this->init_ocl();
      do_init_ocl_ = false;
    }

    //do_update_ = false;
    vcl_cout<<"redrawing\n"
            <<"Cam center: "<<cam_.get_camera_center()<<'\n'
            <<"stare point: "<<stare_point_<<vcl_endl;
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
  //handle update command - keyboard press U
  else if (e.type == vgui_KEY_PRESS && e.key == vgui_key('u')) {
    vcl_cout<<"Continuing update"<<vcl_endl;
    do_update_ = true;
    this->post_idle_request();
  }
  else if (e.type == vgui_KEY_PRESS && e.key == vgui_key('d')) {
    vcl_cout<<"refining"<<vcl_endl;
    this->refine_model();
    this->post_idle_request();
  }
  //HANDLE idle events - do model updating
  else if (e.type == vgui_IDLE)
  {
    if (do_update_) {
      vcl_cout<<"Idling - i will be updating scene"<<vcl_endl;
      this->update_model();
      this->post_redraw();
      return true;
    }
    else {
      vcl_cout<<"done idling"<<vcl_endl;
      return false;
    }
  }
  //if you click on the canvas, you wanna render, so turn off update
  else if (e.type == vgui_BUTTON_DOWN) {
    do_update_ = false;
  }

  //otherwise trigger cam handling events
  return boxm_cam_tableau::handle(e);
}

