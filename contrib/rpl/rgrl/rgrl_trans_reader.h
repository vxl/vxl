#ifndef rgrl_trans_reader_h_
#define rgrl_trans_reader_h_

//:
// \file
// \author Gehua Yang, Chia-ling Tsai, Brad King
// Smart reader to read in any transformation class

#include <rgrl/rgrl_transformation_sptr.h>
#include <vcl_iostream.h>

//: Read a transformation from input stream
//  The type of transformation depends on the content of the input stream.
//  NULL smart ptr is returned if reading operation fails.
//  Please check the validity of the return smart ptr
rgrl_transformation_sptr rgrl_trans_reader( vcl_istream& is );

//: input opreator for reading transformation
vcl_istream& operator>>( vcl_istream& is, rgrl_transformation_sptr& sptr );

#endif // rgrl_trans_reader_h_
