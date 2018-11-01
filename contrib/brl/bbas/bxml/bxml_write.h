// This is brl/bbas/bxml/bxml_write.h
#ifndef bxml_write_h_
#define bxml_write_h_
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

#include <iostream>
#include "bxml_document.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Write the entire contents of \p filepath into an XML document class
void bxml_write(const std::string& filepath, const bxml_document& doc);


//: Write the entire data stream \p is into an XML document class
void bxml_write(std::ostream& os, const bxml_document& doc);


//: Write the document declaration (header)
void bxml_write_declaration(std::ostream& os, const bxml_document& doc);

//: Write the data (element or text)
void bxml_write_data(std::ostream& os, const bxml_data_sptr& data);

//: Write the XML element
void bxml_write_element(std::ostream& os, const bxml_element& element);


#endif // bxml_write_h_
