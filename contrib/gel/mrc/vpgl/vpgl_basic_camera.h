
// The code is taken and adapted from BasicCamera.h of TargetJr for VxL photgrammetry library.


// <begin copyright notice>
// ---------------------------------------------------------------------------
//
//                   Copyright (c) 1997 TargetJr Consortium
//               GE Corporate Research and Development (GE CRD)
//                             1 Research Circle
//                            Niskayuna, NY 12309
//                            All Rights Reserved
//              Reproduction rights limited as described below.
//
//      Permission to use, copy, modify, distribute, and sell this software
//      and its documentation for any purpose is hereby granted without fee,
//      provided that (i) the above copyright notice and this permission
//      notice appear in all copies of the software and related documentation,
//      (ii) the name TargetJr Consortium (represented by GE CRD), may not be
//      used in any advertising or publicity relating to the software without
//      the specific, prior written permission of GE CRD, and (iii) any
//      modifications are clearly marked and summarized in a change history
//      log.
//
//      THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
//      EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
//      WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
//      IN NO EVENT SHALL THE TARGETJR CONSORTIUM BE LIABLE FOR ANY SPECIAL,
//      INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND OR ANY
//      DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
//      WHETHER OR NOT ADVISED OF THE POSSIBILITY OF SUCH DAMAGES, OR ON
//      ANY THEORY OF LIABILITY ARISING OUT OF OR IN CONNECTION WITH THE
//      USE OR PERFORMANCE OF THIS SOFTWARE.
//
// ---------------------------------------------------------------------------
// <end copyright notice>
#ifndef VPGL_BASIC_CAMERA_H
#define VPGL_BASIC_CAMERA_H
//-----------------------------------------------------------------------------
//
// .NAME        vpgl_basic_camera - Base class for cameras
// .LIBRARY     vpgl
// .HEADER      Photogrammetry Package
// .INCLUDE     vpgl/vpgl_basic_camera.h
// .FILE        vpgl_basic_camera.h
// .FILE        vpgl_basic_camera.C
//
// .SECTION Description
//
//
// .SECTION Author
//     Patti Vrobel
//
//-----------------------------------------------------------------------------

#include <vcl/vcl_iostream.h>
#include <vcl/vcl_vector.h>
#include <vbl/vbl_ref_count.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_math.h>
#include <vpgl/vpgl_basic_camera_ref.h>

class vpgl_graphics_camera;
class vpgl_basic_camera;
class vpgl_matrix_camera;


class vpgl_basic_camera : public vbl_ref_count
{
public:

 // Constructors
  vpgl_basic_camera();
 // Destructors
  virtual ~vpgl_basic_camera();

  // virtual interface for camera classes

  //---------------------------------------
  // Different world-to-image methods
  virtual void world_to_image(const  vnl_vector<double>&, double&, double&);

  virtual void world_to_image(
        const double&, const double&, const double&, double&, double&);

  //-----------------------------------------
  virtual void image_to_world(vnl_vector<double>& ray3d_origin,
                              vnl_vector<double>& world, double u, double v);

  //: A function to set the initilization point
  virtual void set_init_pt(const vnl_vector<double>& pt);
  //: A function to get the initilization point
  virtual void get_init_pt(vnl_vector<double>& pt);

  virtual void get_matrix(vnl_matrix<double>&) const;

#if 0
  inline friend ostream &operator<<(ostream &os, const vpgl_basic_camera& obj)
    {obj.Print(os); return os;}
  inline friend ostream &operator<<(ostream &os, const vpgl_basic_camera* obj)
    {if(obj) obj->Print(os); else os << "NULL Camera"; return os;}
#endif

protected:

  //: Holds a 3D point used for Levenberg Marquardt initialization in ImageToSurface
  vnl_vector<double> init_pt;
};

//: fill passed in matrix with camera matrix.
// this only is implemented when MatrixProjection returns true.
inline void vpgl_basic_camera::get_matrix(vnl_matrix<double>&) const
{
  vcl_cerr<<"Nothing returned";
}

#endif   // VPGL_BASIC_CAMERA_H
