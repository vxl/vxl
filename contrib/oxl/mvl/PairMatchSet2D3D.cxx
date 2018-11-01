// This is oxl/mvl/PairMatchSet2D3D.cxx

#include <iostream>
#include "PairMatchSet2D3D.h"
#include <mvl/HomgInterestPointSet.h>
#include <mvl/PairMatchSetCorner.h>
#include <mvl/ProjStructure.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

// Default ctor
PairMatchSet2D3D::PairMatchSet2D3D()
{
  corners_ = nullptr;
  structure_ = nullptr;
}

// Copy ctor
PairMatchSet2D3D::PairMatchSet2D3D(const PairMatchSet2D3D& that)
  : PairMatchSet(that),
    corners_(new HomgInterestPointSet(*that.corners_)),
    structure_(new std::vector<HomgPoint3D>(*that.structure_))
{
}

PairMatchSet2D3D::PairMatchSet2D3D(const HomgInterestPointSet* corners, std::vector<HomgPoint3D>* structure)
{
  set(corners, structure);
}

// Assignment
PairMatchSet2D3D& PairMatchSet2D3D::operator=(const PairMatchSet2D3D& )
{
  std::cerr << "PairMatchSet2D3D::operator= not implemented\n";
  return *this;
}

// Destructor
PairMatchSet2D3D::~PairMatchSet2D3D() = default;

void PairMatchSet2D3D::set(const HomgInterestPointSet* corners, std::vector<HomgPoint3D>* structure)
{
  corners_ = corners;
  structure_ = structure;
  set_size(corners_->size());
}

void PairMatchSet2D3D::set(int num_corners, std::vector<HomgPoint3D>* structure)
{
  corners_ = nullptr;
  structure_ = structure;
  set_size(num_corners);
}

void PairMatchSet2D3D::set_from(const PairMatchSet2D3D& otherframe_to_3d, const PairMatchSetCorner& otherframe_to_this)
{
  corners_ = otherframe_to_this.get_corners2();
  structure_ = otherframe_to_3d.get_structure();
  set_size(otherframe_to_this.size());

  clear();
  for (PairMatchSetCorner::iterator match = otherframe_to_this; match; match.next()) {
    int corner1 = match.get_i1();
    int corner2 = match.get_i2();
    int structure1 = otherframe_to_3d.get_match_12(corner1);
    add_match(corner2, structure1);
  }
}

HomgMetric PairMatchSet2D3D::get_conditioner() const
{
  if (!corners_) {
    std::cerr << "PairMatchSet2D3D::get_conditioner() WARNING corners_ not set!\n";
    return nullptr;
  }
  return corners_->get_conditioner();
}

const HomgPoint2D& PairMatchSet2D3D::get_point_2d(int i1) const
{
  if (!corners_) {
    static HomgPoint2D dummy;
    std::cerr << "PairMatchSet2D3D::get_point_2d() WARNING corners_ not set!\n";
    return dummy;
  }
  return corners_->get_homg(i1);
}

const HomgPoint3D& PairMatchSet2D3D::get_point_3d(int i2) const
{
  return structure_->operator[](i2);
}

const HomgInterestPointSet* PairMatchSet2D3D::get_corners() const
{
  if (!corners_) {
    std::cerr << "PairMatchSet2D3D::get_point_2d() WARNING corners_ not set!\n";
    return nullptr;
  }
  return corners_;
}
