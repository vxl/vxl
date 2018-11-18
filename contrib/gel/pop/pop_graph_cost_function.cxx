// This is gel/pop/pop_graph_cost_function.cxx
#include "pop_graph_cost_function.h"
//:
// \file

//: constructor
//  We send in the parameters that can change,
//  the residuals that would be evaluated and the pop manager
//  that is responsible for updating the graph based on the
//  current parameter values.
pop_graph_cost_function::pop_graph_cost_function(std::vector<pop_parameter*> &params,
                                                 std::vector<pop_geometric_cost_function*> &cfs,
                                                 pop_manager *manager):
  vnl_least_squares_function(params.size(),cfs.size(),no_gradient)
{
  params_ = params;
  cfs_ = cfs;
  manager_ = manager;
}


//: destructor
pop_graph_cost_function::~pop_graph_cost_function()
{
}


//: the method for evaluating the cost of the parameter values (x) and getting the residuals (fx)
void pop_graph_cost_function::f(vnl_vector<double> const &x, vnl_vector<double> &fx)
{
  // step 1: update the parameters
  for (int i=0;i<params_.size();i++) {
    params_[i]->value_ = x[i];
  }

  // step 2: update the graph
  manager_->update();

  // step 3: collect the residuals
  for (int i=0;i<cfs_.size();i++) {
    fx(i) = cfs_[i]->cost();
  }
}


//: get the current parameter value
vnl_vector<double> pop_graph_cost_function::get_parameter_values()
{
  vnl_vector<double> x(params_.size());
  for (int i=0;i<params_.size();i++)
    x[i] = params_[i]->value_;
  return x;
}

//: get the current costs
vnl_vector<double> pop_graph_cost_function::get_current_costs()
{
  vnl_vector<double> costs(cfs_.size());
  vnl_vector<double> params = this->get_parameter_values();

  this->f(params,costs);

  return costs;
}
