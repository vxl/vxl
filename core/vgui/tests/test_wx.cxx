/*
  An opengl example program using wx.

  Has a pure VGUI and pure wx/opengl version controlled by the USE_VGUI flag.
  The pure wx version makes some vgui_utils calls, but they are all pure
  gl calls, for all practical purposes.

  DPI Scalable, and should correctly scale and Windows, Linux, and macos
*/
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include "vgui/impl/wx/vgui_wx.h"
#include "vgui/impl/wx/vgui_wx_window.h"
#include "vgui/impl/wx/vgui_wx_adaptor.h"
// #include "wx/sizer.h"
#include "vgui/vgui_shell_tableau.h"
#include "vgui/vgui_easy2D_tableau.h"
#include "vgui/vgui_rubberband_tableau.h"
#include "vgui/vgui_viewer2D_tableau.h"
#include "vgui/vgui.h"
#include "vgui/vgui_utils.h"
#include "vil/vil_image_view.h"
#include "vil/vil_save.h"
#include "vnl/vnl_double_4x4.h"
#include "vgui/vgui_font_bitmap.h"
using namespace std;

#define USE_VGUI

void
draw_stuff(int window_width, int window_height, double scale = 0);

class BasicGLPane : public wxGLCanvas
{
  wxGLContext * m_context;
  wxFrame * parent_frame;

public:
  wxTimer * m_timer;
  BasicGLPane(wxFrame * parent, int * args);
  virtual ~BasicGLPane() {}
  void
  resized(wxSizeEvent & evt);
  void
  on_maximize(wxMaximizeEvent & event);
  void
  on_mouse_event(wxMouseEvent & event);
  void
  display(wxPaintEvent & evt);
  int
  getWidth()
  {
    return GetSize().x;
  };
  int
  getHeight()
  {
    return GetSize().y;
  };
  // Make the program run without scale correction by uncommenting this
  // double GetContentScaleFactor(){return 1.0;};
  DECLARE_EVENT_TABLE()
};
BEGIN_EVENT_TABLE(BasicGLPane, wxGLCanvas)
EVT_MAXIMIZE(BasicGLPane::on_maximize)
EVT_SIZE(BasicGLPane::resized)
EVT_PAINT(BasicGLPane::display)
EVT_MOUSE_EVENTS(BasicGLPane::on_mouse_event)
END_EVENT_TABLE()
void
BasicGLPane::on_maximize(wxMaximizeEvent & event)
{
  parent_frame->SetStatusText("maximize");
}

void
BasicGLPane::on_mouse_event(wxMouseEvent & event)
{
  int x = event.GetX(), y = event.GetY();

  if (event.ButtonDown(wxMOUSE_BTN_LEFT))
  {
    std::stringstream ss;
    ss << "(" << x << ' ' << y << ")" << std::endl;
    parent_frame->SetStatusText(ss.str());

    double x_d = ((double)x / getWidth()) * 2 - 1;
    double y_d = 1 - ((double)y / getHeight()) * 2;

    cout << x_d << " : " << y_d << endl;

    glPointSize(5 * GetContentScaleFactor());
    glBegin(GL_POINTS);
    glColor3d(0, 0, 0);
    glVertex3f(x_d, y_d, 0);
    glEnd();
    SwapBuffers();
  }

  char message[100];
  snprintf(message, sizeof(message), "(%d, %d)", x, y);
  parent_frame->SetStatusText(message);
}

void
BasicGLPane::resized(wxSizeEvent & evt)
{
  // static int alternate = 0;
  // alternate++;
  // char message[100];
  // snprintf(message, sizeof(message), "resize - %d", alternate);

#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__)
  if (!GetXWindow() || !IsShown())
    return;
#elif defined(__WXMSW__)
  if (!GetHDC() || !IsShown())
    return;
#endif

  SetCurrent(*m_context);

  // parent_frame->SetStatusText(message);

  // display();
}

void
BasicGLPane::display(wxPaintEvent & evt)
{
  draw_stuff(getWidth(), getHeight(), GetContentScaleFactor());
  SwapBuffers();
}

