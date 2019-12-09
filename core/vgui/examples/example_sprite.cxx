#include "vul/vul_arg.h"

#include "vil/vil_load.h"
#include "vil/vil_image_view.h"

#include "vgui/vgui.h"
#include "vgui/vgui_shell_tableau.h"
#include "vgui/vgui_image_tableau.h"
#include "vgui/vgui_easy2D_tableau.h"
#include "vgui/vgui_viewer2D_tableau.h"
#include "vgui/vgui_soview2D.h"

vgui_event_condition left_event(vgui_key('h'));
vgui_event_condition right_event(vgui_key('l'));
vgui_event_condition up_event(vgui_key('k'));
vgui_event_condition down_event(vgui_key('j'));

struct move_sprite_tableau : public vgui_wrapper_tableau
{
  move_sprite_tableau(vgui_tableau_sptr tab, vgui_soview2D * so)
    : vgui_wrapper_tableau(tab)
    , so_view_(so)
  {}

  bool
  handle(vgui_event const & e)
  {
    if (left_event(e))
    {
      so_view_->translate(-5.0f, 0.0f);
      post_redraw();
      return true;
    }
    else if (right_event(e))
    {
      so_view_->translate(+5.0f, 0.0f);
      post_redraw();
      return true;
    }
    else if (up_event(e))
    {
      so_view_->translate(0.0f, -5.0f);
      post_redraw();
      return true;
    }
    else if (down_event(e))
    {
      so_view_->translate(0.0f, +5.0f);
      post_redraw();
      return true;
    }
    else
      return vgui_wrapper_tableau::handle(e);
  }

  vgui_soview2D * so_view_;
};

typedef vgui_tableau_sptr_t<move_sprite_tableau> move_sprite_tableau_sptr;

struct move_sprite_tableau_new : public move_sprite_tableau_sptr
{
  move_sprite_tableau_new(vgui_tableau_sptr tab, vgui_soview2D * so)
    : move_sprite_tableau_sptr(new move_sprite_tableau(tab, so))
  {}
};

int
main(int argc, char ** argv)
{
  vul_arg<char *> background("-bg", "Background image", nullptr);
  vul_arg<char *> sprite("-fg", "Foreground sprite image", nullptr);

  vgui::init(argc, argv);
  vul_arg_parse(argc, argv);

  vil_image_view_base_sptr back_img;
  vil_image_view_base_sptr sprite_img;

  if (background.set())
  {
    back_img = vil_load(background());
    if (!back_img)
      std::cerr << "Couldn't load background image " << background() << '\n';
  }
  if (!back_img)
  {
    vil_image_view<vxl_byte> * img = new vil_image_view<vxl_byte>(256, 256, 1);
    for (unsigned j = 0; j < img->nj(); ++j)
      for (unsigned i = 0; i < img->ni(); ++i)
        (*img)(i, j) = (j + i) % 256;
    back_img = img;
  }

  if (sprite.set())
  {
    sprite_img = vil_load(sprite());
    if (!sprite_img)
      std::cerr << "Couldn't load foreground sprite image " << sprite() << '\n';
  }
  if (!sprite_img)
  {
    vil_image_view<vxl_byte> * img = new vil_image_view<vxl_byte>(32, 48, 4);
    for (unsigned j = 0; j < img->nj(); ++j)
      for (unsigned i = 0; i < img->ni(); ++i)
      { // r,g,b,a
        (*img)(i, j, 0) = 255;
        (*img)(i, j, 1) = 0;
        (*img)(i, j, 2) = 0;
        (*img)(i, j, 3) = vxl_byte(i * 255 / img->ni());
      }
    sprite_img = img;
  }

  std::cout << "Move sprite around with\n"
            << " left:  " << left_event.as_string() << '\n'
            << " right: " << right_event.as_string() << '\n'
            << " up:    " << up_event.as_string() << '\n'
            << " down:  " << down_event.as_string() << std::endl;

  vgui_image_tableau_new    back_img_tab(*back_img);
  vgui_easy2D_tableau_new   easy2d_tab(back_img_tab);
  vgui_viewer2D_tableau_new viewer_tab(easy2d_tab);

  float           x = (back_img->ni() - sprite_img->ni()) / 2.0f + 0.25f;
  float           y = (back_img->nj() - sprite_img->nj()) / 2.0f + 0.25f;
  vgui_soview2D * sprite_soview = easy2d_tab->add_image(x, y, *sprite_img);

  // After a soview2D has been created for the sprite image (which
  // happens in add_image), we don't need the sprite data any more.
  //
  sprite_img = nullptr;

  move_sprite_tableau_new move_tab(viewer_tab, sprite_soview);
  vgui_shell_tableau_new  shell_tab(move_tab);

  return vgui::run(shell_tab, back_img->ni(), back_img->nj());
}
