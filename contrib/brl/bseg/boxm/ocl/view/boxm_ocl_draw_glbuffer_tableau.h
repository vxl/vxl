#ifndef boxm_ocl_draw_glbuffer_tableau_h
#define boxm_ocl_draw_glbuffer_tableau_h

#include <vgui/vgui_tableau.h>
#include <vgui/vgui_gl.h>
#include <boxm/ocl/view/boxm_ocl_draw_glbuffer_tableau_sptr.h>
#include <boxm/boxm_scene_base.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <bocl/bocl_utils.h>

class boxm_ocl_draw_glbuffer_tableau:public vgui_tableau
{

public:
    boxm_ocl_draw_glbuffer_tableau();
    ~boxm_ocl_draw_glbuffer_tableau();


    virtual bool handle( vgui_event const& e );

    bool init(boxm_scene_base_sptr scene, unsigned ni, unsigned nj, vpgl_perspective_camera<double> * cam);
    //;  set the GL buffer which needs to be displayed.
    void set_glbuffer(GLuint  pbuffer){pbuffer_=pbuffer;}

    void setup_gl_matrices();
protected:
    bool render_frame();
    GLuint pbuffer_;
    boxm_scene_base_sptr scene_;
    unsigned ni_;
    unsigned nj_;
    vpgl_perspective_camera<double> * cam_;
};
//: Create a smart-pointer to a boxm_ocl_draw_glbuffer_tableau tableau.
struct boxm_ocl_draw_glbuffer_tableau_new : public boxm_ocl_draw_glbuffer_tableau_sptr
{

  //: Constructor - create an empty vgui_easy3D_tableau.
  boxm_ocl_draw_glbuffer_tableau_new() : boxm_ocl_draw_glbuffer_tableau_sptr( new boxm_ocl_draw_glbuffer_tableau ) { }
};
#endif // boxm_ocl_draw_glbuffer_tableau