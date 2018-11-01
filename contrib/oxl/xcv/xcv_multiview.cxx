// This is oxl/xcv/xcv_multiview.cxx
//:
// \file
// \author K.Y.McGaul
// See xcv_multiview.h for a description of this file.
//
// \verbatim
//  Modifications:
//   K.Y.McGaul     27-JAN-2000  Initial version.
// \endverbatim

#include <string>
#include <iostream>
#include <fstream>
#include "xcv_multiview.h"
#include "xcv_twoview_manager.h"
#include "xcv_threeview_manager.h"
#include "xcv_tjunction.h"
#include "xcv_segmentation.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil1/vil1_image.h>

#include <mvl/HomgInterestPointSet.h>
#include <mvl/TripleMatchSetCorner.h>
#include <mvl/SimilarityMetric.h>
#include <mvl/TriTensor.h>

#include <vgui/vgui.h>
#include <vgui/vgui_find.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_rubberband_tableau.h>
#include <vgui/vgui_parent_child_link.h>
#include <vgui/vgui_soview.h>
#include <vgui/vgui_soview2D.h>

std::vector<xcv_twoview_manager*> xcv_multiview::twoview_mgrs;
std::vector<vgui_rubberband_tableau_sptr> xcv_multiview::twoview_rubber0;
std::vector<vgui_rubberband_tableau_sptr> xcv_multiview::twoview_rubber1;
std::vector<xcv_threeview_manager*> xcv_multiview::threeview_mgrs;
std::vector<vgui_rubberband_tableau_sptr> xcv_multiview::threeview_rubber0;
std::vector<vgui_rubberband_tableau_sptr> xcv_multiview::threeview_rubber1;
std::vector<vgui_rubberband_tableau_sptr> xcv_multiview::threeview_rubber2;

extern std::string* get_loadfile();
extern std::string* get_savefile();
extern void get_current(unsigned*, unsigned*);
extern bool get_twoviews(std::vector<int>*, std::vector<int>*);
extern bool get_threeviews(std::vector<int>*, std::vector<int>*);
extern vgui_rubberband_tableau_sptr get_rubberbander_at(unsigned, unsigned);
extern vgui_easy2D_tableau_sptr get_easy2D_at(unsigned, unsigned);
extern bool get_image_at(vil1_image*, unsigned, unsigned);

//-----------------------------------------------------------------------------
//: Gets a twoview_manager between the tableaux at the given positions.
//   If a manager already exists this manager is returned, otherwise a
//   new manager is created and returned.
//-----------------------------------------------------------------------------
xcv_twoview_manager* xcv_multiview::get_twoview_manager(std::vector<int>& col_pos,
                                                        std::vector<int>& row_pos)
{
  vgui_rubberband_tableau_sptr rubbers[2];
  rubbers[0] = get_rubberbander_at(col_pos[0], row_pos[0]);
  rubbers[1] = get_rubberbander_at(col_pos[1], row_pos[1]);
  // First check to see if we already have a manager for these two tableaux:
  for (unsigned i=0; i<twoview_rubber0.size(); i++)
  {
    if (rubbers[0] == twoview_rubber0[i] && rubbers[1] == twoview_rubber1[i])
      return twoview_mgrs[i];
    if (rubbers[0] == twoview_rubber1[i] && rubbers[1] == twoview_rubber0[i])
    {
      // Put the vectors of column and row positions into the original order:
      int temp_col = col_pos[0]; col_pos[0] = col_pos[1]; col_pos[1] = temp_col;
      int temp_row = row_pos[0]; row_pos[0] = row_pos[1]; row_pos[1] = temp_row;
      return twoview_mgrs[i];
    }
  }

  // Above the rubberbander for each tableau, insert a t-junction tableau to
  // collect events.  Create a twoview_manager to pass events between these two
  // and add it to the list of managers:
  xcv_twoview_manager* mgr = new xcv_twoview_manager();
  twoview_rubber0.push_back(rubbers[0]);
  twoview_rubber1.push_back(rubbers[1]);
  twoview_mgrs.push_back(mgr);

  for (unsigned j=0; j<2; j++)
  {
    xcv_tjunction* tee = new xcv_tjunction(mgr);
#if 0 // fsm changed. it was probably broken as a result of the easy2D changes.
    vgui_tableau_sptr old = rubbers[j];
#else
    vgui_tableau_sptr old = vgui_find_above_by_type_name(rubbers[j],
                                                         "vgui_composite_tableau");
#endif
    vgui_parent_child_link::replace_child_everywhere(old, tee);
    tee->set_child(old);
    mgr->set_tableau(tee, j);
  }
  return mgr;
}

