// This is brl/bseg/boxm/ocl/view/boxm_change_detection_bit_tableau.h
#ifndef boxm_change_detection_bit_tableau_h_
#define boxm_change_detection_bit_tableau_h_
//:
// \file
// \brief A tableau to view updating "bit_tree" models in real time
// \author Andrew Miller
// \date Sept 15, 2010
#include "boxm_ocl_include_glew.h"
#include <boxm/ocl/view/boxm_cam_tableau.h>
#include <vgui/vgui_gl.h>
#include <vgui/vgui_event_condition.h>
#include <boxm/ocl/view/boxm_change_detection_bit_tableau_sptr.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <bocl/bocl_utils.h>
#include <vcl_vector.h>
#include <vgui/vgui_statusbar.h>
#include <boxm/ocl/boxm_ocl_bit_scene.h>


class boxm_change_detection_bit_tableau: public vgui_tableau
{
 public:
    boxm_change_detection_bit_tableau();
    ~boxm_change_detection_bit_tableau();


    virtual bool handle( vgui_event const& e );

    bool init(boxm_ocl_bit_scene * scene, 
              unsigned ni, unsigned nj,
              vcl_vector<vcl_string> cam_files,
              vcl_vector<vcl_string> img_files,
              vcl_vector<vcl_string> exp_img_files);

    //:  set the GL buffer which needs to be displayed.
    void set_glbuffer(GLuint  pbuffer) {pbuffer_=pbuffer;}
    void set_statusbar(vgui_statusbar* status) { status_ = status; }
    
 protected:
    //render and update functions
    bool change_detection();
    bool init_ocl();
    bool do_init_ocl_;


    //openGL buffer
    GLuint pbuffer_;
    boxm_ocl_bit_scene * scene_;
    unsigned ni_;
    unsigned nj_;
    vnl_random rand;
    
    //list of cameras and images for update
    int curr_frame_;
    vcl_vector<vcl_string> cam_files_;
    vcl_vector<vcl_string> img_files_;
    vcl_vector<vcl_string> exp_img_files_;
    bool do_update_;
    int count_, curr_count_, refine_count_;

    bool toggle_old_;
    void setup_gl_matrices();
    //status bar for some info
    vgui_statusbar* status_;

};

//: Create a smart-pointer to a boxm_change_detection_bit_tableau tableau.
struct boxm_change_detection_bit_tableau_new : public boxm_change_detection_bit_tableau_sptr
{
  //: Constructor - create an empty vgui_easy3D_tableau.
  boxm_change_detection_bit_tableau_new() : boxm_change_detection_bit_tableau_sptr( new boxm_change_detection_bit_tableau ) { }
};

#endif // boxm_change_detection_bit_tableau_h_
