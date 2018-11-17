#ifndef sdet_contour_h_
#define sdet_contour_h_
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
//             O(nlogn) time for quicksort if n=|chains| < 100000,
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
//  Joe Mundy       (2002) Extensive repairs and consolidation after conversion to VXL
//  Peter Vanroose  (2003) removed z coord arg of Translate()
//  Joe Mundy       (2003) [Aug] Eliminate zig-zags in ::FindChains
//-----------------------------------------------------------------------------

#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vbl/vbl_array_2d.h>
#include <vdgl/vdgl_digital_curve_sptr.h>
#include <vtol/vtol_vertex_2d_sptr.h>
#include <vtol/vtol_edge_2d.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <gevd/gevd_bufferxy.h>

class sdet_contour
{
 public:
  sdet_contour(float min_strength, int min_length, // hysteresis
               float min_jump,         // jump in strength at junctions
               float max_gap=2.236068f); // bridge small gaps (sqrt(5))
  ~sdet_contour();

  //: Trace the edgel locations to form a topological network (edges, vertices)
  bool FindNetwork(gevd_bufferxy& edgels, bool junctionp,
                   const int njunction,
                   const int* junctionx, const int* junctiony,
                   std::vector<vtol_edge_2d_sptr>*& edges,
                   std::vector<vtol_vertex_2d_sptr >*& vertices);

  //: Use interpolation of the gradient to localize to sub-pixel accuracy
  void SubPixelAccuracy(std::vector<vtol_edge_2d_sptr>& edges,
                        std::vector<vtol_vertex_2d_sptr >& vertices,
                        const gevd_bufferxy& locationx,
                        const gevd_bufferxy& locationy);

  //: Insert a border at the ROI boundary to support connected components
  void InsertBorder(std::vector<vtol_edge_2d_sptr>& edges,
                    std::vector<vtol_vertex_2d_sptr >& vertices);

  //: apply a smoothing filter to edgel_chain(s)
  static void EqualizeSpacing(std::vector<vtol_edge_2d_sptr>& chains);


  //: computation is carried out in a zero origin ROI - translate back
  static void Translate(std::vector<vtol_edge_2d_sptr>& edges,
                        std::vector<vtol_vertex_2d_sptr >& vertices,
                        float tx=0.5, float ty = 0.5);

  //: clear network storage (edges and vertices)
  static void ClearNetwork(std::vector<vtol_edge_2d_sptr>*& edges,
                           std::vector<vtol_vertex_2d_sptr >*& vertices);

  //: Set edgel gradient and direction values from pixel arrays
  static void SetEdgelData(gevd_bufferxy& grad_mag, gevd_bufferxy& angle,
                           std::vector<vtol_edge_2d_sptr>& edges);


  void BeSilent() {talkative_ = false;}
  void BeTalkative() {talkative_ = true;}
  void SetDebug() {debug_ = true;}
  void ClearDebug() {debug_ = false;}

  static  bool talkative_; // output comentaries or not
  static  bool debug_;

  //: internal routines
 protected:
  //: link detected edgels and junctions into chains
  int FindChains(gevd_bufferxy& edgels,
                 const int njunction,
                 const int* junctionx, const int* junctiony,
                 std::vector<vtol_edge_2d_sptr>& edges);

  //: Establish vertices; improve connectivity by jumping small gaps
  int FindJunctions(gevd_bufferxy& edgels,
                    std::vector<vtol_edge_2d_sptr>& edges,
                    std::vector<vtol_vertex_2d_sptr >& vertices);

  bool move_junction(vtol_vertex_2d_sptr const& junction,
                     int& index, vdgl_digital_curve_sptr const & dc);

  void update_edgel_chain(vtol_edge_2d_sptr const& edge,
                          const int old_x, const int old_y,
                          vtol_vertex_2d_sptr& v);

  bool near_border(vtol_vertex_2d_sptr const&  v);

  //: Detect a nearby vertex by carrying out a spiral search
  bool DetectJunction(vtol_vertex_2d_sptr const& end, int& index,
                      vtol_edge_2d_sptr& weaker,
                      vtol_edge_2d_sptr& stronger,
                      const int maxSpiral,
                      const gevd_bufferxy& edgels);

