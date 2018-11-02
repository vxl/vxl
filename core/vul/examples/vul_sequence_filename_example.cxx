// This is core/vul/examples/vul_sequence_filename_example.cxx
#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_sequence_filename_map.h>

int main (int argc, char** argv)
{
  if (argc > 1) {
    // Initialize a vul_sequence_filename_map with the pattern
    vul_sequence_filename_map map(argv[1]);

    // Print out all the files that are specified by that pattern.
    for (int i = 0; i < map.get_nviews(); ++i) {
      std::cout << map.name(i) << std::endl;
    }
  } else {
    // Capes' examples
    {
      vul_sequence_filename_map map("images/img.%03d.pgm,0:2:100");
    }
    {
      vul_sequence_filename_map map("plop/plip/img_####.tiff",5,10,200);
    }
    {
      vul_sequence_filename_map map("pop.pop/poo_poo/img.###.ppm;1:2:10");
    }
    {
      vul_sequence_filename_map map("im.###,:5:");
      std::cerr << map.name(10) << std::endl
               << map.pair_name(10,11) << std::endl
               << map.triplet_name(10,11,12) << std::endl
               << map.image_name(10) << std::endl;
    }
  }
  return 0;
}
