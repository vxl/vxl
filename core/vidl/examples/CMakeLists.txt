# vxl/vidl/examples/CMakeLists.txt

ADD_EXECUTABLE( vidl_mpegcodec_example vidl_mpegcodec_example.cxx )
TARGET_LINK_LIBRARIES( vidl_mpegcodec_example vidl vbl )

ADD_EXECUTABLE( vidl_video2images vidl_video2images.cxx )
TARGET_LINK_LIBRARIES( vidl_video2images vidl vil vul vcl )
