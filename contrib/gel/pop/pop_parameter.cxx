#include <pop/pop_parameter.h>

pop_parameter::pop_parameter()
{
  // assume changeable
  is_changeable_ = true;
  
  // default values
  value_ = 0;
  std_ = 1.0;
}

//: destructor
pop_parameter::~pop_parameter()
{

}
