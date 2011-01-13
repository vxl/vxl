// This is brl/bseg/boxm2/view/boxm2_render_tableau.h
#ifndef boxm2_render_tableau_h
#define boxm2_render_tableau_h
//:
// \file
// \brief A tableau to view octree models in real time
// \author Vishal Jain
// \date Aug 11, 2010

#include "boxm2_include_glew.h"
#include "boxm2_cam_tableau.h"

//vgui includes
#include <vgui/vgui_gl.h>
#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_statusbar.h>
#include <vgui/vgui_tableau_sptr.h>

//utilities
#include <vpgl/vpgl_perspective_camera.h>
#include <bocl/bocl_utils.h>
#include <vil/vil_convert.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <vil/vil_math.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vnl/vnl_random.h>

//boxm2 includes
#include <boxm2/boxm2_scene.h>
#include <boxm2/ocl/boxm2_opencl_processor.h>
#include <boxm2/ocl/boxm2_opencl_render_process.h>
#include <boxm2/ocl/boxm2_opencl_update_process.h>
#include <boxm2/io/boxm2_dumb_cache.h>
#include <boxm2/io/boxm2_nn_cache.h>

//brdb stuff
#include <brdb/brdb_value.h>

class boxm2_render_tableau : public boxm2_cam_tableau
{
 public:
  boxm2_render_tableau();
  virtual ~boxm2_render_tableau() {}
  
  //: initialize tableau with scene_file, viewport size, initial cam,  
  bool init(vcl_string scene_file,
            unsigned ni,
            unsigned nj,
            vpgl_perspective_camera<double>* cam,
            vcl_vector<vcl_string> cam_files,
            vcl_vector<vcl_string> img_files);
  
  //: virtual function handles mouse and keyboard actions
  virtual bool handle( vgui_event const& e );

  //;  set the GL buffer which needs to be displayed.
  void set_glbuffer(GLuint  pbuffer) { pbuffer_ = pbuffer; }
  void set_statusbar(vgui_statusbar* status) { status_ = status; }

 protected:

  //vector of image files, vector of 
  vcl_vector<vcl_string> cam_files_;
  vcl_vector<vcl_string> img_files_;

  //: gpu processor and render_process, update_process
  boxm2_opencl_processor* gpu_pro_;
  boxm2_opencl_render_process render_;
  boxm2_opencl_update_process update_; 

  //: Boxm2 Scene
  boxm2_scene_sptr scene_;
  unsigned ni_;
  unsigned nj_;
  vgui_statusbar* status_;
  
  //: shared GL_CL image buffer
  GLuint pbuffer_;
  cl_mem clgl_buffer_;
  bocl_mem* exp_img_;

  //: update count, 
  bool do_update_;
  int update_count_;     
  vnl_random rand;

  //--Render, update, refine, save helper methods ------------------------------
  //func to render frame on GPU (returns gpu time)
  float render_frame();

  //updates a single (random) frame
  float update_frame(); 
  
  bool save_model(); 

  //-- HELPER INIT Methods and vars---------------------------------------------
  //: create cl_gl_context
  cl_context create_clgl_context();
  bool init_clgl();
  bool do_init_ocl;
};

//: declare smart pointer
typedef vgui_tableau_sptr_t<boxm2_render_tableau> boxm2_render_tableau_sptr;

//: Create a smart-pointer to a boxm2_render_tableau tableau.
struct boxm2_render_tableau_new : public boxm2_render_tableau_sptr
{
  //: Constructor - create an empty vgui_easy3D_tableau.
  typedef boxm2_render_tableau_sptr base;
  boxm2_render_tableau_new() : base( new boxm2_render_tableau ) { }
};

#endif // boxm2_render_tableau_h
