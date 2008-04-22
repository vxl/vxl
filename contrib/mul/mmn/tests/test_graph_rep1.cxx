// This is mul/mmn/tests/test_graph_rep1.cxx
#include <testlib/testlib_test.h>
#include <vcl_vector.h>
#include <mmn/mmn_graph_rep1.h>


void test_graph_rep1_a()
{
  vcl_cout<<"==== test mmn_graph_rep1 (chain) ====="<<vcl_endl;

  unsigned n=4;
  // Generate linked list
  vcl_vector<mmn_arc> arc(n);
  for (unsigned i=0;i<n;++i)
    arc[i]=mmn_arc(i,i+1);

  {
    mmn_graph_rep1 graph;
    graph.build(n+1,arc);

    TEST("N.arcs",graph.n_arcs(),n);
    vcl_vector<mmn_dependancy> deps;
    TEST("N.removed",graph.remove_all_leaves(deps),n);
    TEST("No arcs left",graph.n_arcs(),0);
  }

  {
    mmn_graph_rep1 graph;
    graph.build(n+1,arc);

    vcl_vector<mmn_dependancy> deps;
    TEST("Compute dependencies",graph.compute_dependancies(deps),true);
    TEST("No arcs left",graph.n_arcs(),0);
    TEST("Number of deps.",deps.size(),n);
  }

  for (unsigned root_index=0;root_index<=n;++root_index)
  {
    mmn_graph_rep1 graph;
    graph.build(n+1,arc);

    vcl_vector<mmn_dependancy> deps;
    TEST("Compute dependencies",graph.compute_dependancies(deps,root_index),true);
    TEST("No arcs left",graph.n_arcs(),0);
    TEST("Number of deps.",deps.size(),n);
    vcl_cout<<root_index<<") root="<<deps[n-1].v1<<vcl_endl;

    TEST("Root index correct",deps[n-1].v1,root_index);
  }
}

void test_graph_rep1_b(unsigned n)
{
  vcl_cout<<"==== test mmn_graph_rep1 (loop) =====\n"
          <<"N.nodes in loop = "<<n<<vcl_endl;

  // Generate list of arcs
  vcl_vector<mmn_arc> arc(n);
  for (unsigned i=0;i<n;++i)
    arc[i]=mmn_arc(i,(i+1)%n);

  {
    mmn_graph_rep1 graph;
    graph.build(n,arc);

    TEST("N.arcs",graph.n_arcs(),n);
    vcl_vector<mmn_dependancy> deps;
    TEST("N.removed",graph.remove_all_leaves(deps),0);
  }

  {
    mmn_graph_rep1 graph;
    graph.build(n,arc);

    vcl_vector<mmn_dependancy> deps;
    TEST("Compute dependencies",graph.compute_dependancies(deps),true);
    TEST("No arcs left",graph.n_arcs(),0);
    TEST("Number of deps.",deps.size(),n-1);

    vcl_cout<<"Dependency list: "<<vcl_endl;
    for (unsigned i=0;i<deps.size();++i) vcl_cout<<i<<')'<<deps[i]<<vcl_endl;
  }

  for (unsigned root_index=0;root_index<n;++root_index)
  {
    mmn_graph_rep1 graph;
    graph.build(n,arc);

    vcl_vector<mmn_dependancy> deps;
    TEST("Compute dependencies",graph.compute_dependancies(deps,root_index),true);
    TEST("No arcs left",graph.n_arcs(),0);
    TEST("Number of deps.",deps.size(),n-1);
    vcl_cout<<root_index<<") root="<<deps[n-2].v1<<vcl_endl;
    vcl_cout<<"Last dep="<<deps[n-2]<<vcl_endl;

    TEST("Root index correct",deps[n-2].v1,root_index);
  }

}


void test_graph_rep1()
{
  test_graph_rep1_a();
  test_graph_rep1_b(3);
  test_graph_rep1_b(5);
}

TESTMAIN(test_graph_rep1);
