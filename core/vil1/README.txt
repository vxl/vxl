
Examples:

 vil1_crop ~awf/images/HannoverDino/ppm/viff.000.ppm /tmp/t.pgm 300 100 170 200
 xv /tmp/t.pgm


Need:

 smart pointer - vil1_generic_image_ptr

 make a vil1_stream that implements seeking on forward-only streams.

 vil1_memory_image_of<T>


file_format: string vs. char const*
block copies of memory_images
blocking hints on generic


Extensions protocol: e.g. blocking/tiling

  1. implement get_property("tiling", int[2])
  2. test
  3. if widely used, promote to virtual get_tiling(int[2]);