//-----------------------------------------------------------------------------
//: Gets a threeview_manager between the tableaux at the given positions.
//   If a manager already exists then this manager is returned, otherwise a
//   new manager is created and returned.
//-----------------------------------------------------------------------------
xcv_threeview_manager* xcv_multiview::get_threeview_manager(std::vector<int>& col_pos,
                                                            std::vector<int>& row_pos)
{
  vgui_rubberband_tableau_sptr rubbers[3];
  for (int i=0; i<3; i++)
    rubbers[i] = get_rubberbander_at(col_pos[i], row_pos[i]);

  int col0 = col_pos[0], col1 = col_pos[1], col2 = col_pos[2];
  int row0 = row_pos[0], row1 = row_pos[1], row2 = row_pos[2];
  // Check to see if we already have a manager for these three views (in any order):
  for (unsigned i=0; i<threeview_rubber0.size(); i++)
  {
    if (rubbers[0] == threeview_rubber0[i] &&
        rubbers[1] == threeview_rubber1[i] &&
        rubbers[2] == threeview_rubber2[i])
    {
      return threeview_mgrs[i];
    }
    if (rubbers[0] == threeview_rubber0[i] &&
        rubbers[1] == threeview_rubber2[i] &&
        rubbers[2] == threeview_rubber1[i])
    {
      // Put the vectors of column and row positions into the original order:
      col_pos[1] = col2; row_pos[1] = row2;
      col_pos[2] = col1; row_pos[2] = row1;
      return threeview_mgrs[i];
    }
    if (rubbers[0] == threeview_rubber1[i] &&
        rubbers[1] == threeview_rubber0[i] &&
        rubbers[2] == threeview_rubber2[i])
    {
      col_pos[0] = col1; row_pos[0] = row1;
      col_pos[1] = col0; row_pos[1] = row0;
      return threeview_mgrs[i];
    }
    if (rubbers[0] == threeview_rubber1[i] &&
        rubbers[1] == threeview_rubber2[i] &&
        rubbers[2] == threeview_rubber0[i])
    {
      col_pos[0] = col1; row_pos[0] = row1;
      col_pos[1] = col2; row_pos[1] = row2;
      col_pos[2] = col0; row_pos[2] = row0;
      return threeview_mgrs[i];
    }
    if (rubbers[0] == threeview_rubber2[i] &&
        rubbers[1] == threeview_rubber0[i] &&
        rubbers[2] == threeview_rubber1[i])
    {
      col_pos[0] = col2; row_pos[0] = row2;
      col_pos[1] = col0; row_pos[1] = row0;
      col_pos[2] = col1; row_pos[2] = row1;
      return threeview_mgrs[i];
    }
    if (rubbers[0] == threeview_rubber2[i] &&
        rubbers[1] == threeview_rubber1[i] &&
        rubbers[2] == threeview_rubber0[i])
    {
      col_pos[0] = col2; row_pos[0] = row2;
      col_pos[2] = col0; row_pos[2] = row0;
      return threeview_mgrs[i];
    }
  }
  // No manager yet for these three views:
  xcv_threeview_manager *mgr = new xcv_threeview_manager;
  threeview_rubber0.push_back(rubbers[0]);
  threeview_rubber1.push_back(rubbers[1]);
  threeview_rubber2.push_back(rubbers[2]);
  threeview_mgrs. push_back(mgr);

  for (unsigned j=0; j<3; j++)
  {
    xcv_tjunction* tee = new xcv_tjunction(mgr);
    vgui_parent_child_link::replace_child_everywhere(rubbers[j] , tee);
    tee->set_child(rubbers[j]);
    mgr->set_tableau(tee, j);
  }
  return mgr;
}

//-----------------------------------------------------------------------------
//: Compute an FMatrix for the two selected views.
//-----------------------------------------------------------------------------
void xcv_multiview::compute_f_matrix()
{
  std::vector<int> col_pos, row_pos;
  if (!get_twoviews(&col_pos, &row_pos))
    return;

  FMatrix* fmat = new FMatrix;

  xcv_twoview_manager* mgr = get_twoview_manager(col_pos, row_pos);
  mgr->set_f_matrix(fmat);
}

