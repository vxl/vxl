// This is core/vgui/vgui_tag.h
#ifndef vgui_tag_h_
#define vgui_tag_h_
//:
// \file
// \brief  Allow clients to register 'tag functions' which are called later.
// \author fsm
//
// Real Purpose:
//
// An impl library "blah" should register a tag function at library
// initialization time. The old method, where a library had a line
// something like
// \code
//   static vgui_blah *init_dummy = new vgui_blah;
// \endcode
// caused problems (segv) if a vgui_blah uses run-time libraries which
// have not yet been initialized. No particular order of library
// initialization may be assumed, so static objects in libA might not
// yet have been constructed when libB is initialized.
//
// The workaround provided by vgui_tag is to make impl libraries register
// tag functions instead. Class vgui then promises to call all the
// registered tag functions near the beginning of vgui::init(); There
// is no problem with registering function pointers, as these are POD
// (Plain Old Data). Thus, vgui_blah_tag.cxx now looks like this :
// \code
//   static int vgui_blah_function() { new vgui_blah; return 0; }
//   int vgui_blah_tag = vgui_tag_add(vgui_blah_function, "blah");
// \endcode
// Thus, when 'vgui_blah_tag' is initialized, the tag function called
// 'vgui_blah_tag_function' is registered. There is no instance of
// vgui_blah created until that tag function is called by vgui::init().
//
// \verbatim
//  Modifications
//   07-AUG-2002 K.Y.McGaul - Changed to Doxygen style comments.
// \endverbatim

//: The type of a tag function.
typedef int (*vgui_tag_function)(void);

//: Registers a tag function - returns 0.
int vgui_tag_add(vgui_tag_function, char const *);

//: Returns null-terminated list of tag functions.
vgui_tag_function const *vgui_tag_list();

//: Call all registered tag functions and remove them from the list.
void                     vgui_tag_call();

// Easy macro. It will provide two external symbols
//  int vgui_${tk}_tag;
//  int vgui_${tk}_tag_function();
// which may be pulled from the link line. The
// function is idempotent in the sense that calling
// it multiple times will not have any further side
// effects and will given the same return value.
#define vgui_tag(tk) \
int vgui_##tk##_tag_function(); \
int vgui_##tk##_tag = vgui_tag_add(vgui_##tk##_tag_function, #tk); \
static int vgui_##tk##_tag_function_(); \
int vgui_##tk##_tag_function() { \
  static bool once = false; \
  static int  value = 0; \
  if (!once) \
    value = vgui_##tk##_tag_function_(); \
  once = true; \
  return value; \
} \
int vgui_##tk##_tag_function_()

#endif // vgui_tag_h_
