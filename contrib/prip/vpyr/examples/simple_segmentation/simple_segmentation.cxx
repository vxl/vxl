#include "simple_segmentation_builder.h"
#include "vil/vil_load.h"
#include "vil/vil_save.h"

int main()
{
  vil_image_view<vxl_byte> img;
	img= vil_load("lena.pgm") ;
	simple_segmentation_builder builder ;
	builder.initialise(img) ;
	vil_image_view<vxl_byte> img2;
	builder.draw_down_projection(img2, img) ;
	vil_save(img2,"lena.org.pgm");
	builder.filter_edge_below(1) ;
	builder.draw_down_projection(img2, img) ;
	vil_save(img2,"lena.1.pgm");
	builder.filter_edge_below(2) ;
	builder.draw_down_projection(img2, img) ;
	vil_save(img2,"lena.2.pgm");
	builder.filter_edge_below(3) ;
	builder.draw_down_projection(img2, img) ;
	vil_save(img2,"lena.3.pgm");
	builder.filter_edge_below(100) ;
	builder.draw_down_projection(img2, img) ;
	vil_save(img2,"lena.100.pgm");
	return 0 ;
}
