#ifndef gevd_contour_h_
#define gevd_contour_h_
//:
// \file
// \brief tracing connected contours and junctions
//
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
// \author
//  John Canny      (1986) SM Thesis            \and
//  Chris Connolly  (1987) Use Fu-Tsao thinning \and
//  Van-Duc Nguyen  (1989) Eliminate short & weak contours \and
//  Arron Heller    (1992) Translate from CLOS to C++ \and
//  Van-Duc Nguyen  (1995) Trace/search breadth-first instead of thinning \and
//  Joe Mundy       (1997) Added continuous edgel orientation output \and
//  Van-Duc Nguyen  (1998) Merge from end points of dangling chains only \and
//  Joe Mundy       (1999) Modified ::InsertBorder to use ROI bounds
//  Peter Vanroose  (2003) removed z coord arg of Translate()
//-----------------------------------------------------------------------------

#include <vcl_vector.h>
#include <vbl/vbl_array_2d.h>
#include <vtol/vtol_vertex_2d_sptr.h>
#include <vtol/vtol_edge_2d_sptr.h>
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
                        const float tx=0.5, const float ty = 0.5);
  static void ClearNetwork(vcl_vector<vtol_edge_2d_sptr>*& edges, // remove network of edges
                           vcl_vector<vtol_vertex_2d_sptr >*& vertices); // and vertices
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
  int minLength;      // number of pixels in shortest chain
  float minJump;      // change in strength at junction
  int maxSpiral;      // number of spiral search for max_gap
  vbl_array_2d<vtol_edge_2d_sptr> *edgeMap;
  vbl_array_2d<vtol_vertex_2d_sptr> *vertexMap; // map pixel to junction/chain

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

#endif // gevd_contour_h_
