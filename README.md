## Introduction: What is VXL?
VXL (the Vision-something-Libraries) is a collection of C++ libraries designed for computer vision research and implementation. It was created from TargetJr and the IUE with the aim of making a light, fast and consistent system. VXL is written in ANSI/ISO C++ and is designed to be portable over many platforms. The core libraries in VXL are:

    vnl (numerics): Numerical containers and algorithms. e.g. matrices, vectors, decompositions, optimisers.
    vil (imaging): Loading, saving and manipulating images in many common file formats, including very large images.
    vgl (geometry): Geometry for points, curves and other elementary objects in 1, 2 or 3 dimensions.
    vsl (streaming I/O), vbl (basic templates), vul (utilities): Miscellaneous platform-independent functionality.

As well as the core libraries, there are libraries covering numerical algorithms, image processing, co-ordinate systems, camera geometry, stereo, video manipulation, structure recovery from motion, probability modelling, GUI design, classification, robust estimation, feature tracking, topology, structure manipulation, 3d imaging, and much more.

Each core library is lightweight, and can be used without reference to the other core libraries. Similarly, the non-core libraries don't depend on more than absolutely necessary, so you can compile and link just the libraries you really need.

For a more detailed description of the libraries see the VXL book.

VXL is developed and used by an international team from academia and industry, including some of the world's leading computer vision experts.

## Documentation
Documentation on the whole of VXL
There are two main references for information on the whole of VXL:

    The VXL package documentation includes auto-generated descriptions of every class and function in VXL.
        Latest release.
        Current development version (Git snapshot) (hosted at Kitware, Brown and Manchester ).
    The VXL book is a single reference for all high-level documentation on VXL.
        Latest release.
        Current development version (Git snapshot).

You can also download the release 1.17 documentation.

## VXL guides
These are short guides to specific areas in VXL:

    xcv is an application built to demonstrate the functionality of the VXL libraries.
        There is a guide to using xcv (also available in postscript format).
        Download xcv for Windows, xcv for solaris. and xcv for linux
        Here is some example data suitable for use with xcv.

    vgui is the VXL graphical user interface library.
        For a simple introduction to using VGUI there is the VGUI users guide (also in postscript format). Here is the example code used in the guide.
        The VGUI reference manual (also in postscript format) is designed for developers of VGUI.

xcv and vgui have not changed a lot since the last release of VXL so these guides should be useful for both the release and development versions of VXL.

    The MUL book is an overview of libraries concerned with probability modelling, classification, and 3d and registered images.

## Download and install

    VXL License

You can download the latest official release vxl-1.17

    Download VXL-1.17


    Installing VXL-1.17 version

Or get the latest development version from the Git repository at SourceForge.

    VXL Git Repository Instructions
    Installing latest development version of VXL

## Problems and where to get help
### FAQs

There is a vxl-users FAQ.

There are other FAQs in the VXL book.

### Mailing list

The VXL mailing lists are at SourceForge. You can find list descriptions, subscription information, and archives there. vxl-users is the best list for bug reports, etc. Please read the vxl-users list policy before posting.

### Dashboard

The dashboards (at cdash.org and at rpi.edu) are a summary (collated from different machines at different sites in different timezones) of the current build status of VXL. More builds are welcome; just follow the instructions for submitting a build.

### Reporting bugs to the mailing list

If you think you have found a bug, or if you have an installation or build problem, please submit a bug report on the vxl-users mailing list. Please check the vxl-users list policy to find out what information will be helpful in sorting out your bug.
