// This is gel/pop/pop_graph_cost_function.h
#ifndef pop_graph_cost_function_h_
#define pop_graph_cost_function_h_
//:
// \file
// \brief this is a vertex/coordinate system in a coordinate system graph
//
// \author
//            Peter Tu April 2003
//            General Electric
//
//-------------------------------------------------------------------------------
#include <iostream>
#include <vector>
#include <vnl/vnl_least_squares_function.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <pop/pop_parameter.h>
#include <pop/pop_geometric_cost_function.h>
#include <pop/pop_manager.h>

//: A parameter that can be optimized
class pop_graph_cost_function : public vnl_least_squares_function
{
 public:
  //: constructor
  //  We send in the parameters that can change,
  //  the residuals that would be evaluated and the pop manager
  //  that is responsible for updating the graph based on the
  //  current parameter values.
  pop_graph_cost_function(std::vector<pop_parameter*> &params,
                          std::vector<pop_geometric_cost_function*> &cfs,
                          pop_manager *manager);

  //: destructor
  ~pop_graph_cost_function();

  //: the method for evaluating the cost of the parameter values (x) and getting the residuals (fx)
  virtual void f(vnl_vector<double> const &x, vnl_vector<double> &fx);

  //: get the current parameter values
  vnl_vector<double> get_parameter_values();

  //: get the current costs
  vnl_vector<double> get_current_costs();

 private:
  std::vector<pop_parameter*> params_;
  std::vector<pop_geometric_cost_function*> cfs_;
  pop_manager *manager_;
};

#endif // pop_graph_cost_function_h_
