// awfmfcDoc.cpp : implementation of the example_CAwfmfcDoc class
//

#include <vgui/impl/mfc/StdAfx.h>
#include "awfmfc.h"

#include "awfmfcDoc.h"


/////////////////////////////////////////////////////////////////////////////
// example_CAwfmfcDoc

IMPLEMENT_DYNCREATE(example_CAwfmfcDoc, CDocument)

BEGIN_MESSAGE_MAP(example_CAwfmfcDoc, CDocument)
        //{{AFX_MSG_MAP(example_CAwfmfcDoc)
        ON_COMMAND(awf_tool_1, Ontool1)
        //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// example_CAwfmfcDoc construction/destruction

// #undef new

#include <vcl_fstream.h>
#include <vcl_cassert.h>
#include <vnl/vnl_file_matrix.h>
#include <vnl/vnl_double_3.h>
#include <vul/vul_awk.h>

#include <vgui/vgui.h>
#include <vgui/vgui_easy2D.h>
#include <vgui/vgui_viewer2D.h>
#include <vgui/vgui_easy3D.h>
#include <vgui/vgui_viewer3D.h>
#include <vgui/vgui_deck_tableau.h>
#include <vgui/vgui_composite.h>
#include <vgui/vgui_clear_tableau.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vrml/vgui_vrml_tableau.h>
#include <vgui/vgui_projection_inspector.h>
#include <vgui/vgui_soview.h>
#include <vgui/vgui_dialog.h>

typedef vgui_tableau_sptr_t<vgui_easy3D> vgui_easy3D_sptr;
typedef vgui_tableau_sptr_t<vgui_viewer3D> vgui_viewer3D_sptr;
typedef vgui_tableau_sptr_t<vgui_vrml_tableau> vgui_vrml_tableau_sptr;
typedef vgui_tableau_sptr_t<vgui_deck_tableau> vgui_deck_tableau_sptr;

struct mydata;

struct awf_stuff {
  vgui_easy2D_sptr easy2D;
  vgui_viewer3D_sptr v3d;
  vgui_vrml_tableau_sptr vrml;
  mydata* all[2];
};

struct mytab;
typedef vgui_tableau_sptr_t<mytab> mytab_sptr;

struct mydata {
  awf_stuff * awf;
  int id;
  vgui_easy2D_sptr easy2D;
  vgui_image_tableau_sptr img;
  vgui_composite_sptr comp;
  vgui_viewer2D_sptr z;
  mytab_sptr t;

  mydata(char const* f, int id, awf_stuff*);
  ~mydata();
};

int last_down = -1;
double lx, ly;
struct corr {
  double x0, y0;
  double x1, y1;
};

vcl_vector<corr> all_correspondences;
void compute_f(vcl_vector<corr>& c) {
  vcl_cerr << "n = " << c.size() << vcl_endl;
  vcl_ofstream f("/tmp/f.txt");
  for (int i = 0; i < c.size(); ++i) {
    f << c[i].x0 << ' ' << c[i].y0 << ' ' << c[i].x1 << ' ' << c[i].y1 << '\n';
  }
}

struct mytab : public vgui_tableau {
  mydata* p;

  bool motion(int x, int y);

  bool mouse_down(int x, int y, vgui_button b, vgui_modifier);

  bool key_press(int x, int y, int c, vgui_modifier) {
    if (c == 'F' || c == 'f') {
      // Compute f
      compute_f(all_correspondences);
      return true;
    }
    return false;
  }
};

extern char g_status_bar_text[1024];
bool mytab::motion(int x, int y)
{
  vgui_projection_inspector pi;
  float ix, iy;
  pi.window_to_image_coordinates(int(x),int(y), ix,iy);

  int r = 0;
  int g = 0;
  int b = 0;
  vcl_sprintf(g_status_bar_text, "%.1f,%.1f (%d,%d,%d)", ix, iy, r,g,b);

  return false;
}

bool mytab::mouse_down(int x, int y, vgui_button b, vgui_modifier mod)
{
  if (b == vgui_MIDDLE && mod == vgui_modifier(0)) {
    vgui_projection_inspector pi;
    float ix, iy;
    pi.window_to_image_coordinates(int(x),int(y), ix,iy);
    p->easy2D->add_point(ix, iy);
    p->easy2D->post_redraw();
    return true;
  }

  if (b == vgui_RIGHT) {
    vgui_projection_inspector pi;
    float ix, iy;
    pi.window_to_image_coordinates(int(x),int(y), ix,iy);
    p->easy2D->add_point(ix, iy);
    last_down = p->id;
    lx = ix;
    ly = iy;

    p->easy2D->post_redraw();
    return true;
  }

  return false;
}


