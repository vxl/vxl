//*****************************************************************************
// File name: test_vsol_group_2d.cxx
// Description: Test the vsol_group_2d class
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/04/26| François BERTEL          |Creation
// 1.1     |2003/01/08| Peter Vanroose           |Now using testlib macros
//*****************************************************************************
#include <testlib/testlib_test.h>
//:
// \file

#include <vsol/vsol_group_2d.h>
#include <vsol/vsol_group_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>

void test_vsol_group_2d()
{
  vsol_group_2d_sptr group1=new vsol_group_2d;

  TEST("vsol_group_2d::size()", group1->size(), 0);
  TEST("vsol_group_2d::deep_size()", group1->deep_size(), 0);

  vsol_point_2d_sptr p=new vsol_point_2d(10,4);
  group1->add_object(p->cast_to_spatial_object());

  TEST("vsol_group_2d::add_object(point)", group1->size(), 1);
  TEST("vsol_group_2d::deep_size()", group1->deep_size(), 1);

  vsol_group_2d_sptr group2=new vsol_group_2d;
  group1->add_object(group2->cast_to_spatial_object());

  TEST("vsol_group_2d::add_object(group)", group1->size(), 2);
  TEST("vsol_group_2d::deep_size()", group1->deep_size(), 1);

  TEST("vsol_group_2d::object(0)", *(group1->object(0)), *p);
  TEST("vsol_group_2d::object(1)", *(group1->object(1)), *group2);

  group2->add_object(p->cast_to_spatial_object());

  TEST("object(1)->add_object(point)", group1->size(), 2);
  TEST("vsol_group_2d::deep_size()", group1->deep_size(), 2);

  group1->remove_object(0);
  TEST("vsol_group_2d::remove_object(0)", group1->size(), 1);
  TEST("vsol_group_2d::deep_size()", group1->deep_size(), 1);

  TEST("cast_to_group()", group1->object(0)->cast_to_group(), group2);
}

TESTMAIN(test_vsol_group_2d);
