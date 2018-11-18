#include <iostream>
#include <cstdlib>
#include <sstream>
#include "mmn_dp_solver.h"
//:
// \file
// \brief Solve restricted class of Markov problems (trees and tri-trees)
// \author Tim Cootes

#include <mmn/mmn_order_cost.h>
#include <mmn/mmn_graph_rep1.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <mbl/mbl_parse_block.h>
#include <mbl/mbl_read_props.h>

//: Default constructor
mmn_dp_solver::mmn_dp_solver() = default;

//: Input the arcs that define the graph
void mmn_dp_solver::set_arcs(unsigned num_nodes,
                             const std::vector<mmn_arc>& arcs)
{
  // Copy in arcs, and ensure ordering v1<v2
  std::vector<mmn_arc> ordered_arcs(arcs.size());
  for (unsigned i=0;i<arcs.size();++i)
  {
    if (arcs[i].v1<arcs[i].v2)
      ordered_arcs[i]= arcs[i];
    else
      ordered_arcs[i]= mmn_arc(arcs[i].v2,arcs[i].v1);
  }

  mmn_graph_rep1 graph;
  graph.build(num_nodes,ordered_arcs);
  std::vector<mmn_dependancy> deps;
  if (!graph.compute_dependancies(deps,0))
  {
    std::cerr<<"Graph cannot be decomposed - too complex.\n"
            <<"Arc list: ";
    for (auto arc : arcs) std::cout<<arc;
    std::cerr<<'\n';
    std::abort();
  }

  set_dependancies(deps,num_nodes,graph.max_n_arcs());
}


//: Index of root node
unsigned mmn_dp_solver::root() const
{
  if (deps_.size()==0) return 0;
  return deps_[deps_.size()-1].v1;
}

//: Define dependencies
void mmn_dp_solver::set_dependancies(const std::vector<mmn_dependancy>& deps,
                                     unsigned n_nodes, unsigned max_n_arcs)
{
  deps_ = deps;
  nc_.resize(n_nodes);
  pc_.resize(max_n_arcs);
  index1_.resize(n_nodes);
  index2_.resize(n_nodes);
}

void mmn_dp_solver::process_dep1(const mmn_dependancy& dep)
{
  // dep->v0 depends on dep->v1 through arc dep->arc1
  const vnl_vector<double>& nc0 = nc_[dep.v0];
  vnl_vector<double>& nc1 = nc_[dep.v1];
  vnl_matrix<double>& p = pc_[dep.arc1];
  std::vector<unsigned>& i0 = index1_[dep.v0];

  // Check sizes of matrices
  if (dep.v0<dep.v1)
  {
    assert(p.rows()==nc0.size());
    assert(p.cols()==nc1.size());
  }
  else
  {
    if (p.rows()!=nc1.size())
    {
      std::cerr<<"p.rows()="<<p.rows()<<"p.cols()="<<p.cols()
              <<" nc0.size()="<<nc0.size()
              <<" nc1.size()="<<nc1.size()<<std::endl
              <<"dep: "<<dep<<std::endl;
    }
    assert(p.rows()==nc1.size());
    assert(p.cols()==nc0.size());
  }

  // Set i0[i1] to the optimal choice of node v0 if v1 is i1
  i0.resize(nc1.size());
  for (unsigned j=0;j<nc1.size();++j)
  {
    double min_v;
    unsigned best_i=0;
    if (dep.v0<dep.v1)
    {
      min_v=nc0[0]+p(0,j);
      for (unsigned i=1;i<nc0.size();++i)
      {
        double v=nc0[i]+p(i,j);
        if (v<min_v) { min_v=v; best_i=i; }
      }
    }
    else
    {
      min_v=nc0[0]+p(j,0);
      for (unsigned i=1;i<nc0.size();++i)
      {
        double v=nc0[i]+p(j,i);
        if (v<min_v) { min_v=v; best_i=i; }
      }
    }
    i0[j]=best_i;
    nc1[j]+=min_v;  // Update costs for node v1
  }
}

