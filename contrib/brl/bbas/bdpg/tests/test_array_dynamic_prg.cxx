// This is brl/bbas/bdpg/tests/test_array_dynamic_prg.cxx
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_math.h> //for vnl_math::pi
#include <bdpg/bdpg_node.h>
#include <bdpg/bdpg_array_dynamic_prg.h>

void test_array_dynamic_prg()
{
  bdpg_array_dynamic_prg find(10,10);
  std::cout <<"++++++++++++ TEST THE ARRAY DYNAMIC PROGRAM ++++++++++++++++\n"
           << "test the find operation\n";
  for (unsigned r = 0; r<10; ++r)
    for (unsigned c =0; c<10; ++c)
    {
      bdpg_node_sptr n = new bdpg_node();
      if (r+1==c)
      {
        n->set_previous_row((int)r-1);
        n->set_previous_col((int)c-1);
        n->set_prior_prob(1.0);
        find.put(r, c, n);
        continue;
      }
      find.put(r, c, bdpg_node_sptr());// null
    }
  find.print_array();
  find.print_path(8,9);
  bool find5 = find.find(8,9,5), not_find0 = !find.find(8,9,0);
  TEST("find on path",find5&&not_find0, true);

  std::cout << "test dynamic program\n";
  bdpg_array_dynamic_prg dp(8,8);
  // load the array with initial probabilities
  double radius = 1.0;
  for (unsigned r = 0; r<8; ++r)
    for (unsigned c =0; c<8; ++c)
    {
      bdpg_node_sptr n = new bdpg_node();
      double arg = (c-r)*(c-r);
      double p = std::exp(-arg/4.0)/std::sqrt(8*vnl_math::pi);
      double del = radius*(std::rand()/(RAND_MAX+1.0));
      p = (0.9*p + 0.1*del);
      n->set_prior_prob(p);
      dp.put(r, c, n);
    }
  dp.print_array();
  dp.scan();
  unsigned best_col = dp.best_assign_col();
  std::cout << "Best Assignment Column " << best_col << '\n';
  dp.print_path(7, best_col);
  TEST("scan array", best_col==6 || best_col==7, true);
  std::vector<unsigned> assign = dp.assignment();
  unsigned i = 0;
  bool good_assign = true;
  for (auto ait = assign.begin();
       ait !=assign.end(); ++ait, ++i)
  {
    std::cout << " c["<< i << "]=" << *ait;
    good_assign = good_assign && i/2 == *ait/2;
  }
  std::cout << '\n';
  TEST("assignment", good_assign, true);

  std::cout << "test with image more dominant\n";
  bdpg_array_dynamic_prg dj(8,8);
  // load the array with initial probabilities
  for (unsigned r = 0; r<8; ++r)
    for (unsigned c =0; c<8; ++c)
    {
      bdpg_node_sptr n = new bdpg_node();
      double arg = (c-r)*(c-r);
      double p = std::exp(-arg/4.0)/(2*std::sqrt(6.28));
      double del = radius*(std::rand()/(RAND_MAX+1.0));
      p = (0.8*p + 0.2*del);
      n->set_prior_prob(p);
      dj.put(r, c, n);
    }
  dj.print_array();
  dj.scan();
  best_col = dj.best_assign_col();
  std::cout << "Best Assignment Column " << best_col << '\n';
  dj.print_path(7, best_col);
  TEST("scan array", best_col>=5, true);
}

TESTMAIN(test_array_dynamic_prg);
