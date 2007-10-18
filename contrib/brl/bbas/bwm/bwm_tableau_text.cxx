#include "bwm_tableau_text.h"

#include <vcl_iostream.h>

void bwm_tableau_text::set_text(vcl_string filename)
{
  if (filename.empty()) {
      vcl_cout << "Bad filename\n";
      return;
  }

  vcl_ifstream is(filename.data());
  if(!is.is_open())  {
    vcl_cout << "Can't open file\n";
    return;
  }

  vcl_string str;
  unsigned length = ydim_ - yinc_;
  float ypos = ymarg_;
  char* s = new char(length);
  this->clear();
  this->set_size(1);
  while (!is.eof()) {
    is.getline(s, length);
    vcl_cout << s << vcl_endl;
    if (s[0] == '-')
      this->set_colour(1, 0, 0);
    else 
      this->set_colour(1, 1, 1);
    add(xmarg_, ypos, s);
    ypos += 15;
  }
}
