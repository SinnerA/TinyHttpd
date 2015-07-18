objects = parse.o utility.o tinyhttpd.o -lpthread

main : $(objects)
g++ -o main $(objects)
parse.o : parse.cpp parse.h
utility.o : utility.cpp utility.h
tinyhttpd.o : tinyhttpd.cpp tinyhttpd.h
clean :
rm edit $(objects)