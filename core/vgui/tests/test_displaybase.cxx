#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_soview2D.h>
#include <vgui/vgui_style.h>
#include <vcl_iostream.h>
#include <testlib/testlib_test.h>

void test_displaybase()
{
  vgui_easy2D_tableau_new tab1;
  tab1->set_foreground( 1.0f, 1.0f, 1.0f );
  vgui_soview2D* line1 = tab1->add_line( 0.0f, 0.0f, 1.0f, 1.0f );

  TEST_NEAR("Try to retrieve style for line 1", line1->get_style()->rgba[0], 1.0f, 1e-7f);

  vgui_easy2D_tableau_new tab2;
  tab2->set_foreground( 0.5f, 0.1f, 0.2f );
  vgui_soview* line2 = tab2->add_line( 0.0f, 0.0f, 1.0f, 1.0f );

  TEST_NEAR("Try to retrieve style for line 2", line2->get_style()->rgba[0], 0.5f, 1e-7f);

  vcl_cout << "Clear tableau 1\n";
  tab1->clear();

  // this should create a new style object
  tab1->set_foreground( 0.0f, 0.0f, 0.0f );

  TEST_NEAR("Try to retrieve style for line 2 again", line2->get_style()->rgba[0], 0.5f, 1e-7f);

  vgui_soview* line3 = tab1->add_line( 0.0f, 0.0f, 3.0f, 3.0f );

  TEST_NEAR("Try to retrieve style for line 3", line3->get_style()->rgba[0], 0.0f, 1e-7f);
}

TESTMAIN(test_displaybase);
