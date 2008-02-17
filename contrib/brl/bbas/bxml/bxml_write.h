// This is brl/bbas/bxml/bxml_write.h
#ifndef bxml_write_h_
#define bxml_write_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief functions to write XML documents
// \author Matt Leotta (Brown)
// \date   October 5, 2006
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include "bxml_document.h"
#include <vcl_iostream.h>

//: Write the entire contents of \p filepath into an XML document class
void bxml_write(const vcl_string& filepath, const bxml_document& doc);


//: Write the entire data stream \p is into an XML document class
void bxml_write(vcl_ostream& os, const bxml_document& doc);


//: Write the document declaration (header)
void bxml_write_declaration(vcl_ostream& os, const bxml_document& doc);

//: Write the data (element or text)
void bxml_write_data(vcl_ostream& os, const bxml_data_sptr& data);

//: Write the XML element
void bxml_write_element(vcl_ostream& os, const bxml_element& element);


#endif // bxml_write_h_