void mmn_dp_solver::process_dep2(const mmn_dependancy& dep)
{
  // n_dep==2
  // dep->v0 depends on dep->v1 and dep->v2
  // dep->v0 depends on dep->v1 through arc dep->arc1
  const vnl_vector<double>& nc0 = nc_[dep.v0];
  const vnl_vector<double>& nc1 = nc_[dep.v1];
  const vnl_vector<double>& nc2 = nc_[dep.v2];
  const vnl_matrix<double>& pa1 = pc_[dep.arc1];
  const vnl_matrix<double>& pa2 = pc_[dep.arc2];
  vnl_matrix<double>& pa12 = pc_[dep.arc12];
  vnl_matrix<int>& ind0 = index2_[dep.v0];

  if (pa12.size()==0)
  {
    if (dep.v1<dep.v2)
      pa12.set_size(nc1.size(),nc2.size());
    else
      pa12.set_size(nc2.size(),nc1.size());
    pa12.fill(0.0);
  }

  // i0[i1,i2] to the optimal choice of node v0 if v1 is i1, v2 is i2
  ind0.set_size(nc1.size(),nc2.size());

  for (unsigned i1=0;i1<nc1.size();++i1)
  {
    vnl_vector<double> sum0(nc0);
    if (dep.v0<dep.v1) sum0+=pa1.get_column(i1);
    else               sum0+=pa1.get_row(i1);

    for (unsigned i2=0;i2<nc2.size();++i2)
    {
      vnl_vector<double> sum(sum0);
      if (dep.v0<dep.v2) sum+=pa2.get_column(i2);
      else               sum+=pa2.get_row(i2);

      // sum[i] is the cost of choosing i, given (i1,i2)
      // Select minimum
      unsigned best_i=0;
      double min_v=sum[0];
      for (unsigned i=1;i<sum.size();++i)
        if (sum[i]<min_v) { min_v=sum[i]; best_i=i; }

      // Record position of minima
      ind0(i1,i2)=best_i;
      // Update pairwise cost for arc between v1 and v2
      if (dep.v1<dep.v2) { pa12(i1,i2)+=min_v; }
      else               { pa12(i2,i1)+=min_v; }
    }
  }
}


//: Compute optimal choice for dep.v0 given v1 and v2
//  Includes cost depending on (v0,v1,v2) as well as pairwise and
//  node costs.
// tri_cost(i,j,k) is cost of associating smallest node index
// with i, next with j and largest node index with k.
void mmn_dp_solver::process_dep2t(const mmn_dependancy& dep,
                                  const vil_image_view<double>& tri_cost)
{
  // n_dep==2
  // dep->v0 depends on dep->v1 and dep->v2
  // dep->v0 depends on dep->v1 through arc dep->arc1
  const vnl_vector<double>& nc0 = nc_[dep.v0];
  const vnl_vector<double>& nc1 = nc_[dep.v1];
  const vnl_vector<double>& nc2 = nc_[dep.v2];
  const vnl_matrix<double>& pa1 = pc_[dep.arc1];
  const vnl_matrix<double>& pa2 = pc_[dep.arc2];
  vnl_matrix<double>& pa12 = pc_[dep.arc12];
  vnl_matrix<int>& ind0 = index2_[dep.v0];

  // Create a re-ordered view of tri_cost, so we can use tc(i1,i2,i3)
  vil_image_view<double> tc=mmn_unorder_cost(tri_cost,
                                             dep.v0,dep.v1,dep.v2);
  std::ptrdiff_t tc_step0=tc.istep();

  if (pa12.size()==0)
  {
    if (dep.v1<dep.v2)
      pa12.set_size(nc1.size(),nc2.size());
    else
      pa12.set_size(nc2.size(),nc1.size());
    pa12.fill(0.0);
  }

  // i0[i1,i2] to the optimal choice of node v0 if v1 is i1, v2 is i2
  ind0.set_size(nc1.size(),nc2.size());

  for (unsigned i1=0;i1<nc1.size();++i1)
  {
    vnl_vector<double> sum0(nc0);
    if (dep.v0<dep.v1) sum0+=pa1.get_column(i1);
    else               sum0+=pa1.get_row(i1);

    for (unsigned i2=0;i2<nc2.size();++i2)
    {
      vnl_vector<double> sum(sum0);
      if (dep.v0<dep.v2) sum+=pa2.get_column(i2);
      else               sum+=pa2.get_row(i2);

      // sum[i] is the cost of choosing i, given (i1,i2)
      // Select minimum
      unsigned best_i=0;
      const double *tci=&tc(0,i1,i2);
      double min_v=sum[0]+tci[0];
      tci+=tc_step0; // move to element 1
      for (unsigned i=1;i<sum.size();++i,tci+=tc_step0)
      {
        sum[i]+=(*tci);
        if (sum[i]<min_v) { min_v=sum[i]; best_i=i; }
      }

      // Record position of minima
      ind0(i1,i2)=best_i;
      // Update pairwise cost for arc between v1 and v2
      if (dep.v1<dep.v2) { pa12(i1,i2)+=min_v; }
      else               { pa12(i2,i1)+=min_v; }
    }
  }
}


