#include <iostream>
#include <sstream>
#include "boxm2_ocl_articulated_render_tableau.h"
#include "../boxm2_vecf_scene_articulation.h"

//:
// \file
#include <vpgl/vpgl_perspective_camera.h>
#include <vgui/vgui_modifier.h>
#include <vgui/vgui_soview2D.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boxm2/view/boxm2_view_utils.h>
#include <boxm2/ocl/pro/boxm2_ocl_processes.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>

#include <vgui/vgui_adaptor.h>
#include <brdb/brdb_value.h>
#include <brdb/brdb_selection.h>

#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_parameters.h>
#include <bprb/bprb_macros.h>
#include <bprb/bprb_func_process.h>
#include "../boxm2_vecf_orbit_params.h"
//: Constructor
boxm2_ocl_articulated_render_tableau::boxm2_ocl_articulated_render_tableau()
{
  is_bw_ = false;
  animation_on_ = false;
  pbuffer_=0;
  ni_=640;
  nj_=480;
  DECLARE_FUNC_CONS(boxm2_ocl_articulated_render_gl_expected_image_process);
  DECLARE_FUNC_CONS(boxm2_ocl_articulated_render_gl_expected_color_process);
  DECLARE_FUNC_CONS(boxm2_ocl_articulated_render_gl_view_dep_app_expected_image_process);
  DECLARE_FUNC_CONS(boxm2_ocl_articulated_render_gl_view_dep_app_expected_color_process);
  DECLARE_FUNC_CONS(boxm2_ocl_articulated_render_gl_expected_depth_process);
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ocl_render_gl_expected_image_process, "boxm2OclRenderGlExpectedImageProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ocl_render_gl_expected_color_process, "boxm2OclRenderGlExpectedColorProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ocl_render_gl_view_dep_app_expected_image_process, "boxm2OclRenderGlViewDepExpectedImageProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ocl_render_gl_view_dep_app_expected_color_process, "boxm2OclRenderGlViewDepExpectedColorProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ocl_render_gl_expected_depth_process, "boxm2OclRenderGlExpectedDepthProcess");

  REGISTER_DATATYPE(boxm2_opencl_cache_sptr);
  REGISTER_DATATYPE(boxm2_scene_sptr);
  REGISTER_DATATYPE(bocl_mem_sptr);
  REGISTER_DATATYPE(float);
  articulated_scene_ =0;
  target_scene_ = 0;
  scene_articulation_ = 0;
}

//: initialize tableau properties
bool boxm2_ocl_articulated_render_tableau::init(bocl_device_sptr device,
                                                boxm2_opencl_cache_sptr opencl_cache,
                                                boxm2_vecf_articulated_scene_sptr articulated_scene,
                                                boxm2_vecf_scene_articulation* sa,
                                                boxm2_scene_sptr target_scene,
                                                unsigned ni,
                                                unsigned nj,
                                                vpgl_perspective_camera<double> * cam, std::string identifier)
{
    //set image dimensions, camera and scene
    ni_ = ni;
    nj_ = nj;

    identifier_ = identifier;

    cam_   = (*cam);
    default_cam_ = (*cam);

    default_stare_point_.set(target_scene->bounding_box().centroid().x(),
                             target_scene->bounding_box().centroid().y(),
#if 0
                             target_scene->bounding_box().min_z());
#else
                             target_scene->bounding_box().centroid().z());
#endif
    //default_cam_.look_at(default_stare_point_);
    stare_point_=default_stare_point_;
    //cam_.look_at(stare_point_);
    scale_ =target_scene->bounding_box().height();
    //create the scene
    articulated_scene_ = articulated_scene;
    target_scene_ = target_scene;
    scene_articulation_ = sa;
    opencl_cache_=opencl_cache;
    device_=device;
    do_init_ocl=true;
    render_trajectory_ = false;
    trajectory_ = new boxm2_trajectory(30.0, 45.0, -1.0, target_scene_->bounding_box(), ni, nj);
    cam_iter_ = trajectory_->begin();
    render_depth_ = false;
    // set depth_scale_ and depth_offset_
    calibrate_depth_range();
    post_redraw();
    return true;
}


