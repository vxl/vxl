//:
// \file
// \brief Find choice of values at each node which minimises Markov problem
// \author Tim Cootes

#include <mmn/mmn_dp_solver.h>

  //: Default constructor
mmn_dp_solver::mmn_dp_solver()
{
}

//: Index of root node
unsigned mmn_dp_solver::root() const
{
  if (deps_.size()==0) return 0;
  return deps_[deps_.size()-1].v1;
}

//: Define dependancies
void mmn_dp_solver::set_dependancies(const vcl_vector<mmn_dependancy>& deps,
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
  vcl_vector<unsigned>& i0 = index1_[dep.v0];

  // Check sizes of matrices
  if (dep.v0<dep.v1)
  {
    assert(p.rows()==nc0.size());
    assert(p.cols()==nc1.size());
  }
  else
  {
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

double mmn_dp_solver::solve(const vcl_vector<vnl_vector<double> >& node_cost,
                 const vcl_vector<vnl_matrix<double> >& pair_cost,
                 vcl_vector<unsigned>& x)
{
  nc_ = node_cost;
  for (unsigned i=0;i<pair_cost.size();++i) pc_[i]=pair_cost[i];
  for (unsigned i=pair_cost.size();i<pc_.size();++i) pc_[i].set_size(0,0);

  if (deps_.size()==0)
  {
    vcl_cerr<<"No dependancies."<<vcl_endl;
    return 999.99;
  }

  // Process dependancies in given order
  vcl_vector<mmn_dependancy>::const_iterator dep=deps_.begin();
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

//: Compute optimal values for x[i] given that root node is root_value
//  Assumes that solve() has been already called.
void mmn_dp_solver::backtrace(unsigned root_value,vcl_vector<unsigned>& x)
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



