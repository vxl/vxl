// This is contrib/prip/vmap/vmap_types.h
#ifndef vmap_types_h_
#define vmap_types_h_
//:
// \file
// \brief provides the basic types and functions used by vmap.
// \author Jocelyn Marchadier
// \date 06/05/2004
//
// \verbatim
//  06 May 2004 Jocelyn Marchadier
// \endverbatim

//: Darts indices.
typedef int vmap_dart_index ;

//: Edges indices.
typedef int vmap_edge_index;

//: Vertices indices.
typedef int vmap_vertex_index;

//: Faces indices.
typedef int vmap_face_index;

//: Level indices.
typedef unsigned vmap_level_index ;

//: 
enum vmap_level_type
{
    vmap_contraction_type=0x00000001,
    vmap_pendant_contraction_type=0x00000003,
    vmap_redundant_contraction_type=0x00000005,

    vmap_removal_type=0x00000000,
    vmap_self_loop_removal_type=0x00000002,
    vmap_double_removal_type=0x00000004
} ;

//: 
inline bool vmap_is_contraction_type(vmap_level_index arg)
{
    return arg%2==1 ;
}

//: 
inline bool vmap_is_removal_type(vmap_level_index arg)
{
    return arg%2==0 ;
}

//: 
inline vmap_level_type vmap_get_level_type(vmap_level_index arg)
{
    return (vmap_level_type)(arg&0x0000000F) ;
}

//: 
inline vmap_level_index vmap_top_level_index()
{
    return (vmap_level_index)0x0FFFFFFF ;
}

//: 
template <class D>
struct vmap_at_level
{
		vmap_level_index level ;
		vmap_at_level(vmap_level_index arg) : level(arg) {}
		bool operator()(const D* arg)
   	{
				return arg->last_level()==level ;
   	}
} ;

//: 
template <class D>
struct vmap_sequence_index_not_equal
{
		int index ;
		vmap_sequence_index_not_equal(int arg) : index(arg) {}
		bool operator()(const D* arg)
   	{
				return arg->sequence_index()!=index ;
   	}
} ;

//: The tag class to mark the classes that respect the tmap interface.
struct vmap_2_tmap_tag {} ;

//: The tag class to mark the classes that respect the map interface.
struct vmap_2_map_tag {} ;

#endif
