// This is mul/mbl/mbl_cloneables_factory.h
#ifndef mbl_cloneables_factory_h
#define mbl_cloneables_factory_h

#include "mbl_cloneables_factory_def.h"

template <class BASE>
void mbl_cloneables_factory<BASE>::add(const BASE & object)
{
  add(object, object.is_a());
}

template <class BASE>
void mbl_cloneables_factory<BASE>::add(const BASE & object, const std::string & name)
{
  objects()[name] = object;
}

template <class BASE>
std::unique_ptr<BASE > mbl_cloneables_factory<BASE>::get_clone(const std::string & name)
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
    return std::unique_ptr<BASE >();
  }
  return std::unique_ptr<BASE >(found->second->clone());
}

#endif  // mbl_cloneables_factory_h
