// This is gel/pop/pop_manager.h
#ifndef pop_manager_h_
#define pop_manager_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief this class keeps track of all parameterized objects.
//
// \author
//            Peter Tu April 2003
//            General Electric
//
//-------------------------------------------------------------------------------
#include <pop/pop_parameter.h>
#include <pop/pop_object.h>
#include <vcl_vector.h>
#include <vcl_list.h>
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
  vcl_vector<pop_parameter*> new_parameters(int num_param);

  //: add a new parameter object
  void add_object(pop_object *obj);

  //: update all the objects
  void update();

  //: get a vector of changeable parameters
  vcl_vector<pop_parameter*> get_changeable_parameters();

  //: optimize the parameters using Levenberg Marquardt
  void optimize(vcl_vector<pop_geometric_cost_function*> &obs_costs);

 private:
  vcl_list<pop_parameter*> params_;
  vcl_list<pop_object*> objects_;
};

#endif // pop_manager_h_
