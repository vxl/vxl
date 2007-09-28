// This is mul/mmn/tests/test_dp_solver.cxx
#include <testlib/testlib_test.h>
#include <vcl_vector.h>
#include <mmn/mmn_graph_rep1.h>
#include <mmn/mmn_dp_solver.h>


void test_dp_solver_a()
{
  vcl_cout<<"==== test test_dp_solver (chain) ====="<<vcl_endl;

  unsigned n=5;
  // Generate linked list
  vcl_vector<mmn_arc> arc(n-1);
  for (unsigned i=0;i<n-1;++i)
    arc[i]=mmn_arc(i,i+1);

  mmn_graph_rep1 graph;
  graph.build(n,arc);
  vcl_vector<mmn_dependancy> deps;
  graph.compute_dependancies(deps);

  vcl_vector<vnl_vector<double> > node_cost(n);
  vcl_vector<vnl_matrix<double> > pair_cost(arc.size());

  vcl_cout<<"Set up trivial problem. Optimal node=i, pair_costs all flat"<<vcl_endl;
  for (unsigned i=0;i<n;++i)
  {
    node_cost[i].set_size(5+i);
    node_cost[i].fill(10);
    node_cost[i][i]=0;
  }

  for (unsigned a=0;a<arc.size();++a)
  {
    unsigned v1=arc[a].min_v();
    unsigned v2=arc[a].max_v();
    pair_cost[a].set_size(node_cost[v1].size(), node_cost[v2].size());
    pair_cost[a].fill(1.0);
  }

  mmn_dp_solver solver;
  solver.set_dependancies(deps,n,graph.max_n_arcs());

  vcl_cout<<"Run solver."<<vcl_endl;

  vcl_vector<unsigned> x;
  double min_cost = solver.solve(node_cost,pair_cost,x);

  TEST_NEAR("Optimum value",min_cost,n-1,1e-6);
  TEST("Correct number of nodes",x.size(),n);
  for (unsigned i=0;i<n;++i)
  {
    TEST("Correct node value",x[i],i);
    vcl_cout<<"x["<<i<<"]="<<x[i]<<vcl_endl;
  }
}

void test_dp_solver_b()
{
  vcl_cout<<"==== test test_dp_solver (reversed chain) ====="<<vcl_endl;

  unsigned n=5;
  // Generate linked list
  vcl_vector<mmn_arc> arc(n-1);
  for (unsigned i=0;i<n-1;++i)
    arc[i]=mmn_arc(i+1,i);

  mmn_graph_rep1 graph;
  graph.build(n,arc);
  vcl_vector<mmn_dependancy> deps;
  graph.compute_dependancies(deps);

  vcl_vector<vnl_vector<double> > node_cost(n);
  vcl_vector<vnl_matrix<double> > pair_cost(arc.size());

  vcl_cout<<"Set up trivial problem. Optimal node=i, pair_costs all flat"<<vcl_endl;
  for (unsigned i=0;i<n;++i)
  {
    node_cost[i].set_size(5+i);
    node_cost[i].fill(10);
    node_cost[i][i]=0;
  }

  for (unsigned a=0;a<arc.size();++a)
  {
    unsigned v1=arc[a].min_v();
    unsigned v2=arc[a].max_v();
    pair_cost[a].set_size(node_cost[v1].size(), node_cost[v2].size());
    pair_cost[a].fill(1.0);
  }

  mmn_dp_solver solver;
  solver.set_dependancies(deps,n,graph.max_n_arcs());

  vcl_cout<<"Run solver."<<vcl_endl;

  vcl_vector<unsigned> x;
  double min_cost = solver.solve(node_cost,pair_cost,x);

  TEST_NEAR("Optimum value",min_cost,n-1,1e-6);
  TEST("Correct number of nodes",x.size(),n);
  for (unsigned i=0;i<n;++i)
  {
    TEST("Correct node value",x[i],i);
    vcl_cout<<"x["<<i<<"]="<<x[i]<<vcl_endl;
  }
}

