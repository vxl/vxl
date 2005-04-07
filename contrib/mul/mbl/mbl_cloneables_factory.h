// This is mul/mbl/mbl_cloneables_factory.h
#ifndef mbl_cloneables_factory_h
#define mbl_cloneables_factory_h
//:
//  \file
// \brief A general factory pattern.
// \author Ian Scott.

#include <vcl_cassert.h>
#include <vcl_map.h>
#include <vcl_utility.h>
#include <vcl_memory.h>
#include <vcl_string.h>
#include <vcl_sstream.h>
#include <mbl/mbl_exception.h>
#include <mbl/mbl_cloneable_ptr.h>

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
//   virtual vcl_string BASE::is_a() const; {
//     return "BASE";} // Derived classes need unique names.
// }
// \endcode
//
// Example:
// \code
// mbl_cloneables_factory<vimt_image>::add(vimt_image_2d());
// mbl_cloneables_factory<vimt_image>::add(vimt_image_3d());
//
// vcl_auto_ptr<vimt_image> p = mbl_cloneables_factory<vimt_image>::get("vimt_image_2d()");
// assert(dynamic_cast<vimt_image_2d>(p));
// \endcode


template <class BASE>
class mbl_cloneables_factory
{
 private:
  typedef vcl_map<vcl_string, mbl_cloneable_ptr<BASE> > MAP;

  //: Singleton array of names, and association concrete instantiations of BASE.
  static vcl_auto_ptr<MAP> objects_;

 private:

  //: Get singleton instance.
  static MAP &objects()
  {
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
  static void add(const BASE & object, const vcl_string & name)
  {
    objects()[name] = object;
  }

  //: Get a pointer to a new copy of the object identified by name.
  // An exception will be thrown if name does not exist.
  static vcl_auto_ptr<BASE > get_clone(const vcl_string & name)
  {
    typedef VCL_DISAPPEARING_TYPENAME MAP::const_iterator IT;

    IT found = objects().find(name);
    const IT end = objects().end();

    if (found == end)
    {
      vcl_ostringstream ss;
      IT it = objects().begin();
      if (!objects().empty())
      {
        ss << it->first;
        while ( ++it != end)
          ss << ", " << it->first;
      }
      mbl_exception_error(mbl_exception_no_name_in_factory(name, ss.str()));
      return vcl_auto_ptr<BASE >();
    }
    return vcl_auto_ptr<BASE >(found->second->clone());
  }
};

// Macro to instantiate template, and initialise singleton data item.
#define MBL_CLONEABLES_FACTORY_INSTANTIATE(T) \
template class mbl_cloneables_factory< T >; \
template <class BASE > \
vcl_auto_ptr<VCL_DISAPPEARING_TYPENAME mbl_cloneables_factory<BASE >::MAP > \
  mbl_cloneables_factory<BASE >::objects_ =  \
    vcl_auto_ptr<VCL_DISAPPEARING_TYPENAME mbl_cloneables_factory<BASE >::MAP >(0)

#endif  // mbl_cloneables_factory_h
