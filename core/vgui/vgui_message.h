// This is core/vgui/vgui_message.h
#ifndef vgui_message_h_
#define vgui_message_h_
//:
// \file
// \author  fsm
// \brief   Used to send messages from observables to observers.
//
// \verbatim
//  Modifications
//   pcp - I removed the type and user fields and replaced them with
//      a vcl_string. this is because I think the types of message that
//      will be sent are much more diverse than the event types
//      and so shouldn't follow that model. It is more difficult
//      to create a unique user tag than it is to put a different msg into the
//      vcl_string.
//      Also I have changed the pointer to a vgui_observable to a void*
//      This is because the observable class doesn't contain any useful info
//      as far as the observer is concerned and a static cast to another class
//      (usually some tableau subclass) would be necessary - and would be
//      based on the contents of the message. Observable is only useful as a
//      mixin class to provide observer management. I don't think it should
//      actually store observers though as some classes may not store their
//      observers locally.
//
//   fsm - I have a license from pcp to put them back in, so I might do that
//         without warning.
//
//   fsm - now I've done it.
//   10-SEP-2004 Peter Vanroose  Inlined all 1-line methods in class decl
// \endverbatim

//: Used to send messages from observables to observers.
//
// example :
//
// \code
// class sender : public vgui_observable
// {
//   static const char begin[];
//   static const char end[];
//   void f()
//   {
//     vcl_string moo="moo moo moo";
//     vgui_message m;
//     m.from = this;
//     m.user = sender::begin;
//     m.data = &moo;
//     notify(m);
//   }
//   void g()
//   {
//     vgui_message m;
//     m.from = this;
//     m.user = sender::end;
//     m.data = 0;
//     notify(m);
//   }
// };
//
// class receiver : public vgui_observer
// {
//   void update(const vgui_message &m)
//   {
//     if (m.user == sender::begin)
//     {
//       vcl_string *s = static_cast<vcl_string*>(m.data);
//       vcl_cerr << "begin : " << *s << vcl_endl;
//     }
//     else if (m.user == sender::end)
//     {
//       sender *s = static_cast<sender*>(m.from);
//       vcl_cerr << "end\n";
//     }
//     else
//     {
//       // dunno
//     }
//   }
// };
// \endcode
class vgui_message
{
 public:
  vgui_message() : from(0), user(0), data(0) {}

  //: Pointer to sender.
  //  When the message was broadcast from a vgui_observer
  //  via the notify() method, this will point to the observer.
  const void *from;

  //: This field must uniquely identify the type of message sent.
  //  Usually it will point to some static POD somewhere.
  const void *user;

  //: Extra data can be packaged into this.
  //  It is up to the sender of the message to ensure that 'data' can be
  //  safely cast to whatever the receiver expects when receiving a message
  //  with a particular value of 'user' set.
  const void *data;
};

#endif // vgui_message_h_
