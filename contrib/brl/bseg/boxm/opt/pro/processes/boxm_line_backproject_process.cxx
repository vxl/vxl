// This is brl/bseg/boxm/pro/processes/boxm_line_backproject_process.cxx
//:
// \file
// \brief  A process for computing the backprojection of lines represented 
//         as an image with 3 plane images (plane 0=a, plane1=b, plane2=c, 
//         line is defined as ax+by=c). The backprojection, which is a plane
//         is stored as an other image with 4 planes (ax+by+cz=d) as the output
//
// \author Gamze Tunali
// \date   Feb 4, 2010
//
// \verbatim
//  Modifications
//
// \endverbatim

// \file
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <vpgl/vpgl_proj_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>
#include <vpgl/algo/vpgl_ray.h>

#include <vgl/vgl_plane_3d.h>


#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>

#include <vcl_cstdio.h>

//: globals
namespace boxm_line_backproject_process_globals
{
  const unsigned n_inputs_ = 2;
  const unsigned n_outputs_ = 1;
}

//: set input and output types
bool boxm_line_backproject_process_cons(bprb_func_process& pro)
{
  using namespace boxm_line_backproject_process_globals;
  // process takes 2 input:
  // input[0]: The edge image (vil_image_view<float> with three planes for (a,b,c))
  // input[1]: camera
  vcl_vector<vcl_string> input_types_(n_inputs_);
  unsigned i = 0;
  input_types_[i++] = "vil_image_view_base_sptr";
  input_types_[i++] = "vpgl_camera_double_sptr"; 

  if (!pro.set_input_types(input_types_))
    return false;

  // process has 1 output:
  // output[0]: The plane image
  vcl_vector<vcl_string> output_types_(n_outputs_);
  unsigned j = 0;
  output_types_[j++] = "vil_image_view_base_sptr";
  return pro.set_output_types(output_types_);
}

//:  optimizes rpc camera parameters based on edges
bool boxm_line_backproject_process(bprb_func_process& pro)
{
  using namespace boxm_line_backproject_process_globals;

  //check number of inputs
  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << " The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }

  // get the inputs
  unsigned i = 0;  

  // image
  vil_image_view_base_sptr edge_image_sptr = pro.get_input<vil_image_view_base_sptr>(i++);

  // camera
  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(i++);

  // make sure that image has 3 planes
  if (edge_image_sptr->nplanes() != 3) {
    vcl_cerr << "boxm_line_backproject_process: The edge image is expected to have 3 planes!" << vcl_endl;
    return false;
  }
  
  vil_image_view<float> *plain_image;
  if (edge_image_sptr->pixel_format() == VIL_PIXEL_FORMAT_FLOAT) {
    vil_image_view<float> edge_image(edge_image_sptr);
    unsigned ni=edge_image.ni();
    unsigned nj=edge_image.nj();

    // the output image
    plain_image = new vil_image_view<float>(ni,nj,4);
    float a,b,c;

    if (camera->type_name() == "vpgl_proj_camera") {
      vpgl_proj_camera<double>* cam = dynamic_cast<vpgl_proj_camera<double>*>(camera.ptr());

      for (unsigned i=0; i<ni; i++) {
        for (unsigned j=0; j<nj; j++) {
          a=edge_image(i,j,0);
          b=edge_image(i,j,1);
          c=edge_image(i,j,2);
          //create a line and backproject it
          vgl_homg_line_2d<double> image_line(a,b,c); // Camera should be projective!!
          vgl_homg_plane_3d<double> plane = cam->backproject(image_line);
          if (plane.a()==0 || plane.b()==0 || plane.c()==0 || plane.d()==0)
            vcl_cout << "ZERO a,b,c,d " << vcl_endl;
          (*plain_image)(i,j,0)=plane.a();
          (*plain_image)(i,j,1)=plane.b();
          (*plain_image)(i,j,2)=plane.c();
          (*plain_image)(i,j,3)=plane.d();
        }
      }
    } else if (camera->type_name() == "vpgl_local_rational_camera") {
        vpgl_local_rational_camera<double>* cam = dynamic_cast<vpgl_local_rational_camera<double>*>(camera.ptr());
        for (unsigned i=0; i<ni; i++) {
          for (unsigned j=0; j<nj; j++) {
            a=edge_image(i,j,0);
            b=edge_image(i,j,1);
            c=edge_image(i,j,2);
            //create a line and backproject it
            vgl_homg_line_2d<double> image_line(a,b,c); 
            vgl_homg_point_2d<double> p1,p2;
            vgl_plane_3d<double> plane;
            image_line.get_two_points(p1,p2);
            if (vpgl_ray::plane_ray(*cam, vgl_point_2d<double>(p1), vgl_point_2d<double>(p2),plane)) {
              (*plain_image)(i,j,0)=plane.a();
              (*plain_image)(i,j,1)=plane.b();
              (*plain_image)(i,j,2)=plane.c();
              (*plain_image)(i,j,3)=plane.d();
            }
          }
        }
    } else {
        vcl_cerr << "boxm_line_backproject_process: The camera type [" << camera->type_name() << "]is not defined yet!" << vcl_endl;
        return false;
    }
  } else {
    vcl_cerr << "boxm_line_backproject_process: This pixel format is not supported yet!" << vcl_endl;
    return false;
  }
  // output
  unsigned j = 0;
  // update the camera and store
  pro.set_output_val<vil_image_view_base_sptr>(j++, plain_image);

  return true;
}
