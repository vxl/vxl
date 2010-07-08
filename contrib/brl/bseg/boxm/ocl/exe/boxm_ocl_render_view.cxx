
//    Volume rendering sample

//    This sample loads a 3D volume


// Utilities, OpenCL and system includes
#include <GL/glew.h>

#include <bocl/bocl_cl.h>
#include <bocl/bocl_utils.h>
#include <boxm/ocl/boxm_render_image_manager.h>
#include <boxm/ocl/view/boxm_ocl_draw_glbuffer_tableau.h>
#include <boxm/ocl/view/boxm_ocl_draw_glbuffer_tableau_sptr.h>

#include <boxm/boxm_scene.h>
#include <boxm/boxm_scene_parser.h>
#include <boxm/boxm_apm_traits.h>

#include <vpgl/vpgl_perspective_camera.h>
#include <vil/vil_image_view.h>

#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_timer.h>


#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_window.h>

#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_clear_tableau.h>
cl_context_properties * set_gl_props()
{
    cl_platform_id platform_id[1];
    cl_int status = clGetPlatformIDs (1, platform_id, NULL);
    if (status!=CL_SUCCESS) {
        vcl_cout<<error_to_string(status);
        return 0;
    }
#if defined (__APPLE__)
  CGLContextObj kCGLContext = CGLGetCurrentContext();
  CGLShareGroupObj kCGLShareGroup = CGLGetShareGroup(kCGLContext);
  cl_context_properties props[] =
  {
    CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE, (cl_context_properties)kCGLShareGroup,
    0
  };

  return props;
#else
#ifdef UNIX
  cl_context_properties props[] =
  {
    CL_GL_CONTEXT_KHR, (cl_context_properties)glXGetCurrentContext(),
    CL_GLX_DISPLAY_KHR, (cl_context_properties)glXGetCurrentDisplay(),
    CL_CONTEXT_PLATFORM, (cl_context_properties)cpPlatform,
    0
  };
  return props;

#else // Win32

  cl_context_properties props[] =
  {
    CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(),
    CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(),
    CL_CONTEXT_PLATFORM, (cl_context_properties)platform_id[0],
    0
  };
  return props;
#endif
#endif
}

int main(int argc, char ** argv)
{

    vgui::init(argc, argv);
    cl_context_properties * props=set_gl_props();

    vul_arg<vcl_string> camfile("-cam", "camera filename", "");
    vul_arg<vcl_string> scene_file("-scene", "scene filename", "");
    vul_arg<unsigned> ni("-ni", "Width of image", 640);
    vul_arg<unsigned> nj("-nj", "Height of image", 480);

    //// need this on some toolkit implementations to get the window up.
    vul_arg_parse(argc, argv);

    boxm_scene_parser parser;
    boxm_scene_base_sptr scene_ptr=new boxm_scene_base();
    scene_ptr->load_scene(scene_file(), parser);
    if (scene_ptr->appearence_model() == BOXM_APM_MOG_GREY) {
        typedef boct_tree<short,boxm_sample<BOXM_APM_MOG_GREY> > tree_type;
        boxm_scene<tree_type>* scene = new boxm_scene<tree_type>();
        scene->load_scene(parser);
        scene_ptr = scene;
    }
    else if (scene_ptr->appearence_model() == BOXM_APM_SIMPLE_GREY) {
        typedef boct_tree<short,boxm_sample<BOXM_APM_SIMPLE_GREY> > tree_type;
        boxm_scene<tree_type>* scene = new boxm_scene<tree_type>();
        scene->load_scene(parser);
        scene_ptr = scene;
    }
    else {
        vcl_cout<<"type is not defined yet "<<vcl_endl;
    }


    vcl_ifstream ifs(camfile().c_str());
    vpgl_perspective_camera<double>* pcam =new vpgl_perspective_camera<double>;
    if (!ifs.is_open()) {
        vcl_cerr << "Failed to open file " << camfile() << vcl_endl;
        return -1;
    }
    else  {
        ifs >> *pcam;
    }
    

    boxm_ocl_draw_glbuffer_tableau_new glbuffer_tableau;
    vgui_window* win = vgui::produce_window(ni(), nj(), "OpenCl Volume Visualizer");
    win->get_adaptor()->set_tableau(  glbuffer_tableau ); 
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        vcl_cout<< "Error: "<<glewGetErrorString(err)<<vcl_endl;
    }
    GLboolean bGLEW = glewIsSupported("GL_VERSION_2_0  GL_ARB_pixel_buffer_object");
    glbuffer_tableau->init(scene_ptr,ni(),nj(),pcam);

    return  vgui::run();

}