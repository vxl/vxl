#ifndef vgui_message_h_
#define vgui_message_h_
/*
  fsm@robots.ox.ac.uk
*/
// .NAME vgui_message
// .INCLUDE vgui/vgui_message.h
// .FILE vgui_message.cxx
//
// .SECTION Modifications
//pcp - I removed the type and user fields and replaced them with
//      a vcl_string. this is because I think the types of message that
//      will be sent are much more diverse than the event types
//      and so shouldn't follow that model. It is more difficult
//      to create a unique user tag than it is to put a different msg into the
//      vcl_string.
//      Also I have changed the pointer to a vgui_observable to a void*
//      This is because the observable class doesn't contain any useful info
//      as far as the observer is concerned and a static cast to another class
//      (usually some tableau subclass) would be necessary - and would be
//      based on the contents of the message. Observable is only useful as a mixin
//      class to provide observer management. I don't think it should actually store
//      observers though as some classes may not store their observers locally.
//
//fsm - I have a license from pcp to put them back in, so I might do that without
//      warning.
//
//fsm - now I've done it.
//


class vgui_message
{
public:
  vgui_message();

  // pointer to sender. when the message was broadcast from a vgui_observer
  // via the notify() method, this will point to the observer.
  const void *from;
  
  // this field must uniquely identify the type of message sent. usually it
  // will point to some static POD somewhere.
  const void *user; 

  // extra data can be packaged into this. it is up to the sender of the 
  // message to ensure that 'data' can be safely cast to whatever the 
  // receiver expects when receiving a message with a particular value of
  // 'user' set.
  const void *data;
};

#endif // vgui_message_h_
