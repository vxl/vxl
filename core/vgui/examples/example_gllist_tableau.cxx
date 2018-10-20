// \brief  Test if OpenGL lists can be generated during vgui tableau tree
//         initialization.
//         A red triangle should be displayed at the center of the window.
// \author Lianqing Yu
// \date   January 2011

#include <vgui/vgui_wrapper_tableau.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_window.h>

#include <vgui/vgui_gl.h>

class basic_manager : public vgui_wrapper_tableau
{
 public:
  ~basic_manager() {}

  static basic_manager *instance()
  {
    static basic_manager *instance_ = nullptr;
    if (!instance_) {
      instance_ = new basic_manager();
      instance_->init();
    }
    return instance_;
  }

  void init()
  {
    vgui_image_tableau_sptr img = vgui_image_tableau_new();
    vgui_viewer2D_tableau_sptr viewer = vgui_viewer2D_tableau_new(img);
    vgui_shell_tableau_sptr shell = vgui_shell_tableau_new(viewer);
    this->add_child(shell);

    gen_gl_list();
  }

  // Offer public access to gen_gl_list.
  void gen_list()
  {
     if ( listName == 0 ) {
        gen_gl_list();
        this->post_redraw();
     }
  }

  // Create a OpenGL list of a red triangle.
  void gen_gl_list()
  {
    // This does not in gtk2 impl. of vgui, up to version 1.14.0
    listName = glGenLists(1);
    if ( listName == 0 )
       std::cerr << "Fail to generate opengl list.\n";
    else {
      // Create a red triangle
      glNewList(listName, GL_COMPILE);
      glColor3f(1.0, 0.0, 0.0);
      glBegin(GL_TRIANGLES);
      glVertex2f(250.0, 100.0);
      glVertex2f(400.0, 400.0);
      glVertex2f(100.0, 400.0);
      glEnd();
      glEndList();
    }
  }

  virtual bool handle(vgui_event const &e)
  {
    //pass the event to the shell
    this->child.handle(e);

    if ( e.type == vgui_DRAW )
      glCallList(listName);

    return false;
  }

 private:
  basic_manager() : vgui_wrapper_tableau() {}

  GLuint listName;
};

// Provide a menu to give app the second chance to generate GL list.
class basic_menu
{
 public:
   static void gen_list() { basic_manager::instance()->gen_list(); }
   static vgui_menu get_menu()
   {
      vgui_menu menubar;
      menubar.add("Generate List", gen_list);
      return menubar;
   }
 private:
   basic_menu() {}
};

int main(int argc, char** argv)
{
  vgui::init(argc, argv);
  vgui_menu menubar = basic_menu::get_menu();
  unsigned w = 512, h = 512;
  vgui_window* win = vgui::produce_window(w, h, menubar);
  win->show(); // call this function earlier to make GL context available.

  basic_manager* man = basic_manager::instance();
  win->get_adaptor()->set_tableau(man);
  win->set_statusbar(true);
  win->enable_vscrollbar(true);
  win->enable_hscrollbar(true);

  return vgui::run();
}
