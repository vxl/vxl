//--*-c++-*--
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
#ifndef _region_proc_h_
#define _region_proc_h_
//---------------------------------------------------------------------
//
// .NAME region_proc - a processor for extracting expanded resolution regions
// .HEADER CAD package
// .INCLUDE CAD_Detection/region_proc.h
// .FILE CAD_Detection/region_proc.h
// .FILE CAD_Detection/region_proc.C
// 
// .SECTION Description edgel_regions uses a flood fill algorithm and is thus
//                      region labeling is only as localized as one pixel.
//                      In the CAD application, we need sub-pixel details. So
//                      In this algorithm the original image resolution is 
//                      expanded using Gaussian interpolation, typically by
//                      a factor of two. Then the resulting regions are
//                      transformed back to original image coordinates.
// .SECTION Authors
//  J.L. Mundy - April 11, 2001
// .SECTION none
//
//
//-------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_math.h>
#include <vnl/algo/vnl_svd.h>
#include <gevd/gevd_bufferxy.h>
#include <vil/vil_image.h>
#include <gevd/gevd_pixel.h>
#include <gevd/gevd_float_operators.h>
#include <vsol/vsol_curve_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_digital_region.h>
#include <vtol/vtol_topology_object.h>
#include <vtol/vtol_one_chain.h>
#include <vtol/vtol_edge.h>
#include <vtol/vtol_face.h>
#include <vtol/vtol_face_2d.h>
#include <vtol/vtol_face_sptr.h>
#include <vtol/vtol_vertex.h>
#include <vtol/vtol_two_chain.h>
#include <gevd/gevd_detector.h>
#include <vdgl/vdgl_intensity_face.h>
//#include <gevd/gevd_poly_intensity_face.h>
#include <gevd/gevd_clean_edgels.h>
#include <gevd/gevd_edgel_regions.h>
#include <gevd/gevd_region_proc.h>
#include <gevd/gevd_region_proc_params.h>

class region_proc : public gevd_region_proc_params
{
public:
  //Constructors/destructor
  region_proc();

  ~region_proc();
  //Accessors
  void set_image(vil_image& image);
//  void set_roi_proc(lung_roi_proc_ref& roi_proc){_roi_proc = roi_proc;}

  // vector<gevd_poly_intensity_face_sptr>& get_regions(){return _regions;}
  vcl_vector<vdgl_digital_region_sptr>& get_regions(){return _regions;}

  //Utility Methods
  void extract_regions();
  void clear();

  void set_expand_scale(int scale){expand_scale_=scale;}

  //Debug methods
  void set_debug(){debug_ = true;}
  void clear_debug(){debug_ = false;}

protected:
  //protected methods
  gevd_bufferxy* get_image_buffer(vil_image& image);
  gevd_bufferxy* get_float_buffer(gevd_bufferxy* b);
  gevd_bufferxy* put_float_buffer(gevd_bufferxy* fbuf);
  vil_image buffer_to_image(gevd_bufferxy* buf);
  void restore_image_rois();
  //members
  bool _debug;//debug flag
  bool _regions_valid;      //process state flag
  int expand_scale_;
  int burt_adelson_factor_;
  vil_image _image;  //input image
  gevd_bufferxy* _buf;
  //lung_roi_proc_ref _roi_proc; //Lung roi processor
  //vector<gevd_poly_intensity_face_ref> _regions; //resulting intensity faces
  vcl_vector<vdgl_digital_region_sptr> _regions; //resulting digital regions
};
#endif
