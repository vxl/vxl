// This is prip/vdtop/vdtop_8_neighborhood_mask.h
#ifndef vdtop_8_neighborhood_mask_h_
#define vdtop_8_neighborhood_mask_h_
//:
// \file
// \brief gives functionalities for computing certain caracteritics of binary 8-neighborhoods.
// \author Jocelyn Marchadier
// \date 06/05/2004
//
// \verbatim
//  06 May 2004 Jocelyn Marchadier
// \endverbatim

#include <vxl_config.h>
#include "vdtop_freeman_code.h"

//: This class handles sets of points included in an 8 neighborhood.
// A vdtop_8_neighborhood_mask can be viewed as a set of points arround a single point.
//
class vdtop_8_neighborhood_mask
{
 public:
  vdtop_8_neighborhood_mask()
    :_mask(0)
  {}
  vdtop_8_neighborhood_mask(const vdtop_8_neighborhood_mask & arg)
    :_mask(arg._mask)
  {}
  vdtop_8_neighborhood_mask(vxl_byte arg_code)
    :_mask(arg_code)
  {}

  vdtop_8_neighborhood_mask& operator=(const vdtop_8_neighborhood_mask & arg)
  {
    _mask=arg._mask ;
    return *this ;
  }

  bool operator==(vdtop_8_neighborhood_mask arg) const
  {
    return _mask==arg._mask ;
  }

  //: Gives the number of 8-components 8 connected to the center.
  int t8p() const
  {
    return _nb8cc[_mask] ;
  }

  //: Gives the number of 4-components 4 connected to the center.
  int t4p() const
  {
    return _nb4cc[_mask] ;
  }

  //: Gives the number of 4-components of the complementary set 4-connected to the center.
  int t4mm() const
  {
    vxl_byte im=~_mask ;
    return _nb4cc[im] ;
  }

  //: Gives the number of 8-components of the complementary set 8-connected to the center.
  int t8mm() const
  {
    vxl_byte im=~_mask ;
    return _nb8cc[im] ;
  }

  //: Returns true if the center if 4-simple.
  bool is_4_simple() const
  {
    return _4simple[_mask] ;
  }

  //: Returns true if the center if 8-simple.
  bool is_8_simple() const
  {
    return _8simple[_mask] ;
  }

  //: Returns true if the center if 8-simple or is isolated.
  bool is_8_simple_or_isolated() const
  {
    return _8simple_or_isolated[_mask] ;
  }

  //: Returns true if the center if 4-simple or is isolated.
  bool is_4_simple_or_isolated() const
  {
    return _4simple_or_isolated[_mask] ;
  }

  //: Returns true if the set includes the point in direction "arg" from the center.
  bool includes(vdtop_freeman_code arg) const
  {
    return arg.mask() & _mask ;
  }

  //: Returns the set complementary to this.
  vdtop_8_neighborhood_mask operator~() const
  {
    return vdtop_8_neighborhood_mask(~_mask) ;
  }

  //: Returns true if the set is empty.
  bool empty() const
  {
    return _mask==0 ;
  }

  //: Returns true if the set is equal to the complete neighborhood.
  bool complete() const
  {
    return _mask==255 ;
  }

  //: Returns a set composed of a single 8-connected component of this. i varies between 0 and t8p()-1.
  vdtop_8_neighborhood_mask connected_8_component(int i)
  {
    return vdtop_8_neighborhood_mask(_8cc[_mask][i]) ;
  }

  //: Returns a set composed of a single 4-connected component of this. i varies between 0 and t4p()-1.
  vdtop_8_neighborhood_mask connected_4_component(int i)
  {
    return vdtop_8_neighborhood_mask(_4cc[_mask][i]) ;
  }

  //: Returns the number of points in this.
  int nb_8_neighbors() const
  {
    return _nb8neighbors[_mask] ;
  }

  //: return the number of points of this 4-adjacent to the center.
  int nb_4_neighbors() const
  {
    return _nb4neighbors[_mask] ;
  }

  //: Returns the direction of the ith neighbor. i varies form 0 to nb_8_neighbors()-1.
  vdtop_freeman_code direction_8_neighbor(int i) const
  {
    return vdtop_freeman_code(_8dir[_mask][i]) ;
  }

  //: Returns the direction of the ith neighbor. i varies form 0 to nb_4_neighbors()-1.
  vdtop_freeman_code direction_4_neighbor(int i) const
  {
    return vdtop_freeman_code(_4dir[_mask][i]) ;
  }

  //: Adds to this the point in direct arg from the center.
  void add_direction(vdtop_freeman_code arg)
  {
    _mask|=arg.mask() ;
  }

  //: Removes from this the point in direct arg from the center.
  void remove_direction(vdtop_freeman_code arg)
  {
    _mask&=~arg.mask() ;
  }

  //: performs the union of this and arg.
  vdtop_8_neighborhood_mask & operator|=(vdtop_8_neighborhood_mask arg)
  {
    _mask|=arg._mask ;
    return *this ;
  }

  //: performs the intersection of this and arg.
  vdtop_8_neighborhood_mask & operator&=(vdtop_8_neighborhood_mask arg)
  {
    _mask&=arg._mask ;
    return *this ;
  }

  //: returns a binary mask.
  vxl_byte mask() const
  {
    return _mask ;
  }

 private:
  vxl_byte _mask ;

  //precomputed tables
  static const vxl_byte _nb8cc[256] ;
  static const vxl_byte _nb4cc[256] ;
  static const vxl_byte _8cc[256][4] ;
  static const vxl_byte _4cc[256][4] ;
  static const vxl_byte _8dir[256][8] ;
  static const vxl_byte _4dir[256][4] ;
  static const bool _8simple[256] ;
  static const bool _4simple[256] ;
  static const bool _8simple_or_isolated[256] ;
  static const bool _4simple_or_isolated[256] ;
  static const vxl_byte _nb4neighbors[256] ;
  static const vxl_byte _nb8neighbors[256] ;
};

#endif
