#include<pop/pop_transform.h>


//: constructor
pop_transform::pop_transform(vcl_vector<pop_parameter*> params,
							 pop_vertex *cs1, pop_vertex *cs2):
  pop_object(params)
{
  cs1_ = cs1;
  cs2_ = cs2;
}
  

//: destructor
pop_transform::~pop_transform()
{

}


