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
#ifndef _carmen_interface_h
#define _carmen_interface_h

//-----------------------------------------------------------------------------
//
// Class : carmen_interface
//
// .SECTION Description: A interface to Carmen
//
// Man page information:
//
// .NAME carmen_interface - test interface to Carmen
//
//
// .LIBRARY CADSimulation
// .HEADER CAD package
// .INCLUDE CADSimulation/carmen_interface.h
// .FILE carmen_interface.C
//
// .SECTION Author:
//             J. Mundy April 3, 2001
//             GE CRD
// .SECTION Modifications <None>
//
//-----------------------------------------------------------------------------
#include <vcl_string.h>

class Carmen;

class carmen_interface
{
  // PUBLIC INTERFACE----------------------------------------------------------
public:

  // Constructors/Initializers/Destructors-------------------------------------
  carmen_interface();
  ~carmen_interface();

  // Data Access---------------------------------------------------------------
  bool load_correspondence_file(const string& file_path);
  void save_camera(const string& file_path);
  void print_results();
  // Utility Methods-------------------------------------------------------
  void set_carmen_camera(int view_no);
  void solve();

   protected:
  // Utilities
  bool add_full_correspondence(int view_no, int point_id,
                              double x, double y, double z, float u, float ves);
  // Data Members--------------------------------------------------------------
  Carmen* _carmen;
};

#endif
