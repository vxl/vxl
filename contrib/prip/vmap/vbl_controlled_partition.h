// This is contrib/prip/vmap/vbl_controlled_partition.h
#ifndef vbl_controlled_partition_h_
#define vbl_controlled_partition_h_
//:
// \file
// \brief provides the class vbl_controlled_partition.
// \author Jocelyn Marchadier
// \date 06/05/2004
//
// \verbatim
//  Modifications
//   06 May 2004 Jocelyn Marchadier
// \endverbatim


#include "vcl_vector.h"
//: enables to handle partitions with actually choosing the representative for each set.
// vbl_controlled_partition provides a way to choose while merging the representative for each set. It can be used 
// when the representative plays a special role.
class vbl_controlled_partition
{
    struct node
    {
        int father ;
    } ;
    vcl_vector<node> _elements ;
    int _nb_sets ;

    void link(int i, int j)
    {
        _elements[j].father=_elements[i].father ;
        _nb_sets-- ;
    }

public:
    vbl_controlled_partition() ;
    ~vbl_controlled_partition() ;


    //: Initializes the struture with "size" new sets composed
    //   of a single element.
    void initialise(int size) ;

		vbl_controlled_partition & operator=(const vbl_controlled_partition & arg) ;
		
    //: The representative of "root" is the new representative of the set to which "j" is the representative.
    void union_of(int root, int j)
    {
        link(representative(root), representative(j)) ;
    }

    //: Finds the representative of "i".
    int representative(int i) const ;

    //: Sets "i" as the representative its set.
    void set_as_representative(int i) ;

    //: Number of sets.
    int nb_sets() const
    {
        return _nb_sets ;
    }

   //: Number of elements.
    int nb_elements() const
    {
        return _elements.size() ;
    }

    //: Deletes everything
    void clear()
    {
        _elements.clear() ;
    }
};

#endif
