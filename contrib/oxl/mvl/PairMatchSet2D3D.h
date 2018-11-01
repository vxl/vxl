// This is oxl/mvl/PairMatchSet2D3D.h
#ifndef PairMatchSet2D3D_h_
#define PairMatchSet2D3D_h_
//:
//  \file
// \author
//     Andrew W. Fitzgibbon, Oxford RRG, 19 Sep 96
//-----------------------------------------------------------------------------

#include <mvl/PairMatchSet.h>
#include <mvl/HomgPoint2D.h>

class HomgInterestPointSet;
class HomgMetric;
class HomgPoint2D;
class HomgPoint3D;
class PairMatchSetCorner;

class PairMatchSet2D3D : public PairMatchSet
{
 public:
  // Constructors/Destructors--------------------------------------------------

  PairMatchSet2D3D();
  PairMatchSet2D3D(const HomgInterestPointSet* corners, std::vector<HomgPoint3D>* structure);
  PairMatchSet2D3D(const PairMatchSet2D3D& that);
  PairMatchSet2D3D& operator=(const PairMatchSet2D3D&);
 ~PairMatchSet2D3D() override;

  // Computations--------------------------------------------------------------

  // Data Access---------------------------------------------------------------

  // Data Control--------------------------------------------------------------
  bool is_set() const { return (corners_ != nullptr) && (structure_ != nullptr); }
  void set(const HomgInterestPointSet* corners, std::vector<HomgPoint3D>* structure);
  void set(int corners_size, std::vector<HomgPoint3D>* structure);

  void set_from(const PairMatchSet2D3D& otherframe_to_3d, const PairMatchSetCorner& otherframe_to_this);

  const HomgPoint2D& get_point_2d(int i1) const;
  const HomgPoint3D& get_point_3d(int i2) const;

//: Return the set of corners within which the i1 indices point
  const HomgInterestPointSet* get_corners() const;

//: Return the conditioner for the corners.
  HomgMetric get_conditioner() const;

//: Return the projective structure within which the i2 indices point
  std::vector<HomgPoint3D>* get_structure() const { return structure_; }

 private:
  const HomgInterestPointSet* corners_;
  std::vector<HomgPoint3D>* structure_;
};

#endif // PairMatchSet2D3D_h_