void
draw_stuff(int window_width, int window_height, double scale)
{
  GLint vp[4];
  glGetIntegerv(GL_VIEWPORT, vp);
  cout << "GL_VIEWPORT: " << vp[0] << "," << vp[1] << "," << vp[2] << "," << vp[3] << endl;

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black Background
  glEnable(GL_TEXTURE_2D);              // textures
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  int topleft_x = 0, topleft_y = 0, bottomright_x = window_width, bottomright_y = window_height;

  vgui_utils::set_glViewport(topleft_x, topleft_y, bottomright_x - topleft_x, bottomright_y - topleft_y, scale);
  cout << "Logical box: " << topleft_x << "," << topleft_y << "," << bottomright_x - topleft_x << ","
       << bottomright_y - topleft_y << endl;
  cout << "Scale factor: " << scale << endl;
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0,
          window_width, // left, right
          window_height,
          0, // bottom, top
          -1,
          +1); // near, far

  // gluOrtho2D(topleft_x, bottomright_x, bottomright_y, topleft_y);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  // glOrtho(-1, 1,  // left, right
  //         -1, 1, // bottom, top
  //         -1,+1);    // near, far
  // glOrtho(0, window_width,  // left, right
  //         window_height, 0, // bottom, top
  //         -1,+1);    // near, far

  vnl_double_4x4 P; // projection matrix
  vnl_double_4x4 M; // modelview matrix
  glGetDoublev(GL_PROJECTION_MATRIX, P.data_block());
  P.inplace_transpose();

  glGetDoublev(GL_MODELVIEW_MATRIX, M.data_block());
  M.inplace_transpose();
  std::cout << "P\n" << P << std::endl;
  std::cout << "M\n" << M << std::endl;

  // A light grey shade of blue
  glClearColor(0.3f, 0.4f, 0.6f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glShadeModel(GL_SMOOTH);
  glBegin(GL_QUADS);
  glColor3d(1, 0, 0);
  glVertex3f(window_width * .05, window_height * .05, 0);
  glColor3d(1, 1, 0);
  glVertex3f(window_width * .95, window_height * .05, 0);
  glColor3d(1, 1, 1);
  glVertex3f(window_width * .95, window_height * .95, 0);
  glColor3d(0, 1, 1);
  glVertex3f(window_width * .05, window_height * .95, 0);
  glEnd();

  // Test scissors
  vgui_utils::set_glScissor(50, 50, 20, 20, scale);
  glClearColor(1.0f, 1.0f, 0.0f, 1.0f); // yellow
  glEnable(GL_SCISSOR_TEST);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDisable(GL_SCISSOR_TEST);

  // Test Bitmap/RasterPos, the letter F
  GLubyte rasters[24] = { 0xc0, 0x00, 0xc0, 0x00, 0xc0, 0x00, 0xc0, 0x00, 0xc0, 0x00, 0xff, 0x00,
                          0xff, 0x00, 0xc0, 0x00, 0xc0, 0x00, 0xc0, 0x00, 0xff, 0xc0, 0xff, 0xc0 };

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glColor3f(0.8f, 0.0f, 1.0f);
  glRasterPos2d(window_width / 2, window_height / 2);


  int offset_y = 70;
#ifdef USE_VGUI
  // VGUI size includes the status bar, but not the title bar. wx size
  // includes both. So on my machine, this makes the F line up.
  offset_y += 14;
#endif

  // This is just pure gl calls. Should place a magenta F in the yellow square
  // The F is fixed to the center + offset, so when you resize the window, the
  // F should move. This is not a bug, just a detail. If I didn't want this, I
  // would have set the origin to 0 instead of 48,70, and moved the RasterPos.
  // On my machine, it lines up, probably won't when the status bar and title
  // bar have different heights on different themes/os/etc... Not important
  vgui_utils::draw_glBitmap(10, 12, 48, offset_y, 11, 0, rasters, scale);
  vgui_utils::draw_glBitmap(10, 12, 48, offset_y, 11, 0, rasters, scale);


  glColor3f(0.0f, 1.0f, 0.0f);
  glRasterPos2d(window_width / 2, window_height / 2);
  vgui_font_bitmap().draw("VxL");

  vgui_utils::copy_back_to_front();
  glClearColor(1.0f, 0.0f, 0.0f, 1.0f); // Red Background
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  vgui_utils::copy_front_to_back();

#if 0
    // Test the functions that use ReadArray, they work.
    vil_image_view<vxl_byte> temp = vgui_utils::colour_buffer_to_view();
    vil_save(temp, "test.tif");
    vgui_utils::dump_colour_buffer("test2.tif");
#endif
}

