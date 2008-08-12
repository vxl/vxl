//: \file
//  \author Ian Scott
//  \date 12 Aug 2008
//  \brief General Purpose Reverse Polish Notation image maths program.
// To add your own operations, add a function (e.g. plus__image_3d_of_float__image_3d_of_float),
// and add a entry in operations::operations() to register your new operation. That list
// of entries must be kept sorted.


#include <vcl_iostream.h>

#if VCL_HAS_EXCEPTIONS
#include <vcl_exception.h>
#include <vcl_stdexcept.h>
#include <vcl_sstream.h>
#include <vcl_deque.h>
#include <vsl/vsl_stream.h>
#include <vsl/vsl_deque_io.txx>
#include <mbl/mbl_log.h>
#include <mbl/mbl_exception.h>
#include <vil3d/file_formats/vil3d_gen_synthetic.h>
#include <vil3d/algo/vil3d_distance_transform.h>
#include <vil3d/vil3d_convert.h>
#include <vimt3d/vimt3d_load.h>
#include <vimt3d/vimt3d_save.h>
#include <vimt3d/vimt3d_transform_3d.h>
#include <vimt3d/vimt3d_add_all_loaders.h>



//=========================================================================
// Static function to create a static logger when first required
//=========================================================================
static mbl_logger& logger()
{
  static mbl_logger l("mul.tools.image_3d_math");
  return l;
}


//: This is the return value of the program.
// It can be modified using the (as yet unimplemented) "-return" operator.
int global_retval=0;
//: load images as float types if not true
bool global_option_load_as_image_int = false;

class operand;
vcl_ostream& operator <<( vcl_ostream&, const operand&);

//: A value on the stack, to be used as by an operation.
// This is a rather inefficient VARIANT-type class.
class operand
{
  vcl_string string_;
  vimt3d_image_3d_of<float> image_3d_of_float_;
  vimt3d_image_3d_of<int> image_3d_of_int_;
  double double_;

  bool string_to_double(const vcl_string&s, double&d) const
  {
    vcl_istringstream ss(s);
    ss >> d;
    if (!ss) return false;
    char dummy;
    ss >> dummy;
    if (ss) return false;
    return true;
  }
  bool string_to_image(const vcl_string&s, vimt3d_image_3d_of<float>&d) const
  {
    try
    {
      vimt3d_load(s, d, true);
    }
    catch (const vcl_exception& e)
    {
      vcl_cerr << e.what() << '\n';
      return false;
    }
    return true;
  }
  bool string_to_image(const vcl_string&s, vimt3d_image_3d_of<int>&d) const
  {
    try
    {
      vimt3d_load(s, d, true);
    }
    catch (const vcl_exception& e)
    {
      vcl_cerr << e.what() << '\n';
      return false;
    }
    return true;
  }
public:
  enum operand_type_t { e_string, e_image_3d_of_float, e_image_3d_of_int, e_double };
  operand_type_t operand_type_;

  explicit operand(const vcl_string& s):
    string_(s), operand_type_(e_string) {}
  explicit operand(const vimt3d_image_3d_of<float>& i):
    image_3d_of_float_(i), operand_type_(e_image_3d_of_float) {}
  explicit operand(const vimt3d_image_3d_of<int>& i):
    image_3d_of_int_(i), operand_type_(e_image_3d_of_int) {}
  explicit operand(const double& i):
    double_(i), operand_type_(e_double) {}
  
  operand_type_t operand_type() const { return operand_type_; }
  
  const bool is_string() const { return operand_type_==e_string; }
  const vcl_string& as_string() const { assert(is_string()); return string_; }

  const bool is_double() const
  {
    double dummy;
    return (operand_type_==e_string && string_to_double(string_, dummy))
      || operand_type_==e_double;
  }
  const double as_double() const
  {
    assert(is_double());
    if (operand_type_==e_double) return double_;
    else if (operand_type_==e_string)
    {
      double v;
      string_to_double(string_, v);
      return v;
    }
    vcl_ostringstream ss;
    ss << "Tried to use unsuitable operand as a double: " << *this;
    throw mbl_exception_abort(ss.str());
  }