  //: Break a chain at a junction and form a "T"
  void BreakChain(vtol_vertex_2d_sptr const& junction,
                  int& index,
                  vtol_edge_2d_sptr const& stronger,
                  vtol_edge_2d_sptr& longer,
                  vtol_edge_2d_sptr& shorter);

  //: Break a single edge at junction and form a loop
  void LoopChain(vtol_vertex_2d_sptr const& junction, int& index,
                 vtol_edge_2d_sptr const& chain,
                 vtol_edge_2d_sptr& straight,
                 vtol_edge_2d_sptr& curled);

  //: Break a closed cycle and insert a vertex at junction
    void BreakCycle(vtol_vertex_2d_sptr const& junction,
                    int& index, vtol_edge_2d_sptr const& stronger,
                    vtol_edge_2d_sptr & split);

  //: Detect nearby vertices by searching in a spiral pattern
  vtol_vertex_2d_sptr
    DetectTouch(vtol_vertex_2d_sptr const& end, const int maxSpiral);

  //:Connect an isolated endpoint to a nearby single vertex on a different edge
 void  MergeEndPtTouchingEndPt(vtol_vertex_2d_sptr const& end1,
                               vtol_vertex_2d_sptr const& end2,
                               vtol_edge_2d_sptr& merge,
                               vtol_edge_2d_sptr& longer,
                               vtol_edge_2d_sptr& shorter);

  //:Connect an isolated endpoint to a nearby junction (2 or more edges)
 bool  MergeEndPtTouchingJunction(vtol_vertex_2d_sptr const& endpt,
                                  vtol_vertex_2d_sptr const& junction,
                                  vtol_edge_2d_sptr& old_edge,
                                  vtol_edge_2d_sptr& new_edge);

  //:Connect an isolated endpoint to the other vertex on the same edge
 bool MergeEndPtsOfChain(vtol_vertex_2d_sptr const& endpt,
                         vtol_vertex_2d_sptr const& other,
                         vtol_vertex_2d_sptr& removed_vert);


  //: Lookup table operations for managing mutated vertices and edges

  //: insert an edge into a table indexed by vsol id
  static void LookupTableInsert(std::vector<vtol_edge_2d_sptr>& set,
                                const vtol_edge_2d_sptr& elmt);

  //: replace an edge in a table indexed by vsol id
  static void LookupTableReplace(std::vector<vtol_edge_2d_sptr>& set,
                                 const vtol_edge_2d_sptr& deleted,
                                 const vtol_edge_2d_sptr& inserted);

  //: remove an edge from a table indexed by vsol id
  static void LookupTableRemove(std::vector<vtol_edge_2d_sptr>& set,
                                const vtol_edge_2d_sptr& elmt);

  //: eliminate gaps in the table by removing empty entries
  static void LookupTableCompress(std::vector<vtol_edge_2d_sptr>& set);


  //: insert a vertex into a table indexed by vsol id
  static void LookupTableInsert(std::vector<vtol_vertex_2d_sptr >& set,
                                const vtol_vertex_2d_sptr&  elmt);

  //: replace a vertex in a table indexed by vsol id
  static void LookupTableReplace(std::vector<vtol_vertex_2d_sptr >& set,
                                 const vtol_vertex_2d_sptr&  deleted,
                                 const vtol_vertex_2d_sptr&  inserted);

  //: remove a vertex from a table indexed by vsol id
  static void LookupTableRemove(std::vector<vtol_vertex_2d_sptr >& set,
                                const vtol_vertex_2d_sptr&  elmt);

  //: eliminate gaps in the table by removing empty entries
  static void LookupTableCompress(std::vector<vtol_vertex_2d_sptr >& set);

  //: class members
  float minStrength;  // hysteresis or noise threshold
  int minLength;      // number of pixels in shortest chain
  float minJump;      // change in strength at junction
  float max_gap;      // largest gap to span
  int maxSpiral;      // number of spiral search for max_gap
  vbl_array_2d<vtol_edge_2d_sptr> *edgeMap;
  vbl_array_2d<vtol_vertex_2d_sptr> *vertexMap; // map pixel to junction/chain
  std::vector<vtol_vertex_2d_sptr> test_verts_;
};

#endif // sdet_contour_h_