//-----------------------------------------------------------------------------
//: Compute an HMatrix2D for the two selected views.
//-----------------------------------------------------------------------------
void xcv_multiview::compute_h_matrix2d()
{
  std::vector<int> col_pos, row_pos;
  if (!get_twoviews(&col_pos, &row_pos))
    return;

  HMatrix2D* hmat = new HMatrix2D;

  xcv_twoview_manager* mgr = get_twoview_manager(col_pos, row_pos);
  mgr->set_h_matrix(hmat);
}

//-----------------------------------------------------------------------------
//: Compute corner matches for the two selected views.
//-----------------------------------------------------------------------------
void xcv_multiview::compute_corner_matches()
{
  std::vector<int> col_pos, row_pos;
  if (!get_twoviews(&col_pos, &row_pos))
    return;

  // FIXME
}

//-----------------------------------------------------------------------------
//: Load an FMatrix for the two selected views from a file.
//-----------------------------------------------------------------------------
void xcv_multiview::load_f_matrix()
{
  std::vector<int> col_pos, row_pos;
  if (!get_twoviews(&col_pos, &row_pos))
    return;
#ifdef DEBUG
  std::cerr << "Selected positions = (" << col_pos[0] << ", "
           << row_pos[0] << "), (" << col_pos[1] << ", " << row_pos[1] << ").\n";
#endif

  vgui_dialog f_dialog("Load FMatrix");
  std::string* f_matrix_filename = get_loadfile();
  static std::string regexp = "*.*";
  f_dialog.inline_file("File containing FMatrix:", regexp, *f_matrix_filename);
  if (!f_dialog.ask() || f_matrix_filename->size() == 0)
    return;

  // Read the FMatrix from the file:
  FMatrix* fmat = new FMatrix(FMatrix::read(f_matrix_filename->c_str()));

  xcv_twoview_manager* mgr = get_twoview_manager(col_pos, row_pos);
  mgr->set_f_matrix(fmat);
}

//-----------------------------------------------------------------------------
//: Load an HMatrix2D for the two selected views from a file.
//-----------------------------------------------------------------------------
void xcv_multiview::load_h_matrix2d()
{
  std::vector<int> col_pos, row_pos;
  if (!get_twoviews(&col_pos, &row_pos))
    return;

  vgui_dialog h_dialog("Load HMatrix2D");
  std::string* h_matrix_filename = get_loadfile();
  static std::string regexp = "*.*";
  h_dialog.inline_file("File containing HMatrix2D:", regexp, *h_matrix_filename);
  if (!h_dialog.ask())
    return;

  HMatrix2D* hmat = new HMatrix2D();
  hmat->read(h_matrix_filename->c_str());

  xcv_twoview_manager* mgr = get_twoview_manager(col_pos, row_pos);
  mgr->set_h_matrix(hmat);
}

//-----------------------------------------------------------------------------
//: Load corner matches for the two selected views from a file.
//-----------------------------------------------------------------------------
void xcv_multiview::load_corner_matches()
{
  std::vector<int> col_pos, row_pos;
  if (!get_twoviews(&col_pos, &row_pos))
    return;

  vgui_dialog corner_dialog("Load corner matches");
  std::string* cm_filename = get_loadfile();
  static std::string regexp = "*.*";
  corner_dialog.inline_file("File containing corner matches:", regexp, *cm_filename);
  if (!corner_dialog.ask())
    return;

  // FIXME
}

//-----------------------------------------------------------------------------
//: Save the FMatrix of the two selected views to file.
//-----------------------------------------------------------------------------
void xcv_multiview::save_f_matrix()
{
  std::vector<int> col_pos, row_pos;
  if (!get_twoviews(&col_pos, &row_pos))
    return;

  vgui_dialog fsave_dialog("Save FMatrix");
  std::string* f_matrix_filename = get_savefile();
  static std::string regexp = "*.*";
  fsave_dialog.inline_file("File to save FMatrix:", regexp, *f_matrix_filename);
  if (!fsave_dialog.ask())
    return;

  // Get the FMatrix from the twoview_manager and save it to file:
  xcv_twoview_manager* mgr = get_twoview_manager(col_pos, row_pos);
  FMatrix* fm = mgr->get_f_matrix();
  if (fm == nullptr)
  {
    std::cerr << "No FMatrix exists to save\n";
    return;
  }
  std::ofstream output_filestream(f_matrix_filename->c_str());
  output_filestream << *fm;
}

