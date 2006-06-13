PROJECT(wx_example)

SET(wx_docview_example_sources
  wx_docview_app.h     wx_docview_app.cxx
  wx_docview_frame.h   wx_docview_frame.cxx
  wx_docview_doc.h     wx_docview_doc.cxx
  wx_docview_view.h    wx_docview_view.cxx
)

SET(wx_childview_example_sources
  wx_childview_app.h     wx_childview_app.cxx
  wx_childview_frame.h   wx_childview_frame.cxx
)

ADD_EXECUTABLE(wx_docview_example WIN32 ${wx_docview_example_sources})
TARGET_LINK_LIBRARIES(wx_docview_example vgui vil)

ADD_EXECUTABLE(wx_childview_example WIN32 ${wx_childview_example_sources})
TARGET_LINK_LIBRARIES(wx_childview_example vgui vil)