mydata::mydata(char const* f, int id, awf_stuff* awf)
{
  this->awf = awf;
  this->id = id;
  img = vgui_image_tableau_new(f);
  easy2D = vgui_easy2D_new(f);
  t = new mytab;
  t->p = this;
  vcl_vector<vgui_tableau_sptr> c;
  c.push_back(vgui_clear_tableau_new());
  c.push_back(img);
  c.push_back(easy2D);
  c.push_back(t);
  comp = vgui_composite_new(c);

  z= vgui_viewer2D_new(comp);
}

mydata::~mydata()
{
}

int find(vcl_vector<vnl_double_3> const& v, const vnl_double_3& p)
{
  for (int i = 0; i < v.size(); ++i)
    if ((v[i] - p).squared_magnitude() < 1e-7)
      return i;
  return -1;
}

void save(vgui_easy3D_sptr dl, char const* fn)
{
  vcl_ofstream f(fn);
  vcl_vector<vgui_soview*> all = dl->get_all();

  // Store all points for tests
  vcl_vector<vnl_double_3> pts;
  for (int i = 0; i < all.size(); ++i) {
    vgui_soview* so = all[i];
    if (so->type_name() == "vgui_point3D") {
      vgui_point3D* p = (vgui_point3D*)so;
      pts.push_back(vnl_double_3(p->x, p->y, p->z));
    }
  }

  for (int i = 0; i < all.size(); ++i) {
    vgui_soview* so = all[i];
    if (so->type_name() == "vgui_point3D") {
      vgui_point3D* p = (vgui_point3D*)so;
      f << "p " << p->x << ' '  << p->y << ' '  << p->z << '\n';
    } else if (so->type_name() == "vgui_lineseg3D") {
      vgui_lineseg3D* p = (vgui_lineseg3D*)so;
      int i1 = find(pts, vnl_double_3(p->x0, p->y0, p->z0));
      int i2 = find(pts, vnl_double_3(p->x1, p->y1, p->z1));
      if (i1 != -1 && i2 != -1)
        f << "il " << i1 << ' ' << i2 << vcl_endl;
      else
        f <<  "l "
          << p->x0 << ' '  << p->y0 << ' '  << p->z0 << ' '
          << p->x1 << ' '  << p->y1 << ' '  << p->z1 << '\n';
    }
  }
}

struct my3dvtab : public vgui_viewer3D {
  vgui_easy3D_sptr dl;
  vcl_vector<vgui_point3D*> pts;

  my3dvtab(vgui_easy3D_sptr t);
  ~my3dvtab() {}

  bool key_press(float x, float y, vgui_key key, vgui_modifier m) {
    if (key == 's') {
      // Save 3d
      save(dl, "/tmp/t.e3d");
    }

    if (key == 'j' || key == 'J') {
      vcl_vector<vgui_soview*> sel = dl->get_selected_soviews();
      dl->deselect_all();
      if (sel.size() == 2) {
        vgui_point3D* p1 = getpt(sel[0]);
        vgui_point3D* p2 = getpt(sel[1]);
        if (p1 && p2) {
          dl->add_line(p1->x, p1->y, p1->z, p2->x, p2->y, p2->z);
          dl->post_redraw();
          return true;
        }
      }
    }
    return vgui_viewer3D::key_press(x,y,key,m);
  }

  vgui_point3D* getpt(vgui_soview* s) {
    for (int i = 0; i < pts.size(); ++i)
      if (pts[i] == s) return pts[i];
    return 0;
  }
};

void tr(vnl_double_3& x)
{
  vnl_double_3 pinfty(.7,.01,.1);
  x /= dot_product(x,pinfty) + 1;
}

my3dvtab::my3dvtab(vgui_easy3D_sptr t):
  vgui_viewer3D(t),
  dl(t)
{
  dl->set_point_radius(5);
  dl->set_foreground(1,1,1);
  dl->add_point(0,0,0);

  // yellow
  dl->set_foreground(1,1,0);

  if (true) {
    // load pts
    vnl_file_matrix<double> fm("/tmp/t.X");
    // compute centroid
    vnl_double_3 c(0,0,0);
    for (int i = 0; i < fm.rows(); ++i)
      c += fm.get_row(i);
    c /= fm.rows();

    for (int i = 0; i < fm.rows(); ++i)
      pts.push_back(dl->add_point(fm(i,0)-c[0], fm(i,1)-c[1], fm(i,2)-c[2]));

    // Add lines
    int lp[] = {
      -1,
      26, 23,
      25, 24,
      25, 26,
      24, 23,
      34, 31,
      38, 35,
      42, 39,
      30, 27,
      16, 20,
      73, 20,
       8, 13,
       1,  4,
       2,  3,
       1,  2,
       4,  3,
      40, 37,
      35, 42,
      36, 41,
      38, 39,
      58, 57,
      59, 71,
      71, 57,
      58, 59,
      -1};
      for (int*p = lp; *p != -1; p += 2) {
        int i = p[0]-1;
        int j = p[1]-1;
        dl->add_line(pts[i]->x, pts[i]->y, pts[i]->z,
                   pts[j]->x, pts[j]->y, pts[j]->z);
      }
  } else {
    // Load t3d
    vcl_ifstream f("/tmp/t.e3d");
    assert(f.good());
    for (vul_awk awk(f); awk; ++awk) {
      vcl_string s = awk[0];
      if (s == "p") {
        vnl_double_3 X(atof(awk[1]),atof(awk[2]),atof(awk[3]));
        tr(X);
        dl->add_point(X[0],X[1],X[2]);
      } else if (s == "l") {
        vnl_double_3 X1(atof(awk[1]),atof(awk[2]),atof(awk[3]));
        vnl_double_3 X2(atof(awk[4]),atof(awk[5]),atof(awk[6]));
        tr(X1); tr(X2);
        dl->add_line(X1[0], X1[1], X1[2], X2[0], X2[1], X2[2]);
      }
    }
  }
}

