// This is prip/vpyr/vpyr_top_tmap.h
#ifndef vpyr_top_tmap_h_
#define vpyr_top_tmap_h_
//:
// \file
// \brief defines a class building pyramids.
// \author Jocelyn Marchadier
// \date 06/05/2004
//
// \verbatim
//  06 May 2004 Jocelyn Marchadier
// \endverbatim

#include <vmap/vmap_2_tmap.h>

//: class duplicates the structure of the top level of a pyramid with associated types.
// It helps for efficiently constructing a pyramid by successive modifications of its top level.
template <class TPyramid, class V=vmap_2_tmap_vertex, class E=vmap_2_tmap_edge, class F=vmap_2_tmap_face, class D=vmap_2_tmap_dart>
class vpyr_top_tmap : public vmap_2_tmap<V,E,F,D>
{
 public:

  //:
  typedef vmap_2_tmap<V,E,F,D> Base_ ;

  typedef typename Base_::contraction_kernel contraction_kernel ;
  typedef typename Base_::removal_kernel removal_kernel ;

  //:
  typedef TPyramid pyramid_type ;

  //:
  vpyr_top_tmap(TPyramid & arg)
    :pyramid_(arg)
  {
    synchronise() ;
  }

  //:
  virtual ~vpyr_top_tmap();

  //:
  //  Must be inlined here to circumvent a bug in MSVC.
  template <class M >
  void set_structure(M const& arg)
  {
    pyramid_.set_base_structure(arg) ;
    Base_::set_structure(arg) ;
  }

  //:
  virtual void contraction(const contraction_kernel & arg) ;

  //:
  virtual void removal(const removal_kernel & arg) ;

  const pyramid_type & pyramid() const
  {
    return pyramid_ ;
  }

  //: Synchronises the top level of its pyramid and its structure
  void synchronise()
  {
    if (!pyramid_.empty()) Base_::set_structure(pyramid_.top_level()) ;
  }

  void set_pyramid_base_structure()
  {
    pyramid_.set_base_structure(*this) ;
  }

 private :

  //:
  pyramid_type & pyramid_ ;
};

#include "vpyr_top_tmap.txx"

#endif