//: Handles tableau events (drawing and keys)
bool boxm2_ocl_articulated_render_tableau::handle(vgui_event const &e)
{
  //draw handler - called on post_draw()
  if (e.type == vgui_DRAW)
  {
    if (do_init_ocl) {
      this->init_clgl();
      do_init_ocl = false;
    }

    vgl_homg_point_3d<double> cent(-26.25, -18.25, -18.25);
    vgl_homg_point_2d<double> hpcent = cam_.project(cent);
    vgl_point_2d<double> pc(hpcent);
    double x0 = pc.x()/1280.0, y0 = pc.y()/720.0;
    vgui_soview2D_point seg0(x0,y0);
    cent.set(0.0, 0.0, 0.0);
    hpcent = cam_.project(cent);
    vgl_point_2d<double> pc1(hpcent);
    double x1 = pc1.x()/1280.0, y1 = pc1.y()/720.0;
    vgui_soview2D_point seg1(x1,y1);
    cent.set(26.25, 18.25, 18.25);
    hpcent = cam_.project(cent);
    vgl_point_2d<double> pc2(hpcent);
    double x2 = pc2.x()/1280.0, y2 = pc2.y()/720.0;
    vgui_soview2D_point seg2(x2,y2);
    float gpu_time = this->render_frame();
    this->setup_gl_matrices();
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glRasterPos2i(0, 1);
    glPixelZoom(1,-1);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, pbuffer_);
    glDrawPixels(ni_, nj_, GL_RGBA, GL_UNSIGNED_BYTE, 0);
   glPointSize(5);
    glColor3f(1.0, 0.0, 0.0);
    //seg0.draw(); seg1.draw(); seg2.draw();
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
        //calculate and write fps to status
    std::stringstream str;
    str<<".  rendering at ~ "<< (1000.0f / gpu_time) <<" fps ";
    if (status_) {
      status_->write(str.str().c_str());
    }

    return true;
  }

  //toggle color - this is a hack to get color models to show as grey
  if (e.type == vgui_KEY_PRESS) {
    if (e.key == vgui_key('c')) {
      std::cout<<"Toggling b and w"<<std::endl;
      is_bw_ = !is_bw_;
    }
    else if (e.key == vgui_key('d')) {
      std::cout << "Toggling depth and expected image" << std::endl;
      render_depth_ = !render_depth_;
      if (render_depth_) {
        calibrate_depth_range();
      }
    }
  }
  if(articulated_scene_ && scene_articulation_->handle(e))
    return true;

  if ((e.type == vgui_KEY_PRESS  && e.key == vgui_key('v'))||(e.type == vgui_TIMER&&e.timer_id == 1234)) {

    if(e.type == vgui_KEY_PRESS  && e.key == vgui_key('v'))
      animation_on_ = !animation_on_;

    if(!animation_on_){
      if(boxm2_cam_tableau::handle(e))
        return true;
      else
        return false;
    }
    if(articulated_scene_){
      unsigned nsa = static_cast<unsigned>(scene_articulation_->size());
      opencl_cache_->clear_cache();

      if(nsa == 1){//no need to map to target after the first articulation
        if(play_index_ == 0){
          articulated_scene_->set_params((*scene_articulation_)[play_index_]);
           std::cout<<"apply vector field"<<std::endl;
          articulated_scene_->map_to_target(target_scene_);
          play_index_=1;
        }
      }else{
        articulated_scene_->set_params((*scene_articulation_)[play_index_]);
        std::cout<<"apply vector field"<<std::endl;
        articulated_scene_->map_to_target(target_scene_);
        play_index_++;
      }
     if(nsa>1&&play_index_ == nsa)
        play_index_ = 0;
     post_redraw();
     e.origin->post_timer(0.2f, 1234);
    }
  }
  if (boxm2_cam_tableau::handle(e)) {
    return true;
  }
  return false;
}

void boxm2_ocl_articulated_render_tableau::calibrate_depth_range()
{
  vgl_box_3d<double> bbox = target_scene_->bounding_box();
  std::vector<vgl_point_3d<double> > bbox_verts;
  bbox_verts.push_back(vgl_point_3d<double>(bbox.min_x(), bbox.min_y(), bbox.min_z()));
  bbox_verts.push_back(vgl_point_3d<double>(bbox.min_x(), bbox.min_y(), bbox.max_z()));
  bbox_verts.push_back(vgl_point_3d<double>(bbox.min_x(), bbox.max_y(), bbox.min_z()));
  bbox_verts.push_back(vgl_point_3d<double>(bbox.min_x(), bbox.max_y(), bbox.max_z()));
  bbox_verts.push_back(vgl_point_3d<double>(bbox.max_x(), bbox.min_y(), bbox.min_z()));
  bbox_verts.push_back(vgl_point_3d<double>(bbox.max_x(), bbox.min_y(), bbox.max_z()));
  bbox_verts.push_back(vgl_point_3d<double>(bbox.max_x(), bbox.max_y(), bbox.min_z()));
  bbox_verts.push_back(vgl_point_3d<double>(bbox.max_x(), bbox.max_y(), bbox.max_z()));

  vgl_point_3d<double> cam_center = vgl_point_3d<double>(cam_.camera_center());
  double min_dist = vnl_numeric_traits<double>::maxval;
  double max_dist = 0.0;
  for (std::vector<vgl_point_3d<double> >::const_iterator vit=bbox_verts.begin(); vit!=bbox_verts.end(); ++vit) {
    double dist = (*vit - cam_center).length();
    if (dist > max_dist) {
      max_dist = dist;
    }
    if (dist < min_dist) {
      min_dist = dist;
    }
  }
#if 1
  // invert map - bright is close, dark is far
  depth_scale_ = 1.0/(min_dist - max_dist);
  depth_offset_ =  -max_dist*depth_scale_;
#else
  // bright is far, dark is close
  depth_scale_ = 1.0/(max_dist - min_dist);
  depth_offset_ = -min_dist*depth_scale_;

#endif
}

