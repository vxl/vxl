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
#ifndef _gevd_contour_h_
#define _gevd_contour_h_
//
// .NAME gevd_contour - tracing connected contours and junctions
// .LIBRARY Detection
// .HEADER Segmentation package
// .INCLUDE Detection/gevd_contour.h
// .FILE gevd_contour.h
// .FILE gevd_contour.C
//
// .SECTION Description
// Operator to implement the tracing of connected contours and junctions.
// All contours are assumed chains or cycles with zero width,
// and so can be traced using 4/8-connected pixels.
// Junctions will be detected from end points touching multiple
// other end points, or touching a stronger contour with a significant
// jump in filter response.
// Extensively use the image grid to insure planarity of the network
// of edges and vertices, when projected onto the image plane.
// The recipe is:
//
//    1. Trace 4/8-connected pixels to enumerate disjoint chains
//       and cycles. Prune contours that are either short or have
//       no pixel stronger than a high hysteresis threshold.
//
//    2. Find junctions from end points touching internal
//       pixels of some stronger chain/cycle. Break a stronger
//       contour at a junction, only if there is a detectable
//       jump in filter response.
//       Next, merge end points touching other end points or junctions.
//       Finally, create dummy end point for isolated cycles.
//
//    3. Insert subpixel accuracy into edges/vertices.
//       Because of truncation errors, the mapping from edgel
//       locations to integral grid locations may no longer be
//       preserved, after this step.
//
//    4. Optionally reduce noisy zig-zags along the contours, and
//       evenly space the contour points. The zig-zags are never
//       more than 0.5 pixel, and happen when sub-pixel locations
//       are noisy and so out-of-sync with the 4/8-connected tracing.
//
//    5. Optionally insert virtual border at the image boundary,
//       to form closed region beyond the image boundary.
//
//    6. Insert depth/z values into edges/vertices, through
//       interpolation of the range image, for example.
//       For an intensity image, set this depth to a constant
//       value, since the 3D edges/vertices all lie in the
//       image plane.
//
// Input: connected edge elements, with response strength,
//        and subpixel location, describing isolated contours
//        disjoint only at junction pixels.
//        min_strength and min_length are used to prune weak or
//        short edges. min_jump is used to prune weak junctions.
//
// Output: planar network of linked edges and vertices.
//
// Complexity: O(|edgels|) time and space.
//             O(nlogn) time for quicksort if n=|chains| < 1000,
//             to make sure that junctions are found from
//             longer/stronger chains first.
//
// .EXAMPLE ../Examples/contour.C
// .SECTION Authors
//  John Canny      (1986) SM Thesis
//  Chris Connolly  (1987) Use Fu-Tsao thinning
//  Van-Duc Nguyen  (1989) Eliminate short & weak contours
//  Arron Heller    (1992) Translate from CLOS to C++
//  Van-Duc Nguyen  (1995) Trace/search breadth-first instead of thinning
//  Joe Mundy       (1997) Added continous edgel orientation output
//  Van-Duc Nguyen  (1998) Merge from end points of dangling chains only
//  Joe Mundy       (1999) Modified ::InsertBorder to use ROI bounds
//-----------------------------------------------------------------------------

#include <vcl_vector.h>
#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_edge_2d.h>
#include <gevd/gevd_bufferxy.h>

class gevd_contour
{
public:
  gevd_contour(float min_strength, int min_length, // hysteresis
               float min_jump,         // jump in strength at junctions
               float max_gap=2.236068f); // bridge small gaps (sqrt(5))
  ~gevd_contour();

  bool FindNetwork(gevd_bufferxy& edgels, // link pixels into network
                   const int njunction, // junctions detected previously
                   const int* junctionx, const int* junctiony,
                   vcl_vector<vtol_edge_2d_sptr>*& edges,
                   vcl_vector<vtol_vertex_2d_sptr >*& vertices);
  void SubPixelAccuracy(vcl_vector<vtol_edge_2d_sptr>& edges, // insert subpixel
                        vcl_vector<vtol_vertex_2d_sptr >& vertices, // accuracy
                        const gevd_bufferxy& locationx, // along normal to
                        const gevd_bufferxy& locationy); // contour only
  void InsertBorder(vcl_vector<vtol_edge_2d_sptr>& edges, // border location = 3
                    vcl_vector<vtol_vertex_2d_sptr >& vertices); // virtual chain/junction

