
//    Volume rendering sample

//    This sample loads a 3D volume


// Utilities, OpenCL and system includes
#include <bocl/bocl_cl.h>
#include <bocl/bocl_utils.h>
#include <boxm/ocl/boxm_render_image_manager.h>

#include <boxm/boxm_scene.h>
#include <boxm/boxm_scene_parser.h>
#include <boxm/boxm_apm_traits.h>

#include <vpgl/vpgl_perspective_camera.h>
#include <vil/vil_image_view.h>

#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_timer.h>
#if defined(WIN32)
#include <windows.h>
#endif
#ifdef UNIX
    #if defined(__APPLE__) || defined(MACOSX)
        #include <OpenGL/OpenGL.h>
        #include <GLUT/glut.h>
    #else
        #include <GL/glut.h>
        #include <GL/glx.h>
    #endif
#endif


#if defined (__APPLE__) || defined(MACOSX)
  #define GL_SHARING_EXTENSION "cl_APPLE_gl_sharing"
#else
  #define GL_SHARING_EXTENSION "cl_khr_gl_sharing"
  // GLEW and GLUT includes
  #include <GL/glew.h>
  #include <GL/glut.h>
#endif

// Constants, defines, typedefs and global declarations
//*****************************************************************************

typedef unsigned int uint;
typedef unsigned char uchar;

uint width = 640;
uint height = 480;

vul_file_iterator file_it;
vul_timer t;
GLuint pbo = 0;                 // OpenGL pixel buffer object
int iwindowhandle;          // handle to the GLUT window

// OpenCL vars
cl_mem pbo_cl;

cl_bool g_glInterop = false;

char camdirname[]="f:/apl/cams360/*.txt";
//char camdirname[]="D:/data/Fort-APL/good_cams/*.txt";
int iFrameCount = 0;                // FPS count for averaging

// Forward Function declarations
//*****************************************************************************
// OpenCL Functions
void render();


// OpenGL functionality
void InitGL(int argc,  char** argv);
void DisplayGL();
//void Reshape(int w, int h);
void Idle(void);

// Main program
//*****************************************************************************
int main(int argc,  char** argv)
{
  vcl_cout<<"Render Expected Image "<<vcl_endl;
  vul_arg<vcl_string> cam_file("-cam", "camera filename", "");
  vul_arg<vcl_string> camdir("-camdir", "camera directory", "");
  vul_arg<vcl_string> scene_file("-scene", "scene filename", "");
  vul_arg<vcl_string> img_outfile("-img", "image filename", "");
  vul_arg<unsigned> ni("-ni", "Width of image", 640);
  vul_arg<unsigned> nj("-nj", "Height of image", 480);

  // need this on some toolkit implementations to get the window up.
  InitGL(argc,argv);
  vul_arg_parse(argc, argv);

  // load the scene;
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

  // load the camera;
  file_it.reset(camdirname);
  vcl_string filename(file_it());

  vcl_ifstream ifs(filename.c_str());
  vpgl_perspective_camera<double>* pcam =new vpgl_perspective_camera<double>;
  if (!ifs.is_open()) {
    vcl_cerr << "Failed to open file " << cam_file() << vcl_endl;
    return -1;
  }
  else {
    ifs >> *pcam;
  }

  vil_image_view<typename boxm_apm_traits<BOXM_APM_SIMPLE_GREY>::obs_datatype > expected(ni(),nj());
  boxm_render_image_manager<boxm_sample<BOXM_APM_SIMPLE_GREY> >* ray_mgr
      = boxm_render_image_manager<boxm_sample<BOXM_APM_SIMPLE_GREY> >::instance();

  cl_platform_id platform_id[1];
  cl_int status = clGetPlatformIDs (1, platform_id, NULL);

#if defined (__APPLE__)
  CGLContextObj kCGLContext = CGLGetCurrentContext();
  CGLShareGroupObj kCGLShareGroup = CGLGetShareGroup(kCGLContext);
  cl_context_properties props[] =
  {
    CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE, (cl_context_properties)kCGLShareGroup,
    0
  };
  ray_mgr->context_ = clCreateContext(props, 1,&ray_mgr->devices()[0], NULL, NULL, &status);

#else
#ifdef UNIX
  cl_context_properties props[] =
  {
    CL_GL_CONTEXT_KHR, (cl_context_properties)glXGetCurrentContext(),
    CL_GLX_DISPLAY_KHR, (cl_context_properties)glXGetCurrentDisplay(),
    CL_CONTEXT_PLATFORM, (cl_context_properties)cpPlatform,
    0
  };
  ray_mgr->context_ = clCreateContext(props, 1, &ray_mgr->devices()[0], NULL, NULL, &status);

#else // Win32

  cl_context_properties props[] =
  {
    CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(),
    CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(),
    CL_CONTEXT_PLATFORM, (cl_context_properties)platform_id[0],
    0
  };

  ray_mgr->context_ = clCreateContext(props, 1, &ray_mgr->devices()[0], NULL, NULL, &status);

#endif
#endif

  int bundle_dim=8;
  ray_mgr->set_bundle_ni(bundle_dim);
  ray_mgr->set_bundle_nj(bundle_dim);
  if (scene_ptr->appearence_model()==BOXM_APM_SIMPLE_GREY)
  {
    typedef boct_tree<short, boxm_sample<BOXM_APM_SIMPLE_GREY> > type;
    boxm_scene<type>* scene = dynamic_cast<boxm_scene<type>*> (scene_ptr.as_pointer());
    ray_mgr->init_ray_trace(scene, pcam, expected);
    bool good=true;
    good=good && ray_mgr->set_scene_data();
    good=good && ray_mgr->set_all_blocks();
    good=good && ray_mgr->set_scene_data_buffers();
    good=good && ray_mgr->set_tree_buffers();
    // run the raytracing
    good=good && ray_mgr->set_persp_camera(pcam);
    good=good && ray_mgr->set_persp_camera_buffers();
    good=good && ray_mgr->set_input_image();
    good=good && ray_mgr->set_image_dims_buffers();

    if (pbo) {
      // delete old buffer
      clReleaseMemObject(ray_mgr->image_buf_);
      glDeleteBuffersARB(1, &pbo);
    }

    // create pixel buffer object for display
    glGenBuffersARB(1, &pbo);
    glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, pbo);
    glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, ray_mgr->wni() * ray_mgr->wnj() * sizeof(GLubyte) * 4, 0, GL_STREAM_DRAW_ARB);
    glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);

    cl_int status;
    // create OpenCL buffer from GL PBO
    ray_mgr->image_buf_ = clCreateFromGLBuffer(ray_mgr->context(),CL_MEM_WRITE_ONLY, pbo, &status);
    ray_mgr->set_kernel();
    ray_mgr->set_args();
    ray_mgr->set_commandqueue();
    ray_mgr->set_workspace();
  }
  t.mark();
  glutMainLoop();
}

