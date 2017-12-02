// This is prip/vmap/vbl_controlled_partition.h
#ifndef vbl_controlled_partition_h_
#define vbl_controlled_partition_h_
//:
// \file
// \brief provides the class vbl_controlled_partition.
// \author Jocelyn Marchadier
// \date 06 May 2004
//
// \verbatim
//  Modifications
//   06 May 2004 Jocelyn Marchadier
// \endverbatim

#include <iostream>
#include <vector>
#include <vcl_compiler.h>

//: enables to handle partitions with actually choosing the representative for each set.
// vbl_controlled_partition provides a way to choose while merging the representative for each set. It can be used
// when the representative plays a special role.
class vbl_controlled_partition
{
  struct node
  {
    int father ;
  };
  std::vector<node> elements_ ;
  int nb_sets_ ;

  void link(int i, int j)
  {
    elements_[j].father=elements_[i].father ;
    nb_sets_-- ;
  }

 public:
  vbl_controlled_partition() ;
  ~vbl_controlled_partition() ;

  //: Initializes the structure with "size" new sets composed of a single element.
  void initialise(int size) ;

  vbl_controlled_partition & operator=(const vbl_controlled_partition & arg) ;

  //: The representative of "root" is the new representative of the set to which "j" is the representative.
  void union_of(int root, int j)
  {
    link(representative(root), representative(j)) ;
  }

  //: Finds the representative of "i".
  int representative(int i) const ;

  //: Sets "i" as the representative of its set.
  void set_as_representative(int i) ;

  //: Number of sets.
  int nb_sets() const
  {
    return nb_sets_ ;
  }

 //: Number of elements.
  int nb_elements() const
  {
    return elements_.size() ;
  }

  //: Deletes everything
  void clear()
  {
    elements_.clear() ;
  }
};

#endif
