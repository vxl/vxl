// This is gel/pop/pop_geometric_cost_function.h
#ifndef pop_geometric_cost_function_h_
#define pop_geometric_cost_function_h_
//:
// \file
// \brief compute cost between an observable and an observation
//
// \author
//            Peter Tu April 2003
//            General Electric
//
//-------------------------------------------------------------------------------

#include<pop/pop_geometric_object.h>

//: base class for geometric cost methods
class pop_geometric_cost_function
{
 public:
  //: constructor
  pop_geometric_cost_function(pop_geometric_object *observable,
                              pop_geometric_object *observation);

  //: destructor
  ~pop_geometric_cost_function();

  //: this is the cost between two objects

  double cost();

 private:
  // the observable
  pop_geometric_object *observable_;

  // the observation
  pop_geometric_object *observation_;
};

#endif // pop_geometric_cost_function_h_
