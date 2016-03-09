//:
// \file
#include "my_pyramid.h"
#include <vmap/tests/my_map.h>
#include "my_pyramid_functions.hxx"

#include <vmap/vmap_non_oriented_kernel.h>
#include <vmap/vmap_oriented_kernel.h>

//: tests the basic functions for building a pyramid and extracting (copying) the structure of one of its levels.
void vpyr_test_contraction_removal_copy()
{
  my_tpyramid p ;
  int i;

  std::cout<<std::endl<<"-----------------------------------------"<<std::endl
          <<"pyramid_type general contraction/removal test."<<std::endl
          <<"-----------------------------------------"<<std::endl ;

  build_base_grid(p) ;

  std::cout<<"Contraction of {1,7}/{"<<p.top_level().alpha(0)<<','<<p.top_level().alpha(12)<<'}'<<std::endl ;

  my_tpyramid::contraction_kernel K(p.top_level()) ;
  K.initialise() ;
  K.add(find_dart(p.top_level(),1).alpha()) ;
  K.add(find_dart(p.top_level(),7).alpha()) ;
  K.finalise() ;
  std::cout<<K.size()<<std::endl ;
  p.top_level().contraction(K) ;
  display_tpyramid(p) ;

#if 0
  std::cout<<"Vertices :"<<std::endl ;
  for (my_pyr_level::vertex_iterator v=p.level(0).begin_vertex(); v!=p.level(0).end_vertex(); ++v)
  {
    std::cout<<my_pyr_level::cast(*v).last_level()<<' ' ;
  }

  std::cout<<std::endl
          <<"Edges :"<<std::endl ;
  for (my_pyr_level::edge_iterator e=p.level(0).begin_edge(); e!=p.level(0).end_edge(); ++e)
  {
    std::cout<<my_pyr_level::cast(*e).last_level()<<' ' ;
  }
  std::cout<<std::endl ;
#endif // 0

  std::cout<<"Removal of {8,4} :"<<std::endl ;
  my_tpyramid::removal_kernel K2(p.top_level()) ;

  K2.initialise() ;
  K2.add(find_dart(p.top_level(),8)) ;
  K2.add(find_dart(p.top_level(),4).alpha()) ;
  K2.finalise() ;
  p.top_level().removal(K2) ;
  display_tpyramid(p) ;

  std::cout<<"Setting structure of level 1..."<<std::endl ;
  my_tmap copy_map ;
  copy_map.set_structure(p.level(1)) ;
  for (i=0;i<copy_map.nb_vertices();++i)
  {
    copy_map.vertex(i).id()=p.level(1).vertex(i).id() ;
  }
  for (i=0;i<copy_map.nb_edges();++i)
  {
    copy_map.edge(i).id()=p.level(1).edge(i).id() ;
  }
  for (i=0;i<copy_map.nb_faces();++i)
  {
    copy_map.face(i).id()=p.level(1).face(i).id() ;
  }
  for (i=0;i<copy_map.nb_darts();++i)
  {
    copy_map.dart(i).id()=p.level(1).dart(i).id() ;
  }
  if (!copy_map.valid_permutations())
  {
    std::cerr<<"Problem !!!!"<<std::endl ;
  }
  display_tmap(copy_map) ;

#if 0
    std::cout<<"Removal of {11,4} :"<<std::endl ;
    my_tpyramid::removal_kernel Kr(p.top_level()) ;
    Kr.add(20) ;
    Kr.add(6) ;
    std::cout<<Kr.size()<<std::endl ;
    p.top_level().removal(Kr) ;
    display_tpyramid(p) ;
#endif // 0

#if 0
  std::cout<<"Contraction of {8,10} :"<<std::endl ;
  K.clear() ;
  K.insert(p.alpha(7)) ;
  K.insert(p.alpha(9)) ;
  std::cout<<K.size()<<std::endl ;
  p.top_level().contraction(K) ;
  display_pyramid(p) ;
#endif // 0

#if 0
  std::cout<<"Removal of {11,4} :"<<std::endl ;
  pyramid_type::removal_kernel Kr(p) ;
  Kr.insert(10) ;
  Kr.insert(3) ;
  std::cout<<Kr.size()<<std::endl ;
  p.top_level().removal(Kr) ;
  display_pyramid(p) ;
#endif // 0
}

//: tests the pendant edges contraction.
void vpyr_test_pendant()
{
  my_tpyramid p ;

  std::cout<<std::endl<<"-----------------------------------------"<<std::endl
          <<"pyramid_type general contraction/removal test."<<std::endl
          <<"-----------------------------------------"<<std::endl ;

  build_base_grid(p) ;

  std::cout<<"Removal of {2,6}/{"<<p.top_level().alpha(0)<<','<<p.top_level().alpha(12)<<'}'<<std::endl ;

  my_tpyramid::removal_kernel K(p.top_level()) ;
  K.initialise() ;
  K.add(find_dart(p.top_level(),2).alpha()) ;
  K.add(find_dart(p.top_level(),6)) ;
  K.finalise() ;
  p.top_level().removal(K) ;
  display_tpyramid(p) ;

  my_tpyramid::contraction_kernel K2(p.top_level()) ;

  K2.initialise() ;
  K2.add_1_cycles() ;
  K2.finalise() ;
  std::cout<<"Contraction of " ; display_kernel(K2) ; std::cout<<std::endl ;
  p.top_level().contraction(K2) ;
  display_tpyramid(p) ;
}

