/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation "vil_stream_url"
#endif
#include "vil_stream_url.h"

#include <vcl_cassert.h>
#include <vcl_cstdio.h>  // sprintf()
#include <vcl_cstring.h>
#include <vcl_cstdlib.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vil/vil_stream_core.h>

#if defined(unix)

# include <unistd.h>       // read(), write(), close()
# include <netdb.h>        // gethostbyname(), sockaddr_in()
# include <sys/socket.h>
# include <netinet/in.h>   // htons()
# ifdef __alpha
#  include <fp.h>           // htons() [ on e.g. DEC alpha, htons is in machine/endian.h]
# endif
# define SOCKET int
#elif defined (VCL_WIN32)
# include <winsock2.h>
#endif


static const
int base64_encoding[]=
{
  'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P',
  'Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f',
  'g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v',
  'w','x','y','z','0','1','2','3','4','5','6','7','8','9','+','/'
};

static char out_buf[4];

static const char * encode_triplet(char data[3], unsigned n)
{
  assert (n>0 && n <4);
  out_buf[0] = base64_encoding[(data[0] & 0xFC) >> 2];

  if (n==1)
  {
    out_buf[2] = out_buf[3] = '=';
    return out_buf;
  }

  out_buf[1] = base64_encoding[
    ((data[0] & 0x3) << 4) + ((data[1] & 0xf0)>>4)];
  out_buf[2] = base64_encoding[
    ((data[1] & 0xf) << 2) + ((data[2] & 0xc0)>>6)];

  if (n==2)
  {
    out_buf[3] = '=';
    return out_buf;
  }

  out_buf[3] = base64_encoding[ (data[2] & 0x3f) ];
  return out_buf;
}

//=======================================================================

static vcl_string encode_base64(const vcl_string& in)
{
  vcl_string out;
    unsigned i = 0, line_octets = 0;
  const unsigned l = in.size();
  char data[3];
    while(i < l)
    {
    data[0] = in[i++];
    data[1] = data[2] = 0;

    if(i == l)
    {
      out.append(encode_triplet(data,1),4);
      return out;
    }

    data[1] = in[i++];

    if(i == l)
    {
      out.append(encode_triplet(data,2),4);
      return out;
    }

    data[2] = in[i++];

    out.append(encode_triplet(data,3),4);

    if(line_octets >= 68/4) // print carriage return
    {
      out.append("\r\n",2);
      line_octets = 0;
    }
    else
      ++line_octets;
    }

    return out;
}


