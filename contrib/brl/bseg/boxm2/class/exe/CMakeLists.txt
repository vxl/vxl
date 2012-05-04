#boxm2 material classifier exes

ADD_EXECUTABLE( boxm2_class_write_samples boxm2_class_write_samples.cxx )
TARGET_LINK_LIBRARIES( boxm2_class_write_samples boxm2 boxm2_class boxm2_basic boxm2_io vil vbl vul)


