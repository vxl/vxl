// This is mul/mbl/tests/test_dyn_prog.cxx
#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <mbl/mbl_dyn_prog.h>
#include <testlib/testlib_test.h>
#include <mbl/mbl_combination.h>
#include <vnl/vnl_random.h>

static inline int mbl_abs(int i) { return i>=0 ? i : -i; }

double dp_cost1(const vnl_matrix<double>& W,
                const vnl_vector<double>& pair_cost,
                const std::vector<unsigned>& x)
{
  unsigned n=x.size();
  double sum=0.0;
  for (unsigned i=0;i<n;++i) sum+=W(i,x[i]);
  for (unsigned i=1;i<n;++i)
    sum+=pair_cost(mbl_abs(int(x[i])-int(x[i-1])));
  return sum;
}

double dp_cost2(const vnl_matrix<double>& W,
                const vnl_vector<double>& pair_cost,
                const std::vector<unsigned>& x)
{
  unsigned n=x.size();
  double sum=0.0;
  for (unsigned i=0;i<n;++i) sum+=W(i,x[i]);
  for (unsigned i=1;i<n;++i)
    sum+=pair_cost(mbl_abs(int(x[i])-int(x[i-1])));
  sum+=pair_cost(mbl_abs(int(x[0])-int(x[n-1])));
  return sum;
}

double global_optima1(const vnl_matrix<double>& W,
                      const vnl_vector<double>& pair_cost,
                      std::vector<int>& x)
{
  unsigned n = W.rows();
  unsigned n_states = W.columns();
  std::vector<unsigned> nd(n,n_states);

  std::vector<unsigned> ux = mbl_combination_begin(nd);
  std::vector<unsigned> best_x=ux;
  double best_cost = dp_cost1(W,pair_cost,ux);
  do
  {
    double cost = dp_cost1(W,pair_cost,ux);
    if (cost<best_cost) { best_cost=cost; best_x=ux; }
  } while (mbl_combination_next(ux,nd));

  x.resize(n);
  for (unsigned i=0;i<n;++i) x[i]=best_x[i];
  return best_cost;
}

double global_optima2(const vnl_matrix<double>& W,
                      const vnl_vector<double>& pair_cost,
                      std::vector<int>& x)
{
  unsigned n = W.rows();
  unsigned n_states = W.columns();
  std::vector<unsigned> nd(n,n_states);

  std::vector<unsigned> ux = mbl_combination_begin(nd);
  std::vector<unsigned> best_x=ux;
  double best_cost = dp_cost2(W,pair_cost,ux);
  do
  {
    double cost = dp_cost2(W,pair_cost,ux);
    if (cost<best_cost) { best_cost=cost; best_x=ux; }
  } while (mbl_combination_next(ux,nd));

  x.resize(n);
  for (unsigned i=0;i<n;++i) x[i]=best_x[i];
  return best_cost;
}


void test_dyn_prog1(unsigned n, unsigned n_states)
{
  std::cout<<n_states<<" states, "<<n<<" variables."<<std::endl;

  // Generate some random data
  vnl_random rand1(473849);
  vnl_matrix<double> W(n,n_states);
  for (unsigned i=0;i<n;++i)
    for (unsigned j=0;j<n_states;++j)
      W(i,j)=rand1.drand64(0,1);

  vnl_vector<double> pair_cost(n_states);
  for (unsigned i=0;i<n_states;++i) pair_cost[i]=0.1*i;

  std::vector<int> x,true_x;

  mbl_dyn_prog dp;
  double min_cost = dp.solve(x,W,pair_cost);

  double true_min=global_optima1(W,pair_cost,true_x);

  TEST_NEAR("Min correct",min_cost,true_min,1e-6);

  for (unsigned i=0;i<n;++i)
  {
    std::cout<<i<<") x="<<x[i]<<std::endl;
    TEST("State correct",x[i],true_x[i]);
  }
}

void test_dyn_prog_loop(unsigned n, unsigned n_states)
{
  std::cout<<n_states<<" states, "<<n<<" variables."<<std::endl;

  // Generate some random data
  vnl_random rand1(473349);
  vnl_matrix<double> W(n,n_states);
  for (unsigned i=0;i<n;++i)
    for (unsigned j=0;j<n_states;++j)
      W(i,j)=rand1.drand64(0,1);

  vnl_vector<double> pair_cost(n_states);
  for (unsigned i=0;i<n_states;++i) pair_cost[i]=0.1*i;

  std::vector<int> x,true_x;

  mbl_dyn_prog dp;
  double min_cost = dp.solve_loop(x,W,pair_cost);

  double true_min=global_optima2(W,pair_cost,true_x);

  TEST_NEAR("Min correct",min_cost,true_min,1e-6);

  for (unsigned i=0;i<n;++i)
  {
    std::cout<<i<<") x="<<x[i]<<std::endl;
    TEST("State correct",x[i],true_x[i]);
  }
}

void test_dyn_prog()
{
  std::cout << "********************\n"
           << " Testing mbl_dyn_prog\n"
           << "********************\n";
  test_dyn_prog1(4,4);
  test_dyn_prog1(5,5);
  test_dyn_prog_loop(4,4);
  test_dyn_prog_loop(5,5);
}

TESTMAIN(test_dyn_prog);
