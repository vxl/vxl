// This is prip/vdtop/vdtop_8_neighborhood_mask.h
#ifndef vdtop_8_neighborhood_mask_h_
#define vdtop_8_neighborhood_mask_h_
//:
// \file
// \brief gives functionalities for computing certain characteritics of binary 8-neighborhoods.
// \author Jocelyn Marchadier
// \date 06/05/2004
//
// \verbatim
//  06 May 2004 Jocelyn Marchadier
// \endverbatim

#include <vxl_config.h>
#include "vdtop_freeman_code.h"

//: This class handles sets of points included in an 8-neighborhood.
// A vdtop_8_neighborhood_mask can be viewed as a set of points around a single point.
//
class vdtop_8_neighborhood_mask
{
 public:
  vdtop_8_neighborhood_mask() :mask_(0) {}
  vdtop_8_neighborhood_mask(vdtop_8_neighborhood_mask const& arg) :mask_(arg.mask_) {}
  vdtop_8_neighborhood_mask(vxl_byte arg_code) :mask_(arg_code) {}

  vdtop_8_neighborhood_mask& operator=(const vdtop_8_neighborhood_mask & arg)
  {
    mask_=arg.mask_ ;
    return *this ;
  }

  bool operator==(vdtop_8_neighborhood_mask arg) const
  {
    return mask_==arg.mask_ ;
  }

  //: Gives the number of 8-components 8 connected to the center.
  int t8p() const
  {
    return nb8cc_[mask_] ;
  }

  //: Gives the number of 4-components 4 connected to the center.
  int t4p() const
  {
    return nb4cc_[mask_] ;
  }

  //: Gives the number of 4-components of the complementary set 4-connected to the center.
  int t4mm() const
  {
    vxl_byte im=~mask_ ;
    return nb4cc_[im] ;
  }

  //: Gives the number of 8-components of the complementary set 8-connected to the center.
  int t8mm() const
  {
    vxl_byte im=~mask_ ;
    return nb8cc_[im] ;
  }

  //: Returns true if the center if 4-simple.
  bool is_4_simple() const
  {
    return m4simple_[mask_] ;
  }

  //: Returns true if the center if 8-simple.
  bool is_8_simple() const
  {
    return m8simple_[mask_] ;
  }

  //: Returns true if the center if 8-simple or is isolated.
  bool is_8_simple_or_isolated() const
  {
    return m8simple_or_isolated_[mask_] ;
  }

  //: Returns true if the center if 4-simple or is isolated.
  bool is_4_simple_or_isolated() const
  {
    return m4simple_or_isolated_[mask_] ;
  }

  //: Returns true if the set includes the point in direction "arg" from the center.
  bool includes(vdtop_freeman_code arg) const
  {
    return (arg.mask() & mask_) != 0;
  }

  //: Returns the set complementary to this.
  vdtop_8_neighborhood_mask operator~() const
  {
    return vdtop_8_neighborhood_mask(~mask_) ;
  }

  //: Returns true if the set is empty.
  bool empty() const
  {
    return mask_==0;
  }

  //: Returns true if the set is equal to the complete neighborhood.
  bool complete() const
  {
    return mask_==255 ;
  }

  //: Returns a set composed of a single 8-connected component of this. i varies between 0 and t8p()-1.
  vdtop_8_neighborhood_mask connected_8_component(int i)
  {
    return vdtop_8_neighborhood_mask(m8cc_[mask_][i]) ;
  }

  //: Returns a set composed of a single 4-connected component of this. i varies between 0 and t4p()-1.
  vdtop_8_neighborhood_mask connected_4_component(int i)
  {
    return vdtop_8_neighborhood_mask(m4cc_[mask_][i]) ;
  }

  //: Returns the number of points in this.
  int nb_8_neighbors() const
  {
    return nb8neighbors_[mask_] ;
  }

  //: return the number of points of this 4-adjacent to the center.
  int nb_4_neighbors() const
  {
    return nb4neighbors_[mask_] ;
  }

  //: Returns the direction of the ith neighbor. i varies form 0 to nb_8_neighbors()-1.
  vdtop_freeman_code direction_8_neighbor(int i) const
  {
    return vdtop_freeman_code(m8dir_[mask_][i]) ;
  }

  //: Returns the direction of the ith neighbor. i varies form 0 to nb_4_neighbors()-1.
  vdtop_freeman_code direction_4_neighbor(int i) const
  {
    return vdtop_freeman_code(m4dir_[mask_][i]) ;
  }

  //: Adds to this the point in direct arg from the center.
  void add_direction(vdtop_freeman_code arg)
  {
    mask_|=arg.mask() ;
  }

  //: Removes from this the point in direct arg from the center.
  void remove_direction(vdtop_freeman_code arg)
  {
    mask_&=~arg.mask() ;
  }

  //: performs the union of this and arg.
  vdtop_8_neighborhood_mask & operator|=(vdtop_8_neighborhood_mask arg)
  {
    mask_|=arg.mask_ ;
    return *this ;
  }

  //: performs the intersection of this and arg.
  vdtop_8_neighborhood_mask & operator&=(vdtop_8_neighborhood_mask arg)
  {
    mask_&=arg.mask_ ;
    return *this ;
  }

  //: returns a binary mask.
  vxl_byte mask() const
  {
    return mask_ ;
  }

 private:
  vxl_byte mask_ ;

  //precomputed tables
  static const vxl_byte nb8cc_[256] ;
  static const vxl_byte nb4cc_[256] ;
  static const vxl_byte m8cc_[256][4] ;
  static const vxl_byte m4cc_[256][4] ;
  static const vxl_byte m8dir_[256][8] ;
  static const vxl_byte m4dir_[256][4] ;
  static const bool m8simple_[256] ;
  static const bool m4simple_[256] ;
  static const bool m8simple_or_isolated_[256] ;
  static const bool m4simple_or_isolated_[256] ;
  static const vxl_byte nb4neighbors_[256] ;
  static const vxl_byte nb8neighbors_[256] ;
};

#endif