void test_dp_solver_loop_a(unsigned n)
{
  vcl_cout << "==== test test_dp_solver (loop) =====\n"
           << n << " nodes." << vcl_endl;

  // Generate arcs
  vcl_vector<mmn_arc> arc(n);
  for (unsigned i=0;i<n;++i)
    arc[i]=mmn_arc(i,(i+1)%n);

  mmn_graph_rep1 graph;
  graph.build(n,arc);
  vcl_vector<mmn_dependancy> deps;
  graph.compute_dependancies(deps);

  vcl_vector<vnl_vector<double> > node_cost(n);
  vcl_vector<vnl_matrix<double> > pair_cost(arc.size());

  vcl_cout<<"Set up trivial problem. Optimal node=i, pair_costs all flat"<<vcl_endl;
  for (unsigned i=0;i<n;++i)
  {
    node_cost[i].set_size(5+i);
    node_cost[i].fill(10);
    node_cost[i][i]=0;
  }

  for (unsigned a=0;a<arc.size();++a)
  {
    unsigned v1=arc[a].min_v();
    unsigned v2=arc[a].max_v();
    pair_cost[a].set_size(node_cost[v1].size(), node_cost[v2].size());
    pair_cost[a].fill(1.0);
  }

  mmn_dp_solver solver;
  solver.set_dependancies(deps,n,graph.max_n_arcs());

  vcl_cout<<"Run solver."<<vcl_endl;

  vcl_vector<unsigned> x;
  double min_cost = solver.solve(node_cost,pair_cost,x);

  TEST_NEAR("Optimum value",min_cost,n,1e-6);
  TEST("Correct number of nodes",x.size(),n);
  for (unsigned i=0;i<n;++i)
  {
    TEST("Correct node value",x[i],i);
    vcl_cout<<"x["<<i<<"]="<<x[i]<<vcl_endl;
  }
}

void test_dp_solver_loop_b(unsigned n)
{
  vcl_cout<<"==== test test_dp_solver (loop) ====="<<vcl_endl;

  // Generate arcs
  vcl_vector<mmn_arc> arc(n);
  for (unsigned i=0;i<n;++i)
    arc[i]=mmn_arc(i,(i+1)%n);

  mmn_graph_rep1 graph;
  graph.build(n,arc);
  vcl_vector<mmn_dependancy> deps;
  graph.compute_dependancies(deps);

  vcl_vector<vnl_vector<double> > node_cost(n);
  vcl_vector<vnl_matrix<double> > pair_cost(arc.size());

  vcl_cout << "Set up trivial problem. Optimal node=i\n"
           << "node_costs all flat"<<vcl_endl;
  for (unsigned i=0;i<n;++i)
  {
    node_cost[i].set_size(5+i);
    node_cost[i].fill(0);
  }

  for (unsigned a=0;a<arc.size();++a)
  {
    unsigned v1=arc[a].min_v();
    unsigned v2=arc[a].max_v();
    pair_cost[a].set_size(node_cost[v1].size(), node_cost[v2].size());
    pair_cost[a].fill(10.0);
    pair_cost[a](v1,v2)=1.0;
  }

  mmn_dp_solver solver;
  solver.set_dependancies(deps,n,graph.max_n_arcs());

  vcl_cout<<"Run solver."<<vcl_endl;

  vcl_vector<unsigned> x;
  double min_cost = solver.solve(node_cost,pair_cost,x);

  TEST_NEAR("Optimum value",min_cost,n,1e-6);
  TEST("Correct number of nodes",x.size(),n);
  for (unsigned i=0;i<n;++i)
  {
    TEST("Correct node value",x[i],i);
    vcl_cout<<"x["<<i<<"]="<<x[i]<<vcl_endl;
  }
}

void test_dp_solver()
{
  test_dp_solver_a();
  test_dp_solver_b();
  test_dp_solver_loop_a(3);
  test_dp_solver_loop_a(4);
  test_dp_solver_loop_a(5);
  test_dp_solver_loop_b(4);
}

TESTMAIN(test_dp_solver);
