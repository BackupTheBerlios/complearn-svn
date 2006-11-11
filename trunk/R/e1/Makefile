rcomplearn.so: rcomplearn.c
	gcc -shared  -o rcomplearn.so rcomplearn.c   -L/usr/lib/R/lib -lR -I/usr/share/R/include /usr/lib/libcomplearn.a -I/usr/include/complearn-1.0 -lcsoap -lnanohttp -lgdbm -lgsl -lz -lbz2
#	GOODCFLAGS="-DNDEBUG\ -Wall\ -I/usr/include/complearn-1.0\ -I/usr/local/include\ -I/usr/include/libxml2\ -I/usr/include/nanohttp-1.0\ -I/usr/include/libcsoap-1.0" ; export MAKEFLAGS="CFLAGS=$$GOODCFLAGS\ -L/usr/lib\ -lcomplearn" ;  echo "it is <$$MAKEFLAGS>" ; 

clean:
	rm rcomplearn.so rcomplearn.o
