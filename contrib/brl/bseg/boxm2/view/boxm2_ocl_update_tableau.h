// This is brl/bseg/boxm2/view/boxm2_ocl_update_tableau.h
#ifndef boxm2_ocl_update_tableau_h
#define boxm2_ocl_update_tableau_h
//:
// \file
// \brief A tableau for updating/refining and viewing a model.
// \author Vishal Jain
// \date Mar 25, 2011
#include <vnl/vnl_math.h>
#include "boxm2_include_glew.h"
#include "boxm2_ocl_render_tableau.h"

//vgui includes
#include <vgui/vgui_gl.h>
#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_statusbar.h>
#include <vgui/vgui_tableau_sptr.h>

//utilities
#include <vpgl/vpgl_perspective_camera.h>
#include <vil/vil_image_view.h>
#include <vnl/vnl_random.h>

//boxm2 includes
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_data.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/view/boxm2_trajectory.h>
#include <bocl/bocl_manager.h>
//brdb stuff
#include <brdb/brdb_value.h>

class boxm2_ocl_update_tableau : public boxm2_ocl_render_tableau
{
 public:
  boxm2_ocl_update_tableau();
  virtual ~boxm2_ocl_update_tableau() {}

  //: initialize tableau with scene_file, viewport size, initial cam,
  bool init_update (bocl_device_sptr device,
                    boxm2_opencl_cache_sptr opencl_cache,
                    boxm2_scene_sptr scene,
                    unsigned ni,
                    unsigned nj,
                    vpgl_perspective_camera<double>* cam,
                    std::vector<std::string>& update_imgs,
                    std::vector<std::string>& update_cams);

  //: virtual function handles mouse and keyboard actions
  virtual bool handle( vgui_event const& e );

 protected:

  //func to update frame on GPU (returns gpu time)
  float update_frame(vil_image_view_base_sptr in_im, vpgl_camera_double_sptr in_cam);
  float refine(float thresh);
  float merge(float thresh);
  float filter();
  float save();
  std::vector<std::string> cams_;
  std::vector<std::string> imgs_;
  vnl_random random_;
  bool do_update_;

  boxm2_cache_sptr cache_; //for saving
};

//: declare smart pointer
typedef vgui_tableau_sptr_t<boxm2_ocl_update_tableau> boxm2_ocl_update_tableau_sptr;

//: Create a smart-pointer to a boxm2_ocl_update_tableau tableau.
struct boxm2_ocl_update_tableau_new : public boxm2_ocl_update_tableau_sptr
{
  //: Constructor - create an empty vgui_easy3D_tableau.
  typedef boxm2_ocl_update_tableau_sptr base;
  boxm2_ocl_update_tableau_new() : base( new boxm2_ocl_update_tableau ) { }
};

#endif // boxm2_ocl_update_tableau_h
