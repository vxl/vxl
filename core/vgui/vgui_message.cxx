// This is oxl/vgui/vgui_message.cxx
#include "vgui_message.h"
//:
// \file
// \author fsm@robots.ox.ac.uk
// \brief  See vgui_message.h for a description of this file.

vgui_message::vgui_message()
  : from(0)
  , user(0)
  , data(0)
{
}

// example :
//
// class sender : public vgui_observable { 
//   static const char begin[];
//   static const char end[];
//   void f() { 
//     vcl_string moo="moo moo moo";
//     vgui_message m;
//     m.from = this;
//     m.user = sender::begin;
//     m.data = &moo;
//     notify(m);
//   }
//   void g() {
//     vgui_message m;
//     m.from = this;
//     m.user = sender::end;
//     m.data = 0;
//     notify(m);
//   }
// };
//
// class receiver : public vgui_observer {
//   void update(const vgui_message &m) {
//     if (m.user == sender::begin) {
//       vcl_string *s = static_cast<vcl_string*>(m.data);
//       vcl_cerr << "begin : " << *s << vcl_endl;
//     }
//     else if (m.user == sender::end) {
//       sender *s = static_cast<sender*>(m.from);
//       vcl_cerr << "end" << vcl_endl;
//     }
//     else {
//       // dunno
//     }
//   }
// };
//
