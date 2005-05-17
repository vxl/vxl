// This is brl/bbas/bdpg/tests/test_array_dynamic_prg.cxx
#include <testlib/testlib_test.h>
#include <vcl_cstdlib.h> // for rand()
#include <vcl_cmath.h> //for exp
#include <vnl/vnl_math.h> //for exp
#include <vcl_iostream.h>
#include <bdpg/bdpg_node.h>
#include <bdpg/bdpg_array_dynamic_prg.h>

void test_array_dynamic_prg()
{
  bdpg_array_dynamic_prg find(10,10);
  vcl_cout <<"++++++++++++ TEST THE ARRAY DYNAMIC PROGRAM ++++++++++++++++\n"
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

  vcl_cout << "test dynamic program\n";
  bdpg_array_dynamic_prg dp(8,8);
  // load the array with initial probabilities
  double radius = 1.0;
  for (unsigned r = 0; r<8; ++r)
    for (unsigned c =0; c<8; ++c)
    {
      bdpg_node_sptr n = new bdpg_node();
      double arg = (c-r)*(c-r);
      double p = vcl_exp(-arg/4.0)/(2*vcl_sqrt(2*vnl_math::pi));
      double del = radius*(vcl_rand()/(RAND_MAX+1.0));
      p = (0.9*p + 0.1*del);
      n->set_prior_prob(p);
      dp.put(r, c, n);
    }
  dp.print_array();
  dp.scan();
  unsigned best_col = dp.best_assign_col();
  vcl_cout << "Best Assignment Column " << best_col << '\n';
  dp.print_path(7, best_col);
  TEST("scan array", best_col==6 || best_col==7, true);
  vcl_vector<unsigned> assign = dp.assignment();
  unsigned i = 0;
  bool good_assign = true;
  for (vcl_vector<unsigned>::iterator ait = assign.begin();
       ait !=assign.end(); ++ait, ++i)
  {
    vcl_cout << " c["<< i << "]=" << *ait;
    good_assign = good_assign && i/2 == *ait/2;
  }
  vcl_cout << '\n';
  TEST("assignment", good_assign, true);

  vcl_cout << "test with image more dominant\n";
  bdpg_array_dynamic_prg dj(8,8);
  // load the array with initial probabilities
  for (unsigned r = 0; r<8; ++r)
    for (unsigned c =0; c<8; ++c)
    {
      bdpg_node_sptr n = new bdpg_node();
      double arg = (c-r)*(c-r);
      double p = vcl_exp(-arg/4.0)/(2*vcl_sqrt(6.28));
      double del = radius*(vcl_rand()/(RAND_MAX+1.0));
      p = (0.8*p + 0.2*del);
      n->set_prior_prob(p);
      dj.put(r, c, n);
    }
  dj.print_array();
  dj.scan();
  best_col = dj.best_assign_col();
  vcl_cout << "Best Assignment Column " << best_col << '\n';
  dj.print_path(7, best_col);
  TEST("scan array", best_col>=5, true);
}

TESTMAIN(test_array_dynamic_prg);