double mmn_dp_solver::solve(
                 const std::vector<vnl_vector<double> >& node_cost,
                 const std::vector<vnl_matrix<double> >& pair_cost,
                 std::vector<unsigned>& x)
{
  nc_ = node_cost;
  for (unsigned i=0;i<pair_cost.size();++i) pc_[i]=pair_cost[i];
  for (unsigned i=pair_cost.size();i<pc_.size();++i) pc_[i].set_size(0,0);

  if (deps_.size()==0)
  {
    std::cerr<<"No dependencies.\n";
    return 999.99;
  }

  // Process dependencies in given order
  std::vector<mmn_dependancy>::const_iterator dep=deps_.begin();
  for (;dep!=deps_.end();dep++)
  {
    if (dep->n_dep==1) process_dep1(*dep);
    else               process_dep2(*dep);
  }

  const vnl_vector<double>& root_cost = nc_[root()];
  unsigned best_i=0;
  double min_v=root_cost[0];
  for (unsigned i=1;i<root_cost.size();++i)
    if (root_cost[i]<min_v) { min_v=root_cost[i]; best_i=i; }

  backtrace(best_i,x);
  return min_v;
}

double mmn_dp_solver::solve(
                 const std::vector<vnl_vector<double> >& node_cost,
                 const std::vector<vnl_matrix<double> >& pair_cost,
                 const std::vector<vil_image_view<double> >& tri_cost,
                 std::vector<unsigned>& x)
{
  nc_ = node_cost;
  for (unsigned i=0;i<pair_cost.size();++i) pc_[i]=pair_cost[i];
  for (unsigned i=pair_cost.size();i<pc_.size();++i) pc_[i].set_size(0,0);

  if (deps_.size()==0)
  {
    std::cerr<<"No dependencies.\n";
    return 999.99;
  }

  // Process dependencies in given order
  std::vector<mmn_dependancy>::const_iterator dep=deps_.begin();
  for (;dep!=deps_.end();dep++)
  {
    if (dep->n_dep==1) process_dep1(*dep);
    else
    {
      if (dep->tri1==mmn_no_tri) process_dep2(*dep);
      else
      {
        // dep->v0 depends on arcs and a triplet relationship
        assert(dep->tri1 < tri_cost.size());
        process_dep2t(*dep,tri_cost[dep->tri1]);
      }
    }
  }

  const vnl_vector<double>& root_cost = nc_[root()];
  unsigned best_i=0;
  double min_v=root_cost[0];
  for (unsigned i=1;i<root_cost.size();++i)
    if (root_cost[i]<min_v) { min_v=root_cost[i]; best_i=i; }

  backtrace(best_i,x);
  return min_v;
}


//: Compute optimal values for x[i] given that root node is root_value
//  Assumes that solve() has been already called.
void mmn_dp_solver::backtrace(unsigned root_value,std::vector<unsigned>& x)
{
  x.resize(nc_.size());
  x[root()]=root_value;

  // Perform backtracing to find optimal solution.
  for (int i=deps_.size()-1; i>=0; --i)
  {
    unsigned v0=deps_[i].v0;
    unsigned v1=deps_[i].v1;
    if (deps_[i].n_dep==1)
       x[v0]=index1_[v0][x[v1]];
    else
    {
      const vnl_matrix<int>& ind0 = index2_[v0];
      x[v0]=ind0(x[v1],x[deps_[i].v2]);
    }
  }
}

//=======================================================================
// Method: set_from_stream
//=======================================================================
//: Initialise from a string stream
bool mmn_dp_solver::set_from_stream(std::istream &is)
{
  // Cycle through stream and produce a map of properties
  std::string s = mbl_parse_block(is);
  std::istringstream ss(s);
  mbl_read_props_type props = mbl_read_props_ws(ss);

  // No properties expected.

  // Check for unused props
  mbl_read_props_look_for_unused_props(
      "mmn_dp_solver::set_from_stream", props, mbl_read_props_type());
  return true;
}


//=======================================================================
// Method: version_no
//=======================================================================

short mmn_dp_solver::version_no() const
{
  return 1;
}

//=======================================================================
// Method: is_a
//=======================================================================

std::string mmn_dp_solver::is_a() const
{
  return std::string("mmn_dp_solver");
}

//: Create a copy on the heap and return base class pointer
mmn_solver* mmn_dp_solver::clone() const
{
  return new mmn_dp_solver(*this);
}

//=======================================================================
// Method: print
//=======================================================================

void mmn_dp_solver::print_summary(std::ostream& /*os*/) const
{
}

//=======================================================================
// Method: save
//=======================================================================

void mmn_dp_solver::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,unsigned(deps_.size()));
  for (const auto & dep : deps_)
    vsl_b_write(bfs,dep);
}

//=======================================================================
// Method: load
//=======================================================================

void mmn_dp_solver::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;
  short version;
  unsigned n;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      vsl_b_read(bfs,n);
      deps_.resize(n);
      for (unsigned i=0;i<n;++i) vsl_b_read(bfs,deps_[i]);
      break;
    default:
      std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&)\n"
               << "           Unknown version number "<< version << std::endl;
      bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}
