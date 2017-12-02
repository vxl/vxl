#ifndef bwm_tableau_text_h_
#define bwm_tableau_text_h_

#include <vgui/vgui_text_tableau.h>

class bwm_tableau_text: public vgui_text_tableau
{
public:
  // if you do not ref(), it kills the class instance after you add a line of text!!!
  bwm_tableau_text(int xdim =500, int ydim=500, int xmarg=50, int ymarg=50, int yinc=15)
    : xdim_(xdim), ydim_(ydim), xmarg_(xmarg), ymarg_(ymarg), yinc_(yinc) { this->ref(); }

  virtual ~bwm_tableau_text() { this->unref(); }

  void set_text(std::string filename);
  void set_string(std::string& str);
private:
  int xdim_, ydim_;
  int xmarg_, ymarg_, yinc_;
};

#endif
