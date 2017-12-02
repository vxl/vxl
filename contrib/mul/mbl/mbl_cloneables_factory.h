// This is mul/mbl/mbl_cloneables_factory.h
#ifndef mbl_cloneables_factory_h
#define mbl_cloneables_factory_h
//:
//  \file
// \brief A general factory pattern.
// \author Ian Scott.

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <sstream>
#include <vcl_compiler.h>
#include <mbl/mbl_exception.h>
#include <mbl/mbl_cloneable_ptr.h>
#include <mbl/mbl_export.h>

//=======================================================================
//: A general factory pattern.
// After templating this on a base class, and loading in
// a bunch of concrete instantiations of the derived classes,
// you can create any of the derived classes simply from
// the class's name.
//
// BASE must define a clone() and is_a() members
// \code
// class BASE
// {
//   ...
// public:
//   virtual BASE* clone() const=0; // Derived classes must copy themselves.
//   virtual std::string BASE::is_a() const; {
//     return "BASE";} // Derived classes need unique names.
// }
// \endcode
//
// Example:
// \code
// mbl_cloneables_factory<vimt_image>::add(vimt_image_2d());
// mbl_cloneables_factory<vimt_image>::add(vimt_image_3d());
//
// std::auto_ptr<vimt_image> p = mbl_cloneables_factory<vimt_image>::get_clone("vimt_image_2d()");
// assert(dynamic_cast<vimt_image_2d>(p));
// \endcode


template <class BASE>
class mbl_cloneables_factory
{
 private:
  typedef std::map<std::string, mbl_cloneable_ptr<BASE> > MAP;

  //: Singleton array of names, and association concrete instantiations of BASE.

 private:

  //: Get singleton instance.
  static MAP &objects()
  {
    static std::auto_ptr<MAP> objects_;
    if (objects_.get() == 0)
      objects_.reset(new MAP);

    return *objects_;
  }

 public:

  //: Add an object for later cloning by the factory.
  // Use the object's class name via the is_a() member.
  static void add(const BASE & object) { add(object, object.is_a()); }

  //: Add an object for later cloning by the factory.
  // If there already is an object called name, it will
  // be overwritten.
  static void add(const BASE & object, const std::string & name)
  {
    objects()[name] = object;
  }

  //: Get a pointer to a new copy of the object identified by name.
  // An exception will be thrown if name does not exist.
  static std::auto_ptr<BASE > get_clone(const std::string & name)
  {
    typedef typename MAP::const_iterator IT;

    IT found = objects().find(name);
    const IT end = objects().end();

    if (found == end)
    {
      std::ostringstream ss;
      IT it = objects().begin();
      if (!objects().empty())
      {
        ss << it->first;
        while ( ++it != end)
          ss << ", " << it->first;
      }
      mbl_exception_error(mbl_exception_no_name_in_factory(name, ss.str()));
      return std::auto_ptr<BASE >();
    }
    return std::auto_ptr<BASE >(found->second->clone());
  }
};

// Macro to instantiate template, and initialise singleton data item.
#define MBL_CLONEABLES_FACTORY_INSTANTIATE(T) \
template class mbl_cloneables_factory< T >

#endif  // mbl_cloneables_factory_h
