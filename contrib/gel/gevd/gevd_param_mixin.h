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

#include <vcl_string.h>
class ParamModifier;

class gevd_param_mixin
{
  bool valid_;
  vcl_string error_msg_;
 public:
  //=====================================================
  //: Constructor.  By default a constructed parameter block is valid.
  gevd_param_mixin() : valid_(true) {}
  // Destructor
  virtual ~gevd_param_mixin() {}
  //=====================================================
  virtual bool SanityCheck();
  bool Valid() { return valid_; }
  const char* GetErrorMsg() { return error_msg_.c_str(); }
  void SetErrorMsg(const char* msg);
  virtual void Describe(ParamModifier&) {}
};

class ParamModifier
{
 public:
  //------------------------------------------------------------
  //: These are some standard boolean choice styles defined for convenience.
  enum BoolChoiceStyle { TrueFalse, OnOff, YesNo };

  //------------------------------------------------------------
  //: Name those parameters which follow.
  virtual void Name(const vcl_string& name) = 0;

  //------------------------------------------------------------
  //: Add a float parameter.
  virtual void AddParam(const vcl_string& name, float& value) = 0;

  //------------------------------------------------------------
  //: Add a double parameter.
  virtual void AddParam(const vcl_string& name, double& value) = 0;

  //------------------------------------------------------------
  //: Add an integer parameter.
  virtual void AddParam(const vcl_string& name, int& value) = 0;

  //------------------------------------------------------------
  //: Add a boolean parameter.
  virtual void AddParam(const vcl_string& name, bool& value) = 0;

  //------------------------------------------------------------
  //: Add a choice parameter.
  //  virtual void AddParam(const vcl_string& name, int& value, UIChoice* choices) = 0;

  //------------------------------------------------------------
  //: Add a boolean choice parameter, using one of the convenient standard styles.
  virtual void AddParam(const vcl_string& name, bool& value,
                        BoolChoiceStyle style = TrueFalse) = 0;
};

#endif // gevd_param_mixin_h_
