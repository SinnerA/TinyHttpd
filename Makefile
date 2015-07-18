objects = ./TinyHttpd/parse.o ./TinyHttpd/utility.o ./TinyHttpd/tinyhttpd.o -lpthread

main : $(objects)
	g++ -o main $(objects)
parse.o : ./TinyHttpd/parse.cpp ./TinyHttpd/parse.h
utility.o : ./TinyHttpd/utility.cpp ./TinyHttpd/utility.h
tinyhttpd.o : ./TinyHttpd/tinyhttpd.cpp ./TinyHttpd/tinyhttpd.h
clean :
	rm main ./TinyHttpd/parse.o ./TinyHttpd/utility.o ./TinyHttpd/tinyhttpd.o