  const bool is_image_3d_of_float() const
  {
    vimt3d_image_3d_of<float> dummy;
    return (operand_type_==e_string && !global_option_load_as_image_int && string_to_image(string_, dummy))
      || operand_type_==e_image_3d_of_float;
  }
  const vimt3d_image_3d_of<float> as_image_3d_of_float() const
  {
    if (operand_type_==e_image_3d_of_float) return image_3d_of_float_;
    else if (operand_type_==e_string && !global_option_load_as_image_int)
    {
      vimt3d_image_3d_of<float> v;
      string_to_image(string_, v);
      return v;
    }
    vcl_ostringstream ss;
    ss << "Tried to use unsuitable operand as a vimt_image_3d_of<float>: " << *this;
    mbl_exception_abort(ss.str());
    return vimt3d_image_3d_of<float>();
  }

  const bool is_image_3d_of_int() const
  {
    vimt3d_image_3d_of<int> dummy;
    return (operand_type_==e_string && global_option_load_as_image_int && string_to_image(string_, dummy))
      || operand_type_==e_image_3d_of_int;
  }
  const vimt3d_image_3d_of<int> as_image_3d_of_int() const
  {
    if (operand_type_==e_image_3d_of_int) return image_3d_of_int_;
    else if (operand_type_==e_string && global_option_load_as_image_int)
    {
      vimt3d_image_3d_of<int> v;
      string_to_image(string_, v);
      return v;
    }
    vcl_ostringstream ss;
    ss << "Tried to use unsuitable operand as a vimt_image_3d_of<int>: " << *this;
    mbl_exception_abort(ss.str());
    return vimt3d_image_3d_of<int>();
  }


  void print_summary(vcl_ostream &ss) const
  {
    // Forward declaration.
    vcl_ostream& operator <<( vcl_ostream&ss, const operand::operand_type_t& t);

    ss << operand_type_ << ": ";
    switch (operand_type_)
    {
    case e_string: 
      ss << string_;
      break;
    case e_image_3d_of_float:
      ss << vsl_stream_summary(image_3d_of_float_);
      break;
    case e_image_3d_of_int:
      ss << vsl_stream_summary(image_3d_of_int_);
      break;
    case e_double:
      ss << double_;
      break;
    default:
      {
        vcl_ostringstream ss;
        ss << "Unknown operand_type: " << operand_type_;
        throw mbl_exception_abort(ss.str());
      }
    }
  }
  bool is_a(operand_type_t t) const
  {
    switch (t)
    {
    case e_string: 
      return is_string();
    case e_image_3d_of_float:
      return is_image_3d_of_float();
    case e_image_3d_of_int:
      return is_image_3d_of_int();
    case e_double:
      return is_double();
    default:
      {
        vcl_ostringstream ss;
        ss << "Unknown operand_type: " << operand_type_;
        throw mbl_exception_abort(ss.str());
      }
    }
    return false;
  }
};

void vsl_print_summary( vcl_ostream&ss, const operand& p) { p.print_summary(ss); }
vcl_ostream& operator <<( vcl_ostream&ss, const operand& p) { p.print_summary(ss); return ss; }

void vsl_print_summary(vcl_ostream& os, const vcl_deque<operand> &v)
{
  os << "Deque length: " << v.size() << vcl_endl;
  for (unsigned int i=0; i<v.size() && i<5; i++)
  {
    os << ' ' << i << ": ";
    vsl_print_summary(os,v[i]);
    os << vcl_endl;
  }
  if (v.size() > 5)
    os << " ...\n";
}
vcl_ostream& operator <<( vcl_ostream&ss, const operand::operand_type_t& t)
{
  switch (t)
  {
    case operand::e_string: 
      ss << "string"; break;
    case operand::e_image_3d_of_float:
      ss << "image_3d_of_float"; break;
    case operand::e_image_3d_of_int:
      ss << "image_3d_of_int"; break;
    case operand::e_double:
      ss << "double"; break;
    default:
      {
        vcl_ostringstream ss;
        ss << "Unknown operand_type: " << static_cast<int>(t);
        throw mbl_exception_abort(ss.str());
      }
  }
  return ss;
}
typedef vcl_deque<operand> stack_t;


void print_operations(vcl_ostream&);

//: Operation implementation
void help(stack_t& s)
{
  vcl_cerr <<
    "usage: image3d_math [--operand | operation] [--operand | operation] ... --operand\n"
    "Manipulate images using Reverse polish notiation.\n"
    "List of commands: \n";
  print_operations(vcl_cerr);
  vcl_exit(3);
}

//: Operation implementation
void sum__image_3d_of_float__image_3d_of_float(stack_t& s)
{
  assert(s.size() >= 2);
  vimt3d_image_3d_of<float> o2(s[0].as_image_3d_of_float());
  vimt3d_image_3d_of<float> o1(s[1].as_image_3d_of_float());

  vimt3d_image_3d_of<float> result;
  vil3d_math_image_sum(o1.image(), o2.image(), result.image());
  result.world2im() = o1.world2im();

  s.pop_front();
  s.pop_front();
  s.push_front(operand(result));
}

