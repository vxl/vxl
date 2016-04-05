// This is brl/bseg/boxm2/view/boxm2_ocl_change_tableau.h
#ifndef boxm2_ocl_change_tableau_h
#define boxm2_ocl_change_tableau_h
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

//boxm2 includes
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_data.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/view/boxm2_trajectory.h>
#include <bocl/bocl_manager.h>
//brdb stuff
#include <brdb/brdb_value.h>

class boxm2_ocl_change_tableau : public boxm2_ocl_render_tableau
{
 public:
  boxm2_ocl_change_tableau();
  virtual ~boxm2_ocl_change_tableau() {}

  //: initialize tableau with scene_file, viewport size, initial cam,
  bool init_change (bocl_device_sptr device,
                    boxm2_opencl_cache_sptr opencl_cache,
                    boxm2_scene_sptr scene,
                    unsigned ni,
                    unsigned nj,
                    vpgl_perspective_camera<double>* cam,
                    std::vector<std::string>& change_imgs,
                    std::vector<std::string>& change_cams);

  //: virtual function handles mouse and keyboard actions
  virtual bool handle( vgui_event const& e );

 protected:

  //func to update frame on GPU (returns gpu time)
  float change_detect(int frame);
  std::vector<std::string>  cams_;
  std::vector<std::string>  imgs_;
  int                     frame_;
  bool                    do_render_change_;
  int                     n_;               //for nxn change detection
};

//: declare smart pointer
typedef vgui_tableau_sptr_t<boxm2_ocl_change_tableau> boxm2_ocl_change_tableau_sptr;

//: Create a smart-pointer to a boxm2_ocl_change_tableau tableau.
struct boxm2_ocl_change_tableau_new : public boxm2_ocl_change_tableau_sptr
{
  //: Constructor - create an empty vgui_easy3D_tableau.
  typedef boxm2_ocl_change_tableau_sptr base;
  boxm2_ocl_change_tableau_new() : base( new boxm2_ocl_change_tableau ) { }
};

#endif // boxm2_ocl_change_tableau_h
