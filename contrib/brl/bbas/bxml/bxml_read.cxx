// This is brl/bbas/bxml/bxml_read.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Matt Leotta
// \date   October 5, 2006

#include "bxml_read.h"
#include <vcl_deque.h>
#include <vcl_utility.h>
#include <vcl_fstream.h>
#include <vcl_cassert.h>
#include <expatpplib.h>

// anonymous namespace
namespace {

class bxml_expat_parser : public expatpp
{
 public:
  bxml_expat_parser(bool online = false) : online_mode_(online) {}
  virtual void startElement(const XML_Char* name, const XML_Char** atts);
  virtual void endElement(const XML_Char* name);
  virtual void charData(const XML_Char*, int len);
  virtual void xmlDecl( const XML_Char *version,
                        const XML_Char *encoding,
                        int            standalone);

  bxml_document document() const { return document_; }

  bool pop_complete_data(bxml_data_sptr& data, unsigned int& depth);

 private:
  bool online_mode_;
  vcl_vector<bxml_data_sptr> stack_;
  vcl_deque<vcl_pair<bxml_data_sptr,unsigned int> > complete_;
  bxml_document document_;
};

bool bxml_expat_parser::pop_complete_data(bxml_data_sptr& data, unsigned int& depth)
{
  if (complete_.empty())
    return false;

  data = complete_.front().first;
  depth = complete_.front().second;
  complete_.pop_front();
  return true;
}


//: Handle the start of elements
void bxml_expat_parser::startElement(const XML_Char* name, const XML_Char** atts)
{
  bxml_element* element = new bxml_element(name);
  bxml_data_sptr data(element);
  // set all the attributes
  for (int i=0; atts[i]; i+=2) {
    element->set_attribute(atts[i],atts[i+1]);
  }

  // add this element to the current element or document
  if (stack_.empty()) {
    if (!online_mode_) {
      document_.set_root_element(data);
      stack_.push_back(data);
    }
    else
      stack_.push_back(NULL);
  }
  else{
    if (stack_.back().ptr()) {
      bxml_element* parent = static_cast<bxml_element*>(stack_.back().ptr());
      parent->append_data(data);
    }
    stack_.push_back(data);
  }
}


//: Handle the start of elements
void bxml_expat_parser::endElement(const XML_Char* name)
{
  if (stack_.back().ptr()) {
    bxml_element* element = static_cast<bxml_element*>(stack_.back().ptr());
    assert(element->name() == vcl_string(name));
    complete_.push_back(vcl_pair<bxml_data_sptr,unsigned int>(stack_.back(),stack_.size()-1));
  }
  stack_.pop_back();
}


//: Handle character data
void bxml_expat_parser::charData(const XML_Char* text, int len)
{
  assert(!stack_.empty());
  if (stack_.back().ptr()) {
    bxml_element* parent = static_cast<bxml_element*>(stack_.back().ptr());
    parent->append_text(vcl_string(text,len));
  }
}


//: Handle the XML declaration
void bxml_expat_parser::xmlDecl( const XML_Char *version,
                                 const XML_Char *encoding,
                                 int            standalone)
{
  document_.set_version(version);
  document_.set_encoding(encoding);
  document_.set_standalone(standalone != 0);
}

}; // end anonymous namespace


//: Read the entire contents of \p filepath into an XML document class
bxml_document bxml_read(const vcl_string& filepath)
{
  vcl_ifstream file(filepath.c_str());
  return bxml_read(file);
}


//: Read the entire data stream \p is into an XML document class
bxml_document bxml_read(vcl_istream& is)
{
  bxml_expat_parser parser;

  char buf[4096];
  //char buf[9096];
  int done;

  while (is.good()) {
    is.get(buf,sizeof(buf),0);
    unsigned int n = is.gcount();

    done = (n+1 < sizeof(buf)) ? 1 : 0;

    if (parser.XML_Parse(buf,n,done) != XML_STATUS_OK ) {
      vcl_cerr << "Error parsing\n";
      break;
    }
  }
  return parser.document();
}


class bxml_stream_read::pimpl
{
 public:
  pimpl(unsigned int max_depth) : parser(true), depth(max_depth) {}

  bxml_expat_parser parser;
  unsigned int depth;
};

//: Constructor
bxml_stream_read::bxml_stream_read(int max_depth)
  : p_(new pimpl(max_depth))
{
}

//: Destructor
bxml_stream_read::~bxml_stream_read()
{
  delete p_;
}


//: Reset the state of the reader
void bxml_stream_read::reset()
{
  if (p_) {
    unsigned int depth = p_->depth;
    delete p_;
    p_ = new pimpl(depth);
  }
}


//: Read the next element
bxml_data_sptr
bxml_stream_read::next_element(vcl_istream& is, unsigned int& depth)
{
  char buf[4096];
  int done = 0;

  bxml_data_sptr data = NULL;
  depth = 0;
  while ( p_->parser.pop_complete_data(data, depth) )
    if (depth <= p_->depth)
      return data;

  while (is.good()){
    is.get(buf,sizeof(buf),0);
    int n = is.gcount();
    if (p_->parser.XML_Parse(buf,n,done) != XML_STATUS_OK ) {
      vcl_cerr << "Error parsing\n";
      break;
    }

    while ( p_->parser.pop_complete_data(data, depth) )
      if (depth <= p_->depth)
        return data;
  }
  return NULL;
}

