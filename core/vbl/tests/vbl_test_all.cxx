/*
  fsm@robots.ox.ac.uk
*/
#include <vbl/vbl_test.h>

#include <vbl/vbl_bounding_box.h>
#include <vbl/vbl_fwd.h>
#include <vbl/vbl_array_1d.h>
#include <vbl/vbl_array_2d.h>
#include <vbl/vbl_array_3d.h>
#include <vbl/vbl_bool_ostream.h>
#include <vbl/vbl_protection_traits.h>
#include <vbl/vbl_qsort.h>
#include <vbl/vbl_quadruple.h>
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_shared_pointer.h>
#include <vbl/vbl_smart_ptr.h>
#include <vbl/vbl_sort.h>
#include <vbl/vbl_sparse_array_1d.h>
#include <vbl/vbl_sparse_array_2d.h>
#include <vbl/vbl_sparse_array_3d.h>
#include <vbl/vbl_triple.h>

#if 0 // These files use the nonexistent vcl_hash_map.h and/or vcl_hash_string.h
#include <vbl/vbl_hash_map.h>
#include <vbl/vbl_basic_optional.h>
#include <vbl/vbl_basic_relation.h>
#include <vbl/vbl_basic_tuple.h>
#include <vbl/vbl_basic_relation_where.h>
#include <vbl/vbl_br_default.h>
#include <vbl/vbl_br_impl.h>
#endif
#include <vbl/vbl_basic_relation_type.h>



void test_bounding_box()
{ 
  vcl_cout << "\n\n\n*******************\n"
                 << " Test Bounding Box \n"
                 << "*******************" << vcl_endl;
  vbl_bounding_box<double,3> bb;
  TEST("bounding box dimension", bb.dimension(), 3);
  bb.update(-3.0,4.0,5.0);
  bb.update(3.0,-4.0,5.0);
  bb.update(3.0,4.0,-5.0);
  TEST("bounding box min_x", bb.min()[0], -3.0);
  TEST("bounding box min_y", bb.min()[1], -4.0);
  TEST("bounding box min_z", bb.min()[2], -5.0);
  TEST("bounding box max_x", bb.max()[0], 3.0);
  TEST("bounding box max_y", bb.max()[1] ,4.0);
  TEST("bounding box max_z", bb.max()[2] ,5.0);
  TEST("bounding box volume", bb.volume() ,480.0);
  bb.reset();
  TEST("bounding box volume", bb.volume() ,0.0);
  bb.update(3.0,-4.0,5.0);
  TEST("bounding box min_x", bb.min()[0] ,3.0);
  TEST("bounding box min_y", bb.min()[1] ,-4.0);
  TEST("bounding box min_z", bb.min()[2] ,5.0);
  TEST("bounding box max_x", bb.max()[0] ,3.0);
  TEST("bounding box max_y", bb.max()[1] ,-4.0);
  TEST("bounding box max_z", bb.max()[2] ,5.0);
  TEST("bounding box volume", bb.volume() ,0.0);
}

void test_qsort()
{ 
  vcl_cout << "\n\n\n************\n"
                 << " Test qsort \n"
                 << "************" << vcl_endl;
  vcl_vector<double> v(10);
  for (int i=0; i<10; ++i) v[i] = 0.1*i*i - i + 1; // parabola with top (5,-1.5)
  vbl_qsort_ascending(v);
  for (int i=1; i<10; ++i)
    TEST("qsort ascending", v[i-1] <= v[i], true);
  vbl_qsort_descending(v);
  for (int i=1; i<10; ++i)
    TEST("qsort descending", v[i-1] >= v[i], true);
}

void test_triple()
{ 
  vcl_cout << "\n\n\n*************\n"
                 << " Test Triple \n"
                 << "*************" << vcl_endl;
  vbl_triple<double,int,int> t(7.0,1,2);
  TEST("vbl_triple constructor", t.first == 7.0 && t.second == 1 && t.third == 2, true);
  vbl_triple<double,int,int> t2 = t; t2.first = 6.0;
  TEST("vbl_triple copy constructor", t2.first == 6.0 && t2.second == 1 && t2.third == 2, true);
  TEST("vbl_triple compare", t2 != t, true);
  TEST("vbl_triple compare", t2 < t, true);
  t2 = t; t2.third = 3;
  TEST("vbl_triple assignment operator", t2.first == 7.0 && t2.second == 1 && t2.third == 3, true);
  TEST("vbl_triple compare", t != t2, true);
  TEST("vbl_triple compare", t < t2, true);
}

void test_quadruple()
{ 
  vcl_cout << "\n\n\n****************\n"
                 << " Test Quadruple \n"
                 << "****************" << vcl_endl;
  vbl_quadruple<int,int,int,int> t(7,0,1,2);
  TEST("vbl_quadruple constructor", t.first == 7 && t.second == 0 && t.third == 1 && t.fourth == 2, true);
  vbl_quadruple<int,int,int,int> t2 = t; t2.first = 6;
  TEST("vbl_quadruple copy constructor", t2.first == 6 && t2.second == 0 && t2.third == 1 && t2.fourth == 2, true);
  TEST("vbl_quadruple compare", t2 != t, true);
  TEST("vbl_quadruple compare", t2 < t, true);
  t2 = t; t2.third = 3;
  TEST("vbl_quadruple assignment operator", t2.first == 7 && t2.second == 0 && t2.third == 3 && t2.fourth == 2, true);
  TEST("vbl_quadruple compare", t != t2, true);
  TEST("vbl_quadruple compare", t < t2, true);
}



void vbl_test_all()
{
  test_bounding_box();
  test_quadruple();
  test_triple();
  test_qsort();
}
    

TESTMAIN(vbl_test_all);
