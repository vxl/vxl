#include "boxm2_view_utils.h"
//:
// \file
#include <bocl/bocl_utils.h> //for check_val and error_to_string
#include <vnl/vnl_quaternion.h>
#include <vpgl/vpgl_perspective_camera.h>

//: Helper method to create a shared cl-gl context on a device
cl_context boxm2_view_utils::create_clgl_context(cl_device_id& device)
{
  //init glew
  GLenum err = glewInit();
  if (GLEW_OK != err)
    std::cout<< "GlewInit Error: "<<glewGetErrorString(err)<<std::endl;    // Problem: glewInit failed, something is seriously wrong.

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
    std::cout<<"Error: Failed to create a compute CL/GL context!" << error_to_string(status) <<std::endl;
    return 0;
  }
  return ComputeContext;
}


void
boxm2_view_utils::convert_camera_to_vrml(vgl_vector_3d<double>  look,
                                         vgl_vector_3d<double> up_in,
                                         vgl_vector_3d<double> & res_axis_out,
                                         double & res_angle)
{
    look=normalize(look);
    up_in=normalize(up_in);
    double temp_d = dot_product(look,up_in);

    vgl_vector_3d<double> v_vxl = (up_in-look*temp_d);
    v_vxl = normalize(v_vxl);
    vgl_vector_3d<double> norm_axis_vxl(look.y(),-look.x(),0);
    norm_axis_vxl = normalize(norm_axis_vxl);

    vnl_quaternion<double> norm_quat_vxl;

    if ( dot_product(norm_axis_vxl, norm_axis_vxl) < 1e-8 )
    {
        // Already aligned, or maybe inverted.
        if ( look.z() > 0.0 )
            norm_quat_vxl=vnl_quaternion<double>(0,1,0,0.0);
        else
            norm_quat_vxl=vnl_quaternion<double>(0,0,0,1.0);
    }
    else
    {
        norm_quat_vxl=vnl_quaternion<double>(vnl_vector_fixed<double,3>(norm_axis_vxl.x(),norm_axis_vxl.y(),norm_axis_vxl.z()),std::acos(-look.z()));
    }
    // norm_quat now holds the rotation needed to line up the view directions.
    // We need to find the rotation to align the up vectors also.

    // Need to rotate the world y vector to see where it ends up.
    // Find the inverse rotation.

    vnl_quaternion<double> inv_norm_quat_vxl;
    inv_norm_quat_vxl.r()=norm_quat_vxl.r();
    inv_norm_quat_vxl.x()=-norm_quat_vxl.x();
    inv_norm_quat_vxl.y()=-norm_quat_vxl.y();
    inv_norm_quat_vxl.z()=-norm_quat_vxl.z();

    vnl_quaternion<double> y_quat_vxl(0,1,0,0.0);
    vnl_quaternion<double> new_y_quat_vxl=norm_quat_vxl*y_quat_vxl;
    new_y_quat_vxl=new_y_quat_vxl*inv_norm_quat_vxl;
    // Rotate the y.
    vgl_vector_3d<double> new_y_vxl(new_y_quat_vxl.x(),new_y_quat_vxl.y(),new_y_quat_vxl.z());
    vgl_vector_3d<double> temp_v_vxl=cross_product(new_y_vxl,v_vxl);
    // Now need to find out how much to rotate about n to line up y.
    if (dot_product(temp_v_vxl,temp_v_vxl)<1.e-8 )
    {
        // The old and new may be pointing in the same or opposite. Need
        // to generate a vector perpendicular to the old or new y.
        temp_v_vxl.set(0,-v_vxl.z(),v_vxl.y());
        if (dot_product(temp_v_vxl,temp_v_vxl)<1.e-8 )
        {
            temp_v_vxl.set(v_vxl.z(),0,-v_vxl.x());
        }
    }
    temp_v_vxl=normalize(temp_v_vxl);
    vnl_quaternion<double> rot_y_quat_vxl(vnl_vector_fixed<double,3>(temp_v_vxl.x(),temp_v_vxl.y(),temp_v_vxl.z()),std::acos(dot_product(new_y_vxl,v_vxl)));

    vnl_quaternion<double> rot_quat_vxl=rot_y_quat_vxl*norm_quat_vxl;
    // rot_y_quat holds the rotation about the initial camera direction needed
    // to align the up vectors in the final position.
    res_axis_out.set(rot_quat_vxl.axis()[0],rot_quat_vxl.axis()[1],rot_quat_vxl.axis()[2]);
    res_angle=rot_quat_vxl.angle();
}

void boxm2_view_utils::add_vrml_animation(std::ostream & os,std::vector<vpgl_camera_double_sptr > &  cameras)
{
    std::vector<vnl_vector_fixed<double,4> >  orientations;
    std::vector<vnl_vector_fixed<double,3> >  positions;
    double rot_angle;
    vgl_vector_3d<double> up(0,0,1);
    vgl_vector_3d<double> rot_axis;

    for (unsigned i =0;i<cameras.size();i++)
    {
      if (vpgl_perspective_camera<double>* cam = dynamic_cast<vpgl_perspective_camera<double>* >(cameras[i].ptr()))
      {
        positions.push_back(vnl_vector_fixed<double,3>(cam->camera_center().x(),
                                                       cam->camera_center().y(),
                                                       cam->camera_center().z()));
        vgl_vector_3d<double> up(0,0,1);
        vgl_vector_3d<double> rot_axis;
        boxm2_view_utils::convert_camera_to_vrml(cam->principal_axis(), up, rot_axis, rot_angle);
        orientations.push_back(vnl_vector_fixed<double,4>(rot_axis.x(),rot_axis.y(),rot_axis.z(),rot_angle));
      }
    }

    float inc =1.0/(float)cameras.size();

    os<< " DEF Clock TimeSensor {\n"
      << " cycleInterval 50.0\n"
      << " loop TRUE\n"
      << "}\n"
      <<"DEF camerapath PositionInterpolator {\n"
      <<"\tkey [\n";
    for (unsigned i=0;i<cameras.size();i++)
      os<<inc*(float)i<<',';
    os<<"]\n"
      <<"\tkeyValue [\n";
    for (unsigned i=0;i<cameras.size();i++)
      os<<positions[i][0]<<' '<<positions[i][1]<<' '<<positions[i][2]<<','<<std::endl;
    os<<"]\n"
      <<"}\n"
      <<"DEF camerarot OrientationInterpolator {\n"
      <<"\tkey [";
    for (unsigned i=0;i<cameras.size();i++)
      os<<inc*(float)i<<',';
    os<<"]\n"
      <<std::endl
      <<"\tkeyValue [";
    for (unsigned i=0;i<cameras.size();i++)
      os<<orientations[i][0]<<' '<<orientations[i][1]<<' '<<orientations[i][2]<<' '<<orientations[i][3]<<','<<std::endl;
    os<<"]\n"
      <<"}\n"
      <<" DEF flypos Viewpoint {\n"
      <<" fieldOfView 0.3\n"
      <<" position 0 0 0\n"
      <<" orientation  0 0 1 0\n"
      <<" description \"v2\"\n"
      <<" jump FALSE\n"
      <<" }\n"

      <<"ROUTE Clock.fraction_changed  TO camerapath.set_fraction\n"
      <<"ROUTE Clock.fraction_changed  TO camerarot.set_fraction\n"
      <<"ROUTE camerapath.value_changed TO flypos.position\n"
      <<"ROUTE camerarot.value_changed TO flypos.orientation\n"
      <<std::endl;
}
