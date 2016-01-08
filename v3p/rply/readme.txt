June 22, 2006
Nhon Trinh

This library is downloaded from http://www.cs.princeton.edu/~diego/professional/rply on June 13, 2006

Below is the description of the library on the website:

---------------------------------------------------------------
RPly is a library that lets applications read and write PLY files. The PLY file format is widely used to store geometric information, such as 3D models, but is general enough to be useful for other purposes.

There are other libraries out there, of course. I tried using them and finally decided to write my own. I tried to write it in such a way that others won't have to do it again and again. Everything that made me unhappy about the existing libraries was eliminated from RPly.

RPly is easy to use, well documented, small, free, open-source, ANSI C, efficient, and well tested. I will keep supporting it for a while because all my tools use the library for input/output. The highlights are:

A callback mechanism that makes PLY file input straightforward;
Support for the full range of numeric formats though the user only deals with doubles;
Binary (big and little endian) and text modes are fully supported;
Input and output are buffered for efficiency;
Available under the MIT license for added freedom.
The format was developed at Stanford University for use with their 3D scanning projects. Greg Turk's PLY library, available from Georgia Institute of Technology, seems to be the standard reference to the PLY file format, although there are some variations out there.

Whatever documentation and examples were found, were taken into consideration to create RPly. In theory, since RPly doesn't try to interpret the meaning of the data in a PLY file, it should be able to read any PLY file. In practice, the library works with all PLY files that I could find.
------------------------------------------------------------------