void save__image_3d_of_float__string(stack_t& s)
{
  assert(s.size() >= 2);

  vimt3d_save(s[0].as_string(), s[1].as_image_3d_of_float(), true);

  s.pop_front();
  s.pop_front();
}

void save__image_3d_of_int__string(stack_t& s)
{
  assert(s.size() >= 2);

  vimt3d_save(s[0].as_string(), s[1].as_image_3d_of_int(), true);

  s.pop_front();
  s.pop_front();
}


void scale_and_offset__image_3d_of_float__double__double(stack_t& s)
{
  assert(s.size() >= 3);
  vimt3d_image_3d_of<float> o1=s[2].as_image_3d_of_float();

  vil3d_math_scale_and_offset_values(o1.image(),
    s[1].as_double(), static_cast<float>(s[0].as_double()) );

  s.pop_front();
  s.pop_front();
  s.pop_front();
  s.push_front(operand(o1));

}

void convert_to_int__image_3d_of_float(stack_t& s)
{
  assert(s.size() >= 1);
  vimt3d_image_3d_of<float> o1=s[0].as_image_3d_of_float();
  vimt3d_image_3d_of<int> result;

  vil3d_convert_round(o1.image(), result.image());
  result.world2im() = o1.world2im();

  s.pop_front();
  s.push_front(operand(result));

}

void option_load_as_image_int(stack_t& s)
{
  global_option_load_as_image_int = true;
}

void option_load_as_image_float(stack_t& s)
{
  global_option_load_as_image_int = false;
}

void convert_to_float__image_3d_of_int(stack_t& s)
{
  assert(s.size() >= 1);
  vimt3d_image_3d_of<int> o1=s[0].as_image_3d_of_int();
  vimt3d_image_3d_of<float> result;

  vil3d_convert_cast(o1.image(), result.image());
  result.world2im() = o1.world2im();

  s.pop_front();
  s.push_front(operand(result));
}



void product__image_3d_of_float__image_3d_of_float(stack_t& s)
{
  assert(s.size() >= 2);
  vimt3d_image_3d_of<float> o2(s[0].as_image_3d_of_float());
  vimt3d_image_3d_of<float> o1(s[1].as_image_3d_of_float());
  vimt3d_image_3d_of<float> result;
  vil3d_math_image_product(o1.image(), o2.image(), result.image());
  result.world2im() = o1.world2im();

  s.pop_front();
  s.pop_front();
  s.push_front(operand(result));

}

void signed_distance_transform__image_3d_of_float(stack_t& s)
{
  vimt3d_image_3d_of<float> o1(s[0].as_image_3d_of_float());

  vgl_vector_3d<double> voxel_size=o1.world2im().inverse().delta(
    vgl_point_3d<double>(0,0,0), vgl_vector_3d<double>(1.0,1.0,1.0) );


  vil3d_signed_distance_transform(o1.image(),
    static_cast<float>(vcl_abs(voxel_size.x())),
    static_cast<float>(vcl_abs(voxel_size.y())),
    static_cast<float>(vcl_abs(voxel_size.z())) );

  s.pop_front();
  s.push_front(operand(o1));
}


class operations
{
private:
  //: Syntax sugar to allow
  class function_type_t:public vcl_vector<operand::operand_type_t>
  {
  public:
    function_type_t& operator << (operand::operand_type_t t)
    {
      this->push_back(t);
      return *this;
    }
  };


  typedef void(*function_t)(stack_t& stack);
  vcl_vector<vcl_string> names_;
  vcl_vector<function_t> functions_;
  vcl_vector<function_type_t > function_types_;

  static operations singleton_;

  //: Add a single operation to the list.
  void add_operation(const char* name, function_t func, function_type_t& t)
  {
    names_.push_back(name);
    functions_.push_back(func);
    function_types_.push_back(t);
  }