//-----------------------------------------------------------------------------
//: Save the HMatrix2D of the two selected views to file.
//-----------------------------------------------------------------------------
void xcv_multiview::save_h_matrix2d()
{
  std::vector<int> col_pos, row_pos;
  if (!get_twoviews(&col_pos, &row_pos))
    return;

  vgui_dialog hsave_dialog("Save HMatrix2D");
  std::string* h_matrix_filename = get_savefile();
  static std::string regexp = "*.*";
  hsave_dialog.inline_file("File to save HMatrix2D:", regexp, *h_matrix_filename);
  if (!hsave_dialog.ask())
    return;

  // Get the HMatrix2D from the twoview_manager and save it to a file:
  xcv_twoview_manager* mgr = get_twoview_manager(col_pos, row_pos);
  HMatrix2D* hm = mgr->get_h_matrix();
  if (hm == nullptr)
  {
    std::cerr << "No HMatrix2D exists to save\n";
    return;
  }
  std::ofstream output_filestream(h_matrix_filename->c_str());
  output_filestream << *hm;
}

//-----------------------------------------------------------------------------
//: Save the corner matches of the two selected views to file.
//-----------------------------------------------------------------------------
void xcv_multiview::save_corner_matches()
{
  std::vector<int> col_pos, row_pos;
  if (!get_twoviews(&col_pos, &row_pos))
    return;

  vgui_dialog cmsave_dialog("Save HMatrix2D");
  std::string* cm_filename = get_savefile();
  static std::string regexp = "*.*";
  cmsave_dialog.inline_file("File to save corner matches:", regexp, *cm_filename);
  if (!cmsave_dialog.ask())
    return;

  xcv_twoview_manager* mgr = get_twoview_manager(col_pos, row_pos);
  PairMatchSetCorner* corner_matches = mgr->get_corner_matches();
  if (corner_matches == nullptr)
  {
    std::cerr << "No corner matches between these two views exist to save\n";
    return;
  }
  std::ofstream output_filestream(cm_filename->c_str());
  output_filestream << *corner_matches;
}

//-----------------------------------------------------------------------------
//: Toggle display of the FMatrix.
//-----------------------------------------------------------------------------
void xcv_multiview::toggle_f_matrix()
{
  std::vector<int> col_pos, row_pos;
  if (!get_twoviews(&col_pos, &row_pos))
    return;

  xcv_twoview_manager* mgr = get_twoview_manager(col_pos, row_pos);
  mgr->toggle_f_matrix_display();
}

//-----------------------------------------------------------------------------
//: Toggle display of the HMatrix2D.
//-----------------------------------------------------------------------------
void xcv_multiview::toggle_h_matrix()
{
  std::vector<int> col_pos, row_pos;
  if (!get_twoviews(&col_pos, &row_pos))
    return;

  xcv_twoview_manager* mgr = get_twoview_manager(col_pos, row_pos);
  mgr->toggle_h_matrix_display();
}

//-----------------------------------------------------------------------------
//: Display corner matches by highlighting corresponding points in two views.
//-----------------------------------------------------------------------------
void xcv_multiview::display_corner_matches()
{
  std::vector<int> col_pos, row_pos;
  if (!get_twoviews(&col_pos, &row_pos))
    return;

  xcv_twoview_manager* mgr = get_twoview_manager(col_pos, row_pos);
#if 0
  PairMatchSetCorner* corner_matches = mgr->get_corner_matches();
  if (corner_matches == NULL)
  {
    std::cerr << "No corner matches exist between these two views.\n";
    return;
  }
#endif // 0
  mgr->toggle_corner_match_display();
}


