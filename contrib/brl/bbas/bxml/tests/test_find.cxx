#include <testlib/testlib_test.h>
#include <bxml/bxml_find.h>
#include <vcl_iostream.h>

namespace{

bxml_document make_simple_doc()
{
  bxml_document doc;
  bxml_element * root = new bxml_element("root");
  doc.set_root_element(root);

  root->set_attribute("some_attribute","some value");
  root->append_text("\n   ");


  bxml_element * data1 = new bxml_element("empty");
  root->append_data(data1);
  root->append_text("\n   ");
  data1->set_attribute("amount","2.3");

  bxml_element * data2 = new bxml_element("non_empty");
  root->append_data(data2);
  root->append_text("\n");
  data2->set_attribute("value","16");
  data2->append_text("This element contains some text");

  return doc;
}


bxml_document make_long_doc()
{
  bxml_document doc;
  bxml_element * root = new bxml_element("stream");
  doc.set_root_element(root);
  root->append_text("\n");

  for(unsigned i=0; i<100; ++i){
    bxml_element * frame = new bxml_element("frame");
    root->append_text("  ");
    root->append_data(frame);
    root->append_text("\n");
    frame->set_attribute("number",i);

    for(unsigned j=0; j<5; ++j){
      bxml_element * data = new bxml_element("data");
      frame->append_text("\n    ");
      frame->append_data(data);
      vcl_stringstream s;
      s << "This is data element "<<i<<", "<<j;
      data->append_text(s.str());
    }
    frame->append_text("\n  ");
  }

  return doc;
}

};


int
test_find_main( int argc, char* argv[] )
{

  START ("XML Find");

  {
    bxml_document doc = make_simple_doc();

    bxml_element query("empty");
    query.set_attribute("amount","2.3");
    bxml_data_sptr result = bxml_find(doc.root_element(), query);

    TEST("find simple element", !result, false);

    query.set_attribute("amount","2.4");
    result = bxml_find(doc.root_element(), query);

    TEST("find missing element", !result, true);

  }


  SUMMARY();
}


