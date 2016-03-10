// This is brl/bseg/bstm/view/bstm_ocl_multi_render_tableau.h
#ifndef bstm_ocl_multi_render_tableau_h
#define bstm_ocl_multi_render_tableau_h
//:
// \file
// \brief A tableau to render bstm scenes.

#include "bstm_include_glew.h"
#include "bstm_cam_tableau.h"

//vgui includes
#include <vgui/vgui_gl.h>
#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_statusbar.h>
#include <vgui/vgui_tableau_sptr.h>

//utilities
#include <vpgl/vpgl_perspective_camera.h>
#include <bocl/bocl_utils.h>

//bstm includes
#include <bstm/bstm_scene.h>
#include <bstm/ocl/bstm_opencl_cache.h>
#include <bocl/bocl_manager.h>
//brdb stuff
#include <brdb/brdb_value.h>

#include <vgui/vgui_slider_tableau.h>

#include <vgui/vgui_poly_tableau.h>
#include <vgui/vgui_shell_tableau.h>

class bstm_ocl_multi_render_tableau : public bstm_cam_tableau
{
 public:
  bstm_ocl_multi_render_tableau();
  virtual ~bstm_ocl_multi_render_tableau() {}

  //: initialize tableau with scene_file, viewport size, initial cam,
  bool init(std::vector<bocl_device_sptr> devices,
            std::vector<bstm_opencl_cache_sptr> opencl_caches,
            bstm_scene_sptr scene,
            unsigned ni,
            unsigned nj,
            vpgl_perspective_camera<double>* cam,
            vgui_slider_tableau_sptr slider);

  //: virtual function handles mouse and keyboard actions
  virtual bool handle( vgui_event const& e );

  //;  set the GL buffer which needs to be displayed.
  void set_statusbar(vgui_statusbar* status) { status_ = status; }

  double* time() {return &time_;}

  vgui_shell_tableau_sptr shell_;

  //: returns the index of te gpu to do the processing
  unsigned gpu_id(double time);

 protected:

  unsigned long total_gpu_mem_;
  std::vector<unsigned> gpu_ids_;
  unsigned num_gpus_;

  bocl_manager_child* mgr_;

  std::vector<cl_command_queue> queues_;
  std::vector<bocl_device_sptr> devices_;
  std::vector<bstm_opencl_cache_sptr> opencl_caches_;

  //: Boxm2 Scene
  bstm_scene_sptr scene_;

  unsigned ni_;
  unsigned nj_;
  vgui_statusbar* status_;

  double time_;
  double scene_min_t_;
  double scene_max_t_;

  vgui_slider_tableau_sptr slider_;

  //: shared GL_CL image buffers
  std::vector<GLuint> pbuffer_;
  std::vector<cl_mem> clgl_buffer_;
  std::vector<bocl_mem_sptr> exp_img_;
  std::vector<bocl_mem_sptr> exp_img_dim_;

  //computes the gpu assignment to each blk of scene.
  void compute_gpu_assignments();

  //--Render, update, refine, save helper methods ------------------------------
  //func to render frame on GPU (returns gpu time)
  float render_frame();

  bool init_clgl();
  bool do_init_ocl;

  bool render_trajectory_;
  bool render_label_;
};

//: declare smart pointer
typedef vgui_tableau_sptr_t<bstm_ocl_multi_render_tableau> bstm_ocl_multi_render_tableau_sptr;

//: Create a smart-pointer to a bstm_ocl_multi_render_tableau tableau.
struct bstm_ocl_multi_render_tableau_new : public bstm_ocl_multi_render_tableau_sptr
{
  //: Constructor - create an empty vgui_easy3D_tableau.
  typedef bstm_ocl_multi_render_tableau_sptr base;
  bstm_ocl_multi_render_tableau_new() : base( new bstm_ocl_multi_render_tableau ) { }
};

#endif // bstm_ocl_multi_render_tableau_h
