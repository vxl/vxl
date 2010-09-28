#include <boxm/ocl/view/boxm_ocl_change_detection_tableau.h>
//:
// \file

#include <boxm/ocl/boxm_ocl_change_detection_manager.h>

#include <boxm/ocl/boxm_ocl_utils.h>
#include <vpgl/vpgl_calibration_matrix.h>
#include <vgui/internals/trackball.h>
#include <vgl/vgl_distance.h>
#include <vgui/vgui_event_condition.h>

//image load
#include <vil/vil_convert.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_load.h>
#include <vul/vul_file.h>

//: Constructor
boxm_ocl_change_detection_tableau::boxm_ocl_change_detection_tableau()
{
    pbuffer_=0;
    ni_=640;
    nj_=480;
    curr_frame_ = 0;
}

//: Destructor
boxm_ocl_change_detection_tableau::~boxm_ocl_change_detection_tableau()
{
}

//: initialize tableau properties
bool boxm_ocl_change_detection_tableau::init(boxm_ocl_scene * scene,
                                   vcl_vector<vcl_string> cam_files,
                                   vcl_vector<vcl_string> img_files,
                                   vcl_vector<float> &hist,
                                   vcl_string save_img_dir)
{
  //set image dimensions, camera and scene
  scene_ = scene;

  //directory of cameras
  cam_files_ = cam_files;
  img_files_ = img_files;

  save_img_dir_=save_img_dir;
  hist_=hist;
  if(cam_files_.size()>0 && img_files_.size()>0)
  {
      //build the camera from file
      vcl_ifstream ifs(cam_files_[0].c_str());
      cam_ = new vpgl_perspective_camera<double>;
      if (!ifs.is_open()) {
          vcl_cerr << "Failed to open file " << cam_files_[0] << vcl_endl;
          return -1;
      }
      ifs >> *cam_;

      //load image from file
      vil_image_view_base_sptr loaded_image = vil_load(img_files_[0].c_str());
      ni_ = loaded_image->ni();
      nj_ = loaded_image->nj();
      obs_img_.set_size(ni_,nj_);
      if (vil_image_view<vxl_byte> *img_byte = dynamic_cast<vil_image_view<vxl_byte>*>(loaded_image.ptr()))
      {
          vil_convert_stretch_range_limited(*img_byte ,obs_img_, vxl_byte(0), vxl_byte(255), 0.0f, 1.0f);
      }
      else {
          vcl_cerr << "Failed to load image " << img_files_[curr_frame_] << vcl_endl;
          return -1;
      }

  }
  //initialize OCL stuff
  do_init_ocl_ = true;
  return true;
}

//: initializes ocl_related stuff
// (must be called AFTER window is popped up so QT
// can get the correct context )
bool boxm_ocl_change_detection_tableau::init_ocl()
{
  //initialize GLEW
  GLenum err = glewInit();
  if (GLEW_OK != err)
  {
    /* Problem: glewInit failed, something is seriously wrong. */
    vcl_cout<< "Error: "<<glewGetErrorString(err)<<vcl_endl;
  }

  //initialize the render manager
  boxm_ocl_change_detection_manager* cd_mgr
      = boxm_ocl_change_detection_manager::instance();
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
#elif defined(__APPLE__) || defined(MACOSX)
  CGLContextObj kCGLContext = CGLGetCurrentContext();
  CGLShareGroupObj kCGLShareGroup = CGLGetShareGroup(kCGLContext);
  cl_context_properties props[] =
  {
    CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE, (cl_context_properties)kCGLShareGroup,
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
  cd_mgr->context_ = clCreateContext(props, 1, &cd_mgr->devices()[0], NULL, NULL, &status);

  vcl_cout<<error_to_string(status);
  ///initialize update

  vil_image_view<float> expected(ni_,nj_);
  int bundle_dim = 8;
  cd_mgr->set_bundle_ni(bundle_dim);
  cd_mgr->set_bundle_nj(bundle_dim);
  cd_mgr->init_ray_trace(scene_,cam_,obs_img_, expected);
  bool good=true;  
  good = good && cd_mgr->set_scene_data()
      && cd_mgr->set_all_blocks()
      && cd_mgr->set_scene_data_buffers()
      && cd_mgr->set_tree_buffers();

  // run the raytracing
  good = good && cd_mgr->set_persp_camera(cam_)
      && cd_mgr->set_persp_camera_buffers()
      && cd_mgr->set_input_image()
      && cd_mgr->set_input_image_buffers()
      && cd_mgr->set_image_dims_buffers();
  //need to set ray trace
  //delete old buffer
  if (pbuffer_) {
      clReleaseMemObject(cd_mgr->image_buf_);
      glDeleteBuffers(1, &pbuffer_);
  }

  //generate glBuffer, and bind to ocl_mgr->image_gl_buf_
  glGenBuffers(1, &pbuffer_);
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbuffer_);
  glBufferData(GL_PIXEL_UNPACK_BUFFER, cd_mgr->wni() * cd_mgr->wnj() * sizeof(GLubyte) * 4, 0, GL_STREAM_DRAW);
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

  //create OpenCL buffer from GL PBO, and set kernel and arguments
  cd_mgr->image_gl_buf_ = clCreateFromGLBuffer(cd_mgr->context(),
                                                 CL_MEM_WRITE_ONLY,
                                                 pbuffer_,
                                                 &status);

  cd_mgr->set_kernel();
  cd_mgr->set_args();
  cd_mgr->set_commandqueue();
  cd_mgr->set_workspace();

  return true;
}


