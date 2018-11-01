// This is gel/pop/pop_object.h
#ifndef pop_object_h_
#define pop_object_h_
//:
// \file
// \brief an object that has parameters
//
// \author
//            Peter Tu April 2003
//            General Electric
//
// \verbatim
//  Modifications
//   10 Sep. 2004 Peter Vanroose  Inlined all 1-line methods in class decl
// \endverbatim
//
//-------------------------------------------------------------------------------
#include<std::vector.h>
#include<pop/pop_parameter.h>

//: A base class for object that has parameters
class pop_object
{
 public:
  // constructor
  pop_object() {}

  pop_object(std::vector<pop_parameter*> &params) { params_ = params; }

  // destructor
  virtual ~pop_object() {}

  //: get the parameters
  std::vector<pop_parameter*> get_parameters() { return params_; }

  //: set the parameters
  void set(std::vector<pop_parameter*> &params) { params_ = params; update(); }

  //: update the object based on the parameters
  virtual void update();

 protected:

  //: the basic parameters
  std::vector<pop_parameter*> params_;
};


#endif // pop_object_h_
