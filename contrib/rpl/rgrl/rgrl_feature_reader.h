#ifndef rgrl_feature_reader_h_
#define rgrl_feature_reader_h_

//:
// \file
// \author Gehua Yang
// Smart reader to read in any feature

#include <rgrl/rgrl_feature_sptr.h>
#include <vcl_iosfwd.h>

//: Read one feature from input stream
//  IMPORTANCE:
//   - Open text file in BINARY mode!
//   - Stream Pos does not work in Windows when a UNIX ascii file is opened in text mode.
//
//  The type of feature depends on the content of the input stream.
//  NULL smart ptr is returned if the reading operation fails.
//  Please check the validity of the returned smart ptr.
rgrl_feature_sptr rgrl_feature_reader( vcl_istream& is );

//: stream input operator for reading a feature
vcl_istream& operator>>( vcl_istream& is, rgrl_feature_sptr& sptr );

#endif // rgrl_feature_reader_h_
