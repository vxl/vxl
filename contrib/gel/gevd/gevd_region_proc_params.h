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
//-*- c++ -*-------------------------------------------------------------------
#ifndef _region_proc_params_h_
#define _region_proc_params_h_
//
// .NAME region_proc_params - parameter mixin
// .LIBRARY CAD_Detection
// .HEADER CAD Package
// .INCLUDE CAD_Detection/region_proc_params.h
// .FILE region_proc_params.h
// .FILE region_proc_params.C
//
// .SECTION Description
//
// The parameter mixin for region_proc
//
// .SECTION Author:
//             Joseph L. Mundy - Apr. 11, 2001
//             GE Corporate Research and Development
//
// .SECTION Modifications : <none>
//                         
//-----------------------------------------------------------------------------
#include <gevd/gevd_param_mixin.h>
#include <gevd/gevd_detector_params.h>

class gevd_region_proc_params : public gevd_param_mixin
{
public :
  gevd_region_proc_params(float expand_scale = 2,
                     float burt_adelson_factor = .4,
                     bool debug = false,
                     bool verbose = false,
                     const gevd_detector_params& dp = gevd_detector_params());

 gevd_region_proc_params(const gevd_region_proc_params& old_params);
  
 bool SanityCheck();
friend 
  vcl_ostream& operator << (vcl_ostream& os, const gevd_region_proc_params& rpp);
protected:
  void InitParams(float expand_scale,
                  float burt_adelson_factor,
                  bool debug,
                  bool verbose,
                  const gevd_detector_params& dp
                  );
public:
  //
  // Parameter blocks and parameters
  //
  float expand_scale_;         //the scale factor for image expansion
  float burt_adelson_factor_;//the "sigma" of the interpolating kernel
  bool debug_;     //Carry out debug processing
  bool verbose_;   //Print detailed output
  gevd_detector_params dp_;      //parameters associated with step and fold detection
};
#endif



