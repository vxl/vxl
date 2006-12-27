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

#include <rgrl/rgrl_transformation_sptr.h>
#include <vcl_iosfwd.h>
#include <vcl_vector.h>

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
  rgrl_transformation_sptr read( vcl_istream& is );

  //: Read a transformation from input file for convenience
  //
  static
  rgrl_transformation_sptr read( char const* fn );

  //: add a transformation candidate
  static
  void add_xform( rgrl_transformation_sptr );

protected:
  static vcl_vector< rgrl_transformation_sptr >   xform_candidates_;
};

//: stream input operator for reading a transformation
vcl_istream& operator>>( vcl_istream& is, rgrl_transformation_sptr& sptr );

#endif // rgrl_trans_reader_h_