//-----------------------------------------------------------------------------
//: Display corner tracks.
//-----------------------------------------------------------------------------
void xcv_multiview::display_corner_tracks()
{
  std::vector<int> col_pos, row_pos;
  if (!get_twoviews(&col_pos, &row_pos))
    return;

  xcv_twoview_manager* mgr = get_twoview_manager(col_pos, row_pos);
  PairMatchSetCorner* corner_matches = mgr->get_corner_matches();
  if (corner_matches == nullptr)
  {
    std::cerr << "No corner matches exist between these two views.\n";
    return;
  }

  vgui_easy2D_tableau_sptr easy0 = get_easy2D_at(col_pos[0], row_pos[0]);
  vgui_easy2D_tableau_sptr easy1 = get_easy2D_at(col_pos[1], row_pos[1]);

  std::vector<HomgPoint2D> points0, points1;
  corner_matches->extract_matches(points0, points1);
  ImageMetric* metric1 = const_cast<ImageMetric*>( corner_matches->get_corners1()->get_conditioner() );
  ImageMetric* metric2 = const_cast<ImageMetric*>( corner_matches->get_corners2()->get_conditioner() );

  easy0->set_foreground(1,1,0);  // Draw lines in yellow
  easy1->set_foreground(1,1,0);
  for (unsigned i=0; i<points1.size(); ++i)
  {
    vnl_double_2 x0 = metric1->homg_to_image(points0[i]);
    vnl_double_2 x1 = metric2->homg_to_image(points1[i]);

    easy0->add_line(float(x0[0]), float(x0[1]), float(x1[0]), float(x1[1]));
    easy1->add_line(float(x0[0]), float(x0[1]), float(x1[0]), float(x1[1]));
  }
  easy0->set_foreground(0,1,0); // Back to green again!
  easy1->set_foreground(0,1,0);
  easy0->post_redraw();
}

//-----------------------------------------------------------------------------
//: Load a TriTensor into the threeview_manager from file.
//-----------------------------------------------------------------------------
void xcv_multiview::load_tri_tensor()
{
  std::vector<int> col_pos, row_pos;
  if (!get_threeviews(&col_pos, &row_pos))
    return;

  vgui_dialog t_dialog("Load TriTensor");
  std::string* tri_tensor_filename = get_loadfile();
  static std::string regexp = "*.*";
  t_dialog.inline_file("File containing TriTensor:", regexp, *tri_tensor_filename);
  if (!t_dialog.ask())
    return;

  std::ifstream tri_tensor_filestream(tri_tensor_filename->c_str());
  if (!tri_tensor_filestream)
  {
    std::cerr << "Unable to open file: " << tri_tensor_filename->c_str() << std::endl;
    return;
  }
  TriTensor* tt = new TriTensor();
  tri_tensor_filestream >> *tt;

  // Get a threeview_manager and set the TriTensor:
  xcv_threeview_manager* mgr = get_threeview_manager(col_pos, row_pos);
  mgr->set_tri_tensor(tt);

  // Get the twoview_managers and set the FMatrices:
  xcv_twoview_manager* mgr12 = get_twoview_manager(col_pos, row_pos);
  FMatrix* f12 = new FMatrix(tt->get_fmatrix_12());
  mgr12->set_f_matrix(f12);

  std::vector<int> two_col_pos, two_row_pos;
  two_col_pos.push_back(col_pos[0]); two_col_pos.push_back(col_pos[2]);
  two_row_pos.push_back(row_pos[0]); two_row_pos.push_back(row_pos[2]);
  xcv_twoview_manager* mgr13 = get_twoview_manager(two_col_pos, two_row_pos);
  FMatrix* f13 = new FMatrix(tt->get_fmatrix_13());
  mgr13->set_f_matrix(f13);

  std::vector<int> two_col_pos2, two_row_pos2;
  two_col_pos2.push_back(col_pos[1]); two_col_pos2.push_back(col_pos[2]);
  two_row_pos2.push_back(row_pos[1]); two_row_pos2.push_back(row_pos[2]);
  xcv_twoview_manager* mgr23 = get_twoview_manager(two_col_pos2, two_row_pos2);
  FMatrix* f23 = new FMatrix(tt->compute_fmatrix_23());
  mgr23->set_f_matrix(f23);
}

