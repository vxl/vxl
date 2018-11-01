// This is gel/pop/pop_manager.h
#ifndef pop_manager_h_
#define pop_manager_h_
//:
// \file
// \brief this class keeps track of all parameterized objects.
//
// \author
//            Peter Tu April 2003
//            General Electric
//
//-------------------------------------------------------------------------------
#include <vector>
#include <iostream>
#include <list>
#include <pop/pop_parameter.h>
#include <pop/pop_object.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <pop/pop_geometric_cost_function.h>

//: A class for keeping track of parameterized objects
class pop_manager
{
 public:
  //: constructor
  pop_manager();

  //: destructor
  ~pop_manager();

  //: create a new parameter
  pop_parameter* new_parameter();

  //: create a vector of parameters
  std::vector<pop_parameter*> new_parameters(int num_param);

  //: add a new parameter object
  void add_object(pop_object *obj);

  //: update all the objects
  void update();

  //: get a vector of changeable parameters
  std::vector<pop_parameter*> get_changeable_parameters();

  //: optimize the parameters using Levenberg Marquardt
  void optimize(std::vector<pop_geometric_cost_function*> &obs_costs);

 private:
  std::list<pop_parameter*> params_;
  std::list<pop_object*> objects_;
};

#endif // pop_manager_h_
