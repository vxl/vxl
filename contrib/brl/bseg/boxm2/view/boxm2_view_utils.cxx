#include "boxm2_view_utils.h"
#include <bocl/bocl_utils.h> //for check_val and error_to_string

//: Helper method to create a shared cl-gl context on a device
cl_context boxm2_view_utils::create_clgl_context(cl_device_id& device)
{
  //init glew
  GLenum err = glewInit();
  if (GLEW_OK != err)
    vcl_cout<< "GlewInit Error: "<<glewGetErrorString(err)<<vcl_endl;    // Problem: glewInit failed, something is seriously wrong.

  //initialize the render manager
  cl_platform_id platform_id[1];
  int status = clGetDeviceInfo(device,CL_DEVICE_PLATFORM,sizeof(platform_id),(void*) platform_id,NULL);
  if (!check_val(status, CL_SUCCESS, "boxm2_render Tableau::create_cl_gl_context CL_DEVICE_PLATFORM failed."))
    return 0;

  ////create OpenCL context
  cl_context ComputeContext;
#ifdef WIN32
  cl_context_properties props[] =
  {
    CL_GL_CONTEXT_KHR, (cl_context_properties) wglGetCurrentContext(),
    CL_WGL_HDC_KHR, (cl_context_properties) wglGetCurrentDC(),
    CL_CONTEXT_PLATFORM, (cl_context_properties) platform_id[0],
    0
  };
  //create OpenCL context with display properties determined above
  ComputeContext = clCreateContext(props, 1, &device, NULL, NULL, &status);
#elif defined(__APPLE__) || defined(MACOSX)
  CGLContextObj kCGLContext = CGLGetCurrentContext();
  CGLShareGroupObj kCGLShareGroup = CGLGetShareGroup(kCGLContext);

  cl_context_properties props[] = {
    CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE, (cl_context_properties)kCGLShareGroup,
    CL_CONTEXT_PLATFORM, (cl_context_properties) platform_id[0],
    0
  };
  //create a CL context from a CGL share group - no GPU devices must be passed,
  //all CL compliant devices in the CGL share group will be used to create the context. more info in cl_gl_ext.h
  ComputeContext = clCreateContext(props, 0, 0, NULL, NULL, &status);
#else
  cl_context_properties props[] =
  {
      CL_GL_CONTEXT_KHR, (cl_context_properties)glXGetCurrentContext(),
      CL_GLX_DISPLAY_KHR, (cl_context_properties)glXGetCurrentDisplay(),
      CL_CONTEXT_PLATFORM, (cl_context_properties) platform_id[0],
      0
  };
  ComputeContext = clCreateContext(props, 1, &device, NULL, NULL, &status);
#endif

  if (status!=CL_SUCCESS) {
    vcl_cout<<"Error: Failed to create a compute CL/GL context!" << error_to_string(status) <<vcl_endl;
    return 0;
  }
  return ComputeContext;
}