//-----------------------------------------------------------------------------
//: Save a TriTensor to file.
//-----------------------------------------------------------------------------
void xcv_multiview::save_tri_tensor()
{
  std::vector<int>col_pos, row_pos;
  if (!get_threeviews(&col_pos, &row_pos))
    return;

  vgui_dialog t_dialog("Save TriTensor");
  std::string* tri_tensor_filename = get_savefile();
  static std::string regexp = "*.*";
  t_dialog.inline_file("File to save TriTensor:", regexp, *tri_tensor_filename);
  if (!t_dialog.ask())
    return;

  // Get the TriTensor from the threeview_manager and save it to the
  // chosen file:
  xcv_threeview_manager* mgr = get_threeview_manager(col_pos, row_pos);
  TriTensor* tt = mgr->get_tri_tensor();
  if (tt == nullptr)
  {
    std::cerr << "No TriTensor exists to save\n";
    return;
  }
  std::ofstream output_filestream(tri_tensor_filename->c_str());
  output_filestream << *tt;
}

//-----------------------------------------------------------------------------
//:  Toggle display of the TriTensor for the selected views.
//-----------------------------------------------------------------------------
void xcv_multiview::toggle_tri_tensor()
{
  std::vector<int> col_pos, row_pos;
  if (!get_threeviews(&col_pos, &row_pos))
    return;

  xcv_threeview_manager* mgr = get_threeview_manager(col_pos, row_pos);
  mgr->toggle_tri_tensor_display();

  // Toggle off display of all F matrices:
  std::vector<int> two_col_pos(2), two_row_pos(2);
  two_col_pos[0] = col_pos[0]; two_row_pos[0] = row_pos[0];
  two_col_pos[1] = col_pos[1]; two_row_pos[1] = row_pos[1];
  xcv_twoview_manager* mgr2 = get_twoview_manager(two_col_pos, two_row_pos);
  mgr2->toggle_f_matrix_display();

  two_col_pos[0] = col_pos[0]; two_row_pos[0] = row_pos[0];
  two_col_pos[1] = col_pos[2]; two_row_pos[1] = row_pos[2];
  xcv_twoview_manager* mgr3 = get_twoview_manager(two_col_pos, two_row_pos);
  mgr3->toggle_f_matrix_display();

  two_col_pos[0] = col_pos[1]; two_row_pos[0] = row_pos[1];
  two_col_pos[1] = col_pos[2]; two_row_pos[1] = row_pos[2];
  xcv_twoview_manager* mgr4 = get_twoview_manager(two_col_pos, two_row_pos);
  mgr4->toggle_f_matrix_display();
}

//-----------------------------------------------------------------------------
//: Transfers a point on two view to a point on the third view.
//-----------------------------------------------------------------------------
void xcv_multiview::transfer_point()
{
  std::vector<int> col_pos, row_pos;
  if (!get_threeviews(&col_pos, &row_pos))
    return;
  xcv_threeview_manager* mgr = get_threeview_manager(col_pos, row_pos);

  // Go through the view and get the two selected points:
  vgui_soview2D_point* points[3]; points[0]=points[1]=points[2]=nullptr;
  vgui_easy2D_tableau_sptr easys[3];
  int point_count = 0;
  for (unsigned i=0; i<3; i++)
  {
    easys[i] = get_easy2D_at(col_pos[i], row_pos[i]);
    if (! easys[i])
      return;
    std::vector<vgui_soview*> sel_objs = easys[i]->get_selected_soviews();

    for (unsigned int j=0; j<sel_objs.size(); ++j)
      if (sel_objs[j]->type_name() == "vgui_soview2D_point")
      {
        points[i]=static_cast<vgui_soview2D_point*>(sel_objs[j]);
        ++point_count;
        break;
      }
  }
  if (point_count != 2)
  {
    vgui_dialog dl("Error");
    dl.message("");
    dl.message("You must select a point in two different views");
    dl.message("");
    dl.ask();
    return;
  }
  int cw = 5; // width of cross
  // Get the TriTensor and transfer the points:
  TriTensor* tten = mgr->get_tri_tensor();
  if (tten == nullptr)
  {
    vgui_dialog dl("Error");
    dl.message("");
    dl.message("You must compute or load a TriTensor for the three views");
    dl.message("");
    dl.ask();
    return;
  }
  if (points[0] == nullptr)
  {
    HomgPoint2D point2(points[1]->x, points[1]->y);
    HomgPoint2D point3(points[2]->x, points[2]->y);
    HomgPoint2D point1 = tten->image1_transfer(point2, point3);
    double x, y;
    point1.get_nonhomogeneous(x, y);
    easys[0]->add_line(float(x+cw),float(y+cw),float(x-cw),float(y-cw));
    easys[0]->add_line(float(x-cw),float(y+cw),float(x+cw),float(y-cw));
  }
  else if (points[1] == nullptr)
  {
    HomgPoint2D point1(points[0]->x, points[0]->y);
    HomgPoint2D point3(points[2]->x, points[2]->y);
    HomgPoint2D point2 = tten->image2_transfer(point1, point3);
    double x,y;
    point2.get_nonhomogeneous(x,y);
    easys[1]->add_line(float(x+cw),float(y+cw),float(x-cw),float(y-cw));
    easys[1]->add_line(float(x-cw),float(y+cw),float(x+cw),float(y-cw));
  }
  else if (points[2] == nullptr)
  {
    HomgPoint2D point1(points[0]->x, points[0]->y);
    HomgPoint2D point2(points[1]->x, points[1]->y);
    HomgPoint2D point3 = tten->image3_transfer(point1, point2);
    double x,y;
    point3.get_nonhomogeneous(x,y);
    easys[2]->add_line(float(x+cw),float(y+cw),float(x-cw),float(y-cw));
    easys[2]->add_line(float(x-cw),float(y+cw),float(x+cw),float(y-cw));
  }
  easys[0]->post_redraw();
}

