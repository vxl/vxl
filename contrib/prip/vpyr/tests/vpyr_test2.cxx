#include "my_pyramid.h"
#include "my_top_level.h"
#include "vmap/tests/my_map.h"
#include "my_pyramid_functions.txx"

#include "vmap/vmap_non_oriented_kernel.h"
#include "vmap/vmap_oriented_kernel.h"

//: tests non-oriented kernels functionalities.
void vpyr_test_non_oriented_kernel2()
{
  my_tpyramid p ;

  vcl_cout<<vcl_endl<<"-----------------------------------------"<<vcl_endl ;
  vcl_cout<<"pyramid_type general contraction/removal test."<<vcl_endl ;
  vcl_cout<<"-----------------------------------------"<<vcl_endl ;

  build_base_grid(p) ;
	my_top_level top(p) ;
	initialise_ids(top) ;
	display_tmap(top) ;

  vcl_cout<<"Removal of {8,a(8),a(4),4,11,3}"<<vcl_endl ;

  vmap_non_oriented_kernel<my_top_level::removal_kernel> K(top) ;
  K.initialise() ;
  if (!K.add(find_dart(top,8).alpha()))
    vcl_cout<<"Couldn't add 8."<<vcl_endl ;
  if (!K.add(find_dart(top,8)))
    vcl_cout<<"Couldn't add a(8)."<<vcl_endl ;
  if (!K.add(find_dart(top,4).alpha()))
    vcl_cout<<"Couldn't add a(4)."<<vcl_endl ;
  if (!K.add(find_dart(top,4)))
    vcl_cout<<"Couldn't add 4."<<vcl_endl ;
  if (!K.add(find_dart(top,11)))
    vcl_cout<<"Couldn't add 11."<<vcl_endl ;
  if (!K.add(find_dart(top,3)))
    vcl_cout<<"Couldn't add 3."<<vcl_endl ;
  K.finalise() ;
  top.removal(K) ;
  display_tpyramid(p) ;

  my_top_level::contraction_kernel K2(top) ;

  K2.initialise() ;
  K2.add_1_cycles() ;
  K2.finalise() ;
  vcl_cout<<"Contraction of " ; display_kernel(K2) ; vcl_cout<<vcl_endl ;
  top.contraction(K2) ;
  display_tpyramid(p) ;
  display_tmap(top) ;
  K2.clear() ;
  K2.initialise() ;
  K2.add_2_cycles() ;
  K2.finalise() ;
  vcl_cout<<"Contraction of " ; display_kernel(K2) ; vcl_cout<<vcl_endl ;
  top.contraction(K2) ;
  display_tpyramid(p) ;
  display_tmap(top) ;
}

//: tests oriented kernels functionalities.
void vpyr_test_oriented_kernel2()
{
  my_tpyramid p ;

  vcl_cout<<vcl_endl<<"-----------------------------------------"<<vcl_endl ;
  vcl_cout<<"pyramid_type general contraction/removal test."<<vcl_endl ;
  vcl_cout<<"-----------------------------------------"<<vcl_endl ;

  build_base_grid(p) ;
	my_top_level top(p) ;
	initialise_ids(top) ;
	display_tmap(top) ;

  vcl_cout<<"Removal of {8,a(8),a(4),4,11,3}"<<vcl_endl ;

  vmap_oriented_kernel<my_top_level::removal_kernel> K(top) ;
  K.initialise() ;
  if (!K.add(find_dart(top,8)))
    vcl_cout<<"Couldn't add 8."<<vcl_endl ;
  if (!K.add(find_dart(top,8).alpha()))
    vcl_cout<<"Couldn't add a(8)."<<vcl_endl ;
  if (!K.add(find_dart(top,4).alpha()))
    vcl_cout<<"Couldn't add a(4)."<<vcl_endl ;
  if (!K.add(find_dart(top,4)))
    vcl_cout<<"Couldn't add 4."<<vcl_endl ;
  if (!K.add(find_dart(top,11)))
    vcl_cout<<"Couldn't add 11."<<vcl_endl ;
  if (!K.add(find_dart(top,3)))
    vcl_cout<<"Couldn't add 3."<<vcl_endl ;
  K.finalise() ;
  top.removal(K) ;
  display_tpyramid(p) ;
	display_tmap(top) ;

  my_top_level::contraction_kernel K2(top) ;

  K2.initialise() ;
  K2.add_1_cycles() ;
  K2.finalise() ;
  vcl_cout<<"Contraction of " ; vcl_cout.flush() ; display_kernel(K2) ; vcl_cout<<vcl_endl ;
  top.contraction(K2) ;
  display_tpyramid(p) ;
  display_tmap(top) ;
	
	K2.clear() ;
  K2.initialise() ;
  K2.add_2_cycles() ;
  K2.finalise() ;
  vcl_cout<<"Contraction of " ; vcl_cout.flush() ; display_kernel(K2) ; vcl_cout<<vcl_endl ;
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
