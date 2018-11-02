// This is prip/vpyr/vpyr_2_tpyramid.h
#ifndef vpyr_2_tpyramid_h_
#define vpyr_2_tpyramid_h_
//:
// \file
// \brief defines a topological pyramid class.
// \author Jocelyn Marchadier
// \date 06 May 2004
//
// \verbatim
//  Modifications
//   06 May 2004 Jocelyn Marchadier
// \endverbatim

#include <iostream>
#include <iosfwd>
#include <vcl_compiler.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include "vpyr_2_tpyramid_level.h"
#include "vpyr_2_pyramid.h"

//: class for handling arbitrary pyramids.
template < class TLevel=vpyr_2_tpyramid_level<vpyr_2_tpyramid_level_vertex,
                                              vpyr_2_tpyramid_level_edge,
                                              vpyr_2_tpyramid_level_face,
                                              vpyr_2_tpyramid_level_dart> >
class vpyr_2_tpyramid : public vpyr_2_pyramid<TLevel>
{
 public:

  //:
  typedef vpyr_2_pyramid<TLevel> Base_ ;
#if 0
  typedef Basevpyr_2_pyramid Base_ ;
#endif // 0

  //:
  typedef vpyr_2_tpyramid<TLevel> self_type;

  //:
  typedef typename Base_::base_map_type base_map_type ;
#if 0
  typedef vmap_2_tmap<typename TLevel::vertex_type::base_type,
                      typename TLevel::edge_type::base_type,
                      typename TLevel::face_type::base_type,
                      typename TLevel::dart_type::base_type > base_map_type ;
#endif // 0

  //:
  typedef vpyr_2_tpyramid_level< typename TLevel::vertex_type,
                                 typename TLevel::edge_type,
                                 typename TLevel::face_type,
                                 typename TLevel::dart_type > base_level_type ;

  typedef TLevel level_type ;

  //:
  friend class vpyr_2_tpyramid_level< typename TLevel::vertex_type,
                                      typename TLevel::edge_type,
                                      typename TLevel::face_type,
                                      typename TLevel::dart_type > ;

  //:
  typedef vpyr_2_tpyramid_level_dart level_dart_type ;

  //:
  typedef vpyr_2_tpyramid_base_dart base_dart_type ;

  //:
  typedef typename base_map_type::dart_iterator vmap_2_map_dart_base_iterator ;

  //:
  typedef typename base_map_type::face_type base_face_type;

  //:
  typedef typename base_map_type::vertex_type base_vertex_type ;

  //:
  typedef typename base_map_type::edge_type base_edge_type ;

  //:
  typedef typename Base_::level_iterator level_iterator ;

  //:
  typedef typename Base_::const_level_iterator const_level_iterator ;

  //: kernel class for contraction of darts on a level.
  typedef typename level_type::contraction_kernel contraction_kernel ;

  //: kernel class for removal of darts on a level.
  typedef typename level_type::removal_kernel removal_kernel ;

  //:
  vpyr_2_tpyramid();

#if 0
  vpyr_2_tpyramid(self_type const& p);

  self_type & operator=(self_type const& p);
#endif // 0

  //:
  ~vpyr_2_tpyramid() override;

  //: Returns true if all the permutations are valid, false otherwise.
  // May be useful for testing permutations set by hand...
  bool valid() override ;

  //: Deletes everything.
  void clear() override ;

  //: Returns "true" if the map empty.
  bool empty() const
  {
    return this->level_.empty() ;
  }

  //: Initializes the structure of the combinatorial map from "stream".
  virtual void read_structure(std::istream & stream) ;

  //: Writes the structure of the combinatorial map to "stream".
  virtual void write_structure(std::ostream & stream) const ;

  //: For each vertex of index v of the base level, res[v] is the index of v's surviving vertex of level "arg_level".
  void down_projection_vertices(vmap_level_index arg_level,std::vector<vmap_vertex_index> & res) const ;

  //: For each face of index v of the base level, res[v] is the index of v's surviving face of level "arg_level".
  void down_projection_faces(vmap_level_index arg_level,std::vector<vmap_face_index> & res) const ;

 protected:

  //:
  level_type* level_below(vmap_level_type type, const level_type& above)
  {
    return Base_::level_below(type, above) ;
  }
};

#include "vpyr_2_tpyramid.hxx"

#endif
