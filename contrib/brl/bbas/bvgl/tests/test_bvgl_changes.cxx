//:
// \file
#include <testlib/testlib_test.h>
#include <bvgl/bvgl_changes.h>
#include <bvgl/bvgl_changes_sptr.h>
#include <bvgl/bvgl_change_obj.h>
#include <bvgl/bvgl_change_obj_sptr.h>
#include <vgl/vgl_polygon.h>

//: Test changes
static void test_bvgl_changes()
{
  vgl_polygon<double> p;
  p.new_sheet();
  p.push_back( 0.0, 0.0 );
  p.push_back( 5.0, 0.0 );
  p.push_back( 5.0, 1.0 );

  bvgl_change_obj_sptr o = new bvgl_change_obj(p, "vehicle");
  bvgl_change_obj_sptr o1 = new bvgl_change_obj(p, "car");
  bvgl_change_obj_sptr o2 = new bvgl_change_obj(p, "dummy");
  bvgl_changes_sptr c = new bvgl_changes();
  c->add_obj(o);
  c->add_obj(o1);
  c->add_obj(o2);

  TEST("Test bvgl changes ", c->size(), 3);
  TEST("Test bvgl changes ", c->obj(0)->type(), "vehicle");
  TEST_NEAR("Test bvgl changes ", c->obj(0)->poly()[0][0].x(), 0.0, 0.001);
  TEST_NEAR("Test bvgl changes ", c->obj(0)->poly()[0][0].y(), 0.0, 0.001);
  TEST_NEAR("Test bvgl changes ", c->obj(0)->poly()[0][1].x(), 5.0, 0.001);

  vsl_b_ofstream os("./out.bin");
  c->b_write(os);
  os.close();

  vsl_b_ifstream is("./out.bin");
  bvgl_changes_sptr c2 = new bvgl_changes();
  c2->b_read(is);
  is.close();

  TEST("Test bvgl changes ", c2->size(), 3);
  TEST("Test bvgl changes ", c2->obj(0)->type(), "vehicle");
  TEST_NEAR("Test bvgl changes ", c2->obj(0)->poly()[0][0].x(), 0.0, 0.001);
  TEST_NEAR("Test bvgl changes ", c2->obj(0)->poly()[0][0].y(), 0.0, 0.001);
  TEST_NEAR("Test bvgl changes ", c2->obj(0)->poly()[0][1].x(), 5.0, 0.001);

  TEST("Test bvgl changes ", c2->obj(1)->type(), "car");
  TEST("Test bvgl changes ", c2->obj(2)->type(), "dummy");
  TEST_NEAR("Test bvgl changes ", c2->obj(2)->poly()[0][0].x(), 0.0, 0.001);
  TEST_NEAR("Test bvgl changes ", c2->obj(2)->poly()[0][0].y(), 0.0, 0.001);
  TEST_NEAR("Test bvgl changes ", c2->obj(2)->poly()[0][1].x(), 5.0, 0.001);
}

TESTMAIN( test_bvgl_changes );
