// This is prip/vpyr/vpyr_2_tpyramid.h
#ifndef vpyr_2_tpyramid_h_
#define vpyr_2_tpyramid_h_

//:
// \file
// \brief defines a topological pyramid class.
// \author Jocelyn Marchadier
// \date 06/05/2004
//
// \verbatim
//  06 May 2004 Jocelyn Marchadier
// \endverbatim

#include <vcl_iosfwd.h>
#include "vpyr_2_tpyramid_level.h"
#include "vpyr_2_pyramid.h"

//: class for handling arbitrary pyramids.
template < class TLevel=vpyr_2_tpyramid_level<vpyr_2_tpyramid_level_vertex,
                                              vpyr_2_tpyramid_level_edge,
                                              vpyr_2_tpyramid_level_face,
                                              vpyr_2_tpyramid_level_dart> >
class vpyr_2_tpyramid : public vpyr_2_pyramid<TLevel>
{
  //typedef Basevpyr_2_pyramid _Base ;
 public:

  //:
  typedef vpyr_2_pyramid<TLevel> _Base ;

  //:
  typedef vpyr_2_tpyramid<TLevel> self_type;

  //:
  //typedef vmap_2_tmap<typename TLevel::vertex_type::base_type,
  //                    typename TLevel::edge_type::base_type,
  //                    typename TLevel::face_type::base_type,
  //                    typename TLevel::dart_type::base_type > base_map_type ;
  typedef typename _Base::base_map_type base_map_type ;

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
  typedef typename _Base::level_iterator level_iterator ;

  //:
  typedef typename _Base::const_level_iterator const_level_iterator ;

  //: kernel class for contraction of darts on a level.
  typedef typename level_type::contraction_kernel contraction_kernel ;

  //: kernel class for removal of darts on a level.
  typedef typename level_type::removal_kernel removal_kernel ;


  //:
  vpyr_2_tpyramid();

  //vpyr_2_tpyramid(const self_type &right);

  //:
  ~vpyr_2_tpyramid();

  //self_type & operator=(const self_type &right);

  //: Returns true if all the permutations are valid, false otherwise.
  // May be usefull for testing permutations set by hand...
  virtual bool valid() ;

  //: Deletes everything.
  virtual void clear() ;

  //: Returns "true" if the map empty.
  bool empty() const
  {
    return _level.empty() ;
  }

  //: Initializes the sturcture of the combinatorial map from "stream".
  virtual void read_structure(vcl_istream & stream) ;

  //: Writes the sturcture of the combinatorial map to "stream".
  virtual void write_structure(vcl_ostream & stream) const ;

  //: For each vertex of index v of the base level, res[v] is the index of v's surviving vertex of level "arg_level".
  void down_projection_vertices(vmap_level_index arg_level,vcl_vector<vmap_vertex_index> & res) const ;

  //: For each face of index v of the base level, res[v] is the index of v's surviving face of level "arg_level".
  void down_projection_faces(vmap_level_index arg_level,vcl_vector<vmap_face_index> & res) const ;

 protected:

  //:
  level_type* level_below(vmap_level_type type, const level_type& above)
  {
    return _Base::level_below(type, above) ;
  }
};


#include "vpyr_2_tpyramid.txx"

#endif