//: calls on ray manager to render frame into the pbuffer_
float boxm2_ocl_articulated_render_tableau::render_frame()
{
    cl_int status = clEnqueueAcquireGLObjects( queue_, 1,&exp_img_->buffer(), 0, 0, 0);
    exp_img_->zero_gpu_buffer( queue_ );
    if (!check_val(status,CL_SUCCESS,"clEnqueueAcquireGLObjects failed. (gl_image)"+error_to_string(status)))
        return -1.0f;

    std::cout<<cam_<<std::endl;

    //set up brdb_value_sptr arguments...
    brdb_value_sptr brdb_device = new brdb_value_t<bocl_device_sptr>(device_);
    brdb_value_sptr brdb_scene = new brdb_value_t<boxm2_scene_sptr>(target_scene_);
    brdb_value_sptr brdb_opencl_cache = new brdb_value_t<boxm2_opencl_cache_sptr>(opencl_cache_);
    vpgl_camera_double_sptr cam = new vpgl_perspective_camera<double>(cam_);
    brdb_value_sptr brdb_cam = new brdb_value_t<vpgl_camera_double_sptr>(cam);
    brdb_value_sptr brdb_ni = new brdb_value_t<unsigned>(ni_);
    brdb_value_sptr brdb_nj = new brdb_value_t<unsigned>(nj_);
    brdb_value_sptr brdb_depth_scale = new brdb_value_t<float>(depth_scale_);
    brdb_value_sptr brdb_depth_offset = new brdb_value_t<float>(depth_offset_);
    brdb_value_sptr exp_img = new brdb_value_t<bocl_mem_sptr>(exp_img_);
    brdb_value_sptr exp_img_dim = new brdb_value_t<bocl_mem_sptr>(exp_img_dim_);
    brdb_value_sptr ident = new brdb_value_t<std::string>(identifier_);

    bool good = true;
    if (render_depth_) {
      // set the depth_offset_ and depth_scale_ values
      calibrate_depth_range();

      good = bprb_batch_process_manager::instance()->init_process("boxm2OclRenderGlExpectedDepthProcess");
      //set process args
      good = good && bprb_batch_process_manager::instance()->set_input(0, brdb_device); // device
      good = good && bprb_batch_process_manager::instance()->set_input(1, brdb_scene); //  scene
      good = good && bprb_batch_process_manager::instance()->set_input(2, brdb_opencl_cache);
      good = good && bprb_batch_process_manager::instance()->set_input(3, brdb_cam);// camera
      good = good && bprb_batch_process_manager::instance()->set_input(4, brdb_ni);  // ni for rendered image
      good = good && bprb_batch_process_manager::instance()->set_input(5, brdb_nj);   // nj for rendered image
      good = good && bprb_batch_process_manager::instance()->set_input(6, exp_img);   // exp image ( gl buffer)
      good = good && bprb_batch_process_manager::instance()->set_input(7, exp_img_dim);   // exp image dimensions
      good = good && bprb_batch_process_manager::instance()->set_input(8, brdb_depth_scale);   // depth scale
      good = good && bprb_batch_process_manager::instance()->set_input(9, brdb_depth_offset);   // depth offset
    }
    else {

      //if scene has RGB data type, use color render process
    if(target_scene_->has_data_type(boxm2_data_traits<BOXM2_MOG6_VIEW>::prefix()) || target_scene_->has_data_type(boxm2_data_traits<BOXM2_MOG6_VIEW_COMPACT>::prefix()))
        good = bprb_batch_process_manager::instance()->init_process("boxm2OclRenderGlViewDepExpectedImageProcess");
    else if(target_scene_->has_data_type(boxm2_data_traits<BOXM2_GAUSS_RGB_VIEW>::prefix()) || target_scene_->has_data_type(boxm2_data_traits<BOXM2_GAUSS_RGB_VIEW_COMPACT>::prefix()) )
      good = bprb_batch_process_manager::instance()->init_process("boxm2OclRenderGlViewDepExpectedColorProcess");
    else if(target_scene_->has_data_type(boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix()) )
        good = bprb_batch_process_manager::instance()->init_process("boxm2OclRenderGlExpectedColorProcess");
    else
        good = bprb_batch_process_manager::instance()->init_process("boxm2OclRenderGlExpectedImageProcess");

      //set process args
      good = good && bprb_batch_process_manager::instance()->set_input(0, brdb_device); // device
      good = good && bprb_batch_process_manager::instance()->set_input(1, brdb_scene); //  scene
      good = good && bprb_batch_process_manager::instance()->set_input(2, brdb_opencl_cache);
      good = good && bprb_batch_process_manager::instance()->set_input(3, brdb_cam);// camera
      good = good && bprb_batch_process_manager::instance()->set_input(4, brdb_ni);  // ni for rendered image
      good = good && bprb_batch_process_manager::instance()->set_input(5, brdb_nj);   // nj for rendered image
      good = good && bprb_batch_process_manager::instance()->set_input(6, exp_img);   // exp image ( gl buffer)
      good = good && bprb_batch_process_manager::instance()->set_input(7, exp_img_dim);   // exp image dimensions
      good = good && bprb_batch_process_manager::instance()->set_input(8, ident);   // string identifier to specify appearance data

      //hack for toggling
      if(is_bw_) {
        brdb_value_sptr brdb_is_bw = new brdb_value_t<bool>(true);
        good = good && bprb_batch_process_manager::instance()->set_input(9, brdb_is_bw);
      }
    }


    good = good && bprb_batch_process_manager::instance()->run_process();

    //grab float output from render gl process
    unsigned int time_id = 0;
    good = good && bprb_batch_process_manager::instance()->commit_output(0, time_id);
    brdb_query_aptr Q = brdb_query_comp_new("id", brdb_query::EQ, time_id);
    brdb_selection_sptr S = DATABASE->select("float_data", Q);
    if (S->size()!=1){
        std::cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
            << " no selections\n";
    }
    brdb_value_sptr value;
    if (!S->get_value(std::string("value"), value)) {
        std::cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
            << " didn't get value\n";
    }
    float time = value->val<float>();

    //release gl buffer
   status = clEnqueueReleaseGLObjects(queue_, 1, &exp_img_->buffer(), 0, 0, 0);
    clFinish( queue_ );

    return time;
}

