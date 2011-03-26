#include "boxm2_util.h"
//:
// \file

//vgl includes
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_box_2d.h>
#include <vpgl/algo/vpgl_project.h>
#include <vil/vil_convert.h>

//vnl includes
#include <vnl/vnl_vector.h>
#include <vnl/vnl_random.h>
#include <vnl/algo/vnl_svd.h>

//vil includes
#include <vil/vil_image_view.h>

//vcl io stuff
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_cstdio.h>
#include <vcl_algorithm.h>

//vul file includes
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>

void boxm2_util::random_permutation(int* buffer, int size)
{
  vnl_random random(9667566);

  //shuffle the buffer
  for (int i=0; i<size; i++)
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

//: returns a list of cameras from specified directory
vcl_vector<vpgl_perspective_camera<double>* > boxm2_util::cameras_from_directory(vcl_string dir)
{
  vcl_vector<vpgl_perspective_camera<double>* > toReturn;
  if (!vul_file::is_directory(dir.c_str()) ) {
    vcl_cout<<"Cam dir is not a directory"<<vcl_endl;
    return toReturn;
  }

  //get all of the cam and image files, sort them
  vcl_string camglob=dir+"/*.txt";
  vul_file_iterator file_it(camglob.c_str());
  vcl_vector<vcl_string> cam_files;
  while (file_it) {
    vcl_string camName(file_it());
    cam_files.push_back(camName);
    ++file_it;
  }
  vcl_sort(cam_files.begin(), cam_files.end());

  //take sorted lists and load from file
  vcl_vector<vcl_string>::iterator iter;
  for (iter = cam_files.begin(); iter != cam_files.end(); ++iter)
  {
    //load camera from file
    vcl_ifstream ifs(iter->c_str());
    vpgl_perspective_camera<double>* pcam =new vpgl_perspective_camera<double>;
    if (!ifs.is_open()) {
      vcl_cerr << "Failed to open file " << *iter << '\n';
      return toReturn;
    }
    else  {
      ifs >> *pcam;
    }

    toReturn.push_back(pcam);
  }
  return toReturn;
}

//: returns a list of image strings from directory
vcl_vector<vcl_string> boxm2_util::images_from_directory(vcl_string dir)
{
  vcl_vector<vcl_string> img_files;
  if (!vul_file::is_directory(dir.c_str())) {
    vcl_cout<<"img dir is not a directory"<<vcl_endl;
    return img_files;
  }
  vcl_string imgglob=dir+"/*.png";
  vul_file_iterator img_file_it(imgglob.c_str());
  while (img_file_it) {
    vcl_string imgName(img_file_it());
    img_files.push_back(imgName);
    ++img_file_it;
  }
  vcl_sort(img_files.begin(), img_files.end());
  return img_files;
}

//: returns a list of image strings from directory
vcl_vector<vcl_string> boxm2_util::camfiles_from_directory(vcl_string dir)
{
  vcl_vector<vcl_string> cam_files;
  if (!vul_file::is_directory(dir.c_str())) {
    vcl_cout<<"cam dir is not a directory"<<vcl_endl;
    return cam_files;
  }
  vcl_string camglob=dir+"/*.txt";
  vul_file_iterator file_it(camglob.c_str());
  while (file_it) {
    vcl_string camName(file_it());
    cam_files.push_back(camName);
    ++file_it;
  }
  vcl_sort(cam_files.begin(), cam_files.end());
  return cam_files;
}

//Constructs a camera given elevation, azimuth (degrees), radius, and bounding box.
vpgl_perspective_camera<double>*
boxm2_util::construct_camera( double elevation,
                              double azimuth,
                              double radius,
                              unsigned ni,
                              unsigned nj,
                              vgl_box_3d<double> bb,
                              bool fit_bb)
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
  double r = radius; // = vcl_sqrt(w*w + h*h + d*d); // where w=bb.width() etc.
  //r *= 10;
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
  double id = vcl_sqrt(dni*dni + dnj*dnj);
  double f;
  if (fit_bb) {
    vgl_box_2d<double> image_bb = vpgl_project::project_bounding_box(*cam, bb);
    // get 2-d box diameter and image diameter
    double bw = image_bb.width(), bh = image_bb.height();
    double bd = vcl_sqrt(bw*bw + bh*bh);
    //find the adjusted focal length
    f = id/bd;
  }
  else {
    f = id;
  }
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

bool boxm2_util::generate_html(int height, int width, int nrows, int ncols, vcl_string dest)
{
  char html[4*1024];
  vcl_sprintf(html,
              "<!DOCTYPE html>\n \
<html lang='en'>\n \
<head>\n \
  <meta charset='utf-8' content='text/html' http-equiv='Content-type' />\n \
  <title>Volume Visualizer</title>\n \
  <!-- Reel/Jquery Script Includes -->\n \
  <script src='js/jquery.min.js' type='text/javascript'></script>\n \
  <script src='js/jquery.reel-min.js' type='text/javascript'></script>\n \
  <script src='js/jquery.disabletextselect-min.js' type='text/javascript'></script>\n \
  <script src='js/jquery.mousewheel-min.js' type='text/javascript'></script>\n \
  <script src='js/js.js' type='text/javascript'></script>\n \
  <script src='js/miniZoomPan.js' type='text/javascript'></script>\n \
  <style  src='css/miniZoomPan.css' type='text/css' />\n \
  <meta name='viewport' content='width = %d' />\n \
  <style>\n \
    html, body{ margin: 0; background: #000 url(iphone.instructions.gif) no-repeat 0 %dpx; }\n \
  </style>\n \
  <!-- ZOOM Script -->\n \
  <script type= 'text/javascript'>/*<![CDATA[*/\n \
      $(function() {\n \
        $('#zoom01').miniZoomPan({\n \
            sW: %d,\n \
            sH: %d,\n \
            lW: %d,\n \
            lH: %d\n \
          })\n \
      });\n \
      /*]]>*/\n \
  </script>\n \
</head>\n \
<body>\n \
  <div id='zoom01'>\n \
    <img id='image' src='img/scene_0_0.jpg' width='%dpx' height=%dpx' />\n \
  </div>\n \
  <script type='text/javascript'>\n \
    $(document).ready(function(){\n \
      $('#image').reel({\n \
        frame: 1,\n \
        footage: %d,\n \
        frames: %d,\n \
        rows: %d,\n \
        row: 1,\n \
        path: 'img/',\n \
        image: 'img/scene_0_0.jpg',\n \
        images: scene_frames(),\n \
        horizontal: true,\n \
      });\n \
    });\n \
  </script>\n \
</body>\n \
</html>",
              width,
              width,
              width,
              height,
              2*width,
              2*height,
              width,
              height,
              ncols,
              ncols,
              nrows);

  //write to destination file
  vcl_ofstream outfile(dest.c_str());
  if (outfile.is_open())
  {
    outfile << html;
    outfile.close();
  }
  else {
    vcl_cout<<"Couldn't open " << dest << vcl_endl;
    return false;
  }
  return true;
}


bool boxm2_util::generate_jsfunc(vbl_array_2d<vcl_string> img_files, vcl_string dest)
{
  vcl_string js = "function scene_frames(frames){\n var stack = [ ";

  //go through the array in img_files
  for (unsigned int row=0; row<img_files.rows(); ++row) {
    for (unsigned int col=0; col<img_files.cols(); ++col) {
      if (row != 0 || col != 0)
        js += ", "; //don't put a comma at the beginning...
      js += "'" + img_files(row, col) + "'";
    }
  }
  js += "]\nreturn stack\n}";

  //write to destination file
  vcl_ofstream outfile(dest.c_str());
  if (outfile.is_open())
  {
    outfile << js;
    outfile.close();
    return true;
  }
  else {
    vcl_cout<<"Couldn't open " << dest << vcl_endl;
    return false;
  }
}


// private helper method prepares an input image to be processed by update
vil_image_view_base_sptr boxm2_util::prepare_input_image(vil_image_view_base_sptr loaded_image)
{
  //load from file

  //then it's an RGB image (assumes byte image...)
  if (loaded_image->nplanes() == 3 || loaded_image->nplanes() == 4)
  {
    vcl_cout<<"preparing rgb image"<<vcl_endl;
    //load image from file and format it into RGBA
    vil_image_view_base_sptr n_planes = vil_convert_to_n_planes(4, loaded_image);
    vil_image_view_base_sptr comp_image = vil_convert_to_component_order(n_planes);
    vil_image_view<vil_rgba<vxl_byte> >* rgba_view = new vil_image_view<vil_rgba<vxl_byte> >(comp_image);

    //make sure all alpha values are set to 255 (1)
    vil_image_view<vil_rgba<vxl_byte> >::iterator iter;
    for (iter = rgba_view->begin(); iter != rgba_view->end(); ++iter) {
      (*iter) = vil_rgba<vxl_byte>(iter->R(), iter->G(), iter->B(), 255);
    }
    vil_image_view_base_sptr toReturn(rgba_view);
    return toReturn;
  }

  //else if loaded planes is just one...
  if (loaded_image->nplanes() == 1)
  {
    vcl_cout<<"Preparing grey scale image"<<vcl_endl;
    vil_image_view<float>* floatimg = new vil_image_view<float>(loaded_image->ni(), loaded_image->nj(), 1);
    if (vil_image_view<vxl_byte> *img_byte = dynamic_cast<vil_image_view<vxl_byte>*>(loaded_image.ptr()))
        vil_convert_stretch_range_limited(*img_byte, *floatimg, vxl_byte(0), vxl_byte(255), 0.0f, 1.0f);
    else if (vil_image_view<unsigned short> *img_byte = dynamic_cast<vil_image_view<unsigned short>*>(loaded_image.ptr()))
        vil_convert_stretch_range_limited(*img_byte, *floatimg,(unsigned short)28000,(unsigned short)33000,  0.0f, 1.0f); // hardcoded to be fixed.
        //vil_convert_stretch_range(*img_byte, *floatimg,  0.0f, 1.0f); // hardcoded to be fixed.
    else {
        vcl_cerr << "Failed to load image "  << '\n';
        return 0;
    }
    vil_image_view_base_sptr toReturn(floatimg);
    return toReturn;
  }

  //otherwise it's messed up, return a null pointer
  vcl_cerr<<"Failed to recognize input image type " << '\n';
  return 0;
}
