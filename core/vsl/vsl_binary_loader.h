// This is core/vsl/vsl_binary_loader.h
#ifndef vsl_binary_loader_h_
#define vsl_binary_loader_h_
//:
// \file
// \brief Loader to do Polymorphic IO.
// \author Ian Scott, Tim Cootes (Manchester) March 2001
//
// You should include this file if you want to do polymorphic IO
// (i.e. save a class by its base-class pointer.)

#include <vector>
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_binary_loader_base.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Class to load objects by baseclass pointer.
// An example of a singleton design pattern for loading
// a DerivedClass from a stream into a BaseClass*.
// All we are given is a BaseClass* into which
// the object has to be loaded but we can only tell
// what sort of object it is from the is_a() information
// stored in the stream. To handle this we define a loader
// which has a list of BaseClass pointers,
// and the ChainOfResponsibility (Design Pattern)
// approach is used to load the object i.e. each
// ptr->is_a() is matched against the string on the stream
// until we find a match or run out of pointers. If
// a pointer is found which matches the string on
// the stream, we clone it, use the clone to load the object
// from the stream, then return a pointer to the clone.
//
// We use a singleton so that there is only one list of
// concrete derived classes which can be added
// to for loading purposes. If you derive a new
// class you just have to append it to the list of
// classes of the singleton, viz:
//     vsl_binary_loader::instance().append(my_object)
// The BaseClass MUST implement is_a(),is_class(),clone(),b_write() and
// b_read() virtual functions.
//
// vsl_binary_loader.h also provides the function templates for loading
// and saving by base class pointer
//
// All loader singletons can be deleted using vsl_delete_all_loaders()
template<class BaseClass>
class vsl_binary_loader : public vsl_binary_loader_base
{
  //: the singleton object
  static vsl_binary_loader<BaseClass>* instance_;

  //: List of concrete classes that this loader can deal with
  std::vector<BaseClass*> object_;

 public :
  //: Constructor
  vsl_binary_loader() = default;

  //: Destructor
  ~vsl_binary_loader() override;

  //: Returns the instance variable for the singleton.
  static vsl_binary_loader<BaseClass>& instance();

  //: Remove all example objects
  void make_empty();

  //: Add example object to list of those that can be loaded
  void add( const BaseClass& b) { object_.push_back(b.clone());}

  //: Return current list of class objects which can be loaded
  const std::vector<BaseClass*>& object() { return object_; }

  //: Loads object and sets base class pointer
  //  Determines which derived class object on is belongs
  //  to, loads it and sets b to be a pointer to it.
  //  (Class must be one given to Loader by the add method).
  //  If is indicates a NULL pointer, b will be set to NULL.
  //  If b not initially NULL, *b will be deleted.
  void load_object( vsl_b_istream& is, BaseClass*& b);

  std::string is_a() const;
};


//: Loads object and sets base class pointer
//  Determines which derived class object on bfs belongs
//  to, loads it and sets b to be a pointer to it.
//  (Class must be one given to Loader by the append method).
//  If bfs indicates a NULL pointer, b will be set to NULL.
//  If b not initially NULL, *b will be deleted.
template<class BaseClass>
inline void vsl_b_read( vsl_b_istream& bfs, BaseClass*& b)
{
  vsl_binary_loader<BaseClass>::instance().load_object(bfs,b);
}

//: Binary file stream output operator for pointer to class
//  This works correctly even if b is a NULL pointer
template<class BaseClass>
void vsl_b_write(vsl_b_ostream& bfs, const BaseClass* b);

#endif // vsl_binary_loader_h_
