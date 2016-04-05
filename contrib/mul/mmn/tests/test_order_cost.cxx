// This is mul/mmn/tests/test_order_cost.cxx
#include <testlib/testlib_test.h>
#include <mmn/mmn_order_cost.h>

void test_get_order()
{
  std::cout<<"==== test mmn_get_order ====="<<std::endl;

  unsigned i0,i1,i2;
  mmn_get_order(0,1,2,i0,i1,i2);
  TEST("mmn_get_order 0-1-2",i0==0 && i1==1 && i2==2, true);

  mmn_get_order(0,2,1,i0,i1,i2);
  TEST("get_order 0-2-1",i0==0 && i1==2 && i2==1, true);

  mmn_get_order(1,0,2,i0,i1,i2);
  TEST("mmn_get_order 1-0-2",i0==1 && i1==0 && i2==2, true);

  mmn_get_order(1,2,0,i0,i1,i2);
  TEST("mmn_get_order 1-2-0",i0==1 && i1==2 && i2==0, true);

  mmn_get_order(2,0,1,i0,i1,i2);
  TEST("mmn_get_order 2-0-1",i0==2 && i1==0 && i2==1, true);

  mmn_get_order(2,1,0,i0,i1,i2);
  TEST("mmn_get_order 2-1-0",i0==2 && i1==1 && i2==0, true);
}

void test_get_rank()
{
  std::cout<<"==== test mmn_get_rank ====="<<std::endl;

  unsigned i0,i1,i2;
  mmn_get_rank(0,1,2,i0,i1,i2);
  TEST("mmn_get_rank 0-1-2",i0==0 && i1==1 && i2==2, true);

  mmn_get_rank(0,2,1,i0,i1,i2);
  TEST("mmn_get_rank 0-2-1",i0==0 && i1==2 && i2==1, true);

  mmn_get_rank(1,0,2,i0,i1,i2);
  TEST("mmn_get_rank 1-0-2",i0==1 && i1==0 && i2==2, true);

  mmn_get_rank(1,2,0,i0,i1,i2);
  TEST("mmn_get_rank 1-2-0",i0==2 && i1==0 && i2==1, true);

  mmn_get_rank(2,0,1,i0,i1,i2);
  TEST("mmn_get_rank 2-0-1",i0==1 && i1==2 && i2==0, true);

  mmn_get_rank(2,1,0,i0,i1,i2);
  TEST("mmn_get_rank 2-1-0",i0==2 && i1==1 && i2==0, true);
}

void test_order_cost_a()
{
  std::cout<<"==== Test mmn_order_cost + mmn_unorder_cost ===="<<std::endl;
  vil_image_view<double> c(3,4,5),d,e;

  d=mmn_order_cost(c,0,2,1);
  e=mmn_unorder_cost(d,0,2,1);
  TEST("0-2-1",e.ni()==c.ni() && e.nj()==c.nj(), true);

  d=mmn_order_cost(c,1,0,2);
  e=mmn_unorder_cost(d,1,0,2);
  TEST("1-0-2",e.ni()==c.ni() && e.nj()==c.nj(), true);

  d=mmn_order_cost(c,2,1,0);
  e=mmn_unorder_cost(d,2,1,0);
  TEST("2-1-0",e.ni()==c.ni() && e.nj()==c.nj(), true);
}

void test_order_cost()
{
  test_get_order();
  test_get_rank();
  test_order_cost_a();
}

TESTMAIN(test_order_cost);
