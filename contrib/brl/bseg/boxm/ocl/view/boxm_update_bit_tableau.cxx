#include <boxm/ocl/view/boxm_update_bit_tableau.h>
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


//: Constructor
boxm_update_bit_tableau::boxm_update_bit_tableau()
{
    pbuffer_=0;
    ni_=640;
    nj_=480;
    curr_frame_ = 0;
    do_update_ = true;
}

//: Destructor
boxm_update_bit_tableau::~boxm_update_bit_tableau()
{

}

//: initialize tableau properties
bool boxm_update_bit_tableau::init(boxm_ocl_bit_scene * scene,
                                   unsigned ni, unsigned nj,
                                   vpgl_perspective_camera<double> * cam,
                                   vcl_vector<vcl_string> cam_files,
                                   vcl_vector<vcl_string> img_files,
                                   float prob_thresh)
{
  //set image dimensions, camera and scene
  ni_ = ni;
  nj_ = nj;
  default_cam_ = (*cam);
  cam_ = (*cam); //default cam
  scene_ = scene;

  count_=0;
  curr_count_=0;
  refine_count_ = 0;
  ////directory of cameras
  cam_files_ = cam_files;
  img_files_ = img_files;

  prob_thresh_=prob_thresh;
  ////initialize OCL stuff
  do_init_ocl_ = true;
  return true;
}

//: initializes ocl_related stuff
// (must be called AFTER window is popped up so QT
// can get the correct context )
bool boxm_update_bit_tableau::init_ocl()
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
  vil_image_view<float> expected(ni_,nj_);
  int bundle_dim = 8;
  updt_mgr->set_bundle_ni(bundle_dim);
  updt_mgr->set_bundle_nj(bundle_dim);
  updt_mgr->init_scene(scene_, &cam_, expected, prob_thresh_);  //THIS SETS WNI AND WNJ used below
  if (!updt_mgr->setup_norm_data(true, 0.5f, 0.25f))
    return -1;
 
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


  return true;
}


//-------------- OpenCL methods (render and update) ------------------//
bool boxm_update_bit_tableau::save_model()
{
  vcl_cout<<"SAVING MODEL!!!"<<vcl_endl;
  boxm_update_bit_scene_manager* updt_mgr = boxm_update_bit_scene_manager::instance();
  return updt_mgr->save_scene();
}
bool boxm_update_bit_tableau::refine_model()
{
  vcl_cout<<"REFINING MODEL!!!"<<vcl_endl;
  refine_count_++; curr_count_=0;
  boxm_update_bit_scene_manager* updt_mgr = boxm_update_bit_scene_manager::instance();
  return updt_mgr->refine();
  return false;
}
//: calls on update manager to update model
bool boxm_update_bit_tableau::update_model()
{
  vcl_cout<<"UPDATING MODEL!!!"<<vcl_endl;
  count_++; curr_count_++;

  //make sure you get a valid frame...
  if(curr_frame_ >= cam_files_.size())
    curr_frame_ = 0;
  curr_frame_=rand.lrand32(0,cam_files_.size()-1);

  vcl_cout<<"Cam "<<cam_files_[curr_frame_]
          <<"Image "<<img_files_[curr_frame_]<<vcl_endl;

  //load up the update manager instance
  boxm_update_bit_scene_manager* updt_mgr = boxm_update_bit_scene_manager::instance();

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

  //run the opencl update business (MAKE THIS JUST ONE METHOD, NOT FIVE CALLS)
  updt_mgr->set_input_image(floatimg);
  updt_mgr->write_image_buffer();
  updt_mgr->set_persp_camera(pcam);
  updt_mgr->write_persp_camera_buffers();
  updt_mgr->update();
  return true;
}

//: calls on ray manager to render frame into the pbuffer_
bool boxm_update_bit_tableau::render_frame()
{
    vcl_cout<<"RENDERING FRAME!"<<vcl_endl;
    boxm_update_bit_scene_manager* ocl_mgr = boxm_update_bit_scene_manager::instance();
    cl_int status = clEnqueueAcquireGLObjects(ocl_mgr->command_queue_, 1,
                                              &ocl_mgr->image_gl_buf_ , 0, 0, 0);
    if (!ocl_mgr->check_val(status,CL_SUCCESS,"tableau::clEnqueueAcquiredGLObjects failed (render_frame)"+error_to_string(status)))
        return false;

    //MAKE THIS JUST ONE METHOD< NOT THREE CALLS
    ocl_mgr->set_persp_camera(&cam_);
    ocl_mgr->write_persp_camera_buffers();
    ocl_mgr->rendering();
    status = clEnqueueReleaseGLObjects(ocl_mgr->command_queue_, 1, &ocl_mgr->image_gl_buf_ , 0, 0, 0);
    clFinish( ocl_mgr->command_queue_ );
    return true;
}


//--------------------- Event Handlers -------------------------------//

//: Handles tableau events (drawing and keys)
bool boxm_update_bit_tableau::handle(vgui_event const &e)
{
  //draw handler - called on post_draw()
  if (e.type == vgui_DRAW) {
    if (do_init_ocl_) {
      this->init_ocl();
      do_init_ocl_ = false;
    }

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
    
    //update status
    vcl_stringstream str;
    str<<"Num Updates: "<<count_
       <<"  Num Refines: "<<refine_count_
       <<"  (updates since last refine: "<<curr_count_<<")";
    status_->write(str.str().c_str());
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
    return true;
  }
  else if (e.type == vgui_KEY_PRESS && e.key == vgui_key('s')) {
    vcl_cout<<"saving"<<vcl_endl;
    this->save_model();
    return true;
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

