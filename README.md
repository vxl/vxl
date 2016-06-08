## Introduction: What is VXL?
VXL (the Vision-something-Libraries) is a collection of C++ libraries designed for computer vision research and implementation. It was created from TargetJr and the IUE with the aim of making a light, fast and consistent system. VXL is written in ANSI/ISO C++ and is designed to be portable over many platforms. The core libraries in VXL are:

- vnl (numerics): Numerical containers and algorithms. e.g. matrices, vectors, decompositions, optimisers.
- vil (imaging): Loading, saving and manipulating images in many common file formats, including very large images.
- vgl (geometry): Geometry for points, curves and other elementary objects in 1, 2 or 3 dimensions.
- vsl (streaming I/O), vbl (basic templates), vul (utilities): Miscellaneous platform-independent functionality.

As well as the core libraries, there are libraries covering numerical algorithms, image processing, co-ordinate systems, camera geometry, stereo, video manipulation, structure recovery from motion, probability modelling, GUI design, classification, robust estimation, feature tracking, topology, structure manipulation, 3d imaging, and much more.

Each core library is lightweight, and can be used without reference to the other core libraries. Similarly, the non-core libraries don't depend on more than absolutely necessary, so you can compile and link just the libraries you really need.

For a more detailed description of the libraries see the [VXL book](http://public.kitware.com/vxl/doc/release/books/core/book.html).

VXL is developed and used by an [international](http://vxl.sourceforge.net/developers.html) team from academia and industry, including some of the world's leading computer vision experts.

## Documentation
Documentation on the whole of VXL
There are two main references for information on the whole of VXL:

- The VXL package documentation includes auto-generated descriptions of every class and function in VXL.
    - [Latest release](http://public.kitware.com/vxl/doc/release/index.html).
    - Current development version (Git snapshot) (hosted at [Kitware](http://public.kitware.com/vxl/doc/development/index.html), [Brown](http://www.lems.brown.edu/vision/vxl_doc/html/index.html) and [Manchester](http://paine.wiau.man.ac.uk/pub/doc_vxl/) ).
- The VXL book is a single reference for all high-level documentation on VXL.
    - [Latest release](http://public.kitware.com/vxl/doc/release/books/core/book.html).
    - [Current development version (Git snapshot)](http://public.kitware.com/vxl/doc/development/books/core/book.html).

You can also [download](http://sourceforge.net/projects/vxl/) the release 1.17 documentation.

## VXL guides
These are short guides to specific areas in VXL:

- xcv is an application built to demonstrate the functionality of the VXL libraries.
    - There is a [guide to using xcv](http://vxl.sourceforge.net/manuals/xcv/newxcv.html) (also available in [postscript format](http://vxl.sourceforge.net/manuals/xcv.ps.gz)).
    - Download [xcv for Windows](http://vxl.sourceforge.net/manuals/xcv.zip), [xcv for solaris](http://vxl.sourceforge.net/manuals/xcv-SunOS5.bin.gz), and [xcv for linux](http://vxl.sourceforge.net/manuals/xcv-linux.bin.gz)
    - Here is some [example data](http://vxl.sourceforge.net/manuals/xcvdata.tar.gz) suitable for use with xcv.

- vgui is the VXL graphical user interface library.
    - For a simple introduction to using VGUI there is the [VGUI users guide](http://vxl.sourceforge.net/manuals/vgui_users_guide/index.html) (also in [postscript format](http://vxl.sourceforge.net/manuals/vgui_users_guide.ps.gz)). Here is the [example code](http://vxl.sourceforge.net/manuals/examples-tutorial.tar.gz) used in the guide.
    - The [VGUI reference manual](http://vxl.sourceforge.net/manuals/vgui_ref_manual/index.html) (also in [postscript format](http://vxl.sourceforge.net/manuals/vgui_ref_manual.ps.gz)) is designed for developers of VGUI.

xcv and vgui have not changed a lot since the last release of VXL so these guides should be useful for both the release and development versions of VXL.

- The [MUL book](http://public.kitware.com/vxl/doc/release/books/contrib/mul/book.html) is an overview of libraries concerned with probability modelling, classification, and 3d and registered images.

## Download and install

- [VXL License](https://sourceforge.net/p/vxl/git/ci/master/tree/core/vxl_copyright.h)

You can download the latest official release vxl-1.17

- [Download VXL-1.17](http://sourceforge.net/projects/vxl/)
- [Installing VXL-1.17 version](http://vxl.sourceforge.net/releases/install-release.html)

Or get the latest development version from the Git repository at SourceForge.

- [VXL Git Repository Instructions](https://sourceforge.net/p/vxl/wiki/Git/)
- [Installing latest development version of VXL](http://vxl.sourceforge.net/releases/install-latest.html)

## Problems and where to get help
### FAQs

There is a [vxl-users FAQ](http://vxl.sourceforge.net/vxl-users-faq.html).

There are other FAQs in the [VXL book](http://public.kitware.com/vxl/doc/release/books/core/book.html).

### Mailing list

The [VXL mailing lists](http://sourceforge.net/mail/?group_id=24293) are at SourceForge. You can find list descriptions, subscription information, and archives there. [vxl-users](http://lists.sourceforge.net/lists/listinfo/vxl-users) is the best list for bug reports, etc. Please read the [vxl-users list policy](http://vxl.sourceforge.net/vxl-users-policy.html) before posting.

### Dashboard

The dashboards (at [cdash.org](http://www.cdash.org/CDash/index.php?project=vxl) and at [rpi.edu](http://www.cs.rpi.edu/research/groups/vxl/Testing/Dashboard/MostRecentResults-Nightly/Dashboard.html)) are a summary (collated from different machines at different sites in different timezones) of the current build status of VXL. More builds are welcome; just follow the [instructions for submitting a build](http://vxl.sourceforge.net/submit-build.html).

### Reporting bugs to the mailing list

If you think you have found a bug, or if you have an installation or build problem, please submit a bug report on the [vxl-users mailing list](http://lists.sourceforge.net/lists/listinfo/vxl-users). Please check the [vxl-users list policy](http://vxl.sourceforge.net/vxl-users-policy.html) to find out what information will be helpful in sorting out your bug.
