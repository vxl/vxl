// This is prip/vpyr/vpyr_2_pyramid_base.h
#ifndef vpyr_2_pyramid_base_h_
#define vpyr_2_pyramid_base_h_
//:
// \file
// \brief contains the elements of the base of a vmap_2_pyramid.
// \author Jocelyn Marchadier
// \date 06/05/2004
//
// \verbatim
//  Modifications
//   6 May 2004 Jocelyn Marchadier - initial version
// \endverbatim

#include <vmap/vmap_2_map.h>

class vpyr_2_pyramid_base_dart ;

template <typename DPtr>
inline DPtr vpyr_2_pyramid_base_alpha(DPtr arg, vmap_level_index l)
{
  return (DPtr) (*arg).vpyr_2_pyramid_base_dart::alpha(l) ;
}

template <typename DPtr>
inline DPtr vpyr_2_pyramid_base_sigma(DPtr arg, vmap_level_index l)
{
  return (DPtr) (*arg).vpyr_2_pyramid_base_dart::sigma(l) ;
}

template <typename DPtr>
inline DPtr vpyr_2_pyramid_base_phi(DPtr arg, vmap_level_index l)
{
  return (DPtr) (*arg).vpyr_2_pyramid_base_dart::phi(l) ;
}

template <typename DPtr>
inline DPtr vpyr_2_pyramid_base_ialpha(DPtr arg, vmap_level_index l)
{
  return (DPtr) arg->vpyr_2_pyramid_base_dart::ialpha(l) ;
}

template <typename DPtr>
inline DPtr vpyr_2_pyramid_base_isigma(DPtr arg, vmap_level_index l)
{
  return (DPtr) (*arg).vpyr_2_pyramid_base_dart::isigma(l) ;
}

template <typename DPtr>
inline DPtr vpyr_2_pyramid_base_iphi(DPtr arg, vmap_level_index l)
{
  return (DPtr) arg->vpyr_2_pyramid_base_dart::iphi(l) ;
}

template <typename DPtr>
DPtr vpyr_2_pyramid_base_ancestor(DPtr arg, vmap_level_index l)
{
  return (DPtr)(arg->ancestor(l)) ;
}

//: Data associated to each element at each level.
template <class D>
struct vmap_simple_data
{
  typedef D value_type ;
  value_type d;
  value_type & data(vmap_level_index arg_level)
  {
    return d ;
  }

  const value_type & data(vmap_level_index arg_level) const
  {
    return d ;
  }
  void set_level(vmap_level_index l) {}
};

//: Data associated to each element duplicated at level.
template <class D>
struct vmap_replicated_data
{
  typedef D value_type ;
  std::vector<value_type> d;
  vmap_replicated_data()
  {
    d.push_back(this->ReturnType()) ;
  }
  value_type & data(vmap_level_index arg_level)
  {
    return d[arg_level] ;
  }

  const value_type & data(vmap_level_index arg_level) const
  {
    return d[arg_level] ;
  }
  void set_level(vmap_level_index l)
  {
    while (d.size()<l) d.push_back(d.last()) ;
  }
};

//: the dart class of a 2-pyramid base level.
class vpyr_2_pyramid_base_dart : public vmap_2_map_dart
{
 public:
#if 0
  vpyr_2_pyramid_base_dart() ;
#endif // 0

  //: The last level.
  vmap_level_index last_level() const
  {
    return last_level_/2;
  }

  //: Returns true if the dart has been contracted or removed at level "arg".
  bool modified_at_level(vmap_level_index arg) const
  {
    return last_level_ == (arg*2);
  }

  //: Returns true if the dart has been contracted or removed at its last level.
  bool modified_at_last_level() const
  {
    return !(last_level_ & 1);
  }

  //: Sets the last level at which the dart survive.
  void set_last_level(vmap_level_index arg) ;

  //: Returns true if the dart is a pendant dart.
  bool is_pendant(vmap_level_index level) const
  {
    return this == sigma(level) ;
  }

  //: Returns true if the dart is a redundant dart.
  bool is_redundant(vmap_level_index level) const
  {
    return this == sigma(level)->sigma(level) ;
  }

  //: Returns true if the dart is a self direct loop.
  bool is_self_direct_loop(vmap_level_index level) const
  {
    return this==phi(level) ;
  }

  //: Returns true if the dart is a double dart.
  bool is_double(vmap_level_index level) const
  {
    return this == phi(level)->phi(level) ;
  }

  //: Returns the corresponding surviving dart at level "level".
  vpyr_2_pyramid_base_dart* ancestor(vmap_level_index level) ;

  //: Returns the corresponding surviving dart at level "level".
  const vpyr_2_pyramid_base_dart* ancestor(vmap_level_index level) const ;

  //: The following functions should not be used by the user
  const vpyr_2_pyramid_base_dart* alpha(vmap_level_index level) const
  {
    return vmap_2_map_alpha(this)->ancestor(level) ;
  }

  const vpyr_2_pyramid_base_dart* sigma(vmap_level_index level) const
  {
    return vmap_2_map_sigma(this)->ancestor(level) ;
  }

  const vpyr_2_pyramid_base_dart* phi(vmap_level_index level) const
  {
    return vmap_2_map_phi(this)->ancestor(level) ;
  }

  const vpyr_2_pyramid_base_dart* isigma(vmap_level_index level) const
  {
    return vmap_2_map_isigma(this)->ancestor(level) ;
  }

  const vpyr_2_pyramid_base_dart* iphi(vmap_level_index level)  const
  {
    return vmap_2_map_iphi(this)->ancestor(level) ;
  }

  vpyr_2_pyramid_base_dart* alpha(vmap_level_index level)
  {
    return vmap_2_map_alpha(this)->ancestor(level) ;
  }

  vpyr_2_pyramid_base_dart* sigma(vmap_level_index level)
  {
    return vmap_2_map_sigma(this)->ancestor(level) ;
  }

  vpyr_2_pyramid_base_dart* phi(vmap_level_index level)
  {
    return vmap_2_map_phi(this)->ancestor(level) ;
  }

  vpyr_2_pyramid_base_dart* isigma(vmap_level_index level)
  {
    return vmap_2_map_isigma(this)->ancestor(level) ;
  }

  vpyr_2_pyramid_base_dart* iphi(vmap_level_index level)
  {
    return vmap_2_map_iphi(this)->ancestor(level) ;
  }
 protected :
  vmap_level_index last_level_ ;
#if 0
  //: Return the direct ancestor of this dart.
  vpyr_2_pyramid_base_dart* directAncestor() ;

 private :
  vpyr_2_pyramid_base_dart* toplevel_typeAncestor_ ;
  vpyr_2_pyramid_base_dart* directAncestor_ ;
#endif // 0
};

//: The public vmap_2_tmap_dart class.
template <class D>
class vmap_2_pd_dart : public vpyr_2_pyramid_base_dart
{
 public:

  typename D::value_type & data(vmap_level_index arg_level)
  {
    return d_.data(arg_level) ;
  }

  const typename D::value_type & data(vmap_level_index arg_level) const
  {
    return d_.data(arg_level) ;
  }

 protected :
  D d_ ;
};

#endif
