// This is core/vul/vul_url.cxx
//:
// \file
// \author Ian Scott
// Based on vil_stream_url by fsm
// \verbatim
//  Modifications
//   8 Nov 2002 - Peter Vanroose - corrected HTTP client request syntax
// \endverbatim

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <fstream>
#include <iostream>
#include "vul_url.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <vul/vul_file.h>

#if defined (_WIN32) && !defined(__CYGWIN__)
# include <winsock2.h>
#else
# include <unistd.h>       // read(), write(), close()
# include <netdb.h>        // gethostbyname(), sockaddr_in()
# include <sys/socket.h>
# include <netinet/in.h>   // htons()
# define SOCKET int
#endif // unix

#if defined(_WIN32) && !defined(__CYGWIN__)
// So that we don't call WSAStartup more than we need to
static int called_WSAStartup = 0;
#endif

//: only call this method with a correctly formatted http URL
std::istream * vul_http_open(char const *url)
{
  // split URL into auth, host, path and port number.
  std::string host;
  std::string path;
  std::string auth;
  int port = 80; // default

  // check it is an http URL.
  assert (std::strncmp(url, "http://", 7) == 0);

  char const *p = url + 7;
  while (*p && *p!='/')
    ++ p;
  host = std::string(url+7, p);


  if (*p)
    path = p+1;
  else
    path = "";

  //authentication
  for (unsigned int i=0; i<host.size(); ++i)
    if (host[i] == '@') {
      auth = std::string(host.c_str(), host.c_str()+i);
      host = std::string(host.c_str()+i+1, host.c_str() + host.size());
      break;
    }

  // port?
  if (host.size() > 0L)
  for (unsigned int i=(unsigned int)(host.size()-1); i>0; --i)
    if (host[i] == ':') {
      port = std::atoi(host.c_str() + i + 1);
      host = std::string(host.c_str(), host.c_str() + i);
      break;
    }

  // do character translation
  unsigned k =0;
  while (k < path.size())
  {
    if (path[k] == ' ')
      path.replace(k, 1, "%20");
    else if (path[k] == '%')
      path.replace(k, 1, "%25");
    ++k;
  }

  // so far so good.
#ifdef DEBUG
  std::cerr << "auth = \'" << auth << "\'\n"
           << "host = \'" << host << "\'\n"
           << "path = \'" << path << "\'\n"
           << "port = " << port << std::endl;
#endif

#if defined(_WIN32) && !defined(__CYGWIN__)
  if (called_WSAStartup==0)
  {
    WORD wVersionRequested;
    WSADATA wsaData;

    wVersionRequested = MAKEWORD( 2, 2 );

    /* int err = */ WSAStartup( wVersionRequested, &wsaData );
  }
#endif

  // create socket endpoint.
  SOCKET tcp_socket = socket(PF_INET,      // IPv4 protocols.
                             SOCK_STREAM,  // two-way, reliable,
                                           // connection-based stream socket.
                             PF_UNSPEC);   // protocol number.
#if defined(_WIN32) && !defined(__CYGWIN__)
  if (tcp_socket == INVALID_SOCKET) {
# ifndef NDEBUG
    std::cerr << __FILE__ "error code : " << WSAGetLastError() << '\n';
# endif
#else
  if (tcp_socket < 0) {
#endif
    std::cerr << __FILE__ ": failed to create socket.\n";
    return nullptr;
  }

#ifdef DEBUG
  std::cerr << __FILE__ ": tcp_socket = " << tcp_socket << '\n';
#endif

  // get network address of server.
  hostent *hp = gethostbyname(host.c_str());
  if (! hp) {
    std::cerr << __FILE__ ": failed to lookup host\n";

#if defined(_WIN32) && !defined(__CYGWIN__)
    closesocket(tcp_socket);
#else
    close(tcp_socket);
#endif

    return nullptr;
  }

  // make socket address.
  sockaddr_in my_addr;
  my_addr.sin_family = AF_INET;
  // convert port number to network byte order..
  my_addr.sin_port = htons(port);
  std::memcpy(&my_addr.sin_addr, hp->h_addr_list[0], hp->h_length);

  // connect to server.
  if (connect(tcp_socket , (sockaddr *) &my_addr, sizeof my_addr) < 0) {
    std::cerr << __FILE__ ": failed to connect to host\n";
    //perror(__FILE__);

#if defined(_WIN32) && !defined(__CYGWIN__)
    closesocket(tcp_socket);
#else
    close(tcp_socket);
#endif

    return nullptr;
  }

  // buffer for data transfers over socket.
  char buffer[4096];

  // send HTTP 1.1 request.
  std::snprintf(buffer, 4090-std::strlen(buffer),
               "GET %s HTTP/1.1\r\nUser-Agent: vul_url\r\nHost: %s\r\nAccept: */*\r\n",
               url, host.c_str());

  if (auth != "")
    std::snprintf(buffer+std::strlen(buffer), 4090-std::strlen(buffer),
                 "Authorization: Basic %s\r\n",
                 vul_url::encode_base64(auth).c_str());

  if (std::snprintf(buffer+std::strlen(buffer), 4090-std::strlen(buffer), "\r\n") < 0)
  {
    std::cerr << "ERROR: vul_http_open buffer overflow.";
    std::abort();
  }

#if defined(_WIN32) && !defined(__CYGWIN__)
  if (send(tcp_socket, buffer, (int)std::strlen(buffer), 0) < 0) {
#else
  if (::write(tcp_socket, buffer, std::strlen(buffer)) < 0) {
#endif
    std::cerr << __FILE__ ": error sending HTTP request\n";

#if defined(_WIN32) && !defined(__CYGWIN__)
    closesocket(tcp_socket);
#else
    close(tcp_socket);
#endif
    return nullptr;
  }


  // read from socket into memory.
  std::string contents;
  {
    int n;
#if defined(_WIN32) && !defined(__CYGWIN__)
    while ((n = recv(tcp_socket, buffer, sizeof buffer,0 )) > 0) {
#else
    while ((n = ::read(tcp_socket, buffer, sizeof buffer)) > 0) {
#endif
      contents.append(buffer, n);
#ifdef DEBUG
      std::cerr << n << " bytes\n";
#endif
    }
  }

  // close connection to server.
#if defined(_WIN32) && !defined(__CYGWIN__)
  closesocket(tcp_socket);
#else
  close(tcp_socket);
#endif

#ifdef DEBUG
  std::cerr << "HTTP server returned:\n" << contents << '\n';
#endif

  if (contents.find("HTTP/1.1 200") == contents.npos)
  {
    return nullptr;
  }
  std::string::size_type n = contents.find("\r\n\r\n");
  if (n == contents.npos)
  {
    return nullptr;
  }

  contents.erase(0,n+4);
#ifdef DEBUG
  std::cerr << "vul_url::vul_http_open() returns:\n" << contents << '\n';
#endif
  return new std::istringstream(contents);
}


//: only call this method with a correctly formatted http URL
bool vul_http_exists(char const *url)
{
  // split URL into auth, host, path and port number.
  std::string host;
  std::string path;
  std::string auth;
  int port = 80; // default
  assert (std::strncmp(url, "http://", 7) == 0);

  char const *p = url + 7;
  while (*p && *p!='/')
    ++ p;
  host = std::string(url+7, p);


  if (*p)
    path = p+1; // may be the empty string, if URL ends in a slash
  else
    path = "";

  //authentication
  for (unsigned int i=0; i<host.size(); ++i)
    if (host[i] == '@') {
      auth = std::string(host.c_str(), host.c_str()+i);
      host = std::string(host.c_str()+i+1, host.c_str() + host.size());
      break;
    }

  // port?
  for (unsigned int i=0; i<host.size(); ++i)
    if (host[i] == ':') {
      port = std::atoi(host.c_str() + i + 1);
      host = std::string(host.c_str(), host.c_str() + i);
      break;
    }

  // do character translation
  unsigned k =0;
  while (k < path.size())
  {
    if (path[k] == ' ')
      path.replace(k, 1, "%20");
    else if (path[k] == '%')
      path.replace(k, 1, "%25");
    k++;
  }

  // so far so good.
#ifdef DEBUG
  std::cerr << "auth = \'" << auth << "\'\n"
           << "host = \'" << host << "\'\n"
           << "path = \'" << path << "\'\n"
           << "port = " << port << std::endl;
#endif

#if defined(_WIN32) && !defined(__CYGWIN__)
  if (called_WSAStartup==0)
  {
    WORD wVersionRequested;
    WSADATA wsaData;

    wVersionRequested = MAKEWORD( 2, 2 );

    /* int err = */ WSAStartup( wVersionRequested, &wsaData );
  }
#endif

  // create socket endpoint.
  SOCKET tcp_socket = socket(PF_INET,      // IPv4 protocols.
                             SOCK_STREAM,  // two-way, reliable,
                                           // connection-based stream socket.
                             PF_UNSPEC);   // protocol number.

#if defined(_WIN32) && !defined(__CYGWIN__)
  if (tcp_socket == INVALID_SOCKET) {
# ifndef NDEBUG
    std::cerr << "error code : " << WSAGetLastError() << std::endl;
# endif
#else
  if (tcp_socket < 0) {
#endif
    std::cerr << __FILE__ ": failed to create socket.\n";
    return false;
  }

#ifdef DEBUG
  std::cerr << __FILE__ ": tcp_socket = " << tcp_socket << std::endl;
#endif

  // get network address of server.
  hostent *hp = gethostbyname(host.c_str());
  if (! hp) {
    std::cerr << __FILE__ ": failed to lookup host\n";
    return false;
  }

  // make socket address.
  sockaddr_in my_addr;
  my_addr.sin_family = AF_INET;
    // convert port number to network byte order..
  my_addr.sin_port = htons(port);
  std::memcpy(&my_addr.sin_addr, hp->h_addr_list[0], hp->h_length);

  // connect to server.
  if (connect(tcp_socket , (sockaddr *) &my_addr, sizeof my_addr) < 0)
  {
    std::cerr << __FILE__ ": failed to connect to host\n";
    //perror(__FILE__);
#if defined(_WIN32) && !defined(__CYGWIN__)
    closesocket(tcp_socket);
#else
    close(tcp_socket);
#endif

    return false;
  }

  // buffer for data transfers over socket.
  char buffer[4096];

  // send HTTP 1.1 request.
  std::snprintf(buffer, 4090,
               "HEAD %s HTTP/1.1\r\nUser-Agent: vul_url\r\nHost: %s\r\nAccept: */*\r\n",
               url, host.c_str());
  if (auth != "")
    std::snprintf(buffer+std::strlen(buffer), 4090-std::strlen(buffer),
                 "Authorization: Basic %s\r\n",
                 vul_url::encode_base64(auth).c_str() );

  if (std::snprintf(buffer+std::strlen(buffer), 4090-std::strlen(buffer), "\r\n") < 0)
  {
    std::cerr << "ERROR: vul_http_exists buffer overflow.";
    std::abort();
  }

#if defined(_WIN32) && !defined(__CYGWIN__)
  if (send(tcp_socket, buffer, (int)std::strlen(buffer), 0) < 0) {
#else
  if (::write(tcp_socket, buffer, std::strlen(buffer)) < 0) {
#endif
    std::cerr << __FILE__ ": error sending HTTP request\n";

#if defined(_WIN32) && !defined(__CYGWIN__)
    closesocket(tcp_socket);
#else
    close(tcp_socket);
#endif
    return false;
  }


  // read from socket into memory.
  std::string contents;
  {
    int n;
#if defined(_WIN32) && !defined(__CYGWIN__)
    if ((n = recv(tcp_socket, buffer, sizeof buffer,0 )) > 0) {
#else
    if ((n = ::read(tcp_socket, buffer, sizeof buffer)) > 0) {
#endif
      contents.append(buffer, n);
      //std::cerr << n << " bytes\n";
    }
    else
    {
#if defined(_WIN32) && !defined(__CYGWIN__)
      closesocket(tcp_socket);
#else
      close(tcp_socket);
#endif
      return false;
    }
  }

  // close connection to server.
#if defined(_WIN32) && !defined(__CYGWIN__)
  closesocket(tcp_socket);
#else
  close(tcp_socket);
#endif

#ifdef DEBUG
  std::cerr << "HTTP server returned:\n" << contents << '\n';
#endif

  return contents.find("HTTP/1.1 200") != contents.npos;
}


std::istream * vul_url::open(const char * url, std::ios::openmode mode)
{
  // check for null pointer or empty strings.
  if (!url || !*url)
    return nullptr;
  unsigned int l = (unsigned int)std::strlen(url);

  // check for filenames beginning "file:".
  if (l > 7 && std::strncmp(url, "file://", 7) == 0)
    return new std::ifstream(url+7,mode);

  // maybe it's an http URL?
  if (l > 7 && std::strncmp(url, "http://", 7) == 0)
    return vul_http_open(url);

  // maybe it's an ftp URL?
  if (l > 6 && std::strncmp(url, "ftp://", 6) == 0)
  {
    std::cerr << __LINE__ << "ERROR:\n vul_read_url(const char * url)\n"
      "Doesn't support FTP yet, url=" << url << std::endl;
    return nullptr;
  }

  // try an ordinary filename
  return new std::ifstream(url, mode);
}


//: Does that URL exist
bool vul_url::exists(const char * url)
{
  // check for null pointer or empty strings.
  if (!url || !*url)
    return false;
  unsigned int l = (unsigned int)std::strlen(url);

  // check for filenames beginning "file:".
  if (l > 7 && std::strncmp(url, "file://", 7) == 0)
    return vul_file::exists(url+7);

  // maybe it's an http URL?
  if (l > 7 && std::strncmp(url, "http://", 7) == 0)
    return vul_http_exists(url);

  // maybe it's an ftp URL?
  if (l > 6 && std::strncmp(url, "ftp://", 6) == 0)
  {
    std::cerr << "ERROR: vul_read_url(const char * url)\n"
      "Doesn't support FTP yet, url=" << url << std::endl;
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
  unsigned int l = (unsigned int)std::strlen(url);

  // check for filenames beginning "file:".
  if (l > 7 && std::strncmp(url, "file://", 7) == 0)
    return true;

  // maybe it's an http URL?
  if (l > 7 && std::strncmp(url, "http://", 7) == 0)
    return true;

  // maybe it's an https URL?
  if (l > 8 && std::strncmp(url, "https://", 7) == 0)
    return true;

  // maybe it's an ftp URL?
  if (l > 6 && std::strncmp(url, "ftp://", 6) == 0)
    return true;

  return false;
}

//=======================================================================

bool vul_url::is_file(const char * fn)
{
  if (vul_url::is_url(fn))
    return vul_url::exists(fn);
  else
    return vul_file::exists(fn) && ! vul_file::is_directory(fn);
}

//=======================================================================

static const
char base64_encoding[]=
{
  'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P',
  'Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f',
  'g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v',
  'w','x','y','z','0','1','2','3','4','5','6','7','8','9','+','/'
};

static char out_buf[4];

static const char * encode_triplet(char data[3], unsigned int n)
{
  assert (n>0 && n <4);
  out_buf[0] = base64_encoding[(data[0] & 0xFC) >> 2];
  out_buf[1] = base64_encoding[
    ((data[0] & 0x3) << 4) + ((data[1] & 0xf0)>>4)];

  if (n==1)
  {
    out_buf[2] = out_buf[3] = '=';
    return out_buf;
  }

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

std::string vul_url::encode_base64(const std::string& in)
{
  std::string out;
  unsigned int i = 0, line_octets = 0;
  const unsigned int l = (unsigned int)(in.size());
  char data[3];
  while (i <= l)
  {
    if (i == l)
    {
      out.append("=");
      return out;
    }

    data[0] = in[i++];
    data[1] = data[2] = 0;

    if (i == l)
    {
      out.append(encode_triplet(data,1),4);
      return out;
    }

    data[1] = in[i++];

    if (i == l)
    {
      out.append(encode_triplet(data,2),4);
      return out;
    }

    data[2] = in[i++];

    out.append(encode_triplet(data,3),4);

    if (line_octets >= 68/4) // print carriage return
    {
      out.append("\r\n",2);
      line_octets = 0;
    }
    else
      ++line_octets;
  }

  return out;
}

//=======================================================================

static int get_next_char(const std::string &in, unsigned int *i)
{
  while (*i < in.size())
  {
    char c;
    c = in[(*i)++];

    if (c == '+')
      return 62;

    if (c == '/')
      return 63;

    if (c >= 'A' && c <= 'Z')
      return 0 + (int)c - (int)'A';

    if (c >= 'a' && c <= 'z')
      return 26 + (int)c - (int)'a';

    if (c >= '0' && c <= '9')
      return 52 + (int)c - (int)'0';

    if (c == '=')
      return 64;
  }
  return -1;
}

//=======================================================================

std::string vul_url::decode_base64(const std::string& in)
{
  int c;
  char data[3];

  unsigned int i=0;
  const unsigned int l = (unsigned int)(in.size());
  std::string out;
  while (i < l)
  {
    data[0] = data[1] = data[2] = 0;

    // -=- 0 -=-
    // Search next valid char...
    c = get_next_char(in , &i);

    // treat '=' as end of message
    if (c == 64)
      return out;
    if (c==-1)
      return "";

    data[0] = char(((c & 0x3f) << 2) | (0x3 & data[0]));

    // -=- 1 -=-
    // Search next valid char...
    c = get_next_char(in , &i);

      // Error! Second character in octet can't be '='
    if (c == 64 || c==-1)
      return "";

    data[0] = char(((c & 0x30) >> 4) | (0xfc & data[0]));
    data[1] = char(((c & 0x0f) << 4) | (0x0f & data[1]));

    // -=- 2 -=-
    // Search next valid char...

    c = get_next_char(in , &i);

    if (c==-1)
      return "";
    if (c == 64)
    {
      // should really read next char and check it is '='
      out.append(data,1);  // write 1 byte to output
      return out;
    }

    data[1] = char(((c & 0x3c) >> 2) | (0xf0 & data[1]));
    data[2] = char(((c & 0x03) << 6) | (0x3f & data[2]));

    // -=- 3 -=-
    // Search next valid char...
    c = get_next_char(in , &i);

    if (c==-1)
      return "";

    if (c == 64)
    {
      out.append(data,2);  // write 2 bytes to output
      return out;
    }

    data[2] = char((c & 0x3f) | (0xc0 & data[2]));

    out.append(data,3);  // write 3 bytes to output
  }

  return out;
}
