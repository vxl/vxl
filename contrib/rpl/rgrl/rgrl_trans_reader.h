#ifndef rgrl_trans_reader_h_
#define rgrl_trans_reader_h_

//:
// \file
// \author Gehua Yang, Chia-ling Tsai, Brad King
// Smart reader to read in any transformation class
// \date Nov 2004
// \verbatim
// Modifications
//   Dec. 2006 - Gehua Yang - move the function into a class and make it a static function.
//                            This is to allow user to add other local-defined transformations types.
// \endverbatim

#include <iostream>
#include <iosfwd>
#include <vector>
#include <rgrl/rgrl_transformation_sptr.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Reader class for smartly detecting various transformation classes
//  Users can also add user-defined classes, which takes priority before others
class rgrl_trans_reader{

public:
  //: Read a transformation from input stream
  //  IMPORTANCE:
  //   - Open transformation file in BINARY mode!
  //   - Stream Pos does not work in Windows when a UNIX ascii file is opened in text mode.
  //
  //  The type of transformation depends on the content of the input stream.
  //  NULL smart ptr is returned if reading operation fails.
  //  Please check the validity of the returned smart ptr.
  static
  rgrl_transformation_sptr read( std::istream& is );

  //: Read a transformation from input file for convenience
  //
  static
  rgrl_transformation_sptr read( char const* fn );

  //: add a transformation candidate
  static
  void add_xform( const rgrl_transformation_sptr& );

protected:
  static std::vector< rgrl_transformation_sptr >   xform_candidates_;
};

//: stream input operator for reading a transformation
std::istream& operator>>( std::istream& is, rgrl_transformation_sptr& sptr );

#endif // rgrl_trans_reader_h_
