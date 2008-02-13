#include <testlib/testlib_test.h>
#include <bxml/bxml_read.h>
#include <bxml/bxml_write.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_sstream.h>


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


int
test_io_main( int argc, char* argv[] )
{
  vcl_string path_base;
  if ( argc >= 2 ) {
    path_base = argv[1];
#ifdef VCL_WIN32
    path_base += "\\";
#else
    path_base += "/";
#endif
  }

  START ("XML IO");

  {
    bxml_document doc = make_simple_doc();

    vcl_cout << "\n---- start of document ----"<<vcl_endl;
    bxml_write(vcl_cout,doc);
    vcl_cout << "\n----  end of document  ----\n"<<vcl_endl;

    bxml_write("text_out.xml",doc);

    bxml_document doc_in = bxml_read("text_out.xml");

    TEST("loaded equal to saved",doc,doc_in);
  }

  {
    bxml_document doc = make_long_doc();
    vcl_stringstream s;
    bxml_write(s,doc);

    bxml_stream_read str_reader;

    //vcl_ifstream file("text_out.xml");
    unsigned int depth = 0;
    bxml_data_sptr data = NULL;
    bool fail = false;
    int count = 0;
    while(data = str_reader.next_element(s, depth)){
      if(depth == 1){
        if(data->type() != bxml_data::ELEMENT){
          fail = true;
          break;
        }
        bxml_element* el = static_cast<bxml_element*>(data.ptr());
        if(el->name() != "frame"){
          fail = true;
          break;
        }
        int val;
        if(!el->get_attribute("number",val) || val != count){
          fail = true;
          break;
        }
        ++count;
      }
    }

    TEST("Stream I/O",fail, false);
  }

  SUMMARY();
}