//-----------------------------------------------------------------------------
//: Transfers a line on two views to a line on the third view.
//-----------------------------------------------------------------------------
void xcv_multiview::transfer_line()
{
  std::vector<int> col_pos, row_pos;
  if (!get_threeviews(&col_pos, &row_pos))
    return;
  xcv_threeview_manager* mgr = get_threeview_manager(col_pos, row_pos);

  // Go through the views and get the two selected lines:
  vgui_soview2D_lineseg* lines[3]; lines[0]=lines[1]=lines[2]=nullptr;
  vgui_easy2D_tableau_sptr easys[3];
  int line_count = 0;
  for (unsigned i=0; i<3; i++)
  {
    easys[i] = get_easy2D_at(col_pos[i], row_pos[i]);
    if (! easys[i])
      return;
    std::vector<vgui_soview*> sel_objs = easys[i]->get_selected_soviews();
    for (unsigned int j=0; j<sel_objs.size(); ++j)
      if (sel_objs[j]->type_name() == "vgui_soview2D_lineseg")
      {
        lines[i]=static_cast<vgui_soview2D_lineseg*>(sel_objs[j]);
        ++line_count;
        break;
      }
  }
  if (line_count != 2)
  {
    vgui_dialog dl("Error");
    dl.message("");
    dl.message("You must select a line in two different views");
    dl.message("");
    dl.ask();
    return;
  }
  // Get the TriTensor and transfer the lines:
  TriTensor* tten = mgr->get_tri_tensor();
  if (tten == nullptr)
  {
    vgui_dialog dl("Error");
    dl.message("");
    dl.message("You must compute or load a TriTensor for the three views");
    dl.message("");
    dl.ask();
    return;
  }
  if (lines[0] == nullptr)
  {
    HomgLineSeg2D line2(lines[1]->x0, lines[1]->y0, lines[1]->x1, lines[1]->y1);
    HomgLineSeg2D line3(lines[2]->x0, lines[2]->y0, lines[2]->x1, lines[2]->y1);
    HomgLine2D line1 = tten->image1_transfer(line2, line3);
    easys[0]->add_infinite_line(float(line1[0]), float(line1[1]), float(line1[2]));
  }
  else if (lines[1] == nullptr)
  {
    HomgLineSeg2D line1(lines[0]->x0, lines[0]->y0, lines[0]->x1, lines[0]->y1);
    HomgLineSeg2D line3(lines[2]->x0, lines[2]->y0, lines[2]->x1, lines[2]->y1);
    HomgLine2D line2 = tten->image2_transfer(line1, line3);
    easys[1]->add_infinite_line(float(line2[0]), float(line2[1]), float(line2[2]));
  }
  else
  {
    HomgLineSeg2D line1(lines[0]->x0, lines[0]->y0, lines[0]->x1, lines[0]->y1);
    HomgLineSeg2D line2(lines[1]->x0, lines[1]->y0, lines[1]->x1, lines[1]->y1);
    HomgLine2D line3 = tten->image3_transfer(line1, line2);
    easys[2]->add_infinite_line(float(line3[0]), float(line3[1]), float(line3[2]));
  }
  easys[0]->post_redraw();
}
