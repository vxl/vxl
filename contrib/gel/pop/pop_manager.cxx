#include <pop/pop_manager.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>
#include <pop/pop_graph_cost_function.h>

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

//: create a vector of paramters 
vcl_vector<pop_parameter*> pop_manager::new_parameters(int num_param)
{
  vcl_vector<pop_parameter*> params(num_param);
  int i;
  for(i=0;i<num_param;i++){
    params[i] = this->new_parameter();
  }
  return params;
}

//: add a new parameter object
void pop_manager::add_object(pop_object *obj)
{
   objects_.push_back(obj);
}


//: update all the objets 
void pop_manager::update()
{
  // call update on all known objects
  vcl_list<pop_object*>::iterator it;
  for(it=objects_.begin();it!=objects_.end();it++){
    (*it)->update();
  }
}


//: get a vector of changeble parameters 
vcl_vector<pop_parameter*> pop_manager::get_changeable_parameters()
{
  // first find all changeble parameters 
  int num_params =0;
  
  vcl_list<pop_parameter*> cp;

  vcl_list<pop_parameter*>::iterator it;
  for(it=params_.begin();it!=params_.end();it++){
    if((*it)->is_changeable_){
      cp.push_back(*it);
    }
  }
  // now make a vector
  vcl_vector<pop_parameter*> v(cp.size());
		       
  int i;
  for(i=0,it=cp.begin();it!=cp.end();i++,it++){
    v[i] = *it;
  }
  
  return v;
}


//: optimize the parameters using Levenberg Marquardt
void pop_manager::optimize(vcl_vector<pop_geometric_cost_function*> &obs_costs)
{
  // step 1 make a cost function
  pop_graph_cost_function cf(this->get_changeable_parameters(),obs_costs,this);

  vcl_cout << "The initial costs are" << vcl_endl;
  vnl_vector<double> costs = cf.get_current_costs();
  vcl_cout << costs << vcl_endl;


  // step 2 make a lm optimizer
  vnl_levenberg_marquardt lm(cf);
  
  // lm.set_trace(true);
  // lm.set_verbose(true);


  // step 3 start the process
  vnl_vector<double> params = cf.get_parameter_values();
  bool flag = lm.minimize_without_gradient(params);
  
  
  vcl_cout << "The final costs are" << vcl_endl;
  costs = cf.get_current_costs();
  vcl_cout << costs << vcl_endl;


  if(!flag){
    vcl_cout << "warning minimization routine did not converge " << vcl_endl;
  }

}

  
