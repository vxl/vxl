#ifndef sdet_region_edge_h_
#define sdet_region_edge_h_
//:
// \file
// \brief An edge with extract information to support constructing regions from an edgel segmentation.
//
//  This class supports tracing of region boundaries in conjunction with
//  the class EdgelIntensity. The sdet_region_edge maintains a list of region
//  labels which the edge is adjacent to. Ideally, the edge is adjacent to
//  only two regions, left and right, as shown below.
// \verbatim
//                    left_region_
//           V1------------------------V2
//                    right_region_
// \endverbatim
//  In practice, this restriction is too limiting.  It is possible for
//  an edge to be adjacent to more than two regions. For example:
// \verbatim
//    c c c
//   o----\ c
//  E  b b  \ c
//   o------+ \ c
//    c c c | a \ c
//        c +----+ c
//          c c c c
// \endverbatim
//   Note that edge E is adjacent to both regions a and b even though
//   there is no intervening vertex at the transition between a and b.
//   In order to preserve a separation between regions b and c, it is
//   necessary to allow more than two labels (a,b) for a given edge.
//
// \author J. L. Mundy - January 24, 1999
// \verbatim
//  Modifications
//   Feb 10, 1999  - JLM Extended the concept of a sdet_region_edge
//                       to represent a vertex, where the edge is NULL.
//                       This extension permits the propagation of region labels
//                       across junctions.
//   March 7, 1999 - JLM Allowed more than two regions per edge.
//   Sept 10, 2004 - PVr Added copy ctor with explicit vbl_ref_count init
// \endverbatim
//-----------------------------------------------------------------------------

#include <vtol/vtol_edge_2d.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <vbl/vbl_ref_count.h>
#include <vcl_stlfwd.h> // for vcl_vector
#include <sdet/sdet_region_edge_sptr.h>

class sdet_region_edge : public vbl_ref_count
{
 public:
  //Constructors/Destructors
  sdet_region_edge(vtol_edge_2d_sptr e);
  sdet_region_edge(sdet_region_edge const& e)
    : vbl_ref_count(), edge_(e.edge_), labels_(e.labels_) {}
  ~sdet_region_edge();
  //Accessors

  void Prop(sdet_region_edge_sptr const& re,
            unsigned int label, unsigned int max_label);

  vtol_edge_2d_sptr get_edge() const {return edge_;}

  bool is_vertex() const;
  unsigned int NumLabels(unsigned int max_label) const;
  unsigned int GetLabel(unsigned int i, unsigned int max_label) const
  { return i<NumLabels(max_label) ? labels_[i] : 0; }

  //Utitities (especially for testing)
 protected:
  //Utilities
  bool SetNewLabel(unsigned int label);
  //members
  vtol_edge_2d_sptr edge_;
  vcl_vector<unsigned int> labels_;
};

#endif // sdet_region_edge_h_
