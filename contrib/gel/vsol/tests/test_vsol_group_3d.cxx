//*****************************************************************************
//:
// \file
// \brief Test the vsol_group_3d class
//
// \verbatim
//  Modifications
//   Version |Date      | Author                   |Comment
//   --------+----------+--------------------------+---------------------------
//   1.0     |2000/05/09| François BERTEL          |Creation
//   1.1     |2003/01/08| Peter Vanroose           |Now using testlib macros
// \endverbatim
//*****************************************************************************
#include <testlib/testlib_test.h>
#include <vsol/vsol_group_3d.h>
#include <vsol/vsol_group_3d_sptr.h>
#include <vsol/vsol_point_3d.h>
#include <vsol/vsol_point_3d_sptr.h>

void test_vsol_group_3d()
{
  vsol_group_3d_sptr group1=new vsol_group_3d;

  TEST("vsol_group_3d::size()", group1->size(), 0);
  TEST("vsol_group_3d::deep_size()", group1->deep_size(), 0);

  vsol_point_3d_sptr p=new vsol_point_3d(10,4,5);
  group1->add_object(p->cast_to_spatial_object());

  TEST("vsol_group_3d::add_object(point)", group1->size(), 1);
  TEST("vsol_group_3d::deep_size()", group1->deep_size(), 1);

  vsol_group_3d_sptr group2=new vsol_group_3d;
  group1->add_object(group2->cast_to_spatial_object());

  TEST("vsol_group_3d::add_object(group)", group1->size(), 2);
  TEST("vsol_group_3d::deep_size()", group1->deep_size(), 1);

  TEST("vsol_group_3d::object(0)", *(group1->object(0)), *p);
  TEST("vsol_group_3d::object(1)", *(group1->object(1)), *group2);

  group2->add_object(p->cast_to_spatial_object());

  TEST("object(1)->add_object(point)", group1->size(), 2);
  TEST("vsol_group_3d::deep_size()", group1->deep_size(), 2);

  group1->remove_object(0);
  TEST("vsol_group_3d::remove_object(0)", group1->size(), 1);
  TEST("vsol_group_3d::deep_size()", group1->deep_size(), 1);

  TEST("cast_to_group()", group1->object(0)->cast_to_group(), group2);
}

TESTMAIN(test_vsol_group_3d);
