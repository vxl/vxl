# oxl/vgui/examples/geoff/vgui_testbed/CMakeLists.txt

SET(vgui_testbed_sources
 vgui_testbed.cxx
 vgui_point_selector.cxx vgui_point_selector.h vgui_point_selector_sptr.h
)

ADD_EXECUTABLE(vgui_testbed ${vgui_testbed_sources})

TARGET_LINK_LIBRARIES(vgui_testbed vgui)