vil_stream_url::vil_stream_url(char const *url)
  : underlying(0)
{
  // split URL into auth, host, path and port number.
  vcl_string host;
  vcl_string path;
  vcl_string auth;
  int port = 80; // default
  if (vcl_strncmp(url, "http://", 7) != 0)
    return; // doesn't look like a URL to me....

  char const *p = url + 7;
  while (*p && *p!='/')
    ++ p;
  host = vcl_string(url+7, p);

  if (*p)
    path = p+1;
  else
    path = "";


  //authentification
  for (unsigned int i=0; i<host.size(); ++i)
    if (host[i] == '@') {
      auth = vcl_string(host.c_str(), host.c_str()+i);
      host = vcl_string(host.c_str()+i+1, host.c_str() + host.size());
      break;
    }

  // port?
  for (unsigned int i=host.size()-1; i>0; --i)
    if (host[i] == ':') {
      port = vcl_atoi(host.c_str() + i + 1);
      host = vcl_string(host.c_str(), host.c_str() + i);
      break;
    }


  // so far so good.
#ifdef DEBUG
  vcl_cerr << "auth = \'" << auth << "\'" << vcl_endl
           << "host = \'" << host << "\'" << vcl_endl
           << "path = \'" << path << "\'" << vcl_endl
           << "port = " << port << vcl_endl;
#endif

#ifdef VCL_WIN32
  static int called_WSAStartup;
  if (called_WSAStartup==0)
  {
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    wVersionRequested = MAKEWORD( 2, 2 );

    err = WSAStartup( wVersionRequested, &wsaData );
  }
#endif

  // create socket endpoint.
  SOCKET tcp_socket = socket(PF_INET,      // IPv4 protocols.
                             SOCK_STREAM,  // two-way, reliable, connection-based stream socket.
                             PF_UNSPEC);   // protocol number.

#ifdef VCL_WIN32
  if (tcp_socket == INVALID_SOCKET) {
    vcl_cerr << __FILE__ ": failed to create socket." << vcl_endl;
# ifndef NDEBUG
    vcl_cerr << "error code : " << WSAGetLastError() << vcl_endl;
# endif
    return;
  }
#else
  if (tcp_socket < 0)
    vcl_cerr << __FILE__ ": failed to create socket." << vcl_endl;
#endif

#ifdef DEBUG
  vcl_cerr << __FILE__ ": tcp_sockect = " << tcp_socket << vcl_endl;
#endif

  // get network address of server.
  hostent *hp = gethostbyname(host.c_str());
  if (! hp) {
    vcl_cerr << __FILE__ ": failed to lookup host" << vcl_endl;
    return;
  }

  // make socket address.
  sockaddr_in my_addr;
  my_addr.sin_family = AF_INET;
  my_addr.sin_port = htons(port);  // convert port number to network byte order..
  vcl_memcpy(&my_addr.sin_addr, hp->h_addr_list[0], hp->h_length);

  // connect to server.
  if (connect(tcp_socket , (sockaddr *) &my_addr, sizeof my_addr) < 0) {
    vcl_cerr << __FILE__ ": failed to connect to host" << vcl_endl;
    //perror(__FILE__);
    return;
  }

  // buffer for data transfers over socket.
  char buffer[4096];

  // send HTTP 1.1 request.
  vcl_sprintf(buffer, "GET /%s / HTTP/1.1\n", path.c_str());
  if (auth != "")
    vcl_sprintf(buffer+vcl_strlen(buffer), "Authorization:  Basic %s\n", encode_base64(auth).c_str());
//    vcl_sprintf(buffer+vcl_strlen(buffer), "Authorization:  user  testuser:testuser\n");

#ifdef VCL_WIN32
  if (send(tcp_socket, buffer, vcl_strlen(buffer), 0) < 0)
#else
  if (::write(tcp_socket, buffer, vcl_strlen(buffer)) < 0)
#endif
  {
    vcl_cerr << __FILE__ ": error sending HTTP request" << vcl_endl;
    return;
  }

  // force the data to be sent.
#if 1
  shutdown(tcp_socket, 1); // disallow further sends.
#else
  for (int i=0; i<4096; ++i) ::write(tcp_socket, "\n\n\n\n", 4);
#endif

  // read from socket into memory.
  underlying = new vil_stream_core;
  underlying->ref();
  {
    unsigned entity_marker = 0; // count end of header CR and LFs
    int n;
#ifdef VCL_WIN32
    while ((n = recv(tcp_socket, buffer, sizeof buffer,0 )) > 0)
#else
    while ((n = ::read(tcp_socket, buffer, sizeof buffer)) > 0)
#endif
    {
      // search for the CRLFCRLF sequence that marks the end
      // of the http response header
      assert (entity_marker < 5);
      if (entity_marker==4)
        underlying->write(buffer, n);
      else
      {
        for (unsigned i=0; i<n; ++i)
        {
          if ((entity_marker==2||entity_marker==0) && buffer[i]=='\r') entity_marker++;
          else if (entity_marker==1 && buffer[i]=='\n') entity_marker++;
          else if (entity_marker==3 && buffer[i]=='\n')
          {
            entity_marker++;
            underlying->write(buffer+i+1, n-i-1);
            break;
          }
          else entity_marker=0;
        }
      }
    }
  }

  // close connection to server.
#ifdef VCL_WIN32
  closesocket(tcp_socket);
#else
  close(tcp_socket);
#endif
}

vil_stream_url::~vil_stream_url()
{
  if (underlying) {
    underlying->unref();
    underlying = 0;
  }
}

bool vil_stream_url::ok()
{
  return underlying && underlying->ok();
}

int vil_stream_url::write(void const *buf, int n)
{
  // strictly speaking, writes should fail, but that
  // isn't useful in any way.
  return underlying ? underlying->write(buf, n) : 0;
}

int vil_stream_url::read(void *buf, int n)
{
  return underlying ? underlying->read(buf, n) : 0;
}

int vil_stream_url::tell()
{
  return underlying ? underlying->tell() : -1;
}

void vil_stream_url::seek(int position)
{
  if (underlying) underlying->seek(position);
}
