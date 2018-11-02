#include <iostream>
#include <fstream>
#include "bwm_tableau_text.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

void bwm_tableau_text::set_text(std::string filename)
{
  if (filename.empty()) {
    std::cout << "Bad filename\n";
    return;
  }

  std::ifstream is(filename.data());
  if (!is.is_open())  {
    std::cout << "Can't open file\n";
    return;
  }

  std::string str;
  unsigned length = ydim_ - yinc_;
  int ypos = ymarg_;
  char* s = new char(length);
  this->clear();
  this->set_size(1);
  while (!is.eof()) {
    is.getline(s, length);
    std::cout << s << std::endl;
    if (s[0] == '-')
      this->set_colour(1, 0, 0);
    else
      this->set_colour(1, 1, 1);
    add((float)xmarg_, (float)ypos, s);
    ypos += 15;
  }
}

void bwm_tableau_text::set_string(std::string & str)
{
  this->set_colour(1, 1, 1);
  int ypos = ymarg_;
  unsigned int s_pos=0, f_pos;
  while (ypos+ymarg_<ydim_) {
    f_pos= str.find('\n',s_pos);
    if (f_pos >= str.size())
      break;
    unsigned nc = f_pos-s_pos;
    std::string s = str.substr(s_pos, nc);
    add((float)xmarg_, (float)ypos, s.c_str());
    ypos += 15;
    s_pos = f_pos+1;
    if (s_pos >= str.size())
      break;
  }
}
