
VXL Example: vnl_calc


This is a command-line calculator, which can do matrix manipulation on matrices 
stored in files.  Most commonly used to invert homographies:

vnl_calc file.H i

It's a RPN (reverse Polish notation i.e. stack-based) calculator, so each operator 
operates on the top two elements of the stack. e.g.

vnl_calc 2 3 + 4 / 

prints (2 + 3) / 4 = 1.25
