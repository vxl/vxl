#include <iostream>
#include <sstream>
#include <testlib/testlib_test.h>
#include <bxml/bxml_read.h>
#include <bxml/bxml_write.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


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

  for (unsigned i=0; i<100; ++i) {
    bxml_element * frame = new bxml_element("frame");
    root->append_text("  ");
    root->append_data(frame);
    root->append_text("\n");
    frame->set_attribute("number",i);

    for (unsigned j=0; j<5; ++j) {
      bxml_element * data = new bxml_element("data");
      frame->append_text("\n    ");
      frame->append_data(data);
      std::stringstream s;
      s << "This is data element "<<i<<", "<<j;
      data->append_text(s.str());
    }
    frame->append_text("\n  ");
  }

  return doc;
}


static void test_io(int argc, char* argv[])
{
  std::string path_base;
  if ( argc >= 2 ) {
    path_base = argv[1];
    path_base += "/";
  }

  {
    bxml_document doc = make_simple_doc();

    std::cout << "\n---- start of document ----"<<std::endl;
    bxml_write(std::cout,doc);
    std::cout << "\n----  end of document  ----\n"<<std::endl;

    bxml_write("text_out.xml",doc);

    bxml_document doc_in = bxml_read("text_out.xml");

    TEST("loaded equal to saved",doc,doc_in);
  }

  {
    bxml_document doc = make_long_doc();
    std::stringstream s;
    bxml_write(s,doc);

    bxml_stream_read str_reader;

    //std::ifstream file("text_out.xml");
    unsigned int depth = 0;
    bxml_data_sptr data = nullptr;
    bool fail = false;
    int count = 0;
    while ((data = str_reader.next_element(s, depth))) { // assignment, no comparison
      if (depth == 1) {
        if (data->type() != bxml_data::ELEMENT) {
          fail = true;
          break;
        }
        auto* el = static_cast<bxml_element*>(data.ptr());
        if (el->name() != "frame") {
          fail = true;
          break;
        }
        int val;
        if (!el->get_attribute("number",val) || val != count) {
          fail = true;
          break;
        }
        ++count;
      }
    }
    TEST("Stream I/O",fail, false);
  }
}

TESTMAIN_ARGS(test_io);
