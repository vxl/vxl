# brl/bbas/bgui/tests/CMakeLists.txt

INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bbas)

LINK_LIBRARIES(bgui vil testlib vcl)

ADD_EXECUTABLE(bgui_test_all
   test_driver.cxx
)



