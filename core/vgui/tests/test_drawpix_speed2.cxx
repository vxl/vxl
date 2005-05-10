// This is core/vgui/tests/test_drawpix_speed.cxx
#include <vcl_iostream.h>
#include <vcl_iomanip.h>
#include <vcl_cstring.h>
#include <vcl_cstdlib.h>
#include <vcl_cmath.h>

#include <vpl/vpl.h>

#include <vul/vul_timer.h>

#include <vgui/vgui.h>
#include <vgui/vgui_gl.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_pixel.h>
#include <vgui/vgui_macro.h>


#if VGUI_MESA
#  include <GL/xmesa.h>
#endif

// Size of test image
unsigned const ni = 512;
unsigned const nj = 512;


float g_scale = 1.0f;
float g_bias  = 0.0f;
bool g_use_color_maps = false;
GLfloat g_mapRfloat[256];
GLfloat g_mapGfloat[256];
GLfloat g_mapBfloat[256];
GLubyte g_mapRbyte[256];
GLubyte g_mapGbyte[256];
GLubyte g_mapBbyte[256];

struct result_type
{
  // fps including drawing and moving dot
  double total;

  // fps just for drawing
  double draw;

  // fps just for moving the dot
  double pixmove;
};
vcl_ostream& operator<<( vcl_ostream& ostr, result_type const& r )
{
  return ostr << "(" << vcl_fixed << vcl_setprecision(2)
              << vcl_setw(7) << r.draw << " draw fps, "
              << vcl_setw(7) << r.total << " total fps)";
}


template<class InPix, class FramePix>
inline void
convert_pixel( InPix const& p, FramePix& fp )
{
  fp = FramePix( GLubyte(p.R), GLubyte(p.G), GLubyte(p.B), 0 );
}

// map scale and bias
template<class T>
inline GLubyte map_scale_and_bias( T f )
{
  return GLubyte( f*g_scale+g_bias );
}

template<class InPix, class FramePix>
inline void
convert_pixel_scale_and_bias( InPix const& p, FramePix& fp )
{
  fp = FramePix( map_scale_and_bias(p.R), map_scale_and_bias(p.G), map_scale_and_bias(p.B), 0 );
}

template<class InPix, class FramePix>
inline void
convert_pixel_scale_bias_and_map( InPix const& p, FramePix& fp )
{
  fp = FramePix( g_mapRbyte[map_scale_and_bias(p.R)], g_mapGbyte[map_scale_and_bias(p.G)], g_mapBbyte[map_scale_and_bias(p.B)], 0 );
}


template<class InPix, class FramePix>
void
convert_and_draw( InPix* buffer, GLenum pack_type, GLenum pix_type, FramePix* dummy = 0 )
{
  unsigned size = ni*nj;
  FramePix* framebuf = new FramePix[size];
  InPix* p = buffer;
  FramePix* fp = framebuf;
  if( g_scale == 1.0 && g_bias == 0.0 && !g_use_color_maps ) {
    for( unsigned cnt = 0 ; cnt < size; ++cnt, ++p, ++fp ) {
      convert_pixel( *p, *fp );
    }
  } else if ( !g_use_color_maps ) {
    for( unsigned cnt = 0 ; cnt < size; ++cnt, ++p, ++fp ) {
      convert_pixel_scale_and_bias( *p, *fp );
    }
  } else {
    for( unsigned cnt = 0 ; cnt < size; ++cnt, ++p, ++fp ) {
      convert_pixel_scale_bias_and_map( *p, *fp );
    }
  }
  glDrawPixels(ni,nj,pack_type,pix_type,framebuf);
  delete[] framebuf;
}



template<class PixType>
void create_pattern( PixType*& buf, PixType*& buf_copy )
{
  buf = new PixType[ni*ni];
  buf_copy = new PixType[ni*ni];
  for( unsigned j = 0; j < nj; ++j ) {
    unsigned i = 0;
    for( ; i < ni/3; ++i ) {
      buf[j*ni+i] = PixType(255-j*255/nj,0,0,0);
    }
    for( ; i < ni*2/3; ++i ) {
      buf[j*ni+i] = PixType(0,255-j*255/nj,0,0);
    }
    for( ; i < ni; ++i ) {
      buf[j*ni+i] = PixType(0,0,255-j*255/nj,0);
    }
  }
  vcl_memcpy( buf_copy, buf, ni*nj*sizeof(PixType) );
}


template<class PixType>
void
move_dot( unsigned &i, unsigned &j,
          PixType* buffer, PixType* buffer_copy )
{
  for( unsigned jp = j; jp < j+5; ++jp ) {
    for( unsigned ip = i; ip < i+5; ++ip ) {
      buffer[jp*ni+ip] = buffer_copy[jp*ni+ip];
    }
  }
  ++i;
  if( i > ni-12 ) {
    i = 10;
    ++j;
    if( j > nj-12 ) {
      j = 10;
    }
  }
  for( unsigned jp = 0; jp < 5; ++jp ) {
    for( unsigned ip = 0; ip < 5; ++ip ) {
      buffer[(jp+j)*ni+(ip+i)] = buffer_copy[(nj/2-jp-1)*ni+(ni-i+ip)];
    }
  }
}


