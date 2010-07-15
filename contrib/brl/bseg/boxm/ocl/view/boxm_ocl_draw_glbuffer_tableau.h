// This is boxm/ocl/view/boxm_ocl_draw_glbuffer_tableau.h
#ifndef boxm_ocl_draw_glbuffer_tableau_h
#define boxm_ocl_draw_glbuffer_tableau_h
//:
// \file
// \brief A tableau to view octree models in real time
// \author Vishal Jain, Andrew Miller
// \date July 13, 2010
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_gl.h>
#include <vgui/vgui_drag_mixin.h>
#include <vgui/vgui_event_condition.h>
#include <boxm/ocl/view/boxm_ocl_draw_glbuffer_tableau_sptr.h>
#include <boxm/ocl/boxm_ocl_scene.h>
#include <boxm/boxm_scene_base.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <bocl/bocl_utils.h>

class boxm_ocl_draw_glbuffer_tableau: public vgui_tableau, public vgui_drag_mixin
{

public:
    boxm_ocl_draw_glbuffer_tableau();
    ~boxm_ocl_draw_glbuffer_tableau();


    virtual bool handle( vgui_event const& e );

    bool init(boxm_ocl_scene * scene, unsigned ni, unsigned nj, vpgl_perspective_camera<double> * cam);
    bool init_ocl();
    
    //;  set the GL buffer which needs to be displayed.
    void set_glbuffer(GLuint  pbuffer){pbuffer_=pbuffer;}

    void setup_gl_matrices();

    bool mouse_up(int x, int y, vgui_button button, vgui_modifier modifier);
    bool mouse_drag(int x, int y, vgui_button button, vgui_modifier modifier);
    bool mouse_down(int x, int y, vgui_button button, vgui_modifier modifier);

    vgui_event_condition c_mouse_rotate;
    vgui_event_condition c_mouse_translate;
    vgui_event_condition c_mouse_zoom;
    struct token_t
    {
        float quat[4];     // quaternion
        float scale;
        float trans[3];
        float fov;
        token_t() {
            quat[0] = quat[1] = quat[2] = quat[3] = 0.0f;
            scale = 0.0;
            trans[0] = trans[1] = trans[2] = 0.0;
            fov = 45;
        }
    };

    token_t token;
    token_t home;
protected:
    vgui_event event;
    vgui_event last;

    float beginx;
    float beginy;
    token_t lastpos;
    float prevx;
    float prevy;

    bool render_frame();
    GLuint pbuffer_;
    boxm_ocl_scene * scene_;
    unsigned ni_;
    unsigned nj_;
    vpgl_perspective_camera<double> cam_;
    vpgl_perspective_camera<double> default_cam_;
    vgl_homg_point_3d<double> stare_point_;
    bool do_init_ocl;
};
//: Create a smart-pointer to a boxm_ocl_draw_glbuffer_tableau tableau.
struct boxm_ocl_draw_glbuffer_tableau_new : public boxm_ocl_draw_glbuffer_tableau_sptr
{
  //: Constructor - create an empty vgui_easy3D_tableau.
  boxm_ocl_draw_glbuffer_tableau_new() : boxm_ocl_draw_glbuffer_tableau_sptr( new   boxm_ocl_draw_glbuffer_tableau ) { }
};
#endif // boxm_ocl_draw_glbuffer_tableau
