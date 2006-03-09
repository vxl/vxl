// This is gel/mrc/vpgl/algo/vpgl_project.h
#ifndef _vpgl_project_h_
#define _vpgl_project_h_
//:
// \file
// \brief Methods for projecting geometric structures onto the image
// \author J. L. Mundy
// \date Feb 23, 2005
//
#include <vcl_iostream.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_box_3d.h>
#include <vpgl/vpgl_perspective_camera.h>

class vpgl_project
{

 public:
    ~vpgl_project();
    static vgl_box_2d<double> project_bounding_box(vpgl_perspective_camera<double>  cam,
                                                   vgl_box_3d<double> const & box);

 private: 
    //: constructor private - static methods only
    vpgl_project();
};

#endif