vcl_string
option_string( unsigned option )
{
  switch( option ) {
  case 0:  return " direct:           ";
  case 1:  return " as 8,8,8 RGB:     ";
  case 2:  return " as 8,8,8,8 RGBA:  ";
  case 3:  return " as 5,6,5 RGB:     ";
  default: return "UNKNOWN";
  }
}


template<class PixType>
struct test_it
{
  PixType* buffer;
  PixType* buffer_copy;
  GLenum pack_type;
  GLenum pix_type;
  vcl_string in_pix_name;

  result_type best_result;
  unsigned best_option;

  test_it( vcl_string const& name, GLenum pack_type, GLenum pix_type );
  ~test_it();
  void test_pattern( unsigned option );

};


template<class PixType>
test_it<PixType>::
test_it( vcl_string const& name, GLenum in_pack_type, GLenum in_pix_type )
  : pack_type( in_pack_type ),
    pix_type( in_pix_type ),
    in_pix_name( name )
{
  create_pattern( buffer, buffer_copy );
}

template<class PixType>
test_it<PixType>::
~test_it()
{
  delete[] buffer;
  delete[] buffer_copy;
}

template<class PixType>
void
test_it<PixType>::
test_pattern( unsigned option )
{
  vcl_cout << "  " << in_pix_name << option_string(option);
  vcl_cout.flush();

  glPixelTransferf( GL_RED_SCALE, 1.0f );
  glPixelTransferf( GL_RED_BIAS,  0.0f );
  glPixelTransferf( GL_GREEN_SCALE, 1.0f );
  glPixelTransferf( GL_GREEN_BIAS,  0.0f );
  glPixelTransferf( GL_BLUE_SCALE, 1.0f );
  glPixelTransferf( GL_BLUE_BIAS,  0.0f );
  glPixelTransferi( GL_MAP_COLOR, 0 );
  glClear(GL_COLOR_BUFFER_BIT);

  vul_timer timer;
  unsigned long draws = 0;
  unsigned i = 10;
  unsigned j = 10;
  long elapsed;
  vgui_macro_report_errors;
  timer.mark();
  do {
    move_dot( i, j, buffer, buffer_copy );

    switch( option ) {
    case 0:
      glPixelTransferf( GL_RED_SCALE, g_scale );
      glPixelTransferf( GL_RED_BIAS,  g_bias/255 );
      glPixelTransferf( GL_GREEN_SCALE, g_scale );
      glPixelTransferf( GL_GREEN_BIAS,  g_bias/255 );
      glPixelTransferf( GL_BLUE_SCALE, g_scale );
      glPixelTransferf( GL_BLUE_BIAS,  g_bias/255 );
      if( g_use_color_maps ) {
        glPixelMapfv( GL_PIXEL_MAP_R_TO_R, 256, g_mapRfloat );
        glPixelMapfv( GL_PIXEL_MAP_G_TO_G, 256, g_mapGfloat );
        glPixelMapfv( GL_PIXEL_MAP_B_TO_B, 256, g_mapBfloat );
        glPixelTransferi( GL_MAP_COLOR, 1 );
      }
      glDrawPixels(ni,nj,pack_type,pix_type,buffer);
      break;
    case 1:
      convert_and_draw(buffer,GL_RGB,GL_UNSIGNED_BYTE, (vgui_pixel_rgb888*)0 );
      break;
    case 2:
      convert_and_draw(buffer,GL_RGBA, GL_UNSIGNED_BYTE, (vgui_pixel_rgba8888*)0 );
      break;
    case 3:
      convert_and_draw(buffer, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, (vgui_pixel_rgb565*)0 );
      break;
    }
    glFlush();
    ++draws;
    elapsed = timer.real();
  } while (elapsed < 2000);
  vgui_macro_report_errors;

  // measure time for moving the dot around.
  timer.mark();
  i = 10;
  j = 10;
  for( unsigned long cnt = 0; cnt < draws; ++cnt ) {
    move_dot( i, j, buffer, buffer_copy );
  }
  long elapsed_pix = timer.real();

  // reset the buffer
  vcl_memcpy( buffer, buffer_copy, ni*nj*sizeof(PixType) );

  result_type r;
  r.total   = draws*1000.0 / elapsed;
  r.pixmove = draws*1000.0 / elapsed_pix;
  r.draw    = draws*1000.0 / (elapsed-elapsed_pix);

  if( best_option == -1u || r.draw < best_result.draw ) {
    best_option = option;
    best_result = r;
  }

  vcl_cout << r << vcl_endl;
}


void run_8bit_rgb()
{
  test_it<vgui_pixel_rgb888> test( "8-bit RGB", GL_RGB, GL_UNSIGNED_BYTE );
  test.test_pattern( 0 );
  test.test_pattern( 1 );
  test.test_pattern( 2 );
  test.test_pattern( 3 );
}

