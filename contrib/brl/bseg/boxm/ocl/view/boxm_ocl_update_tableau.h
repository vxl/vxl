// This is brl/bseg/boxm/ocl/view/boxm_ocl_update_tableau.h
#ifndef boxm_ocl_update_tableau_h_
#define boxm_ocl_update_tableau_h_
//:
// \file
// \brief A tableau to view updating octree models in real time
// \author Andrew Miller
// \date July 14, 2010
#include <boxm/ocl/view/boxm_cam_tableau.h>
#include <vgui/vgui_gl.h>
#include <vgui/vgui_event_condition.h>
#include <boxm/ocl/view/boxm_ocl_update_tableau_sptr.h>
#include <boxm/ocl/boxm_ocl_scene.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <bocl/bocl_utils.h>
#include <vcl_vector.h>

class boxm_ocl_update_tableau: public boxm_cam_tableau
{
 public:
    boxm_ocl_update_tableau();
    ~boxm_ocl_update_tableau();


    virtual bool handle( vgui_event const& e );
    //virtual bool idle();

    bool init(boxm_ocl_scene * scene, unsigned ni, unsigned nj,
              vpgl_perspective_camera<double> * cam,
              vcl_vector<vcl_string> cam_files,
              vcl_vector<vcl_string> img_files);

    //:  set the GL buffer which needs to be displayed.
    void set_glbuffer(GLuint  pbuffer){pbuffer_=pbuffer;}
    bool refine_model();

 protected:
    //render and update functions
    bool render_frame();
    bool update_model();
    bool init_ocl();
    bool do_init_ocl_;

    //keep track of mouse dragging
    float beginx;
    float beginy;
    float prevx;
    float prevy;

    //openGL buffer
    GLuint pbuffer_;
    boxm_ocl_scene * scene_;
    unsigned ni_;
    unsigned nj_;

    vnl_random rand;
    //list of cameras and images for update
    int curr_frame_;
    vcl_vector<vcl_string> cam_files_;
    vcl_vector<vcl_string> img_files_;
    bool do_update_;
    int count_;
};

//: Create a smart-pointer to a boxm_ocl_update_tableau tableau.
struct boxm_ocl_update_tableau_new : public boxm_ocl_update_tableau_sptr
{
  //: Constructor - create an empty vgui_easy3D_tableau.
  boxm_ocl_update_tableau_new() : boxm_ocl_update_tableau_sptr( new boxm_ocl_update_tableau ) { }
};

#endif // boxm_ocl_update_tableau_h_