//-------------- OpenCL methods (render and update) ------------------//

//: calls on update manager to update model
bool boxm_ocl_change_detection_tableau::change_detection()
{
    vcl_cout<<"Change Detection!!!"<<vcl_endl;

    //make sure you get a valid frame...
    if (curr_frame_ >= cam_files_.size()) curr_frame_ = 0;
    vcl_cout<<"Cam "<<cam_files_[curr_frame_]<<" Image "<<img_files_[curr_frame_]<<vcl_endl;

    //load up the update manager instance
    boxm_ocl_change_detection_manager* cd_mgr = boxm_ocl_change_detection_manager::instance();
    cl_int status = clEnqueueAcquireGLObjects(cd_mgr->command_queue_, 1,
        &cd_mgr->image_gl_buf_ , 0, 0, 0);
    if (!cd_mgr->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (input_image)"+error_to_string(status)))
        return false;

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


    //run the opencl update business
    cd_mgr->set_input_image(floatimg);
    cd_mgr->write_image_buffer();
    cd_mgr->set_persp_camera(pcam);
    cd_mgr->write_persp_camera_buffers();
    cd_mgr->run();

    if(vul_file::is_directory(save_img_dir_.c_str()))
    {
        cd_mgr->read_output_image();
        vcl_string filename=vul_file::strip_directory(img_files_[curr_frame_].c_str());
        filename=vul_file::strip_extension(filename.c_str());

        vcl_cout<<"Saving to "<<filename<<vcl_endl;

        cd_mgr->save_image(save_img_dir_+"/"+filename+".tiff");
    }
    status = clEnqueueReleaseGLObjects(cd_mgr->command_queue_, 1, &cd_mgr->image_gl_buf_ , 0, 0, 0);
    clFinish( cd_mgr->command_queue_ );
    curr_frame_++ ;

    return true;
}
//: sets up viewport and GL Modes
void boxm_ocl_change_detection_tableau::setup_gl_matrices()
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

//--------------------- Event Handlers -------------------------------//

//: Handles tableau events (drawing and keys)
bool boxm_ocl_change_detection_tableau::handle(vgui_event const &e)
{
  //draw handler - called on post_draw()
  if (e.type == vgui_DRAW) {
    if (do_init_ocl_) {
      this->init_ocl();
      do_init_ocl_ = false;
    }

    this->change_detection();
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
  else if (e.type == vgui_KEY_PRESS && e.key == vgui_key('c')) {
    vcl_cout<<"Change Detection "<<vcl_endl;
    this->post_redraw();
  }


  //otherwise trigger cam handling events
  return vgui_tableau::handle(e);
}

