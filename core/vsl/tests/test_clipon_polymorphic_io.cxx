// This is core/vsl/tests/test_clipon_polymorphic_io.cxx

//:
// \file

#include <vcl_iostream.h>
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_clipon_binary_loader.h>
#include <vsl/vsl_clipon_binary_loader.txx>
#include <testlib/testlib_test.h>
#include <vpl/vpl.h>

//: Base class to test polymorphic loading
class test2_base_class
{
  void vtable_hack();
 public:
  //: Destructor
  virtual ~test2_base_class() {};

  virtual int data() const { return 0; }

  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const
  { return "test2_base_class"; }

  //: Return true if the argument matches this class' identifying string
  virtual bool is_class(vcl_string const& s) const
  { return s == "test2_base_class"; }

  //: Print summary
  virtual void print_summary(vcl_ostream& os) const
  { os<<is_a()<<vcl_endl; }
};

void test2_base_class::vtable_hack() { }

//: Base class to test polymorphic loading
class test2_base_class_io
{
 public:
  virtual test2_base_class* new_object() const =0;

  virtual void b_write_by_base(vsl_b_ostream& os, const test2_base_class& base) const =0;

  virtual void b_read_by_base(vsl_b_istream& os, test2_base_class& base) const =0;

  virtual void print_summary_by_base(vcl_ostream& os, const test2_base_class& base) const =0;

  virtual test2_base_class_io* clone() const =0;

  virtual vcl_string target_classname() const =0;

  virtual bool is_io_for(const test2_base_class& base) const =0;
};

//: Add example object to list of those that can be loaded
//  The vsl_binary_loader must see an example of each derived class
//  before it knows how to deal with them.
//  A clone is taken of b
void vsl_add_to_binary_loader(const test2_base_class_io& b)
{
  vsl_clipon_binary_loader<test2_base_class,test2_base_class_io>::
    instance().add(b);
}


//: Binary save to stream.
inline void vsl_b_write(vsl_b_ostream &os, const test2_base_class * b)
{
  vsl_clipon_binary_loader<test2_base_class,test2_base_class_io>::
    instance().write_object(os,b);
}

//: Binary read from stream.
inline void vsl_b_read(vsl_b_istream &is, test2_base_class* &b)
{
  vsl_clipon_binary_loader<test2_base_class,test2_base_class_io>::
    instance().read_object(is,b);
}

//: Print summary to stream by vnl_nonlinear_minimizer pointer
void vsl_print_summary(vcl_ostream &os, const test2_base_class * b)
{
    vsl_clipon_binary_loader<test2_base_class,test2_base_class_io>::
      instance().print_object_summary(os,b);
}

//: Derived class to test polymorphic loading
class test2_derived_class : public test2_base_class
{
  int data_;
 public:
  test2_derived_class(int d=0) : data_(d) {}

  void set_data(int d) { data_=d; }

  virtual int data() const { return data_; }

  //: Print summary
  virtual void print_summary(vcl_ostream& os) const;

  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const;

  //: Return true if the argument matches this class' or the parent's identifier
  virtual bool is_class(vcl_string const& s) const;

 private:
  void vtable_hack();
};

void test2_derived_class::vtable_hack() { }

//: Return a platform independent string identifying the class
vcl_string test2_derived_class::is_a() const
{ return "test2_derived_class"; }

//: Return true if the argument matches this class' or the parent's identifier
bool test2_derived_class::is_class(vcl_string const& s) const
{ return s == "test2_derived_class" || test2_base_class::is_class(s); }

//: Print summary
void test2_derived_class::print_summary(vcl_ostream& os) const
{
  os<<is_a()<<" Data="<<data_<<vcl_endl;
}

void vsl_b_write(vsl_b_ostream& os, const test2_derived_class& d)
{
  vsl_b_write(os,d.data());
}

void vsl_b_read(vsl_b_istream& is, test2_derived_class& d)
{
  int i;
  vsl_b_read(is,i);
  d.set_data(i);
}

//: Base class to test polymorphic loading
class test2_derived_class_io: public test2_base_class_io
{
 public:
  virtual test2_base_class* new_object() const
  {
    return new test2_derived_class;
  }

  virtual void b_write_by_base(vsl_b_ostream& os, const test2_base_class& base) const
  {
    vsl_b_write(os,(const test2_derived_class&) base);
  }

  virtual void b_read_by_base(
    vsl_b_istream& is, test2_base_class& base) const
  {
    vsl_b_read(is,(test2_derived_class&) base);
  }

  virtual void print_summary_by_base(
    vcl_ostream& os, const test2_base_class& base) const
  {
    base.print_summary(os);
  }

  virtual test2_base_class_io* clone() const
  {
    return new test2_derived_class_io(*this);
  }

  virtual vcl_string target_classname() const
  {
    return vcl_string("test2_derived_class");
  }

  virtual bool is_io_for(const test2_base_class& base) const
  { return base.is_class(target_classname()); }
};


void test_clipon_polymorphic_io()
{
  vcl_cout << "*****************************************\n"
           << "Testing vsl clip-on polymorphic binary io\n"
           << "*****************************************\n";

  // Arrange that the loader knows about derived class
  vsl_add_to_binary_loader(test2_derived_class_io());

  test2_derived_class d1_out(1234);
  test2_base_class *b1_out = &d1_out;
  test2_base_class *b2_out = 0;

  vsl_b_ofstream bfs_out("vsl_clipon_polymorphic_io_test.bvl.tmp");
  TEST("Opened vsl_polymorphic_io_test.bvl.tmp for writing", (!bfs_out), false);
  vsl_b_write(bfs_out,d1_out);
  vsl_b_write(bfs_out,b1_out);
  vsl_b_write(bfs_out,b2_out);
  bfs_out.close();

  test2_derived_class d1_in(0);
  test2_base_class *b1_in = 0;
  test2_base_class *b2_in = new test2_derived_class(7);

  vsl_b_ifstream bfs_in("vsl_clipon_polymorphic_io_test.bvl.tmp");
  TEST("Opened vsl_polymorphic_io_test.bvl.tmp for reading", (!bfs_in), false);
  vsl_b_read(bfs_in,d1_in);
  vsl_b_read(bfs_in,b1_in);
  vsl_b_read(bfs_in,b2_in);
  TEST("Finished reading file successfully", (!bfs_in), false); bfs_in.close();

  TEST("(clipon) derived in = derived out", d1_in.data(), d1_out.data());
  TEST("(clipon) Load derived by base", b1_in->is_a(), d1_out.is_a());
  TEST("(clipon) derived in (by ptr) = derived out", b1_in->data(), b1_out->data());
  TEST("(clipon) IO for NULL pointers", b2_in, 0);

  // Tidy up
  delete b1_in;
  delete b2_in;

  vpl_unlink ("vsl_clipon_polymorphic_io_test.bvl.tmp");
  vsl_delete_all_loaders();

}

TESTMAIN(test_clipon_polymorphic_io);

// Explicitly instantiate loader
VSL_CLIPON_BINARY_LOADER_INSTANTIATE(test2_base_class, test2_base_class_io);
