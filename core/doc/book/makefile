

GS = gswin32c
MAKEINFO= makeinfo.exe # exe to avoid conflict with cygnus makeinfo sh script

T2H_HOME=/pkg/Texi2html
export T2H_HOME
TEXI2HTML=perl /pkg/texi2html/texi2html.pl


all: book.html book.dvi

thelot: book.info book.html book.dvi book.hpj 

ifdef aaa
book.html: book.texi rosenbrock.png
	$(MAKEINFO)  book.texi --no-validate --html -o $@ --number-sections
else
book.html: book.texi rosenbrock.png
	$(TEXI2HTML) book.texi \
              -expand info \
              -number -split chapter -no-section_navigation -frames
endif

rosenbrock.png: rosenbrock.eps
	$(GS) -sDEVICE=png16m -sOutputFile=tmp.png -dQUIET -dNOPAUSE rosenbrock.eps -c showpage -c quit
	pngtopnm.exe tmp.png | pnmcrop.exe | pnmtopng.exe > $@
	rm tmp.png

book.dvi: book.texi atexinfo.tex
	texi2dvi book.texi || touch -t 010100001970 book.dvi



book.hpj book.rtf: book.texi
	-mkinfrtf.exe book --force -o book.rtf --hpj-output book.hpj


book.info: book.texi
	makeinfo.exe book --force -o book.info

publish:
	 rcp index.html book*.html volumnia.robots.ox.ac.uk.awf:WWW/vxl/book
