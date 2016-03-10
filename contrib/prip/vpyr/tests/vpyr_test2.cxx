//:
// \file
#include "my_pyramid.h"
#include "my_top_level.h"
#include <vmap/tests/my_map.h>
#include "my_pyramid_functions.hxx"

#include <vmap/vmap_non_oriented_kernel.h>
#include <vmap/vmap_oriented_kernel.h>

//: tests non-oriented kernels functionalities.
void vpyr_test_non_oriented_kernel2()
{
  my_tpyramid p ;

  std::cout<<std::endl<<"-----------------------------------------"<<std::endl
          <<"pyramid_type general contraction/removal test."<<std::endl
          <<"-----------------------------------------"<<std::endl ;

  build_base_grid(p) ;
  my_top_level top(p) ;
  initialise_ids(top) ;
  display_tmap(top) ;

  std::cout<<"Removal of {8,a(8),a(4),4,11,3}"<<std::endl ;

  vmap_non_oriented_kernel<my_top_level::removal_kernel> K(top) ;
  K.initialise() ;
  if (!K.add(find_dart(top,8).alpha()))
    std::cout<<"Couldn't add 8."<<std::endl ;
  if (!K.add(find_dart(top,8)))
    std::cout<<"Couldn't add a(8)."<<std::endl ;
  if (!K.add(find_dart(top,4).alpha()))
    std::cout<<"Couldn't add a(4)."<<std::endl ;
  if (!K.add(find_dart(top,4)))
    std::cout<<"Couldn't add 4."<<std::endl ;
  if (!K.add(find_dart(top,11)))
    std::cout<<"Couldn't add 11."<<std::endl ;
  if (!K.add(find_dart(top,3)))
    std::cout<<"Couldn't add 3."<<std::endl ;
  K.finalise() ;
  top.removal(K) ;
  display_tpyramid(p) ;

  my_top_level::contraction_kernel K2(top) ;

  K2.initialise() ;
  K2.add_1_cycles() ;
  K2.finalise() ;
  std::cout<<"Contraction of " ; display_kernel(K2) ; std::cout<<std::endl ;
  top.contraction(K2) ;
  display_tpyramid(p) ;
  display_tmap(top) ;
  K2.clear() ;
  K2.initialise() ;
  K2.add_2_cycles() ;
  K2.finalise() ;
  std::cout<<"Contraction of " ; display_kernel(K2) ; std::cout<<std::endl ;
  top.contraction(K2) ;
  display_tpyramid(p) ;
  display_tmap(top) ;
}

//: tests oriented kernels functionalities.
void vpyr_test_oriented_kernel2()
{
  my_tpyramid p ;

  std::cout<<std::endl<<"-----------------------------------------"<<std::endl
          <<"pyramid_type general contraction/removal test."<<std::endl
          <<"-----------------------------------------"<<std::endl ;

  build_base_grid(p) ;
  my_top_level top(p) ;
  initialise_ids(top) ;
  display_tmap(top) ;

  std::cout<<"Removal of {8,a(8),a(4),4,11,3}"<<std::endl ;

  vmap_oriented_kernel<my_top_level::removal_kernel> K(top) ;
  K.initialise() ;
  if (!K.add(find_dart(top,8)))
    std::cout<<"Couldn't add 8."<<std::endl ;
  if (!K.add(find_dart(top,8).alpha()))
    std::cout<<"Couldn't add a(8)."<<std::endl ;
  if (!K.add(find_dart(top,4).alpha()))
    std::cout<<"Couldn't add a(4)."<<std::endl ;
  if (!K.add(find_dart(top,4)))
    std::cout<<"Couldn't add 4."<<std::endl ;
  if (!K.add(find_dart(top,11)))
    std::cout<<"Couldn't add 11."<<std::endl ;
  if (!K.add(find_dart(top,3)))
    std::cout<<"Couldn't add 3."<<std::endl ;
  K.finalise() ;
  top.removal(K) ;
  display_tpyramid(p) ;
  display_tmap(top) ;

  my_top_level::contraction_kernel K2(top) ;

  K2.initialise() ;
  K2.add_1_cycles() ;
  K2.finalise() ;
  std::cout<<"Contraction of " ; std::cout.flush() ; display_kernel(K2) ; std::cout<<std::endl ;
  top.contraction(K2) ;
  display_tpyramid(p) ;
  display_tmap(top) ;

  K2.clear() ;
  K2.initialise() ;
  K2.add_2_cycles() ;
  K2.finalise() ;
  std::cout<<"Contraction of " ; std::cout.flush() ; display_kernel(K2) ; std::cout<<std::endl ;
  top.contraction(K2) ;
  display_tpyramid(p) ;
  display_tmap(top) ;
}


int main()
{
  vpyr_test_non_oriented_kernel2() ;
  vpyr_test_oriented_kernel2() ;
  return 0 ;
}
