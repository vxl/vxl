# vxl/vidl_vil1/examples/CMakeLists.txt

ADD_EXECUTABLE( vidl_vil1_mpegcodec_example vidl_vil1_mpegcodec_example.cxx )
TARGET_LINK_LIBRARIES( vidl_vil1_mpegcodec_example vidl_vil1 vbl )

ADD_EXECUTABLE( vidl_vil1_video2images vidl_vil1_video2images.cxx )
TARGET_LINK_LIBRARIES( vidl_vil1_video2images vidl_vil1 vil1 vul vcl )