// render image using OpenCL
//*****************************************************************************
void render()
{
  boxm_render_image_manager<boxm_sample<BOXM_APM_SIMPLE_GREY> >* ray_mgr = boxm_render_image_manager<boxm_sample<BOXM_APM_SIMPLE_GREY> >::instance();
  cl_int status= clEnqueueAcquireGLObjects(ray_mgr->command_queue_, 1, &ray_mgr->image_buf_ , 0, 0, 0);
  ++file_it;
  if (!file_it)
    file_it.reset(camdirname);

  vcl_ifstream ifs(file_it());
  vpgl_perspective_camera<double>* pcam =new vpgl_perspective_camera<double>;
  if (!ifs.is_open()) {
    vcl_cerr << "Failed to open file " << file_it() << vcl_endl;
    return;
  }
  else{
    ifs >> *pcam;
  }

  ray_mgr->set_persp_camera(pcam);
  ray_mgr->write_persp_camera_buffers();
  ray_mgr->run();
  status=clEnqueueReleaseGLObjects(ray_mgr->command_queue_, 1, &ray_mgr->image_buf_ , 0, 0, 0);
  clFinish( ray_mgr->command_queue_ );
}

// Display callback for GLUT main loop
//*****************************************************************************
void DisplayGL()
{
  if (iFrameCount>30)
  {
    iFrameCount=0;
    t.mark();
  }
  ++iFrameCount;
  // process
  render();

  char cFPS[100];
  vcl_sprintf(cFPS,"Frames per sec %f",(float)iFrameCount/(t.all()*1e-3));

  // draw image from PBO
  glClear(GL_COLOR_BUFFER_BIT);
  glDisable(GL_DEPTH_TEST);
  glRasterPos2i(0, 1);
  glPixelZoom(1,-1);
  glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, pbo);
  glDrawPixels(width, height, GL_RGBA, GL_UNSIGNED_BYTE, 0);
  glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
  // flip backbuffer to screen
  glutSwapBuffers();
  glutPostRedisplay();

  glutSetWindowTitle(cFPS);
}

// GL Idle time callback
void Idle()
{
  glutPostRedisplay();
}

void Reshape(int x, int y)
{
  width = x; height = y;

  glViewport(0, 0, x, y);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, 1.0, 0.0, 1.0, 0.0, 1.0);
}


// Initialize GL
//*****************************************************************************
void InitGL(int argc,  char **argv)
{
  // initialize GLUT

  glutInit(&argc, (char **)argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
  glutInitWindowPosition (glutGet(GLUT_SCREEN_WIDTH)/2 - width/2,
                          glutGet(GLUT_SCREEN_HEIGHT)/2 - height/2);
  glutInitWindowSize(width, height);
  iwindowhandle = glutCreateWindow("OpenCL Expected Image Rendering");

  // register glut callbacks
  glutDisplayFunc(DisplayGL);
  glutReshapeFunc(Reshape);
  glutIdleFunc(Idle);

  glewInit();

  //// initialize necessary OpenGL extensions
  GLboolean bGLEW = glewIsSupported("GL_VERSION_2_0 GL_ARB_pixel_buffer_object");
  g_glInterop = true;
}

