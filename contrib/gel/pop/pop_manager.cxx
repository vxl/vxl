// This is gel/pop/pop_manager.cxx
#include "pop_manager.h"
//:
// \file
#include <vnl/algo/vnl_levenberg_marquardt.h>
#include <pop/pop_graph_cost_function.h>
#include <vcl_iostream.h>


//: constructor
pop_manager::pop_manager()
{
}


//: destructor
pop_manager::~pop_manager()
{
}


//: create a new parameter - only the manager should create new parameters
pop_parameter* pop_manager::new_parameter()
{
  pop_parameter *p = new pop_parameter();
  params_.push_back(p);
  return p;
}

//: create a vector of parameters
vcl_vector<pop_parameter*> pop_manager::new_parameters(int num_param)
{
  vcl_vector<pop_parameter*> params(num_param);
  for (int i=0;i<num_param;i++)
    params[i] = this->new_parameter();
  return params;
}

//: add a new parameter object
void pop_manager::add_object(pop_object *obj)
{
   objects_.push_back(obj);
}


//: update all the objects
void pop_manager::update()
{
  // call update on all known objects
  for (vcl_list<pop_object*>::iterator it=objects_.begin();it!=objects_.end();++it)
    (*it)->update();
}


//: get a vector of changeable parameters
vcl_vector<pop_parameter*> pop_manager::get_changeable_parameters()
{
  // first find all changeable parameters
  vcl_list<pop_parameter*> cp;

  for (vcl_list<pop_parameter*>::iterator it=params_.begin();it!=params_.end();++it) {
    if ((*it)->is_changeable_) {
      cp.push_back(*it);
    }
  }
  // now make a vector
  vcl_vector<pop_parameter*> v(cp.size());
  int i=0;
  for (vcl_list<pop_parameter*>::iterator it=cp.begin();it!=cp.end();++i,++it)
    v[i] = *it;

  return v;
}


//: optimize the parameters using Levenberg Marquardt
void pop_manager::optimize(vcl_vector<pop_geometric_cost_function*> &obs_costs)
{
  // step 1 make a cost function
  vcl_vector<pop_parameter*> cp = this->get_changeable_parameters();
  pop_graph_cost_function cf(cp,obs_costs,this);

  vcl_cout << "The initial costs are\n";
  vnl_vector<double> costs = cf.get_current_costs();
  vcl_cout << costs << vcl_endl;

  // step 2 make a lm optimizer
  vnl_levenberg_marquardt lm(cf);

  // lm.set_trace(true);
  // lm.set_verbose(true);

  // step 3 start the process
  vnl_vector<double> params = cf.get_parameter_values();
  bool flag = lm.minimize_without_gradient(params);

  vcl_cout << "The final costs are\n";
  costs = cf.get_current_costs();
  vcl_cout << costs << vcl_endl;

  if (!flag) {
    vcl_cout << "warning minimization routine did not converge\n";
  }
}

