#include <cstdlib>
#include <iostream>
#include <algorithm>
#include "mbl_dyn_prog.h"
//:
// \file

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_indent.h>
#include <vsl/vsl_binary_io.h>
#include <vnl/io/vnl_io_matrix.h>

//=======================================================================

//=======================================================================
// Dflt ctor
//=======================================================================

mbl_dyn_prog::mbl_dyn_prog() = default;

//=======================================================================
// Destructor
//=======================================================================

mbl_dyn_prog::~mbl_dyn_prog() = default;


//: Construct path from links_, assuming it ends at end_state
void mbl_dyn_prog::construct_path(std::vector<int>& x, int end_state)
{
  unsigned int n = links_.rows();
  int ** b_data = links_.get_rows()-1;  // So that b_data[i] corresponds to i-th row
  if (x.size()!=n+1) x.resize(n+1);
  int *x_data = &x[0];
  x_data[n] = end_state;
  for (unsigned int i=n; i>0; --i)
    x_data[i-1] = b_data[i][x_data[i]];
}

static inline int mbl_abs(int i) { return i>=0 ? i : -i; }

//=======================================================================
//: Compute running costs for DP problem with costs W
//  Pair cost term:  C_i(x1,x2) = c(|x1-x2|)
//  Size of c indicates maximum displacement between neighbouring
//  states.
//  If first_state>=0 then the first is constrained to that index value
void mbl_dyn_prog::running_costs(
               const vnl_matrix<double>& W,
               const vnl_vector<double>& pair_cost,
               int first_state)
{
  int n = W.rows();
  int n_states = W.columns();
  const double * const* W_data = W.data_array();
  int max_d = pair_cost.size()-1;

   // On completion b(i,j) shows the best prior state (ie at i)
   // leading to state j at time i+1
  links_.resize(n-1,n_states);
  int ** b_data = links_.get_rows()-1;
     // So that b_data[i] corresponds to i-th row

    // ci(j) is total cost to get to current state j
  running_cost_ = W.get_row(0);
  double *ci = running_cost_.data_block();
  next_cost_.set_size(n_states);
  double *ci_new = next_cost_.data_block();

  for (int i=1;i<n;++i)
  {
    int *bi = b_data[i];
    const double *wi = W_data[i];

    for (int j=0;j<n_states;++j)
    {
      // Evaluate best route to get to state j at time i
      int k_best = 0;
      double cost;
      double wj = wi[j];
      double cost_best;

      if (i==1 && first_state>=0)
      {
        // Special case: First point pinned down to first_pt
        k_best = first_state;
        int d = mbl_abs(j-k_best);
        if (d>max_d) cost_best=9e9;
        else
                     cost_best = ci[k_best] + pair_cost[d]+ wj;
      }
      else
      {
        int klo = std::max(0,j-max_d);
        int khi = std::min(n_states-1,j+max_d);
        k_best=klo;
        cost_best = ci[klo] + pair_cost[mbl_abs(j-klo)] + wj;
        for (int k=klo+1;k<=khi;++k)
        {
          cost = ci[k] + pair_cost[mbl_abs(j-k)] + wj;
          if (cost<cost_best)
          {
            cost_best=cost;
            k_best = k;
          }
        }
      }

      ci_new[j] = cost_best;
      bi[j] = k_best;
    }

    running_cost_=next_cost_;
  }
}

//=======================================================================
//: Solve the dynamic programming problem with costs W
//  Pair cost term:  C_i(x1,x2) = c(|x1-x2|)
//  Size of c indicates maximum displacement between neighbouring
//  states.
//  If first_state>=0 then the first is constrained to that index value
// \retval x  Optimal path
// \return Total cost of given path
double mbl_dyn_prog::solve(std::vector<int>& x,
                           const vnl_matrix<double>& W,
                           const vnl_vector<double>& pair_cost,
                           int first_state)
{
  running_costs(W,pair_cost,first_state);

  double *ci = running_cost_.data_block();
  int n_states = W.columns();

  // Find the best final cost
  int best_j = 0;
  double best_cost = ci[0];
  for (int j=1;j<n_states;++j)
  {
    if (ci[j]<best_cost) { best_j=j; best_cost=ci[j]; }
  }

  construct_path(x,best_j);

  return best_cost;
}

//: Solve the DP problem including constraint between first and last
//  Cost of moving from state i to state j is move_cost[j-i]
//  (move_cost[i] must be valid for i in range [1-n_states,n_states-1])
// Includes cost between x[0] and x[n-1] to ensure loop closure.
// \retval x  Optimal path
// \return Total cost of given path
double mbl_dyn_prog::solve_loop(std::vector<int>& x,
                                const vnl_matrix<double>& W,
                                const vnl_vector<double>& pair_cost)
{
  int n_states = W.columns();
  int max_d = pair_cost.size()-1;

  double best_overall_cost=9.9e9;

  std::vector<int> x1;
  for (int i0=0;i0<n_states;++i0)
  {
    // Solve with constraint that first is i0
    running_costs(W,pair_cost,i0);

    double *ci = running_cost_.data_block();
    // Find the best final cost
    int klo = std::max(0,i0-max_d);
    int khi = std::min(n_states-1,i0+max_d);
    int k_best=klo;
    double best_cost = ci[klo] + pair_cost[mbl_abs(i0-klo)];
    for (int k=klo+1;k<=khi;++k)
    {
      double cost = ci[k] + pair_cost[mbl_abs(i0-k)];
      if (cost<best_cost) { best_cost=cost; k_best = k; }
    }

    if (best_cost<best_overall_cost)
    {
      best_overall_cost=best_cost;
      construct_path(x,k_best);
    }
  }

  return best_overall_cost;
}

//=======================================================================
// Method: version_no
//=======================================================================

short mbl_dyn_prog::version_no() const
{
  return 1;
}

//=======================================================================
// Method: is_a
//=======================================================================

std::string mbl_dyn_prog::is_a() const
{
  return std::string("mbl_dyn_prog");
}

//=======================================================================
// Method: print_summary
//=======================================================================

// required if data is present in this class
void mbl_dyn_prog::print_summary(std::ostream& os) const
{
  os << is_a();
}

//=======================================================================
// Method: save
//=======================================================================

  // required if data is present in this class
void mbl_dyn_prog::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,is_a());
  vsl_b_write(bfs,version_no());
}

//=======================================================================
// Method: load
//=======================================================================

  // required if data is present in this class
void mbl_dyn_prog::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  std::string name;
  vsl_b_read(bfs,name);
  if (name != is_a())
  {
    std::cerr << "DerivedClass::load :"
             << " Attempted to load object of type "
             << name <<" into object of type " << is_a() << std::endl;
    std::abort();
  }

  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      // vsl_b_read(bfs,data_); // example of data input
      break;
    default:
      std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, mbl_dyn_prog &)\n"
               << "           Unknown version number "<< version << std::endl;
      bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

void vsl_b_write(vsl_b_ostream& bfs, const mbl_dyn_prog& b)
{
    b.b_write(bfs);
}

//=======================================================================
// Associated function: operator>>
//=======================================================================

void vsl_b_read(vsl_b_istream& bfs, mbl_dyn_prog& b)
{
    b.b_read(bfs);
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

std::ostream& operator<<(std::ostream& os,const mbl_dyn_prog& b)
{
  os << b.is_a() << ": ";
  vsl_indent_inc(os);
  b.print_summary(os);
  vsl_indent_dec(os);
  return os;
}
