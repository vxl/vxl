// This is core/vsl/vsl_indent.cxx
//:
// \file

#include <iostream>
#include <map>
#include <mutex>
#include <utility>
#include "vsl_indent.h"
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif

constexpr int default_tab = 2;

using indent_data_type = std::pair<int, int>;
using maps2i_type = std::map<void *, indent_data_type, std::less<void *>>;

// Holder bundling the indent map with the mutex that guards it.
// Bundling the mutex with the data in one Meyers singleton guarantees
// they share lifetime, side-stepping the destruction-order fiasco
// that two separate function-local statics would risk.
struct vsl_indent_storage
{
  std::mutex mtx;
  maps2i_type indent_data_map;
};

static vsl_indent_storage &
vsl_indent_state()
{
  static vsl_indent_storage s;
  return s;
}

// Get pointer to tab and indent data for os.
//
// CAVEAT: callers must use the returned pointer only while no other
// thread is invoking any vsl_indent_* helper on the SAME ostream.
// The internal map is mutex-guarded for insertion, but the returned
// indent_data_type* aliases storage inside the map, and concurrent
// indent_data() calls on the same ostream would race on that data.
// In practice every vsl_b_* operator threads through these helpers
// against a single per-thread vsl_b_*stream, so the per-stream
// access pattern is naturally single-threaded. The mutex protects
// against concurrent insertion of new ostream entries, which is the
// race that previously could corrupt the map's internal red-black
// tree.
indent_data_type *
indent_data(std::ostream & os)
{
  auto & state = vsl_indent_state();
  std::lock_guard<std::mutex> guard(state.mtx);

  auto & m = state.indent_data_map;
  auto entry = m.find(&os);
  if (entry == m.end())
  {
    // Create a new entry
    m[&os] = indent_data_type(0, default_tab);
    entry = m.find(&os);
  }

  return &((*entry).second);
}

//: Increments current indent for given stream
void
vsl_indent_inc(std::ostream & os)
{
  indent_data(os)->first++;
}

//: Decrements current indent for given stream
void
vsl_indent_dec(std::ostream & os)
{
  indent_data(os)->first--;
}

//: Set number of spaces per increment step
void
vsl_indent_set_tab(std::ostream & os, int t)
{
  indent_data(os)->second = t;
}

//: Number of spaces per increment step
int
vsl_indent_tab(std::ostream & os)
{
  return indent_data(os)->second;
}

//: Set indentation to zero
void
vsl_indent_clear(std::ostream & os)
{
  indent_data(os)->first = 0;
}

std::ostream &
operator<<(std::ostream & os, const vsl_indent & /*indent*/)
{
  const indent_data_type * const data = indent_data(os);

  const int n = data->first * data->second;
  for (int i = 0; i < n; i++)
    os << ' ';
  return os;
}

//: Tidy up the internal indent map to remove potential memory leaks
//  The details of indents for each stream are stored in a static
//  map.  When testing for memory leaks, this is flagged, creating
//  lots of noise in the output of memory leak checkers.
//  This call empties the map, removing the potential leak.
//  Pragmatically it is called in the vsl_delete_all_loaders()
//
//  This should no longer be needed, since that static map was made a static
//  inside the function indent_data() instead of a global one. - PVr.
void
vsl_indent_clear_all_data()
{}

// removed explicit instantiation of map<void*, pair<int, int> > -- fsm.
