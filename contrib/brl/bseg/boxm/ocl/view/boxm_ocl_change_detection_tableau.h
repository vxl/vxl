// This is brl/bseg/boxm/ocl/view/boxm_ocl_change_detection_tableau.h
#ifndef boxm_ocl_change_detection_tableau_h_
#define boxm_ocl_change_detection_tableau_h_
//:
// \file
// \brief A tableau to view updating octree models in real time
// \author Andrew Miller
// \date July 14, 2010
#include <vgui/vgui_gl.h>
#include <vgui/vgui_event_condition.h>
#include <boxm/ocl/view/boxm_ocl_change_detection_tableau_sptr.h>
#include <boxm/ocl/boxm_ocl_scene.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <bocl/bocl_utils.h>
#include <vcl_vector.h>
#include <vil/vil_image_view.h>
#include <vgui/vgui_tableau.h>
#include <bsta/bsta_histogram.h>

class boxm_ocl_change_detection_tableau: public vgui_tableau
{
 public:
    boxm_ocl_change_detection_tableau();
    ~boxm_ocl_change_detection_tableau();


    virtual bool handle( vgui_event const& e );
    //virtual bool idle();

    bool init(boxm_ocl_scene * scene,
              vcl_vector<vcl_string> cam_files,
              vcl_vector<vcl_string> img_files,
              vcl_vector<float> &hist,
              vcl_string save_img_dir="");

    //:  set the GL buffer which needs to be displayed.
    void set_glbuffer(GLuint  pbuffer){pbuffer_=pbuffer;}
    void setup_gl_matrices();

 protected:
    //render and update functions
    bool change_detection();
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

    //list of cameras and images for update
    int curr_frame_;
    vcl_vector<vcl_string> cam_files_;
    vcl_vector<vcl_string> img_files_;

    vil_image_view<float> obs_img_;
    vpgl_perspective_camera<double> *cam_;
    vcl_vector<float>  hist_;

    vcl_string save_img_dir_;

};

//: Create a smart-pointer to a boxm_ocl_change_detection_tableau tableau.
struct boxm_ocl_change_detection_tableau_new : public boxm_ocl_change_detection_tableau_sptr
{
  //: Constructor - create an empty vgui_easy3D_tableau.
  boxm_ocl_change_detection_tableau_new() : boxm_ocl_change_detection_tableau_sptr( new boxm_ocl_change_detection_tableau ) { }
};

#endif // boxm_ocl_change_detection_tableau_h_
