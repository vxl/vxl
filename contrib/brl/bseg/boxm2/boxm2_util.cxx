#include "boxm2_util.h"

//vgl includes
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_box_2d.h>
#include <vpgl/algo/vpgl_project.h>

//vnl includes
#include <vnl/vnl_vector.h>
#include <vnl/vnl_random.h>
#include <vnl/algo/vnl_svd.h>

//vil includes
#include <vil/vil_image_view.h>

//vcl io stuff
#include <vcl_iostream.h>
#include <vcl_fstream.h>

void boxm2_util::random_permutation(int* buffer, int size)
{
  vnl_random random(9667566);
  
  //shuffle the buffer
  for(int i=0; i<size; i++) 
  {
    //swap current value with random one after it
    int curr = buffer[i]; 
    int rInd = random.lrand32(i, size-1); 
    buffer[i] = buffer[rInd];
    buffer[rInd] = curr; 
  }

}
float boxm2_util::clamp(float x, float a, float b)
{
    return x < a ? a : (x > b ? b : x);
}



//Constructs a camera given elevation, azimuth (degrees), radius, and bounding box.  
vpgl_perspective_camera<double>* 
boxm2_util::construct_camera( double elevation, 
                              double azimuth, 
                              double radius, 
                              unsigned ni,
                              unsigned nj,
                              vgl_box_3d<double> bb)
{
  double dni = static_cast<double>(ni);
  double dnj = static_cast<double>(nj);
  
  //
  //find a camera that will project the scene bounding box
  //entirely inside the image
  //
  // 1) determine the stare point (center of bounding box)
  vgl_point_3d<double> cn = bb.centroid();
  vgl_homg_point_3d<double> stpt(cn.x(), cn.y(), cn.z());

  // 2) determine camera center
  // the viewsphere radius is set to 10x the bounding box diameter
  //double w = bb.width(), h = bb.height(), d = bb.depth();
  double r = radius;//vcl_sqrt(w*w + h*h + d*d);
  //r *=10;
  double deg_to_rad = vnl_math::pi/180.0;
  double el = elevation*deg_to_rad, az = azimuth*deg_to_rad;
  double cx = r*vcl_sin(el)*vcl_cos(az);
  double cy = r*vcl_sin(el)*vcl_sin(az);
  double cz = r*vcl_cos(el);
  vgl_point_3d<double> cent(cx+cn.x(), cy+cn.y(), cz);

  // 3) start with a unit focal length and position the camera
  vpgl_calibration_matrix<double> K(1.0, vgl_point_2d<double>(ni/2, nj/2));
  vgl_rotation_3d<double> R;
  vpgl_perspective_camera<double>* cam =
    new vpgl_perspective_camera<double>(K, cent, R);

  //stare at the center of the scene
  vgl_vector_3d<double> up(0.0, 1.0, 0.0);
  if (vcl_fabs(el)<1.0e-3)
    cam->look_at(stpt, up);
  else
    cam->look_at(stpt);

  //4) Adjust the focal length so that the box projects into the image
  // project the bounding box
  vgl_box_2d<double> image_bb = vpgl_project::project_bounding_box(*cam, bb);
  // get 2-d box diameter and image diameter
  double bw = image_bb.width(), bh = image_bb.height();
  double bd = vcl_sqrt(bw*bw + bh*bh);
  double id = vcl_sqrt(dni*dni + dnj*dnj);
  //find the adjusted focal length
  double f = id/bd;
  K.set_focal_length(f);
  cam->set_calibration(K);

  //vcl_cout<<"Camera  :" <<*cam<<vcl_endl;
  return cam; 
}


bool boxm2_util::copy_file(vcl_string file, vcl_string dest)
{
  vcl_string line;
  vcl_ifstream myfile (file.c_str());
  vcl_ofstream outfile(dest.c_str());
  if (myfile.is_open() && outfile.is_open())
  {
    while ( myfile.good() )
    {
      getline (myfile,line);
      outfile << line << '\n'; 
    }
    myfile.close();
    outfile.close();
  }
  else {
    vcl_cout<<"Couldn't open " << file << " or " << dest << vcl_endl;
    return false;
  }
  return true; 
}