BasicGLPane::BasicGLPane(wxFrame * parent, int * args)
  : wxGLCanvas(parent, wxID_ANY, args, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE)
{
  m_context = new wxGLContext(this);
  parent_frame = parent;
}
class MyApp : public wxApp
{
  virtual bool
  OnInit();

  wxFrame * frame;
  BasicGLPane * glPane;

public:
};
// IMPLEMENT_APP(MyApp)

bool
MyApp::OnInit()
{
  // wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
  frame = new wxFrame((wxFrame *)NULL, -1, wxT("Hello GL World"), wxPoint(50, 50), wxSize(200, 300));

  int args[] = { WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0 };

  glLoadIdentity();

  glPane = new BasicGLPane((wxFrame *)frame, args);
  frame->CreateStatusBar();
  frame->SetStatusText("GL canvas is fun");

  frame->Show();

  return true;
}

#ifndef USE_VGUI
IMPLEMENT_APP_NO_MAIN(MyApp);
#endif
IMPLEMENT_WX_THEME_SUPPORT;

//-----------------------------------------------------------------------------
// A tableau that displays the mouse position when left mouse button is pressed.
// The mouse position is sent to cout as well as the status bar
struct example_tableau : public vgui_viewer2D_tableau
{
  example_tableau(vgui_tableau_sptr t)
    : vgui_viewer2D_tableau(t)
  {}

  ~example_tableau() {}

  bool
  handle(const vgui_event & e)
  {
    if (e.type == vgui_DRAW || e.type == vgui_DRAW_OVERLAY || e.type == vgui_RESHAPE)
    {
      setup_gl_matrices();
      draw_stuff(vgui_adaptor::current->get_width(), vgui_adaptor::current->get_height(), 0.0);
      return true; // event has been used
    }
    else if (e.type == vgui_MOTION)
    {
      vgui::out << "";
      int x = (int)e.wx;
      int y = (int)e.wy;
      std::stringstream ss;
      ss << "(" << x << ' ' << y << ")" << std::endl;
      vgui::out << ss.str();
      return true; // event has been used
    }
    // else
    // {
    //   cout << "Unknown event type: " << e.type << endl;
    // }

    // We are not interested in other events, so pass event to base class:
    return vgui_viewer2D_tableau::handle(e);
  }
};
//-----------------------------------------------------------------------------
// Make a smart-pointer constructor for our tableau.
struct example_tableau_new : public vgui_viewer2D_tableau_sptr
{
  example_tableau_new(vgui_tableau_sptr t)
    : vgui_viewer2D_tableau_sptr(new example_tableau(t))
  {}
};

//-----------------------------------------------------------------------------
int
main(int argc, char ** argv)
{
#ifndef USE_VGUI
  wxEntryStart(argc, argv);
  wxTheApp->CallOnInit();
  wxTheApp->OnRun();
#else
  vgui_wx::instance()->init(argc, argv);
  vgui_wx_window * wx_win = dynamic_cast<vgui_wx_window *>(vgui_wx::instance()->produce_window(200, 300, "test wx"));
  wx_win->set_statusbar(true);
  vgui_wx_adaptor * adpt = dynamic_cast<vgui_wx_adaptor *>(wx_win->get_adaptor());
  vgui_tableau_sptr t = new vgui_tableau();
  vgui_tableau_sptr extab = example_tableau_new(t);
  vgui_viewer2D_tableau_new viewer(extab);
  // vgui_easy2D_tableau_new easy(extab);
  // vgui_shell_tableau_sptr shell = new vgui_shell_tableau(easy);
  adpt->set_tableau(viewer);
  wx_win->show();
  // easy->add_line(0.0, 0.0, 20.0f, 25.0f);
  vgui_wx::instance()->run();
#endif
  return 0;
}