//: tests the pendant and redundant edges contraction.
void vpyr_test_pendant_redundant()
{
  my_tpyramid p ;

  std::cout<<std::endl<<"-----------------------------------------"<<std::endl
          <<"pyramid_type general contraction/removal test."<<std::endl
          <<"-----------------------------------------"<<std::endl ;

  build_base_grid(p) ;

  std::cout<<"Removal of {8,4,11}"<<std::endl ;

  my_pyramid_level::removal_kernel K(p.top_level()) ;
  K.initialise() ;
  K.add(find_dart(p.top_level(),8)) ;
  K.add(find_dart(p.top_level(),4).alpha()) ;
  K.add(find_dart(p.top_level(),11).alpha()) ;
  K.finalise() ;
  p.top_level().removal(K) ;
  display_tpyramid(p) ;

  my_pyramid_level::contraction_kernel K2(p.top_level()) ;

  K2.initialise() ;
  K2.add_1_cycles() ;
  K2.finalise() ;
  std::cout<<"Contraction of " ; display_kernel(K2) ; std::cout<<std::endl ;
  p.top_level().contraction(K2) ;
  display_tpyramid(p) ;
  K2.clear() ;
  K2.initialise() ;
  K2.add_2_cycles() ;
  K2.finalise() ;
  std::cout<<"Contraction of " ; display_kernel(K2) ; std::cout<<std::endl ;
  p.top_level().contraction(K2) ;
  display_tpyramid(p) ;
}

//: tests non-oriented kernels functionalities.
void vpyr_test_non_oriented_kernel()
{
  my_tpyramid p ;

  std::cout<<std::endl<<"-----------------------------------------"<<std::endl
          <<"pyramid_type general contraction/removal test."<<std::endl
          <<"-----------------------------------------"<<std::endl ;

  build_base_grid(p) ;

  std::cout<<"Removal of {8,a(8),a(4),4,11,3}"<<std::endl ;

  vmap_non_oriented_kernel<my_pyramid_level::removal_kernel> K(p.top_level()) ;
  K.initialise() ;
  if (!K.add(find_dart(p.top_level(),8).alpha()))
    std::cout<<"Couldn't add 8."<<std::endl ;
  if (!K.add(find_dart(p.top_level(),8)))
    std::cout<<"Couldn't add a(8)."<<std::endl ;
  if (!K.add(find_dart(p.top_level(),4).alpha()))
    std::cout<<"Couldn't add a(4)."<<std::endl ;
  if (!K.add(find_dart(p.top_level(),4)))
    std::cout<<"Couldn't add 4."<<std::endl ;
  if (!K.add(find_dart(p.top_level(),11)))
    std::cout<<"Couldn't add 11."<<std::endl ;
  if (!K.add(find_dart(p.top_level(),3)))
    std::cout<<"Couldn't add 3."<<std::endl ;
  K.finalise() ;
  p.top_level().removal(K) ;
  display_tpyramid(p) ;

  my_pyramid_level::contraction_kernel K2(p.top_level()) ;

  K2.initialise() ;
  K2.add_1_cycles() ;
  K2.finalise() ;
  std::cout<<"Contraction of " ; display_kernel(K2) ; std::cout<<std::endl ;
  p.top_level().contraction(K2) ;
  display_tpyramid(p) ;
  K2.clear() ;
  K2.initialise() ;
  K2.add_2_cycles() ;
  K2.finalise() ;
  std::cout<<"Contraction of " ; display_kernel(K2) ; std::cout<<std::endl ;
  p.top_level().contraction(K2) ;
  display_tpyramid(p) ;
}

//: tests oriented kernels functionalities.
void vpyr_test_oriented_kernel()
{
  my_tpyramid p ;

  std::cout<<std::endl<<"-----------------------------------------"<<std::endl
          <<"pyramid_type general contraction/removal test."<<std::endl
          <<"-----------------------------------------"<<std::endl ;

  build_base_grid(p) ;

  std::cout<<"Removal of {8,a(8),a(4),4,11,3}"<<std::endl ;

  vmap_oriented_kernel<my_pyramid_level::removal_kernel> K(p.top_level()) ;
  K.initialise() ;
  if (!K.add(find_dart(p.top_level(),8)))
    std::cout<<"Couldn't add 8."<<std::endl ;
  if (!K.add(find_dart(p.top_level(),8).alpha()))
    std::cout<<"Couldn't add a(8)."<<std::endl ;
  if (!K.add(find_dart(p.top_level(),4).alpha()))
    std::cout<<"Couldn't add a(4)."<<std::endl ;
  if (!K.add(find_dart(p.top_level(),4)))
    std::cout<<"Couldn't add 4."<<std::endl ;
  if (!K.add(find_dart(p.top_level(),11)))
    std::cout<<"Couldn't add 11."<<std::endl ;
  if (!K.add(find_dart(p.top_level(),3)))
    std::cout<<"Couldn't add 3."<<std::endl ;
  K.finalise() ;
  p.top_level().removal(K) ;
  display_tpyramid(p) ;

  my_pyramid_level::contraction_kernel K2(p.top_level()) ;

  K2.initialise() ;
  K2.add_1_cycles() ;
  K2.finalise() ;
  std::cout<<"Contraction of " ; std::cout.flush() ; display_kernel(K2) ; std::cout<<std::endl ;
  p.top_level().contraction(K2) ;
  display_tpyramid(p) ;
  K2.clear() ;
  K2.initialise() ;
  K2.add_2_cycles() ;
  K2.finalise() ;
  std::cout<<"Contraction of " ; std::cout.flush() ; display_kernel(K2) ; std::cout<<std::endl ;
  p.top_level().contraction(K2) ;
  display_tpyramid(p) ;
}


int main()
{
  vpyr_test_contraction_removal_copy() ;
  vpyr_test_pendant_redundant() ;
  vpyr_test_non_oriented_kernel() ;
  vpyr_test_oriented_kernel() ;
  return 0 ;
}
