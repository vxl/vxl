// This is core/vgui/vgui_tag.cxx
//:
// \file
// \author fsm
// \brief  See vgui_tag.h for a description of this file.

#include "vgui_tag.h"
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#define MAXTAGS 255

// POD
static unsigned numtags = 0;
static vgui_tag_function tags[MAXTAGS+1];

int vgui_tag_add(vgui_tag_function f, char const * /*tk*/) {
  if (f) {
    assert(numtags < MAXTAGS);
    tags[numtags++] = f;
  }
  return numtags;
}

vgui_tag_function const *vgui_tag_list() {
  tags[numtags] = nullptr;
  return tags;
}

void vgui_tag_call() {
  for (unsigned i=0; i<numtags; ++i)
    (*tags[i])();
  numtags = 0;
}
