//:
// \file
// \author Philip C. Pritchett, Oxford RRG
// \date   29 Oct 99
//-----------------------------------------------------------------------------

#include <vgui/vgui.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_viewer2D.h>
#include <vgui/vgui_viewer3D.h>

#include <vrml/vgui_vrml.h>

#include <GL/glx.h>
#include <X11/Intrinsic.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>

#include <vgui_Xm/vgui_Xm_adaptor.h>

static int attribs[] = { GLX_RGBA, GLX_DEPTH_SIZE, 1, GLX_RED_SIZE, 1, GLX_DOUBLEBUFFER, None};


int main(int argc, char ** argv)
{
  if (argc < 3) {
    vcl_cerr << "First command line argument: image file\n"
             << "Second command line argument: VRML camera file\n";
    return 1;
  }

  vgui::init("motif");

  XtAppContext context;
  Widget toplevel = XtAppInitialize(&context, "vgui", NULL, 0,
                                    &argc, argv, NULL, NULL, 0);
  XtVaSetValues(toplevel,
                XmNwidth, 512,
                XmNheight, 256,
                NULL);

  Display *dpy = XtDisplay(toplevel);


  Widget form = XmCreateForm(toplevel, "form", NULL, 0);
  XtManageChild(form);


  Widget frame1 = XmCreateFrame(form, "frame", NULL, 0);
  Widget frame2 = XmCreateFrame(form, "frame", NULL, 0);

  Arg args[10];
  int n;

  n = 0;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNrightPosition, 50); n++;
  XtSetValues(frame1, args, n);

  n = 0;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
  XtSetArg(args[n], XmNleftPosition, 50); n++;
  XtSetValues(frame2, args, n);


  XtManageChild(frame1);
  XtManageChild(frame2);

  vgui_Xm_adaptor *adaptor1 = new vgui_Xm_adaptor;
  vgui_Xm_adaptor *adaptor2 = new vgui_Xm_adaptor;


  XVisualInfo *visinfo;
  if (!(visinfo = glXChooseVisual(dpy, DefaultScreen(dpy), attribs)))
    XtAppError(context, "no suitable RGB visual");

  adaptor1->setup_widget(&frame1, visinfo);
  adaptor2->setup_widget(&frame2, visinfo);

  XtRealizeWidget(toplevel);

  adaptor1->setup_context(dpy, visinfo);
  adaptor2->setup_context(dpy, visinfo);


  vgui_image_tableau image(argv[1]);
  vgui_viewer2D viewer2D(&image);

  vgui_vrml vrml_nice(argv[2]);
  vgui_viewer3D viewer3D(&vrml_nice);

  adaptor1->set_tableau(&viewer2D);
  adaptor2->set_tableau(&viewer3D);

  XtAppMainLoop(context);

  return 0;
}
