ALL : pear_ddns
CC = gcc
CFLAGS += -DBSDIFF_EXECUTABLE -DBSPATCH_EXECUTABLE $(BIGFILES) -O3 -fdata-sections -ffunction-sections -Wl,--gc-sections -Wl,--strip-all #-fwhole-program -flto
LIBS = -lcurl -lxml2 -I/usr/include/libxml2

pear_ddns: pear_ddns.o pear_ddns_request.o
	$(CC) $(CFLAGS) pear_ddns.o pear_ddns_request.o -o pear_ddns $(LIBS) 

pear_ddns_request.o: pear_ddns_request.c pear_ddns_request.h pear_ddns_private.h
	$(CC) $(CFLAGS) -c pear_ddns_request.c $(LIBS)
pear_ddns.o: pear_ddns.c pear_ddns_private.h
	$(CC) $(CFLAGS) -c pear_ddns.c $(LIBS)

clean :
	rm -rf *.out *.so *.a *.o pear_ddns
