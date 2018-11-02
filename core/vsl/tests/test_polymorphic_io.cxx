// This is core/vsl/tests/test_polymorphic_io.cxx
#include <iostream>
#include <testlib/testlib_test.h>
//:
// \file

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_binary_loader.h>
#include <vsl/vsl_binary_loader.hxx>
#include <vpl/vpl.h>

//: Base class to test polymorphic loading
class test_base_class
{
 public:
  //: Destructor
  virtual ~test_base_class() = default;

  virtual int data() const { return 0; }

  //: Binary save self to stream.
  virtual void b_write(vsl_b_ostream & /*os*/) const { assert(false); } //= 0;

  //: Binary load self from stream.
  virtual void b_read(vsl_b_istream & /*is*/) { assert(false); } //= 0;

  //: Clone this
  virtual test_base_class* clone() const { assert(false); return nullptr; } //= 0;

  //: Return a platform independent string identifying the class
  virtual std::string is_a() const
  { return "test_base_class"; }

  //: Return true if the argument matches this class' identifying string
  virtual bool is_class(std::string const& s) const
  { return s == "test_base_class"; }

  //: Print summary
  virtual void print_summary(std::ostream& os) const
  {
    os<<is_a()<<std::endl;
  };
};

//: Add example object to list of those that can be loaded
//  The vsl_binary_loader must see an example of each derived class
//  before it knows how to deal with them.
//  A clone is taken of b
void vsl_add_to_binary_loader(const test_base_class& b)
{
  vsl_binary_loader<test_base_class>::instance().add(b);
}


//: Binary save to stream.
inline void vsl_b_write(vsl_b_ostream &os, const test_base_class & v)
{
  v.b_write(os);
}

//: Binary save to stream.
inline void vsl_b_write(vsl_b_ostream &os, const test_base_class* v)
{
  if (v!=nullptr)
  {
    vsl_b_write(os,v->is_a());
    v->b_write(os);
  }
  else
    vsl_b_write(os, std::string("VSL_NULL_PTR"));
}

//: Binary load from stream.
inline void vsl_b_read(vsl_b_istream &is, test_base_class & v)
{
  v.b_read(is);
}

//: Derived class to test polymorphic loading
class test_derived_class : public test_base_class
{
  int data_;

 public:
  test_derived_class(int d=0) : data_(d) {}

  void set_data(int d) { data_=d; }

  int data() const override { return data_; }

  //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const override;

  //: Binary load self from stream.
  void b_read(vsl_b_istream &is) override;

  //: Clone this
  test_base_class* clone() const override;

  //: Print summary
  void print_summary(std::ostream& os) const override;

  //: Return a platform independent string identifying the class
  std::string is_a() const override;

  //: Return true if the argument matches this class' or the parent's identifier
  bool is_class(std::string const& s) const override;
};

//: Binary save self to stream.
void test_derived_class::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os,data_);
}

//: Binary load self from stream.
void test_derived_class::b_read(vsl_b_istream &is)
{
  vsl_b_read(is,data_);
}

//: Clone this
test_base_class* test_derived_class::clone() const
{
  return new test_derived_class(*this);
}

//: Return a platform independent string identifying the class
std::string test_derived_class::is_a() const
{
  return "test_derived_class";
}

//: Return true if the argument matches this class' or the parent's identifier
bool test_derived_class::is_class(std::string const& s) const
{
  return s == "test_derived_class" || test_base_class::is_class(s);
}

//: Print summary
void test_derived_class::print_summary(std::ostream& os) const
{
  os<<is_a()<<" Data="<<data_<<std::endl;
}

void test_polymorphic_io()
{
  std::cout << "*********************************\n"
           << "Testing vsl polymorphic binary io\n"
           << "*********************************\n";

  // Arrange that the loader knows about derived class
  vsl_add_to_binary_loader(test_derived_class());

  test_derived_class d1_out(1234);
  test_base_class *b1_out = &d1_out;
  test_base_class *b2_out = nullptr;

  vsl_b_ofstream bfs_out("vsl_polymorphic_io_test.bvl.tmp");
  TEST("Opened vsl_polymorphic_io_test.bvl.tmp for writing", (!bfs_out), false);
  vsl_b_write(bfs_out,d1_out);
  vsl_b_write(bfs_out,b1_out);
  vsl_b_write(bfs_out,b2_out);
  bfs_out.close();

  test_derived_class d1_in(0);
  test_base_class *b1_in = nullptr;
  test_base_class *b2_in = new test_derived_class(7);

  vsl_b_ifstream bfs_in("vsl_polymorphic_io_test.bvl.tmp");
  TEST("Opened vsl_polymorphic_io_test.bvl.tmp for reading", (!bfs_in), false);
  vsl_b_read(bfs_in,d1_in);
  vsl_b_read(bfs_in,b1_in);
  vsl_b_read(bfs_in,b2_in);
  TEST("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  vpl_unlink ("vsl_polymorphic_io_test.bvl.tmp");

  TEST("derived in = derived out", d1_in.data(), d1_out.data());
  TEST("Load derived by base", b1_in->is_a(), d1_out.is_a());
  TEST("derived in (by ptr) = derived out", b1_in->data(), b1_out->data());
  TEST("IO for NULL pointers", b2_in, 0);

  // Tidy up
  delete b1_in;
  delete b2_in;

}

TESTMAIN(test_polymorphic_io);

// Explicitly instantiate loader
VSL_BINARY_LOADER_WITH_SPECIALIZATION_INSTANTIATE(test_base_class);
