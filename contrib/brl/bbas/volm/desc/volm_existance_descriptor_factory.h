// This is brl/bbas/volm/desc/volm_existance_descriptor_factory.h
#ifndef volm_existance_descriptor_factory_h_
#define volm_existance_descriptor_factory_h_
//:
// \file
// \brief A factory class of volumetric descriptor which constructs the histogram
//  by the existance of the objects

// \author Yi Dong
// \date May 28, 2013
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include "volm_descriptor_factory.h"

class volm_existance_descriptor_factory : public volm_descriptor_factory
{
public:
  //: Default constructor
  volm_existance_descriptor_factory() {}

  //: destructor
  virtual ~volm_existance_descriptor_factory() {}

  //: update the bin valube by count, non zero count means existance, previous count value will be erased
  virtual void set_count(unsigned const& bin, unsigned char const& count);

  //: set the bin to count value given object distance, height, orientation and land type (assign nothing if invalid)
  void set_count(double const& dist, double const& height,
                 depth_map_region::orientation const& orient, unsigned const& land,
                 unsigned char const& count);

  //: set the bin to count value given object dist_id, height_id, orient_id and land_id
  void set_count(unsigned const& dist_idx, unsigned const& height_idx,
                 unsigned const& orient_idx, unsigned const& land_idx,
                 unsigned char const& count);

  //: set the bin to count value given an object
  void set_count(volm_object const& ob, unsigned char const& count)
  {  this->set_count(ob.dist_, ob.height_, ob.orient_, ob.land_, count); }

};


#endif // volm_existance_descriptor_factory_h_