  static void EqualizeSpacing(vcl_vector<vtol_edge_2d_sptr>& chains); // uniform spacing
  static void Translate(vcl_vector<vtol_edge_2d_sptr>& edges, // translate loc to center
                        vcl_vector<vtol_vertex_2d_sptr >& vertices, // instead of upper-left
                        const float tx=0.5, const float ty = 0.5,
                        const float tz = 0);
  static void ClearNetwork(vcl_vector<vtol_edge_2d_sptr>*& edges, // remove network of edges
                           vcl_vector<vtol_vertex_2d_sptr >*& vertices); // and vertices
  static gevd_bufferxy* CreateEdgeMap(vcl_vector<vtol_edge_2d_sptr>&,
                                 const int sizex, const int sizey);
  int CheckInvariants(vcl_vector<vtol_edge_2d_sptr>& edges, // return number of errors
                      vcl_vector<vtol_vertex_2d_sptr >& vertices);

  static void MaskEdgels(const gevd_bufferxy& mask, // byte mask image
                         gevd_bufferxy& edgels, // edge elements AND with mask
                         int& njunction, // vertices AND with mask
                         int* junctionx, int* junctiony);
  static void SetEdgelData(gevd_bufferxy& grad_mag, gevd_bufferxy& angle,
                           vcl_vector<vtol_edge_2d_sptr>& edges);

#if 0 // commented out
  static int ClosedRegions(vcl_vector<vtol_edge_2d*>& edges, // remove dangling/bridge
                           vcl_vector<vtol_vertex_2d*>& vertices); //  edges/vertices
  static void SetRayOrigin(const float x, const float y);
  static int ClockWiseOrder(vtol_edge_2d* const& dc1, vtol_edge_2d* const& dc2);
#endif

  static int LengthCmp(vtol_edge_2d_sptr const& dc1, vtol_edge_2d_sptr const& dc2); // pixel length
  static vcl_vector<vtol_edge_2d_sptr>* CreateLookupTable(vcl_vector<vtol_edge_2d_sptr>&);
  static void LookupTableInsert(vcl_vector<vtol_edge_2d_sptr>& set, vtol_edge_2d_sptr elmt);
  static void LookupTableReplace(vcl_vector<vtol_edge_2d_sptr>& set,
                                 vtol_edge_2d_sptr deleted, vtol_edge_2d_sptr inserted);
  static void LookupTableRemove(vcl_vector<vtol_edge_2d_sptr>& set, vtol_edge_2d_sptr elmt);
  static void LookupTableCompress(vcl_vector<vtol_edge_2d_sptr>& set);

  static vcl_vector<vtol_vertex_2d_sptr >* CreateLookupTable(vcl_vector<vtol_vertex_2d_sptr >&);
  static void LookupTableInsert(vcl_vector<vtol_vertex_2d_sptr >& set, vtol_vertex_2d_sptr  elmt);
  static void LookupTableReplace(vcl_vector<vtol_vertex_2d_sptr >& set,
                                 vtol_vertex_2d_sptr  deleted, vtol_vertex_2d_sptr  inserted);
  static void LookupTableRemove(vcl_vector<vtol_vertex_2d_sptr >& set, vtol_vertex_2d_sptr  elmt);
  static void LookupTableCompress(vcl_vector<vtol_vertex_2d_sptr >& set);

  static void BeSilent() {talkative = false;}
  static void BeTalkative() {talkative = true;}
protected:
  float minStrength;  // hysteresis or noise threshold
  float minLength;    // number of pixels in shortest chain
  float minJump;      // change in strength at junction
  int maxSpiral;      // number of spiral search for max_gap
  gevd_bufferxy *edgeMap, *vertexMap; // map pixel to junction/chain

protected:
  int FindChains(gevd_bufferxy& edgels, // link pixels into chains
                 const int njunction, // junctions detected
                 const int* junctionx, const int* junctiony,
                 vcl_vector<vtol_edge_2d_sptr>& edges);
  int FindJunctions(gevd_bufferxy& edgels, // merge end/end and end/contour
                    vcl_vector<vtol_edge_2d_sptr>& edges, // replace these global lists
                    vcl_vector<vtol_vertex_2d_sptr >& vertices);

  static bool talkative; // output comentaries or not
};


// // Get reference to pixel as a edge/vertex pointer, at indexes (x, y).
// Avoid intermediate cast to void*.

inline vtol_edge_2d *&
edgePtr(gevd_bufferxy& edgeMap, int x, int y)
{
  return (*((vtol_edge_2d **) edgeMap.GetElementAddr(x,y)));
}

inline vtol_edge_2d *
edgePtr(const gevd_bufferxy& edgeMap, int x, int y)
{
  return (*((vtol_edge_2d *const *) edgeMap.GetElementAddr(x,y)));
}

inline vtol_vertex_2d *&
vertexPtr(gevd_bufferxy& vertexMap, int x, int y)
{
  return (*((vtol_vertex_2d **)vertexMap.GetElementAddr(x,y)));
}

inline vtol_vertex_2d *
vertexPtr(const gevd_bufferxy& vertexMap, int x, int y)
{
  return (* ((vtol_vertex_2d *const *)vertexMap.GetElementAddr(x,y)));
}

#endif
