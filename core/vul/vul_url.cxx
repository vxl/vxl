#ifdef __GNUC__
#pragma implementation
#endif
#include "vul_url.h"

//:
// \file
// \author Ian Scott
// Based on vil_stream_url by fsm

#include <vcl_cstdio.h>  // sprintf()
#include <vcl_cstring.h>
#include <vcl_cstdlib.h>
#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vcl_cassert.h>
#include <vcl_fstream.h>
#include <vul/vul_file.h>

#if defined(__unix__)

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


#ifdef VCL_WIN32
// So that we don't call WSAStartup more than we need to
static int called_WSAStartup = 0;
#endif

//: only call this method with a correctly formatted http URL
vcl_istream * vul_http_open(char const *url)
{
  // split URL into auth, host, path and port number.
  vcl_string host;
  vcl_string path;
  vcl_string auth;
  int port = 80; // default
  assert (vcl_strncmp(url, "http://", 7) == 0);// doesn't look like an http URL to me.

  char const *p = url + 7;
  while (*p && *p!='/')
    ++ p;
  host = vcl_string(url+7, p);


  if (*p)
    path = p+1;
  else
    path = "";

  // port?
  for (unsigned int i=0; i<host.size(); ++i)
    if (host[i] == ':') {
      port = vcl_atoi(host.c_str() + i + 1);
      host = vcl_string(host.c_str(), host.c_str() + i);
      break;
    }

  //authentification
  for (unsigned int i=0; i<host.size(); ++i)
    if (host[i] == '@') {
      auth = vcl_string(host.c_str(), host.c_str()+i);
      host = vcl_string(host.c_str()+i+1, host.c_str() + host.size());
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
# ifndef NDEBUG
    vcl_cerr << "error code : " << WSAGetLastError() << vcl_endl;
# endif
#else
  if (tcp_socket < 0) {
#endif
    vcl_cerr << __FILE__ ": failed to create socket." << vcl_endl;
    return 0;
  }

#ifdef DEBUG
  vcl_cerr << __FILE__ ": tcp_sockect = " << tcp_socket << vcl_endl;
#endif

  // get network address of server.
  hostent *hp = gethostbyname(host.c_str());
  if (! hp) {
    vcl_cerr << __FILE__ ": failed to lookup host" << vcl_endl;
    return 0;
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
    return 0;
  }

  // buffer for data transfers over socket.
  char buffer[4096];

  // send HTTP 1.0 request.
  vcl_sprintf(buffer, "GET http://%s/%s\n", host.c_str(), path.c_str());
  if (auth != "")
    vcl_sprintf(buffer+vcl_strlen(buffer), "Authorization:  user %s\n", auth.c_str());

#ifdef VCL_WIN32
  if (send(tcp_socket, buffer, vcl_strlen(buffer), 0) < 0) {
#else
  if (::write(tcp_socket, buffer, vcl_strlen(buffer)) < 0) {
#endif
    vcl_cerr << __FILE__ ": error sending HTTP request" << vcl_endl;
    return 0;
  }

  // force the data to be sent.
#if 1
  shutdown(tcp_socket, 1); // disallow further sends.
#else
  for (int i=0; i<4096; ++i) ::write(tcp_socket, "\n\n\n\n", 4);
#endif

  // read from socket into memory.
  vcl_string contents;
  {
    int n;
#ifdef VCL_WIN32
    while ((n = recv(tcp_socket, buffer, sizeof buffer,0 )) > 0) {
#else
    while ((n = ::read(tcp_socket, buffer, sizeof buffer)) > 0) {
#endif
      contents.append(buffer, n);
      //vcl_cerr << n << " bytes" << vcl_endl;
    }
  }

  // close connection to server.
#ifdef VCL_WIN32
  closesocket(tcp_socket);
#else
  close(tcp_socket);
#endif

  return new vcl_istringstream(contents);
}


//: only call this method with a correctly formatted http URL
bool vul_http_exists(char const *url)
{
  // split URL into auth, host, path and port number.
  vcl_string host;
  vcl_string path;
  vcl_string auth;
  int port = 80; // default
  assert (vcl_strncmp(url, "http://", 7) == 0);// doesn't look like an http URL to me.

  char const *p = url + 7;
  while (*p && *p!='/')
    ++ p;
  host = vcl_string(url+7, p);


  if (*p)
    path = p+1;
  else
    path = "";

  // port?
  for (unsigned int i=0; i<host.size(); ++i)
    if (host[i] == ':') {
      port = vcl_atoi(host.c_str() + i + 1);
      host = vcl_string(host.c_str(), host.c_str() + i);
      break;
    }

  //authentification
  for (unsigned int i=0; i<host.size(); ++i)
    if (host[i] == '@') {
      auth = vcl_string(host.c_str(), host.c_str()+i);
      host = vcl_string(host.c_str()+i+1, host.c_str() + host.size());
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
# ifndef NDEBUG
    vcl_cerr << "error code : " << WSAGetLastError() << vcl_endl;
# endif
#else
  if (tcp_socket < 0) {
#endif
    vcl_cerr << __FILE__ ": failed to create socket." << vcl_endl;
    return false;
  }

#ifdef DEBUG
  vcl_cerr << __FILE__ ": tcp_sockect = " << tcp_socket << vcl_endl;
#endif

  // get network address of server.
  hostent *hp = gethostbyname(host.c_str());
  if (! hp) {
    vcl_cerr << __FILE__ ": failed to lookup host" << vcl_endl;
    return false;
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
    return false;
  }

  // buffer for data transfers over socket.
  char buffer[4096];

  // send HTTP 1.0 request.
  vcl_sprintf(buffer, "GET http://%s/%s\n", host.c_str(), path.c_str());
  if (auth != "")
    vcl_sprintf(buffer+vcl_strlen(buffer), "Authorization:  user %s\n", auth.c_str());

#ifdef VCL_WIN32
  if (send(tcp_socket, buffer, vcl_strlen(buffer), 0) < 0) {
#else
  if (::write(tcp_socket, buffer, vcl_strlen(buffer)) < 0) {
#endif
    vcl_cerr << __FILE__ ": error sending HTTP request" << vcl_endl;

#ifdef VCL_WIN32
    closesocket(tcp_socket);
#else
    close(tcp_socket);
#endif
    return false;
  }

  // force the data to be sent.
#if 1
  shutdown(tcp_socket, 1); // disallow further sends.
#else
  for (int i=0; i<4096; ++i) ::write(tcp_socket, "\n\n\n\n", 4);
#endif

  // read from socket into memory.
  vcl_string contents;
  {
    int n;
#ifdef VCL_WIN32
    if ((n = recv(tcp_socket, buffer, sizeof buffer,0 )) > 0) {
#else
    if ((n = ::read(tcp_socket, buffer, sizeof buffer)) > 0) {
#endif
      contents.append(buffer, n);
      //vcl_cerr << n << " bytes" << vcl_endl;
    }
    else
    {
#ifdef VCL_WIN32
      closesocket(tcp_socket);
#else
      close(tcp_socket);
#endif
      return false;
    }
  }

  // close connection to server.
#ifdef VCL_WIN32
  closesocket(tcp_socket);
#else
  close(tcp_socket);
#endif
  vcl_string::size_type  n;
  if ( ((n = contents.find("<HTML>")) != contents.npos)
    && (contents.find("404 Not Found",n) != contents.npos) )
  {
    return false;
  }

  return true;
}


vcl_istream * vul_url::open(const char * url)
{
  // check for null pointer or empty strings.
  if (!url || !*url)
    return 0;
  unsigned l = vcl_strlen(url);

  // check for filenames beginning "file:".
  if (l > 7 && vcl_strncmp(url, "file://", 7) == 0)
    return new vcl_ifstream(url+7,vcl_ios_binary);

  // maybe it's an http URL?
  if (l > 7 && vcl_strncmp(url, "http://", 7) == 0)
    return vul_http_open(url);

  // maybe it's an ftp URL?
  if (l > 6 && vcl_strncmp(url, "ftp://", 6) == 0)
  {
    vcl_cerr << __LINE__ << "ERROR:\n vul_read_url(const char * url)\n"
      "Doesn't support FTP yet, url=" << url << vcl_endl;
    return 0;
  }

  // try an ordinary filename
  return new vcl_ifstream(url, vcl_ios_binary);
}


//: Does that URL exist
bool vul_url::exists(const char * url)
{
  // check for null pointer or empty strings.
  if (!url || !*url)
    return false;
  unsigned l = vcl_strlen(url);

  // check for filenames beginning "file:".
  if (l > 7 && vcl_strncmp(url, "file://", 7) == 0)
    return vul_file::exists(url+7);

  // maybe it's an http URL?
  if (l > 7 && vcl_strncmp(url, "http://", 7) == 0)
    return vul_http_exists(url);

  // maybe it's an ftp URL?
  if (l > 6 && vcl_strncmp(url, "ftp://", 6) == 0)
  {
    vcl_cerr << __LINE__ << "ERROR:\n vul_read_url(const char * url)\n"
      "Doesn't support FTP yet, url=" << url << vcl_endl;
    return false;
  }

  // try an ordinary filename
  return vul_file::exists(url);
}

//: Is that a URL
bool vul_url::is_url(const char * url)
{
  // check for null pointer or empty strings.
  if (!url || !*url)
    return false;
  unsigned l = vcl_strlen(url);

  // check for filenames beginning "file:".
  if (l > 7 && vcl_strncmp(url, "file://", 7) == 0)
    return true;

  // maybe it's an http URL?
  if (l > 7 && vcl_strncmp(url, "http://", 7) == 0)
    return true;

  // maybe it's an ftp URL?
  if (l > 6 && vcl_strncmp(url, "ftp://", 6) == 0)
  {
    return true;
  }

  return false;
}

//=======================================================================

bool vul_url::is_file(const char * fn)
{
  return
    (vul_url::is_url(fn) && vul_url::exists(fn)) ||
    (! vul_url::is_url(fn) &&
     vul_file::exists(fn) &&
     ! vul_file::is_directory(fn) );
}