void run_8bit_rgba()
{
  test_it<vgui_pixel_rgba8888> test( "8-bit RGBA", GL_RGBA, GL_UNSIGNED_BYTE );
  test.test_pattern( 0 );
  test.test_pattern( 1 );
  test.test_pattern( 2 );
  test.test_pattern( 3 );
}

void run_16bit_rgb()
{
  test_it<vgui_pixel_rgb161616> test( "16-bit RGB", GL_RGB, GL_UNSIGNED_SHORT );
  test.test_pattern( 0 );
  test.test_pattern( 1 );
  test.test_pattern( 2 );
  test.test_pattern( 3 );
}

void run_float_rgb()
{
  test_it<vgui_pixel_rgbfloat> test( "float RGB", GL_RGB, GL_FLOAT );
  test.test_pattern( 0 );
  test.test_pattern( 1 );
  test.test_pattern( 2 );
  test.test_pattern( 3 );
}


void
run_tests( void(*test_func)() )
{
  g_use_color_maps = false;
  g_scale = 1.0f;
  g_bias  = 0.0f;
  glPixelZoom(1.0f,1.0f);
  vcl_cout << "Pixel zoom 1, no scaling, no colormap\n";
  test_func();
  glPixelZoom(0.51f,0.51f);
  vcl_cout << "Pixel zoom 0.51, no scaling, no colormap\n";
  test_func();
  glPixelZoom(1.27f,1.27f);
  vcl_cout << "Pixel zoom 1.27, no scaling, no colormap\n";
  test_func();

  g_scale = 0.5f;
  g_bias  = 30.0f;
  glPixelZoom(1.0f,1.0f);
  vcl_cout << "Pixel zoom 1, scaling, no colormap\n";
  test_func();
  glPixelZoom(0.51f,0.51f);
  vcl_cout << "Pixel zoom 0.51, scaling, no colormap\n";
  test_func();

  g_use_color_maps = true;
  g_scale = 1.0f;
  g_bias  = 0.0f;
  glPixelZoom(1.0f,1.0f);
  vcl_cout << "Pixel zoom 1, no scaling, color map\n";
  test_func();

  g_scale = 0.5f;
  g_bias  = 30.0f;
  glPixelZoom(1.0f,1.0f);
  vcl_cout << "Pixel zoom 1, scaling, color map\n";
  test_func();

}

int main( int argc, char** argv )
{
  vgui::init( argc, argv );
  vgui_window* win = vgui::produce_window( ni, nj, "test speed" );
  win->show();
  // need this on some toolkit implementations to get the window up.
  vgui::run_till_idle();

  vcl_cout << "GL_VERSION : " <<  (const char*) glGetString(GL_VERSION) << '\n'
           << "GL_RENDERER : " << (const char*) glGetString(GL_RENDERER)<< "\n\n"
           << "GL Gets -\n";
  GLint data_int;
  glGetIntegerv(GL_RED_BITS, &data_int);
  vcl_cout << "        red-bits : " << data_int << vcl_endl;
  glGetIntegerv(GL_GREEN_BITS, &data_int);
  vcl_cout << "      green-bits : " << data_int << vcl_endl;
  glGetIntegerv(GL_BLUE_BITS, &data_int);
  vcl_cout << "       blue-bits : " << data_int << vcl_endl;
  glGetIntegerv(GL_ALPHA_BITS, &data_int);
  vcl_cout << "      alpha-bits : " << data_int << vcl_endl;

  glViewport( 0, 0, ni, nj );
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  glOrtho( 0.0, ni, 0.0, nj, -1.0, 1.0 );
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();

  glRasterPos2i(0,0);

  glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
  glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);      // gl_direct_DrawPixels needs this set to 1 !!
  glPixelStorei(GL_UNPACK_ROW_LENGTH, ni);

  glDisable(GL_ALPHA_TEST);
  glDisable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
  glDrawBuffer( GL_FRONT );


  // Generate color maps
  {
    for( unsigned i = 0; i < 256; ++i ) {
      g_mapRfloat[i] = vcl_sqrt( (i-128.0)*(i-128.0) ) / 128;
      g_mapGfloat[i] = vcl_sqrt( (i-128.0)*(i-128.0) ) / 128;
      g_mapBfloat[i] = ( 128 - vcl_sqrt( (i-128.0)*(i-128.0) ) ) / 128;
      g_mapRbyte[i] = GLubyte( g_mapRfloat[i]*255 );
      g_mapGbyte[i] = GLubyte( g_mapGfloat[i]*255 );
      g_mapBbyte[i] = GLubyte( g_mapBfloat[i]*255 );
    }
  }


  vcl_cout << "\n\n8-BIT RGB\n\n";
  run_tests( &run_8bit_rgb );

  vcl_cout << "\n\n8-BIT RGBA\n\n";
  run_tests( &run_8bit_rgba );

  vcl_cout << "\n\n16-BIT RGB\n\n";
  run_tests( &run_16bit_rgb );

  vcl_cout << "\n\nFLOAT RGB\n\n";
  run_tests( &run_float_rgb );

  return 0;
}
