// This is contrib/prip/vpyr/vpyr_2_pyramid_level.h
#ifndef vpyr_2_pyramid_level_h_
#define vpyr_2_pyramid_level_h_

//:
// \file
// \brief provides the class for handling/defining levels of a combinatorial pyramid.
// \author Jocelyn Marchadier
// \date 06/05/2004
// \verbatim
//  06 May 2004 Jocelyn Marchadier
// \endverbatim

#include "vmap/vmap_kernel.h"
#include "vpyr_2_pyramid_level_elts.h"

template <class _level_type>
class vpyr_2_pyramid ;

//: The class of a level of a combinatorial pyramid.
template <class D>
class vpyr_2_pyramid_level : public vmap_ptr_sequence< typename D::base_type >
{
public:
  
  //: 
  typedef vpyr_2_pyramid_level<D> self_type;
	//: 
  static vmap_2_map_tag tag ;  
	//: 
  typedef vpyr_2_pyramid<self_type> pyramid_type ;
  //: 
  typedef vmap_2_map< typename D::base_type > base_map_type ;  
	//: 
  typedef typename base_map_type::dart_type base_dart_type;
  
protected:

	//:
	typedef vmap_ptr_sequence< typename D::base_type > dart_sequence ;
	//: 
  typedef typename dart_sequence::iterator dart_sequence_iterator;
  //: 
  typedef typename dart_sequence::const_iterator const_dart_sequence_iterator;
  //: 
  typedef typename dart_sequence::pointer dart_pointer ;

public:
  // -- Dart tyes --

  //: dart_type class.
  typedef D dart_type ;
  //: reference on a dart.
  typedef dart_type dart_reference ;
  //: const reference on a dart.
  typedef const dart_type const_dart_reference ;
  //: An iterator iterates on a sequence of darts.
  typedef vpyr_2_pyramid_level_dart_base_iterator<dart_type,dart_type&,dart_type*, dart_sequence_iterator> dart_iterator ;
  //: 
  typedef vpyr_2_pyramid_level_dart_base_iterator<dart_type,const dart_type&,const dart_type*, const_dart_sequence_iterator> const_dart_iterator ;

  //: Kernel class for contraction.
  typedef vmap_kernel<self_type> contraction_kernel ;

  //: Kernel class for removal.
  typedef vmap_kernel<self_type> removal_kernel ;

  friend class vmap_kernel<self_type> ;
	
	//: 
  static dart_type& cast(vpyr_2_pyramid_level_dart & a)
  {
    return (dart_type&) (a) ;
  }
  
  //: 
  static const dart_type& cast(const vpyr_2_pyramid_level_dart & a)
  {
    return (const dart_type&) (a) ;
  }


  //: 
  vpyr_2_pyramid_level(vmap_level_index arg_level, pyramid_type & pyramid) ;

  //: 
  vpyr_2_pyramid_level(const self_type & arg)
      : dart_sequence(arg),
      _level_index(arg._level_index),
      _pyramid(arg._pyramid)

  {}

  //: 
  ~vpyr_2_pyramid_level()
  {}

  //: Returns the number of darts.
  int nb_darts () const
  {
    return dart_sequence::size() ;
  }

  //: Returns "true" if the map is empty.
  bool empty() const
  {
    return dart_sequence::empty() ;
  }

  //: Returns the dart of index "arg".
  const_dart_reference dart(vmap_dart_index arg) const
  {
    return dart_type(get_dart_pointer(arg),index()) ;
  }

  //: Returns the dart of index "arg".
  dart_reference dart(vmap_dart_index arg)
  {
    return dart_type(get_dart_pointer(arg),index()) ;
  }

  //: Returns the index of the dart "arg".
  vmap_dart_index index (const dart_type & arg) const
  {
    return arg.sequence_index();//-(*_begin_dart)->index() ;
  }

  //: Returns the index of the level.
  vmap_level_index index () const
  {
    return _level_index ;
  }


  //: Returns an iterator on the first dart.
  const_dart_iterator begin_dart() const
  {
    return const_dart_iterator(begin_dart_sequence(),index()) ;
  }
  
  //: Returns an iterator on the first dart.
  dart_iterator begin_dart()
  {
    return dart_iterator(begin_dart_sequence(),index()) ;
  }

  //: Returns an iterator after the last dart.
  const_dart_iterator end_dart() const
  {
    return const_dart_iterator(end_dart_sequence(),index()) ;
  }
  
  //: Returns an iterator after the last dart.
  dart_iterator end_dart()
  {
    return dart_iterator(end_dart_sequence(),index()) ;
  }

  //: Returns sigma("arg"), the next dart turning clockwise around the beginning vertex of the dart.
  vmap_dart_index sigma (vmap_dart_index arg) const
  {
    return get_dart_pointer(arg)->sigma(index())->sequence_index() ;
  }

  //: Returns alpha("arg"), the opposite dart of the same edge.
  vmap_dart_index alpha (vmap_dart_index arg) const
  {
    return get_dart_pointer(arg)->alpha(index())->sequence_index()  ;
  }

  //: Returns phi("arg"), the next dart turning counter-clockwise around the left face of the dart.
  vmap_dart_index phi(vmap_dart_index arg) const
  {
    return get_dart_pointer(arg)->phi(index())->sequence_index() ;
  }

  //virtual bool valid()const ;

  //: 
  virtual void contraction(const contraction_kernel &arg_kernel) ;
  
  //: 
  virtual void removal(const removal_kernel &arg_kernel) ;

  //: 
  void set_level_index(vmap_level_index arg)
  {
    _level_index=arg ;
  }

  //: 
  void set_nb_darts(int arg)
  {
    dart_sequence::resize(arg,false) ;
  }

protected:

  //: 
  pyramid_type & pyramid()
  {
    return (pyramid_type &)*_pyramid ;
  }

protected:

  //: 
  dart_sequence_iterator begin_dart_sequence()
  {
    return dart_sequence::begin() ;
  }

  //: 
  dart_sequence_iterator end_dart_sequence()
  {
    return dart_sequence::end() ;
  }

  //: 
  const_dart_sequence_iterator begin_dart_sequence() const
  {
    return dart_sequence::begin() ;
  }

  //: 
  const_dart_sequence_iterator end_dart_sequence() const
  {
    return dart_sequence::end() ;
  }

  //: 
  dart_pointer & get_dart_pointer(int arg)
  {
    return dart_sequence::get_pointer(arg) ;
  }

  //: 
  const dart_pointer & get_dart_pointer(int arg) const
  {
    return dart_sequence::get_pointer(arg) ;
  }

protected:
  
  //: the index of the level.
  vmap_level_index _level_index ;
  
  //: The pyramid for which it is a level.
  pyramid_type * _pyramid ;
};

#include "vpyr_2_pyramid_level.txx"

#endif
