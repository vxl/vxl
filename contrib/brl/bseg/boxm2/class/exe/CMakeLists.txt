#boxm2 material classifier exes

add_executable( boxm2_class_write_samples boxm2_class_write_samples.cxx )
target_link_libraries( boxm2_class_write_samples boxm2 boxm2_class boxm2_basic boxm2_io ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vul)


