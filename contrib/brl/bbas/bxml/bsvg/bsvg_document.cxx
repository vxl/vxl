#include <iostream>
#include <sstream>
#include "bsvg_document.h"
//:
// \file
// \author Ozge C. Ozcanli (Brown)
// \date   April 21, 2009

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

bsvg_document::bsvg_document(float w, float h) : w_(w), h_(h)
{
  bxml_element* root = new bxml_element("svg");
  this->set_root_element(root);

  root->set_attribute("xmlns", "http://www.w3.org/2000/svg");

  std::stringstream ss_w; ss_w << w;
  root->set_attribute("width", ss_w.str());

  std::stringstream ss_h; ss_h << h;
  root->set_attribute("height", ss_h.str());

  //: viewBox is by default, the same size as width and height
  std::string v_box = "0,0,"+ss_w.str()+","+ss_h.str();
  root->set_attribute("viewBox", v_box);
  root->append_text("\n");
}

bsvg_document::bsvg_document(float w, float h, float viewBox_x, float viewBox_y, float viewBox_w, float viewBox_h) : w_(w), h_(h)
{
  bxml_element* root = new bxml_element("svg");
  this->set_root_element(root);

  root->set_attribute("xmlns", "http://www.w3.org/2000/svg");

  std::stringstream ss_w; ss_w << w;
  root->set_attribute("width", ss_w.str());

  std::stringstream ss_h; ss_h << h;
  root->set_attribute("height", ss_h.str());

  std::stringstream ss_x; ss_x << viewBox_x;
  std::stringstream ss_y; ss_y << viewBox_y;
  std::stringstream ss_Bw; ss_Bw << viewBox_w;
  std::stringstream ss_Bh; ss_Bh << viewBox_h;

  std::string v_box = ss_x.str()+","+ss_y.str()+","+ss_Bw.str()+","+ss_Bh.str();
  root->set_attribute("viewBox", v_box);
  root->append_text("\n");
}

bool bsvg_document::add_description(const std::string& d)
{
  if (!this->root_element()) {
    std::cout << "Error: Root element of SVG document has not been created!\n";
    return false;
  }

  bxml_element* root = dynamic_cast<bxml_element*>(this->root_element().ptr());

  bxml_element* desc = new bxml_element("desc");
  desc->append_text(d);
  root->append_data(desc);
  root->append_text("\n");

  return true;
}

bool bsvg_document::add_element(const bxml_data_sptr& element)
{
  if (!this->root_element()) {
    std::cout << "Error: Root element of SVG document has not been created!\n";
    return false;
  }

  bxml_element* root = dynamic_cast<bxml_element*>(this->root_element().ptr());
  root->append_data(element);
  root->append_text("\n");

  return true;
}
