ALL : pear_ddns
CC = gcc
CFLAGS +=  -g -fdata-sections -ffunction-sections -Wl,--gc-sections -Wl,--strip-all
LIBS = -lcurl -ljson-c

pear_ddns: pear_ddns.o pear_ddns_query.o pear_ddns_conf.o pear_ddns_base32.o pear_ddns_util.o
	$(CC) $(CFLAGS) pear_ddns.o pear_ddns_query.o pear_ddns_conf.o pear_ddns_base32.o pear_ddns_util.o -o pear_ddns $(LIBS)

pear_ddns.o: pear_ddns.c pear_ddns_query.h pear_ddns_conf.h pear_ddns_base32.h pear_ddns_util.h
	$(CC) $(CFLAGS) -c pear_ddns.c $(LIBS)

pear_ddns_query.o: pear_ddns_query.c pear_ddns_query.h pear_ddns_base32.h pear_ddns_conf.h pear_ddns_util.h
	$(CC) $(CFLAGS) -c pear_ddns_query.c $(LIBS)

pear_ddns_conf.o: pear_ddns_conf.c pear_ddns_conf.h
	$(CC) $(CFLAGS) -c pear_ddns_conf.c $(LIBS)

pear_ddns_base32.o: pear_ddns_base32.c pear_ddns_base32.h
	$(CC) $(CFLAGS) -c pear_ddns_base32.c $(LIBS)

pear_ddns_util.o: pear_ddns_util.c pear_ddns_util.h pear_ddns_base32.h
	$(CC) $(CFLAGS) -c pear_ddns_util.c $(LIBS)

clean :
	rm -rf *.out *.so *.a *.o pear_ddns
