// This is gel/pop/pop_object.h
#ifndef pop_object_h_
#define pop_object_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief an object that has parameters
//
// \author
//            Peter Tu April 2003
//            General Electric
//
//-------------------------------------------------------------------------------
#include<vcl_vector.h>
#include<pop/pop_parameter.h>

//: A base class for object that has parameters
class pop_object
{
 public:
  //: constructor
  pop_object();

  pop_object(vcl_vector<pop_parameter*> &params);

  //: destructor
  virtual ~pop_object();

  //: get the parameters
  vcl_vector<pop_parameter*> get_parameters();

  //: set the parameters
  void set(vcl_vector<pop_parameter*> &params);

  //: update the object based on the parameters
  virtual void update();

 protected:

  //: the basic parameters
  vcl_vector<pop_parameter*> params_;
};

#endif // pop_object_h_
