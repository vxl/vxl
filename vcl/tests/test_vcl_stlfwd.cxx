
#include <vcl/vcl_string.h> // C++ specific includes first

#if TEST == 1
// Normal
#include <vcl/vcl_stlfwd.h>

#else
#if TEST == 2
// stl included first

#include <vcl/vcl_map.h>
#include <vcl/vcl_set.h>
#include <vcl/vcl_list.h>
#include <vcl/vcl_stlfwd.h>

#else
// STL included later
#include <vcl/vcl_stlfwd.h>
#include <vcl/vcl_map.h>
#include <vcl/vcl_set.h>
#include <vcl/vcl_list.h>

#endif
#endif

void f(vcl_map<int, vcl_string,vcl_less<int> >*,
       vcl_set<int,vcl_less<int> >*,
       vcl_list<int>*
       )
{
}


main()
{
}
