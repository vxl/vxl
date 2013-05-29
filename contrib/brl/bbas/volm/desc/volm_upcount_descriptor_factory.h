// This is brl/bbas/volm/desc/volm_upcount_descriptor_factory.h
#ifndef volm_upcount_descriptor_factory_h_
#define volm_upcount_descriptor_factory_h_
//:
// \file
// \brief A factory class of volumetric descriptor which constructs the histogram
//  by the counting the number of object have speicfic orientation, land classification,
//  height at certain distance away

// \author Yi Dong
// \date May 28, 2013
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include "volm_descriptor_factory.h"

class volm_upcount_descriptor_factory : public volm_descriptor_factory
{
public:
  //: Default constructor
  volm_upcount_descriptor_factory() {}

  //: destructor
  virtual ~volm_upcount_descriptor_factory() {}

  //: update the bin value by adding magnitude onto current bin
  virtual void set_count(unsigned const& bin, unsigned char const& magnitude);

  //: update the bin value given the object properties
  void set_count(double const& dist, double const& height,
                 depth_map_region::orientation const& orient, unsigned const& land,
                 unsigned char const& mag);

  void set_count(unsigned const& dist_idx, unsigned const& height_idx,
                 unsigned const& orient_idx, unsigned const& land_idx,
                 unsigned char const& mag);

  void set_count(volm_object const& ob, unsigned char const& mag)
  { this->set_count(ob.dist_, ob.height_, ob.orient_, ob.land_, mag); }


};

#endif // volm_existance_descriptor_factory_h_