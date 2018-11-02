#ifndef bsvg_document_h_
#define bsvg_document_h_
//:
// \file
// \brief classes based on bxml to create SVG documents
//
//        create an XML document with SVG primitives (text, lines, ellipses, polygons, etc.) and display them using any browser
//
// \author Ozge C. Ozcanli (Brown)
// \date   April 21, 2009
//
// \verbatim
//  Modifications
//   Ozge C. Ozcanli - July 08, 09 - ported to vxl from local repository - minor fixes
// \endverbatim

#include <iostream>
#include <string>
#include <bxml/bxml_document.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class bsvg_document : public bxml_document
{
 public:
  bsvg_document(float w, float h);  // if viewBox params are not specified, then each pixel corresponds to one unit square while drawing
  bsvg_document(float w, float h, float viewBox_x, float viewBox_y, float viewBox_w, float viewBox_h);
  bool add_description(const std::string& d);
  bool add_element(const bxml_data_sptr& element);

 public:
  float w_;
  float h_;
};

#endif  // bsvg_document_h_
