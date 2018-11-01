// This is core/vsl/vsl_clipon_binary_loader.h
#ifndef vsl_clipon_binary_loader_h_
#define vsl_clipon_binary_loader_h_
//:
// \file

#include <vector>
#include <string>
#include <vsl/vsl_binary_loader_base.h>
#include <vsl/vsl_binary_io.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Class to load objects by baseclass pointer using `clipon' classes
// An example of a singleton design pattern for loading
// a DerivedClass from a stream into a BaseClass*.
// All we are given is a BaseClass* into which
// the object has to be loaded but we can only tell
// what sort of object it is from the name information
// stored in the stream.
// We assume the existence of a polymorphic hierarchy of `clipon'
// loader objects, eg BaseClassIO and DerivedClassIO, one per
// class in the hierarchy derived from BaseClass. Each XXXXIO
// class is able to read and write the corresponding XXXX class.
//
// BaseClassIO must implement the following functions:
// \code
// //: Base for objects which provide IO for classes derived from BaseClass
// class BaseClassIO
// {
//  public:
//   //: Create new object of type BaseClass on heap
//   virtual BaseClass* new_object() const;
//   //: Write derived class to os using baseclass reference
//   virtual void b_write_by_base(vsl_b_ostream& os,
//                                const BaseClass& base) const;
//   //: Write derived class to os using baseclass reference
//   virtual void b_read_by_base(vsl_b_istream& is, BaseClass& base) const;
//   //: Print summary to stream by BaseClass pointer
//   void vsl_print_summary(vsl_b_ostream &os, const BaseClass * b);
//   //: Copy this object onto the heap and return a pointer
//   virtual BaseClassIO* clone() const;
//   //: Return name of class for which this object provides IO
//   virtual std::string target_classname() const;
//   //: Return true if b is of class target_classname()
//   //  Typically this will just be "return b.is_a()==target_classname()"
//   //  However, third party libraries may use a different system
//   virtual bool is_io_for(const BaseClass& b) const;
// };
// \endcode
//
// To handle the actual IO we define a loader
// which has a list of BaseClassIO pointers,
// and the ChainOfResponsibility (Design Pattern)
// approach is used to load the object i.e. each
// io->target_classname() is matched against the string on the stream
// until we find a match or run out of pointers. If
// a pointer is found which matches the string on
// the stream, we use it to create an object on the
// heap (using io->new_object()) and then load the
// data into that from the stream (using io->b_read_by_base())
//
// We use a singleton so that there is only one list of
// concrete derived classes which can be added
// to for loading purposes. If you derive a new
// class you just have to append it to the list of
// classes of the singleton, viz:
//     vsl_clipon_binary_loader<B,IO>::instance().add(my_object)
//
// For examples of usage please see vsl/tests/test_clipon_polymorphic_io.cxx
// or look in the Binary IO chapter of the VXL book.
//
// To indicate a null pointer (0), the string "VSL_NULL_PTR" is saved
// to the stream.
// All loader singletons can be deleted using vsl_delete_all_loaders()
template<class BaseClass, class BaseClassIO>
class vsl_clipon_binary_loader : public vsl_binary_loader_base
{
  //: the singleton object
  static vsl_clipon_binary_loader<BaseClass,BaseClassIO>* instance_;

  //: List of object loaders
  std::vector<BaseClassIO*> object_io_;

  //: Return index associated with given object name
  int index_for_name(const std::string& name) const;

 public:
  //: Constructor
  vsl_clipon_binary_loader() = default;

  //: Destructor
  ~vsl_clipon_binary_loader() override;

  //: Returns the instance variable for the singleton.
  static vsl_clipon_binary_loader<BaseClass,BaseClassIO>& instance();

  //: Remove all example objects
  void make_empty();

  //: Add example object to list of those that can be loaded
  void add( const BaseClassIO& b);

  //: Return current list of individual IO objects
  const std::vector<BaseClassIO*>& object_io() const { return object_io_; }

  //: Return IO object for given named class
  //  Aborts if not available
  const BaseClassIO& object_io(const std::string& name) const;

  //: Return IO object that can deal with given class
  const BaseClassIO& io_for_class(const BaseClass& b) const;

  //: Reads object from stream and sets base class pointer
  //  Determines which derived class object on stream belongs
  //  to, loads it and sets b to be a pointer to it.
  //  (Class must be one given to Loader by the add method).
  //  If is indicates a NULL pointer, b will be set to NULL.
  //  If b not initially NULL, *b will be deleted.
  void read_object( vsl_b_istream& is, BaseClass*& b);

  //: Writes object to stream given base class pointer
  //  Determines which derived class object is
  //  and calls the appropriate write function.
  //  (Class must be one given to Loader by the add method).
  //  If b==0, a suitable string will be saved
  void write_object( vsl_b_ostream& is, const BaseClass* b);

  //: Prints summary of object state to stream given base class pointer
  //  Determines which derived class object is
  //  and calls the appropriate print summary function.
  //  (Class must be one given to Loader by the add method).
  //  If b==0, a suitable string will be saved
  void print_object_summary( std::ostream& os, const BaseClass* b);
};


#if 0
//: Loads object and sets base class pointer
//  Determines which derived class object on bfs belongs
//  to, loads it and sets b to be a pointer to it.
//  (Class must be one given to Loader by the append method).
//  If bfs indicates a NULL pointer, b will be set to NULL.
//  If b not initially NULL, *b will be deleted.
inline void vsl_b_read( vsl_b_istream& bfs, BaseClass*& b)
{
  vsl_clipon_binary_loader<BaseClass,BaseClassIO>::
    instance().load_object(bfs,b);
}
#endif // 0

#endif // vsl_clipon_binary_loader_h_
