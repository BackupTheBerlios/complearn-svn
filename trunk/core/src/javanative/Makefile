all: CompLearnJ.class CompLearnJ.h complearnj.dll

%.class: %.java
	javac $<

CompLearnJ.h: CompLearnJ.class
	javah -classpath . CompLearnJ

complearnj.dll: complearnj.so
	cp $< $@

complearnj.so: CompLearnJImp.c
	gcc -shared -mno-cygwin -I/usr/include "-I/home/Administrator/tmp/complearntest/include/complearn-1.0" "-I/cygdrive/c/Program Files/Java/jdk1.5.0_03/include" "-I/cygdrive/c/Program Files/Java/jdk1.5.0_03/include/win32" $< "/home/Administrator/tmp/complearntest/lib/libcomplearn.a" -o $@ "-L/cygdrive/c/Program Files/Microsoft Visual C++ Toolkit 2003/lib" `gsl-config --libs` -lgdbm -lz -lbz2
#	cl  "-IC:\cygwin\home\Administrator\tmp\complearntest\include\complearn-1.0" "-IC:\Program Files\Microsoft Visual C++ Toolkit 2003\include" "-Ic:\\Program Files\\Java\\jdk1.5.0_03\\include" "-Ic:\\Program Files\\Java\\jdk1.5.0_03\\include\\win32" "-IC:\cygwin\usr\include" "C:\cygwin\home\Administrator\tmp\complearntest\lib\libcomplearn.a" -LD $< -Fe$@ -link "/LIBPATH:C:\Program Files\Microsoft Visual C++ Toolkit 2003\lib"

clean:
	rm CompLearnJ.class CompLearnJ.h o complearnj.exp CompLearnJImp.obj complearnj.dll complearnj.lib
