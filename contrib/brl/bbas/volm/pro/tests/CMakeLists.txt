add_executable( volm_pro_test_include test_include.cxx )
target_link_libraries( volm_pro_test_include vpgl_pro volm_pro)
# Need vpgl_pro only for the instantiation of brdb_value_t<vbl_smart_ptr<vpgl_camera<double> > >
