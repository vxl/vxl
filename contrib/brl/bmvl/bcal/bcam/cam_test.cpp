#include <vcl_iostream.h>
#include "CameraNode.h"
#include <vcsl/vcsl_graph.h>

int main()
{
	CameraNode c1, c2, c3;

	vcsl_graph cg;
	cg.put(&c1);
	cg.put(&c2);
	cg.put(&c3);
	

	return 0;
}