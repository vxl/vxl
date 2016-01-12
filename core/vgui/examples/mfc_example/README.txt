
This is a bare-bones example VGUI project using MFC.

The steps to make your own are

1. Use the appwizard to make a normal MFC exe project.
2. Edit the myprojectView class to change its baseclass to
   vgui_mfc_adaptor.  Don't  forget to update the message maps
   in myprojectView.cpp
3. Store the tableaux that your application will use in
   the myprojectDoc

3.1. Add an OnCreate in the View to grab a tableau from the document.
3.2. Make View::OnDraw call the adaptor's
3.3. Make View::PreCreateWindow call the adaptor's

4. In general, search for "awf:" in the project files to find
   modifications of the basic MFC code.


awf@robots.ox.ac.uk, april 24, 2000
