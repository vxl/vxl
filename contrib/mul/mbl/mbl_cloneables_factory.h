// This is mul/mbl/mbl_cloneables_factory.h
#ifndef mbl_cloneables_factory_h
#define mbl_cloneables_factory_h
//:
//  \file

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
// Example:
// \code
// mbl_cloneables_factory<vimt_image>::add(vimt_image_2d());
// mbl_cloneables_factory<vimt_image>::add(vimt_image_3d());
// 
// vimt_image *p = mbl_cloneables_factory<vimt_image>::get("vimt_image_2d()");
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
    {
      MAP * p = new MAP;
      objects_ = vcl_auto_ptr<MAP>(p);
    }

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
  // The caller is responsible for the object's deletion.

  // There MUST be an object labelled name in the factory,
  // otherwise behaviour is undefined.
  static BASE* get(const vcl_string & name)
  {
    MAP::iterator found = objects().lower_bound(name);

    if (found == objects().end())
    {
      vcl_ostringstream ss;
      MAP::const_iterator it = objects().begin();
      if (!objects().empty())
      {
        ss << it->first;
        const MAP::const_iterator end = objects().end();
        while ( ++it != end)
          ss << it->first << ", ";
      }
      mbl_exception_warning(mbl_exception_no_name_in_factory(name, ss.str()));
      return 0;
    }
    return found->second->clone();
  }

};


#define MBL_CLONEABLES_FACTORY_INSTANTIATE(T) \
template class mbl_cloneables_factory< T >; \
template <class BASE> \
vcl_auto_ptr<mbl_cloneables_factory<BASE>::MAP> \
  mbl_cloneables_factory<BASE>::objects_ =  \
    vcl_auto_ptr<mbl_cloneables_factory<BASE>::MAP>(0);

#endif  // mbl_cloneables_factory_h
