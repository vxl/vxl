// <begin copyright notice>
// ---------------------------------------------------------------------------
//
//                   Copyright (c) 1997 TargetJr Consortium
//               GE Corporate Research and Development (GE CRD)
//                             1 Research Circle
//                            Niskayuna, NY 12309
//                            All Rights Reserved
//              Reproduction rights limited as described below.
//                               
//      Permission to use, copy, modify, distribute, and sell this software
//      and its documentation for any purpose is hereby granted without fee,
//      provided that (i) the above copyright notice and this permission
//      notice appear in all copies of the software and related documentation,
//      (ii) the name TargetJr Consortium (represented by GE CRD), may not be
//      used in any advertising or publicity relating to the software without
//      the specific, prior written permission of GE CRD, and (iii) any
//      modifications are clearly marked and summarized in a change history
//      log.
//       
//      THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
//      EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
//      WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
//      IN NO EVENT SHALL THE TARGETJR CONSORTIUM BE LIABLE FOR ANY SPECIAL,
//      INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND OR ANY
//      DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
//      WHETHER OR NOT ADVISED OF THE POSSIBILITY OF SUCH DAMAGES, OR ON
//      ANY THEORY OF LIABILITY ARISING OUT OF OR IN CONNECTION WITH THE
//      USE OR PERFORMANCE OF THIS SOFTWARE.
//
// ---------------------------------------------------------------------------
// <end copyright notice>
/* sccs keyword ids:
   @(#) gevd_param_mixin.h 3.1 9/4/92 @(#) 
*/

#ifndef _gevd_param_mixin_h
#define _gevd_param_mixin_h 1

//=========================== Basics ====================================
//
// .NAME	gevd_param_mixin - A mixin to package algorithm parameters
// .HEADER GeneralUtility Package
// .LIBRARY Basics
// .INCLUDE	Basics/gevd_param_mixin.h
// .FILE	gevd_param_mixin.h
// .FILE	gevd_param_mixin.C
//
// .SECTION Description
//   The base class for blocks of algorithm parameters. Algorithms,
//   such as edge detection, would inhert specializations of this mixin
//   which contain the actual parameters and methods to validate the
//   parameters.
//   
// Written by:  J.L. Mundy     
// Date:        November 28, 1997
//
// Modified by: Rupert Curwen
// Date:        February 3, 1998

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
//
//======================================================================
//#include <cool/String.h>
//#include <Basics/types.h>
#include <vcl_string.h>

class ParamModifier;

class gevd_param_mixin 
{
public:
  //=====================================================
  // Constructors and Destructors
  gevd_param_mixin();
  virtual  ~gevd_param_mixin();
  //=====================================================
  virtual bool SanityCheck();
  bool Valid() { return _valid; }
  const char* GetErrorMsg() {return _error_msg.c_str();}
  void SetErrorMsg(const char* msg);
  virtual void Describe(ParamModifier&) {};
private:
  bool _valid;
  vcl_string _error_msg;
};

class ParamModifier
{
public:
  //------------------------------------------------------------
  // -- These are some standard boolean choice styles defined for
  // convenience.
  enum BoolChoiceStyle { TrueFalse, OnOff, YesNo };

  //------------------------------------------------------------
  // -- Name those parameters which follow.
  virtual void Name(const vcl_string& name) = 0;

  //------------------------------------------------------------
  // -- Add a float parameter.
  virtual void AddParam(const vcl_string& name, float& value) = 0;

  //------------------------------------------------------------
  // -- Add a double parameter.
  virtual void AddParam(const vcl_string& name, double& value) = 0;

  //------------------------------------------------------------
  // -- Add an integer parameter.
  virtual void AddParam(const vcl_string& name, int& value) = 0;

  //------------------------------------------------------------
  // -- Add a boolean parameter.
  virtual void AddParam(const vcl_string& name, bool& value) = 0;

  //------------------------------------------------------------
  // -- Add a choice parameter.
  //  virtual void AddParam(const vcl_string& name, int& value, UIChoice* choices) = 0;

  //------------------------------------------------------------
  // -- Add a boolean choice parameter, using one of the convenient
  // standard styles.
  virtual void AddParam(const vcl_string& name, bool& value, 
			BoolChoiceStyle style = TrueFalse) = 0;
};

#endif

