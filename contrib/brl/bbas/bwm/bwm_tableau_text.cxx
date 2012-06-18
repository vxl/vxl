#include "bwm_tableau_text.h"

#include <vcl_iostream.h>
#include <vcl_fstream.h>

void bwm_tableau_text::set_text(vcl_string filename)
{
  if (filename.empty()) {
    vcl_cout << "Bad filename\n";
    return;
  }

  vcl_ifstream is(filename.data());
  if (!is.is_open())  {
    vcl_cout << "Can't open file\n";
    return;
  }

  vcl_string str;
  unsigned length = ydim_ - yinc_;
  int ypos = ymarg_;
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
    add((float)xmarg_, (float)ypos, s);
    ypos += 15;
  }
}

void bwm_tableau_text::set_string(vcl_string & str)
{
  this->set_colour(1, 1, 1);
  int ypos = ymarg_;
  unsigned int s_pos=0, f_pos;
  while (ypos+ymarg_<ydim_) {
    f_pos= str.find('\n',s_pos);
    if (f_pos >= str.size())
      break;
    unsigned nc = f_pos-s_pos;
    vcl_string s = str.substr(s_pos, nc);
    add((float)xmarg_, (float)ypos, s.c_str());
    ypos += 15;
    s_pos = f_pos+1;
    if (s_pos >= str.size())
      break;
  }
}
