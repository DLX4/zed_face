###############################################################
#
# Purpose: Makefile for "M-JPEG Streamer"
# Author.: Tom Stoeveken (TST)
# Version: 0.3
# License: GPL
#
###############################################################

CC = arm-xilinx-linux-gnueabi-gcc

OTHER_HEADERS = ../../mjpg_streamer.h ../../utils.h ../output.h ../input.h

CFLAGS += -O1 -DLINUX -D_GNU_SOURCE -Wall -shared -fPIC -I/workplace/mjpeg_streamer/jpeg_out/include
#CFLAGS +=  -g
#CFLAGS += -DDEBUG
LFLAGS += -lpthread -ldl -ljpeg

all: output_http.so

clean:
	rm -f *.a *.o core *~ *.so *.lo

output_http.so: $(OTHER_HEADERS) output_http.c httpd.lo jpg2bmp.lo
	$(CC) $(CFLAGS) -L/workplace/mjpeg_streamer/jpeg_out/lib -o $@ output_http.c httpd.lo jpg2bmp.lo $(LFLAGS)

httpd.lo: $(OTHER_HEADERS) httpd.h httpd.c jpg2bmp.h
	$(CC) -c $(CFLAGS) -o $@ httpd.c

jpg2bmp.lo: jpg2bmp.c jpg2bmp.h
	$(CC) -c $(CFLAGS) -o $@ jpg2bmp.c