//: private helper method to init_clgl stuff (gpu processor)
bool boxm2_ocl_articulated_render_tableau::init_clgl()
{
  //get relevant blocks
  std::cout<<"Data Path: "<<target_scene_->data_path()<<std::endl;
  device_->context() = boxm2_view_utils::create_clgl_context(*(device_->device_id()));
  opencl_cache_->set_context(device_->context());

  int status_queue=0;
  queue_ =  clCreateCommandQueue(device_->context(),*(device_->device_id()),CL_QUEUE_PROFILING_ENABLE,&status_queue);

  // delete old buffer
  if (pbuffer_) {
      clReleaseMemObject(clgl_buffer_);
      glDeleteBuffers(1, &pbuffer_);
  }

  ////generate glBuffer, and bind to ray_mgr->image_gl_buf_
  glGenBuffers(1, &pbuffer_);
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbuffer_);
  glBufferData(GL_PIXEL_UNPACK_BUFFER, ni_*nj_*sizeof(GLubyte)*4, 0, GL_STREAM_DRAW);
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

  //create OpenCL buffer from GL PBO, and set kernel and arguments
  int status = 0;
  clgl_buffer_ = clCreateFromGLBuffer(device_->context(),
                                      CL_MEM_WRITE_ONLY,
                                      pbuffer_,
                                      &status);
  exp_img_ = new bocl_mem(device_->context(),  NULL, static_cast<unsigned>(RoundUp(ni_,8)*RoundUp(nj_,8)*sizeof(GLubyte)*4), "exp image (gl) buffer");
  exp_img_->set_gl_buffer(clgl_buffer_);

  int img_dim_buff[4];
  img_dim_buff[0] = 0;   img_dim_buff[2] = ni_;
  img_dim_buff[1] = 0;   img_dim_buff[3] = nj_;
  exp_img_dim_=new bocl_mem(device_->context(), img_dim_buff, sizeof(int)*4, "image dims");
  exp_img_dim_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  return true;
}
