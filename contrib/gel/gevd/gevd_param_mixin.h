#ifndef gevd_param_mixin_h_
#define gevd_param_mixin_h_
//=======================================================================
//:
// \file
// \brief A mixin to package algorithm parameters
//
//   The base class for blocks of algorithm parameters. Algorithms,
//   such as edge detection, would inherit specializations of this mixin
//   which contain the actual parameters and methods to validate the
//   parameters.
//
// \author J.L. Mundy
// \date   November 28, 1997
//
// \verbatim
//  Modifications:
//   Rupert Curwen -  February 3, 1998
//      Added the ParamModifier class as an abstract base class which
//      will allow parameter blocks to support generic "modifier"
//      classes.  A parameter class defines the virtual method
//      DescribeParams, which takes a ParamModifier as an argument.
//      The parameter block calls AddParam on this ParamModifier for
//      each of the parameters in the block.  For example, the
//      particular ParamModifier passed into the method might be one
//      which builds a dialog which will then modify the parameter
//      class.  Or it might just print the values to a stream.  See
//      Segmentation/Detection/DetectorParams for an example of use.
// \endverbatim
//======================================================================

#include <iostream>
#include <string>
#include <vcl_compiler.h>

#if 0 // not implemented
class ParamModifier;
#endif

class gevd_param_mixin
{
  bool valid_;
  std::string error_msg_;
 public:
  //=====================================================
  //: Constructor.  By default a constructed parameter block is valid.
  gevd_param_mixin() : valid_(true) {}
  // Destructor
  virtual ~gevd_param_mixin() = default;
  //=====================================================
  virtual bool SanityCheck();
  bool Valid() const { return valid_; }
  const char* GetErrorMsg() const { return error_msg_.c_str(); }
  void SetErrorMsg(const char* msg);
#if 0// not implemented in vxl
  virtual void Describe(ParamModifier&) {}
#endif
};

#if 0 // not implemented in vxl
class ParamModifier
{
 virtual ~ParamModifier() {}

 public:
  //------------------------------------------------------------
  //: These are some standard boolean choice styles defined for convenience.
  enum BoolChoiceStyle { TrueFalse, OnOff, YesNo };

  //------------------------------------------------------------
  //: Name those parameters which follow.
  virtual void Name(const std::string& name) = 0;

  //------------------------------------------------------------
  //: Add a float parameter.
  virtual void AddParam(const std::string& name, float& value) = 0;

  //------------------------------------------------------------
  //: Add a double parameter.
  virtual void AddParam(const std::string& name, double& value) = 0;

  //------------------------------------------------------------
  //: Add an integer parameter.
  virtual void AddParam(const std::string& name, int& value) = 0;

  //------------------------------------------------------------
  //: Add a boolean parameter.
  virtual void AddParam(const std::string& name, bool& value) = 0;

  //------------------------------------------------------------
  //: Add a choice parameter.
  //  virtual void AddParam(const std::string& name, int& value, UIChoice* choices) = 0;

  //------------------------------------------------------------
  //: Add a boolean choice parameter, using one of the convenient standard styles.
  virtual void AddParam(const std::string& name, bool& value,
                        BoolChoiceStyle style = TrueFalse) = 0;
};
#endif
#endif // gevd_param_mixin_h_