  //: Add all operations to the list.
  operations()
  {
    function_type_t no_operands;

    add_operation("--convert_to_float", &convert_to_float__image_3d_of_int,
      function_type_t() << operand::e_image_3d_of_int);
    add_operation("--convert_to_int", &convert_to_int__image_3d_of_float,
      function_type_t() << operand::e_image_3d_of_float);
    add_operation("--help", &help,
      no_operands);
    add_operation("--option_load_as_image_float", &option_load_as_image_float,
      no_operands);
    add_operation("--option_load_as_image_int", &option_load_as_image_int,
      no_operands);
    add_operation("--product", &product__image_3d_of_float__image_3d_of_float,
      function_type_t() << operand::e_image_3d_of_float << operand::e_image_3d_of_float);
    add_operation("--save", &save__image_3d_of_float__string,
      function_type_t() << operand::e_image_3d_of_float << operand::e_string);
    add_operation("--save", &save__image_3d_of_int__string,
      function_type_t() << operand::e_image_3d_of_int << operand::e_string);
    add_operation("--scale_and_offset", &scale_and_offset__image_3d_of_float__double__double,
      function_type_t() << operand::e_image_3d_of_float << operand::e_double << operand::e_double);
    add_operation("--signed_distance_transform", &signed_distance_transform__image_3d_of_float,
      function_type_t() << operand::e_image_3d_of_float);
    add_operation("--sum", &sum__image_3d_of_float__image_3d_of_float,
      function_type_t() << operand::e_image_3d_of_float << operand::e_image_3d_of_float);


    // Check they are correctly sorted.
    vcl_vector<vcl_string>::iterator it=names_.begin(), end=names_.end();
    while ((it+1)!=end)
    {
      vcl_string &left = *it;
      if (left > *++it)
        throw mbl_exception_abort(vcl_string("\"") + left + "\" is not in correct order");
    }
  }

  static bool operation_type_matches(const function_type_t& type, const stack_t& stack)
  {
    if (stack.size() < type.size()) return false;
    for (unsigned i=0; i<type.size(); ++i)
    {
      if (!stack[type.size()-i-1].is_a(type[i]))
        return false;
    }
    return true;
  }
public:
  static void run(vcl_string &name, stack_t& stack)
  {
    typedef vcl_pair<vcl_vector<vcl_string>::iterator, vcl_vector<vcl_string>::iterator> range_t;
    range_t range =
      vcl_equal_range(singleton_.names_.begin(), singleton_.names_.end(), name);
    for (unsigned i = distance(singleton_.names_.begin(), range.first),
      end = distance(singleton_.names_.begin(), range.second); i!=end; ++i)
    {
      if (operation_type_matches(singleton_.function_types_[i], stack))
      {
        (*singleton_.functions_[i])(stack);
        return ;
      }
    }
    vcl_ostringstream ss;
    ss << "Unable to match command \"" << name << "\"\n"
      "Stack is :\n" << vsl_stream_summary(stack);
    throw vcl_runtime_error(ss.str());
  }
  static void print(vcl_ostream& ss)
  {
    unsigned n = singleton_.names_.size();
    for (unsigned i=0; i<n; ++i)
    {
      ss << singleton_.names_[i] << ' ';
      for (unsigned j=0, nj=singleton_.function_types_[i].size(); j<nj; ++j)
        ss << singleton_.function_types_[i][j] << ' ';
      ss << "\n";
    }
  }

};

operations operations::singleton_ = operations();
void print_operations(vcl_ostream&ss) { operations::print(ss); }

//========================================================================
// Actual main function
//========================================================================
int main2(int argc, char*argv[])
{
  stack_t stack;

  if (argc==1)
    help(stack);

  for (int i=1; i<argc; ++i)
  {
    vcl_string option = argv[i];

    if (option.empty()) continue;
    if (option.substr(0, 2) == "--")
    {
      operations::run(option, stack);
    }
    else
    {
      stack.push_front(operand(option));
    }
  }

  if (!stack.empty())
    MBL_LOG(WARN, logger(), "Stack not empty after all operations." <<
    vsl_stream_summary(stack));

  return global_retval;
}


//========================================================================
// Exception-handling wrapper around main function
//========================================================================
int main(int argc, char*argv[])
{
  try
  {
    // Initialize the logger
    mbl_logger::root().load_log_config_file();

    vimt3d_add_all_loaders();
    vil3d_file_format::add_format(new vil3d_gen_synthetic_format);

    main2(argc, argv);
  }
  catch (vcl_exception& e)
  {
    vcl_cout << "caught exception " << e.what() << vcl_endl;
    return 3;
  }
  catch (...)
  {
    vcl_cout << "caught unknown exception " << vcl_endl;
    return 3;
  }


  return 0;
}
#else // VCL_HAS_EXCEPTIONS
int main(int argc, char*argv[])
{
  vcl_cerr << "ERROR: image3d_math needs exception support to compile properly.\n";
}
#endif


