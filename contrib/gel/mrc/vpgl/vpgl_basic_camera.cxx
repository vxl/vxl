// The code is taken and adapted from BasicCamera.c of TargetJr for VxL photgrammetry library.

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

#include <vpgl/vpgl_basic_camera.h>


vpgl_basic_camera::vpgl_basic_camera():
  init_pt(3)
{
  for (int i=0; i<3; i++) init_pt[i]=0.0;
}

vpgl_basic_camera::~vpgl_basic_camera() 
{
}




//---------------------------------------------------------------------
//  Various flavours of world_to_image
//  Routines that call with double arguments are converted to float
//  so as to pick up existing virtual routines that use float.
//

void vpgl_basic_camera::world_to_image(const vnl_vector<double>& xyz, double &u, double &v)
{
  world_to_image (xyz[0], xyz[1], xyz[2], u, v);
}

void vpgl_basic_camera::world_to_image(const double&, const double& , const double& ,
			    double& , double& )
{
  vcl_cerr <<" Can't compute " << "world_to_image";
}


//------------------------------------------------------------------------

void vpgl_basic_camera::image_to_world(vnl_vector<double>& ,vnl_vector<double>& , double , double )
{
  vcl_cerr << " Can't compute ImageToWorld" ;
}






void vpgl_basic_camera::set_init_pt(const vnl_vector<double>& pt)
{
  init_pt = pt;
}

void vpgl_basic_camera::get_init_pt(vnl_vector<double>& pt)
{
	for (int i=0; i<3; i++) pt[i]= init_pt[i];
}
