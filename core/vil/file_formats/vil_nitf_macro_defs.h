// Source

#ifndef vil_nitf_macro_defs_h_
#define vil_nitf_macro_defs_h_

//<: Copy string from y to x, but delete x first and recreate with exactly right length.
#define STRCPY(x,y) (delete [] x, x=new char[strlen(y)+1], vcl_strcpy(x,y))

//<: Copy n characters from y to x, but delete x first and recreate with exactly right length.
#define STRNCPY(x,y,n) (delete [] x, x=new char[n+1], x[n]=0,       \
			vcl_strncpy (x,y?y:"",n))

#endif // vil_nitf_macro_defs_h_
