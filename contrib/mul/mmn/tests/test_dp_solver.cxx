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

void test_dp_solver_tri()
{
  vcl_cout<<"==== test test_dp_solver (triplet) ====="<<vcl_endl;

  // Create 3 nodes, each with 6 options
  // All pairwise relationships
  // One triplet relationship
  vcl_vector<vnl_vector<double> > node_cost(3);
  vcl_vector<vnl_matrix<double> > pair_cost(3); // 01,02,12
  vcl_vector<vil_image_view<double> > tri_cost(1);
  for (unsigned i=0;i<3;++i)
  {
    node_cost[i].set_size(6); node_cost[i].fill(1.0);
    pair_cost[i].set_size(6,6); pair_cost[i].fill(1.0);
  }
  tri_cost[0].set_size(6,6,6);
  tri_cost[0].fill(1.0);
  tri_cost[0](2,3,4)=0.0;  // The optimal solution

  // Set up dependancies
  vcl_vector<mmn_dependancy> deps(2);
  deps[0] = mmn_dependancy(0,1,2, 0,1,2, 0); // 0 depends on 1 & 2
  deps[1] = mmn_dependancy(1,2, 2);  // 1 depends on 2 through arc 2

  mmn_dp_solver dp_solver;
  dp_solver.set_dependancies(deps,3,3);

  vcl_vector<unsigned> x;
  double fit = dp_solver.solve(node_cost,pair_cost,tri_cost,x);

  TEST("Size of x",x.size(),3);
  vcl_cout<<"x: "<<x[0]<<','<<x[1]<<','<<x[2]<<vcl_endl;
  TEST("Node 0 = 2",x[0],2);
  TEST("Node 1 = 3",x[1],3);
  TEST("Node 2 = 4",x[2],4);
  TEST_NEAR("Best Fit",fit,6.0,1e-6);
}

void test_dp_solver()
{
  test_dp_solver_a();
  test_dp_solver_b();
  test_dp_solver_loop_a(3);
  test_dp_solver_loop_a(4);
  test_dp_solver_loop_a(5);
  test_dp_solver_loop_b(4);
  test_dp_solver_tri();
}

TESTMAIN(test_dp_solver);
