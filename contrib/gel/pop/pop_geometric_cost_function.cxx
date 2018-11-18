// This is gel/pop/pop_geometric_cost_function.cxx
#include "pop_geometric_cost_function.h"
//:
// \file
#include<std::iostream.h>

//: constructor
pop_geometric_cost_function::pop_geometric_cost_function(pop_geometric_object *observable,
                                                         pop_geometric_object *observation)
{
  observable_ = observable;
  observation_ = observation;
}

//: destructor
pop_geometric_cost_function::~pop_geometric_cost_function()
{
}

//: this is the cost between two objects

double pop_geometric_cost_function::cost()
{
  // we assume that there is a path from the observable to the observation
  pop_geometric_object *expected_obj = observable_->transform(observation_->coordinate_system_);

  if (expected_obj) {
    double co = observation_->cost(expected_obj);
    delete expected_obj;
    return co;
  }
  else {
    std::cout << "Warning: there is no mapping from the observable to the observation\n";
    return 0;
  }
}
