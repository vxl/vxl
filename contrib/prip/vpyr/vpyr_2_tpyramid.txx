// This is contrib/prip/vpyr/vpyr_2_tpyramid.txx
#include "vpyr_2_tpyramid.h"
#include "vmap/vbl_controlled_partition.h"
#include "vcl_iostream.h"


template <class TLevel>
vpyr_2_tpyramid<TLevel>::vpyr_2_tpyramid()
{
}

/*
template <class TLevel>
vpyr_2_tpyramid<TLevel>::vpyr_2_tpyramid(const self_type &right)
{
 	operator=(right) ;
}*/

template <class TLevel>
vpyr_2_tpyramid<TLevel>::~vpyr_2_tpyramid()
{
}

/*
template <class TLevel>
vpyr_2_tpyramid<TLevel> & vpyr_2_tpyramid<TLevel>::operator=(const self_type &right)
{
		if (this!=&rigth)
    {
    	_Base::operator=(right) ;
      vertex_sequence_iterator first_vertex=base_map().begin_vertex_sequence(), last_vertex=base_map().end_vertex_sequence();
      edge_sequence_iterator firstEdge=base_map().begin_edge_sequence(), lastEdge=base_map().end_edge_sequence();
      face_sequence_iterator firstFace=base_map().begin_face_sequence(), lastFace=base_map().end_face_sequence();
      for (int i=0; i<_level.size();++i)
      {
        //first_vertex=&base_map().vertex((*right._level[i].begin_base_vertex())->index()) ;
        last_vertex=base_map().begin_vertex_sequence()+(right.level(i).end_base_vertex()-right.base_map().begin_vertex_sequence()) ;
        level(i).set_vertex_sequence(first_vertex,last_vertex) ;
        //firstEdge=&base_map().edge((*right._level[i].beginbase_edge())->index()) ;
        lastEdge=base_map().begin_edge_sequence()+(right.level(i).endbase_edge()-right.base_map().begin_edge_sequence()) ;
        level(i).set_edge_sequence(firstEdge,lastEdge) ;
        //firstFace=&base_map().face((*right._level[i].beginbase_type())->index()) ;
        lastFace=base_map().begin_face_sequence()+(right.level(i).endbase_type()-right.base_map().begin_face_sequence()) ;
        level(i).set_face_sequence(firstFace,lastFace) ;
      }
		}
		return *this ;
}*/

template <class TLevel>
bool vpyr_2_tpyramid<TLevel>::valid()
{
}

template <class TLevel>
void vpyr_2_tpyramid<TLevel>::clear()
{
	_Base::clear() ;
}

template <class TLevel>
void vpyr_2_tpyramid<TLevel>::read_structure(vcl_istream & stream)
{
}

template <class TLevel>
void vpyr_2_tpyramid<TLevel>::write_structure(vcl_ostream & stream) const
{
}

template <class TLevel>
void vpyr_2_tpyramid<TLevel>::down_projection_vertices(vmap_level_index arg_level,vcl_vector<vmap_vertex_index> & res) const
{
   vbl_controlled_partition h ;
   vmap_vertex_index v ;
   h.initialise(base_map().nb_vertices()) ;
   res.resize(base_map().nb_vertices()) ;
   for (v=0; v<base_map().nb_vertices(); v++)
   {
			const base_vertex_type & rv=base_map().vertex(v) ;
      if (rv.last_level()<arg_level)
      {
         vmap_2_map_dart_base_iterator d=rv.begin(), end=d ;
         do
         {
            vmap_level_index l=d->last_level() ;
            if (l<arg_level && vmap_is_contraction_type(l) && d->modified_at_last_level())
            {
               vmap_2_map_dart_base_iterator ad=d ; ad.alpha() ;
               vmap_vertex_index av=h.representative(ad->vertex().sequence_index()),
                     representativev=h.representative(v) ;
               if (base_map().vertex(representativev).last_level()>base_map().vertex(av).last_level())
               {
                  h.union_of(representativev,av) ;
               }
               else
               {
                  h.union_of(av,representativev) ;

               }
            }
            d.sigma() ;
         } while (d!=end) ;
      }
   }
   for (v=0; v<base_map().nb_vertices(); v++)
   {
      res[v]=h.representative(v) ;
   }
}

template <class TLevel>
void vpyr_2_tpyramid<TLevel>::down_projection_faces(vmap_level_index arg_level,vcl_vector<vmap_face_index> & res) const
{
   vbl_controlled_partition h ;
   vmap_face_index v ;
   h.initialise(base_map().nb_faces()) ;
   res.resize(base_map().nb_faces()) ;
   for (v=0; v<base_map().nb_faces(); v++)
   {
			const base_face_type & rv=base_map().face(v) ;
      if (rv.last_level()<arg_level)
      {
         vmap_2_map_dart_base_iterator d=rv.begin(), end=d ;
         do
         {
            vmap_level_index l=d->last_level() ;
            if (l<arg_level && vmap_is_removal_type(l) && d->modified_at_last_level())
            {
               vmap_2_map_dart_base_iterator ad=d ; ad.alpha() ;
               vmap_face_index av=h.representative(ad->face().sequence_index()),
                     representativev=h.representative(v) ;
               if (base_map().face(representativev).last_level()>base_map().face(av).last_level())
               {
                  h.union_of(representativev,av) ;
               }
               else
               {
                  h.union_of(av,representativev) ;

               }
            }
            d.phi() ;
         } while (d!=end) ;
      }
   }
   for (v=0; v<base_map().nb_faces(); v++)
   {
      res[v]=h.representative(v) ;
   }
}
