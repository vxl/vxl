// This is core/vgui/vgui_key.h
#ifndef vgui_key_h_
#define vgui_key_h_
//:
// \file
// \brief Enumeration of possible key strokes.

#include <iosfwd>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Enumeration of possible key strokes.
enum vgui_key
{
  vgui_KEY_NULL = 0,
  vgui_TAB='\t',
  vgui_NEWLINE='\n',
  vgui_RETURN='\r',
  vgui_ESC=27, /* */ vgui_ESCAPE=27,
  vgui_space=' ', // 32
  vgui_EXCLAM='!', // 33
  vgui_HASH='#', // 35
  vgui_DOLLAR='$', // 36
  vgui_PERCENT='%', // 37
  vgui_AMPERSAND='&', // 38
  vgui_STAR='*', // 42
  vgui_PLUS='+', // 43
  vgui_COMMA=',', // 44
  vgui_MINUS='-', // 45
  vgui_POINT='.', // 46
  vgui_SLASH='/', // 47
  vgui_LESS='<', // 60
  vgui_EQUALS='=', // 61
  vgui_GREATER='>', // 62
  vgui_QUESTION='?', // 63
  vgui_LEFTBRACKET='[', // 91
  vgui_RIGHTBRACKET=']', // 93
  vgui_CHAR_a='a', // 97
  vgui_CHAR_b='b', // 98
  vgui_CHAR_c='c', // 99
  vgui_CHAR_d='d', // 100
  vgui_CHAR_e='e', // 101
  vgui_CHAR_f='f', // 102
  vgui_CHAR_g='g', // 103
  vgui_CHAR_h='h', // 104
  vgui_CHAR_i='i', // 105
  vgui_CHAR_j='j', // 106
  vgui_CHAR_k='k', // 107
  vgui_CHAR_l='l', // 108
  vgui_CHAR_m='m', // 109
  vgui_CHAR_n='n', // 110
  vgui_CHAR_o='o', // 111
  vgui_CHAR_p='p', // 112
  vgui_CHAR_q='q', // 113
  vgui_CHAR_r='r', // 114
  vgui_CHAR_s='s', // 115
  vgui_CHAR_t='t', // 116
  vgui_CHAR_u='u', // 117
  vgui_CHAR_v='v', // 118
  vgui_CHAR_w='w', // 119
  vgui_CHAR_x='x', // 120
  vgui_CHAR_y='y', // 121
  vgui_CHAR_z='z', // 122
  vgui_LEFTBRACE='{', // 123
  vgui_RIGHTBRACE='}', // 125
  vgui_KEY_XXX=0x100, // non-ascii encoded keys below :
  vgui_F1, vgui_F2 , vgui_F3 , vgui_F4 , // function keys
  vgui_F5, vgui_F6 , vgui_F7 , vgui_F8 ,
  vgui_F9, vgui_F10, vgui_F11, vgui_F12,
  vgui_CURSOR_LEFT, vgui_CURSOR_UP, vgui_CURSOR_RIGHT, vgui_CURSOR_DOWN,  // cursor movement
  vgui_PAGE_UP, /* */ vgui_PGUP=vgui_PAGE_UP,
  vgui_PAGE_DOWN, /* */ vgui_PGDN = vgui_PAGE_DOWN,
  vgui_HOME, vgui_END,
  vgui_DELETE, vgui_INSERT  // other things
};

//: Given a letter character, this function returns the control character.
//
//  Use vgui_key_CTRL('a') or vgui_key_CTRL('A') to signify control+A.
//  Note, the vgui_key in a vgui_event is always a lower case letter (see
//  vgui_event.h), so don't compare event.key to a vgui_key_CTRL'ed letter.
//  You may compare a vgui_key_CTRL'ed to an event.ascii_key.
vgui_key vgui_key_CTRL(int character);

std::ostream& operator<<(std::ostream& s, vgui_key k);

#endif // vgui_key_h_
