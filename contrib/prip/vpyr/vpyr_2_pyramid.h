// This is prip/vpyr/vpyr_2_pyramid.h
#ifndef vpyr_2_pyramid_h_
#define vpyr_2_pyramid_h_
//:
// \file
// \brief provides the definition of a combinatorial pyramid class.
// \author Jocelyn Marchadier
// \date 06/05/2004
//
// \verbatim
//  06 May 2004 Jocelyn Marchadier
// \endverbatim

#include "vpyr_2_pyramid_level.h"
#include <vcl_iosfwd.h>
#include <vcl_map.h>
#include <vmap/vmap_map_iterator_wrapper.h>

//: enables to handle combinatorial pyramids.
template <class TLevel=vpyr_2_pyramid_level<vpyr_2_pyramid_level_dart> >
class vpyr_2_pyramid
{
 public:

  //:
  typedef vpyr_2_pyramid< TLevel > self_type ;

  //: the structure on top of which the pyramid is built.
  typedef typename TLevel::base_map_type base_map_type ;

  //: the type of a dart on the base map.
  typedef typename base_map_type::dart_type base_dart_type ;

  //: the class for handling levels of the pyramid.
  typedef TLevel level_type ;

  friend class vpyr_2_pyramid_level< typename TLevel::dart_type> ;

  //: the type of a dart in one level of the pyramid.
  typedef typename level_type::dart_type level_dart_type ;

 protected:

  //:
  typedef vcl_map<vmap_level_index, level_type> level_array_type ;

 public:

  //: iterator on the level sequence of the pyramid.
  typedef vmap_map_iterator_wrapper<level_type&, level_type*, typename level_array_type::iterator > level_iterator ;

  //: const_iterator on the level sequence of the pyramid.
  typedef vmap_map_iterator_wrapper<const level_type&, const level_type*, typename level_array_type::const_iterator >
          const_level_iterator ;
  //typedef typename level_array_type::iterator level_iterator ;
  //typedef typename level_array_type::const_iterator const_level_iterator ;

  //: kernel for contraction of darts on a level.
  typedef typename level_type::contraction_kernel contraction_kernel ;

  //: kernel class for removal of darts on a level.
  typedef typename level_type::removal_kernel removal_kernel ;


  //
  // Constructors/Destructors
  //
  //////////////////

  vpyr_2_pyramid();
  //vpyr_2_pyramid(const vpyr_2_pyramid &right);

  ~vpyr_2_pyramid();

  //vpyr_2_pyramid & operator=(const vpyr_2_pyramid &right);

  //: Erases everything and sets the base level of the pyramid .
  template <class M>
  void set_base_structure(const M & arg) ;

  //: Returns the base level of the pyramid.
  const base_map_type & base_map() const
  {
    return _base_map ;
  }

  //: Returns the base level of the pyramid.
  const level_type & base_level() const
  {
    return _level.begin()->second ;
  }

  //: Returns the base level of the pyramid.
  level_type & base_level()
  {
    return _level.begin()->second ;
  }

  //: Returns the top level of the pyramid.
  const level_type & top_level() const
  {
    return (--_level.end())->second ;
  }

  //: Returns the top level of the pyramid.
  level_type & top_level()
  {
    return (--_level.end())->second ;
  }

  //: Returns the index of the top level of the pyramid.
  vmap_level_index top_map_index() const
  {
    return (vmap_level_index)-1 ;
  }

  //: Returns the number of levels in the pyramid.
  int nb_levels() const
  {
    return _level.size() ;
  }

  //: Returns an intermediate level of the pyramid.
  level_type & level(vmap_level_index arg)
  {
    typename level_array_type::iterator i=_level.lower_bound(arg) ;
  if (i->first==arg) return i->second ;
  if (i==_level.begin()) return _level.end()->second ;
  return (--i)->second ;
  }

  //: Returns an intermediate level of the pyramid.
  const level_type & level(vmap_level_index arg) const
  {
    typename level_array_type::iterator i=_level.lower_bound(arg) ;
  if (i->first==arg) return i->second ;
  if (i==_level.begin()) return _level.end()->second ;
  return (--i)->second ;
  }

  //: Returns the first iterator on the level sequence.
  level_iterator begin_level()
  {
    return _level.begin() ;
  }

  //: Returns the first iterator on the level sequence.
  const_level_iterator begin_level() const
  {
    return _level.begin() ;
  }

  //: Returns the end iterator on the level sequence.
  level_iterator end_level()
  {
    return _level.end() ;
  }

  //: Returns the end iterator on the level sequence.
  const_level_iterator end_level() const
  {
    return _level.end() ;
  }

  /*//: Initialises nb_levels.
  virtual void initialise_levels(int nb_levels) ;
  */

  //: Returns true if all the permutations are valid, false otherwise.
  // May be usefull for testing permutations set by hand...
  virtual bool valid() ;

  //: Deletes everything.
  virtual void clear() ;

  //: Returns "true" if the map is empty.
  bool empty() const
  {
    return _level.empty() ;
  }


  //: Initializes the sturcture of the combinatorial map from "stream".
  void read_structure(vcl_istream & stream) ;

  //: Writes the sturcture of the combinatorial map to "stream".
  void write_structure(vcl_ostream & stream) const ;

  //: returns the ith dart of the base map.
  base_dart_type & base_dart(int index)
  {
    return _base_map.dart(index) ;
  }

 protected:

  //: Returns the first level below the level "above" being of type "type".
  level_type* level_below(vmap_level_type type, const level_type& above) ;

  //: the base map of the pyramid.
  base_map_type _base_map ;

  //:  all the levels of the pyramid.
  level_array_type _level ;
};

#include "vpyr_2_pyramid.txx"

#endif
