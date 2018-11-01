// This is gel/pop/pop_parameter.h
#ifndef pop_parameter_h_
#define pop_parameter_h_
//:
// \file
// \brief a parameter that can be optimized
//
// \author
//            Peter Tu April 2003
//            General Electric
//
//-------------------------------------------------------------------------------

//: A parameter that can be optimized
class pop_parameter
{
 public:
  //: constructor
  pop_parameter();

  //: destructor
  ~pop_parameter();

  //: the value of the parameter - made public for ease of access
  double value_;

  //: the expected confidence of the value
  double std_;

  //: whether or not the value is fixed or changeable
  bool is_changeable_;
};

#endif // pop_parameter_h_
