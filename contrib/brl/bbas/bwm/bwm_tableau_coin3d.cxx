#include "bwm_tableau_coin3d.h"
#include "bwm_tableau_mgr.h"
#include "bwm_observer_mgr.h"
#include "bwm_tableau_text.h"

#include <vgui/vgui_dialog.h>
#include <vgui/vgui_text_tableau.h>
#include <vgui/vgui_text_tableau_sptr.h>
#include <vgui/vgui_text_put.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>
//----------------------------------------------------------------------------
class bwm_coin3d_move_command : public vgui_command
{
 public:
  bwm_coin3d_move_command(bwm_tableau_coin3d* t) : tab(t) {}
  void execute() { tab->move(); }

  bwm_tableau_coin3d *tab;
};

//----------------------------------------------------------------------------
class bwm_coin3d_extrude_command : public vgui_command
{
 public:
  bwm_coin3d_extrude_command(bwm_tableau_coin3d* t) : tab(t) {}
  void execute() { tab->extrude_face(); }

  bwm_tableau_coin3d *tab;
};

class bwm_coin3d_divide_command : public vgui_command
{
 public:
  bwm_coin3d_divide_command(bwm_tableau_coin3d* t) : tab(t) {}
  void execute() { tab->divide_face(); }

  bwm_tableau_coin3d *tab;
};

class bwm_coin3d_help_command : public vgui_command
{
 public:
  bwm_coin3d_help_command(bwm_tableau_coin3d* t) : tab(t) {}
  void execute() { tab->help_pop(); }

  bwm_tableau_coin3d *tab;
};

void bwm_tableau_coin3d::get_popup(vgui_popup_params const &params, vgui_menu &menu)
{
  vgui_menu submenu;
  submenu.add( "Move (Camera Direction)", new bwm_coin3d_move_command(this));
  submenu.add( "Extrude Face", new bwm_coin3d_extrude_command(this));
  submenu.add( "Divide Face", new bwm_coin3d_divide_command(this));
  submenu.add( "Help", new bwm_coin3d_help_command(this), (vgui_key)'?');

  //add this submenu to the popup menu
  menu.separator();
  menu.add("Coin3D Modeling", submenu);
}

void bwm_tableau_coin3d::move()
{
}

void bwm_tableau_coin3d::extrude_face()
{
  my_observer_->extrude();
}

void bwm_tableau_coin3d::divide_face()
{
  my_observer_->divide();
}

void bwm_tableau_coin3d::create_inner_face()
{
}

void bwm_tableau_coin3d::help_pop()
{
  bwm_tableau_text* text = new bwm_tableau_text(500, 500);

  text->set_text("C:\\lems\\lemsvxlsrc\\lemsvxlsrc\\contrib\\bwm\\doc\\HELP_coin3d.txt");
  vgui_tableau_sptr v = vgui_viewer2D_tableau_new(text);
  vgui_tableau_sptr s = vgui_shell_tableau_new(v);
  vgui_dialog popup("COIN3D TABLEAU HELP");
  popup.inline_tableau(s, 550, 550);
  if (!popup.ask())
    return;
}

