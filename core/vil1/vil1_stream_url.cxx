/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation "vil_stream_url"
#endif
#include "vil_stream_url.h"

#include <vcl_cstdio.h>  // sprintf()
#include <vcl_cstring.h>
#include <vcl_cstdlib.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vil/vil_stream_core.h>

struct vil_stream_url_data
{
  vil_stream *underlying;
};

#if defined(__unix__)

#include <unistd.h>       // read(), write(), close()
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>        // gethostbyname()

vil_stream_url::vil_stream_url(char const *url)
  : data(0)
{
  // split URL into host, path and port number.
  vcl_string host;
  vcl_string path;
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
  
  // port?
  for (int i=0; i<host.size(); ++i)
    if (host[i] == ':') {
      port = vcl_atoi(host.c_str() + i + 1);
      host = vcl_string(host.c_str(), host.c_str() + i);
      break;
    }
  
#if 0  
  vcl_cerr << __FILE__ << vcl_endl;
  perror(0);
  perror(0);
  vcl_cerr << __FILE__ << vcl_endl;
#endif
  
  // so far so good.
#if 0
  vcl_cerr << "host = \'" << host << "\'" << vcl_endl
	   << "path = \'" << path << "\'" << vcl_endl
	   << "port = " << port << vcl_endl;
#endif
  
  // create socket endpoint.
  int tcp_socket = socket(PF_INET,      // IPv4 protocols.
			  SOCK_STREAM,  // two-way, reliable, connection-based stream socket.
			  PF_UNSPEC);   // protocol number.
  if (tcp_socket < 0) {
    vcl_cerr << __FILE__ ": failed to open socket." << vcl_endl;
    return;
  }
  //vcl_cerr << __FILE__ ": tcp_sockect = " << tcp_socket << vcl_endl;
  
  // get network address of server.
  hostent *hp = gethostbyname(host.c_str());
  
  // connect to server.
  sockaddr_in my_addr;
  my_addr.sin_family = AF_INET;
  my_addr.sin_port = htons(port);  // convert port number to network format.
  vcl_memcpy(&my_addr.sin_addr, hp->h_addr_list[0], hp->h_length);
  if (connect(tcp_socket , (sockaddr *) &my_addr, sizeof my_addr) < 0) {
    vcl_cerr << __FILE__ ": failed to connect to host" << vcl_endl;
    //perror(__FILE__);
    return;
  }
  
  // buffer for data transfers over socket.
  char buffer[4096];
  
  // send HTTP 1.0 request.
  if (port == 80)
    vcl_sprintf(buffer, "GET http://%s/%s\n", host.c_str(), path.c_str());
  else
    vcl_sprintf(buffer, "GET http://%s:%d/%s\n", host.c_str(), port, path.c_str());

  if (::write(tcp_socket, buffer, strlen(buffer)) < 0) {
    vcl_cerr << __FILE__ ": error sending HTTP request" << vcl_endl;
    return;
  }
  
  // force the data to be sent.
#if 1
  shutdown(tcp_socket, 1); // disallow further sends.
#else
  for (int i=0; i<4096; ++i) write(tcp_socket, "\n\n\n\n", 4);
#endif
  
  // read from the socket.
  data = new vil_stream_url_data; {
    data->underlying = new vil_stream_core;
    data->underlying->ref();
    int n;
    while ((n = ::read(tcp_socket, buffer, sizeof buffer)) > 0) {
      data->underlying->write(buffer, n);
      //vcl_cerr << n << " bytes" << vcl_endl;
    }
  }
  
  // close connection to server.
  close(tcp_socket);
}
#else
vil_stream_url::vil_stream_url(char const *) : data(0)
{
  vcl_cerr << __FILE__ ": only implemented for unix at the moment" << vcl_endl;
}
#endif

vil_stream_url::~vil_stream_url()
{
  if (data) {
    data->underlying->unref();
    delete data;
    data = 0;
  }
}
  
bool vil_stream_url::ok()
{
  return data && data->underlying && data->underlying->ok();
}

int vil_stream_url::write(void const *buf, int n)
{
  return (data && data->underlying) ? data->underlying->write(buf, n) : 0;
}

int vil_stream_url::read(void *buf, int n)
{
  return (data && data->underlying) ? data->underlying->read(buf, n) : 0;
}

int vil_stream_url::tell()
{
  return (data && data->underlying) ? data->underlying->tell() : -1;
}

void vil_stream_url::seek(int position)
{
  if (data && data->underlying) data->underlying->seek(position);
}
