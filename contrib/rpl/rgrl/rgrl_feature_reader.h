#ifndef rgrl_feature_reader_h_
#define rgrl_feature_reader_h_

//:
// \file
// \author Gehua Yang
// Smart reader to read in any feature
// \verbatim
// Modifications
//   Nov 2008 - J Becker - Added a class width a static read function based on rgrl_trans_reader.
//                         This is to allow user to add other local-defined feature types.  For
//                         backward compatibility I left the original read function.
// \endverbatim

#include <iostream>
#include <iosfwd>
#include <vector>
#include <rgrl/rgrl_feature_sptr.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Read one feature from input stream
//  IMPORTANCE:
//   - Open text file in BINARY mode!
//   - Stream Pos does not work in Windows when a UNIX ascii file is opened in text mode.
//


//: Reader class for smartly detecting various transformation classes
//  Users can also add user-defined classes, which takes priority before others
class rgrl_feature_reader
{
 public:

  //: Read a transformation from input stream
  //  IMPORTANCE:
  //   - Open transformation file in BINARY mode!
  //   - Stream Pos does not work in Windows when a UNIX ascii file
  //     is opened in text mode.
  //
  //  The type of feature depends on the content of the input stream.
  //  NULL smart ptr is returned if reading operation fails.
  //  Please check the validity of the returned smart ptr.
  static
  rgrl_feature_sptr read( std::istream& is );

  //: add a feature candidate
  static
  void add_feature( const rgrl_feature_sptr& feat );

 protected:
  static std::vector< rgrl_feature_sptr >   feature_candidates_;
};

//: The type of feature depends on the content of the input stream.
//  NULL smart ptr is returned if the reading operation fails.
//  Please check the validity of the returned smart ptr.
rgrl_feature_sptr
rgrl_feature_reader( std::istream& is );

//: stream input operator for reading a feature
std::istream& operator>>( std::istream& is, rgrl_feature_sptr& sptr );

#endif // rgrl_feature_reader_h_
