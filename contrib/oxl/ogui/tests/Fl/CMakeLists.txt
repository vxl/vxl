# oxl/vgui/tests/Fl/CMakeLists.txt

SET(test_twowin_sources test_twowin.cxx Fl_twowin_ui.h)
ADD_EXECUTABLE(vgui_test_twowin ${test_twowin_sources})

SET(test_movie_sources test_movie.cxx Fl_movie_ui.h)
ADD_EXECUTABLE(vgui_test_movie ${test_movie_sources})

SET(test_blender_sources test_blender.cxx Fl_blender_ui.h)
ADD_EXECUTABLE(vgui_test_blender ${test_blender_sources})

SET(Fl_twowin_ui_sources Fl_twowin_ui.cxx Fl_twowin_ui.h)
ADD_EXECUTABLE(vgui_Fl_twowin_ui ${Fl_twowin_ui_sources})

SET(Fl_movie_ui_sources Fl_movie_ui.cxx Fl_movie_ui.h)
ADD_EXECUTABLE(vgui_Fl_movie_ui ${Fl_movie_ui_sources})

SET(Fl_blender_ui_sources Fl_blender_ui.cxx Fl_blender_ui.h)
ADD_EXECUTABLE(vgui_Fl_blender_ui ${Fl_blender_ui_sources})

LINK_LIBRARIES( vgui vrml vnl vul )