vgui_tableau_sptr mk3d()
{
  if (false)
    return new my3dvtab(new vgui_easy3D);
  else
    return new vgui_viewer3D(vgui_vrml_tableau_new("/tmp/t.wrl"));
}

// ---------------------------------------------------------------

example_CAwfmfcDoc::example_CAwfmfcDoc()
{
  //extern int vgui_mfc_tag_function();
  //vgui_mfc_tag_function();
  int argc = 0;
  vgui::select( "mfc");
  vgui::init( argc, 0);

  // TODO: add one-time construction code here
  pimpl = new awf_stuff;
  pimpl->all[1] = 0;

  pimpl->all[0] = new mydata("/awf/images/eccv-nr/f.000.ppm",0,pimpl);
#if 0
  pimpl->all[1] = new mydata("/awf/images/eccv-nr/f.004.ppm",1,pimpl);

  vgui_poly_tableau* poly = new vgui_poly_tableau;
  poly->add(pimpl->all[0]->z, 0, 0, .5, 1);
  poly->add(pimpl->all[1]->z, .5, 0, .5, 1);

  pimpl->vrml.vertical_cast(mk3d());
  pimpl->v3d = new vgui_viewer3D(pimpl->vrml);
  vgui_tableau_sptr viewer3d = new vgui_blackbox_tableau(pimpl->v3d);

  vgui_deck_tableau_sptr deck = new vgui_deck_tableau;
  deck->add(poly);
  deck->add(viewer3d);

  // Put into a viewer2D tableau to get zooming, etc:
  vgui_viewer2D* viewer = new vgui_viewer2D(poly);
#endif // 0
  this->tableau = pimpl->all[0]->z;
}

example_CAwfmfcDoc::~example_CAwfmfcDoc()
{
  delete pimpl->all[0];
  delete pimpl->all[1];
  delete pimpl;
}

BOOL example_CAwfmfcDoc::OnNewDocument()

  if (!CDocument::OnNewDocument())
          return FALSE;

  // TODO: add reinitialization code here
  // (SDI documents will reuse this document)

  return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// example_CAwfmfcDoc serialization

void example_CAwfmfcDoc::Serialize(CArchive& ar)
{
  if (ar.IsStoring())
  {
    // TODO: add storing code here
  }
  else
  {
    // TODO: add loading code here
  }
}

/////////////////////////////////////////////////////////////////////////////
// example_CAwfmfcDoc diagnostics

#ifdef _DEBUG
void example_CAwfmfcDoc::AssertValid() const
{
  CDocument::AssertValid();
}

void example_CAwfmfcDoc::Dump(CDumpContext& dc) const
{
  CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// example_CAwfmfcDoc commands

BOOL example_CAwfmfcDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
  // TODO: Add your command handler code here
  if (IsModified())
    TRACE0("Warning: OnOpenDocument replaces an unsaved document.\n");

  DeleteContents();
  SetModifiedFlag(FALSE);
#if 0
  set_tableau(new vgui_vrml_tableau(pFile));
  pimpl->v3d->remove_child(pimpl->vrml);
  pimpl->vrml = new vgui_vrml_tableau(lpszPathName);
  pimpl->v3d->add_child(pimpl->vrml);
  pimpl->v3d->post_redraw();
#endif // 0
  pimpl->all[0]->img->set_image(lpszPathName);

  return TRUE;
}

void example_CAwfmfcDoc::Ontool1()
{
  // TODO: Add your command handler code here
  vgui_dialog db("Hello world");
  static double x = 5;
  db.field("Point radius", x);
  if (db.ask()) {
//  ((my3dvtab*)pimpl->vrml)->dl->set_point_radius(x);
    vcl_cerr << x << vcl_endl;
  }
}
