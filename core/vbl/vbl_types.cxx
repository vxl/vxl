#define vbl_types_nowarn
#include <vbl/vbl_types.h>

vbl_attribute_value_pair* vbl_attribute_value_pair::Copy() const
{
  vbl_attribute_value_pair* r = new vbl_attribute_value_pair;
  r->value = value;
  r->label = label;
  return r;
}

void Delete(vbl_attribute_value_pair* d)
{
  delete d;
}

#ifndef NEW_MENUS_
#ifdef __GNUC__
vbl_attribute_value_pair vbl_attribute_value_pair::null = {(char*)0,(void*)0};
#else
void* vbl_attribute_value_pair::null = (void*)0;
#endif
#else
vbl_attribute_value_pair vbl_attribute_value_pair::null = vbl_attribute_value_pair();
#endif
