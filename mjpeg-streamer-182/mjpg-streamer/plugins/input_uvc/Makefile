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

#CFLAGS += -g
#CFLAGS += -DDEBUG

ifeq ($(USE_LIBV4L2),true)
LFLAGS += -lv4l2
CFLAGS += -DUSE_LIBV4L2
endif


LFLAGS += -ljpeg

all: input_uvc.so	

clean:
	rm -f *.a *.o core *~ *.so *.lo

input_uvc.so: $(OTHER_HEADERS) input_uvc.c v4l2uvc.lo jpeg_utils.lo dynctrl.lo jpg2bmp.lo bmp2jpg.lo
	$(CC) $(CFLAGS) -L/workplace/mjpeg_streamer/jpeg_out/lib -L/home/dlx/opencv/OpenCV-2.3.1/install/lib -o $@ input_uvc.c v4l2uvc.lo jpeg_utils.lo dynctrl.lo jpg2bmp.lo bmp2jpg.lo -lopencv_calib3d -lopencv_core -lopencv_features2d -lopencv_flann -lopencv_highgui -lopencv_imgproc -lopencv_ml -lopencv_objdetect -lopencv_video $(LFLAGS)

v4l2uvc.lo: huffman.h uvc_compat.h v4l2uvc.c v4l2uvc.h
	$(CC) -c $(CFLAGS) -o $@ v4l2uvc.c

jpeg_utils.lo: jpeg_utils.c jpeg_utils.h
	$(CC) -c $(CFLAGS) -o $@ jpeg_utils.c

dynctrl.lo: dynctrl.c dynctrl.h
	$(CC) -c $(CFLAGS) -o $@ dynctrl.c

jpg2bmp.lo: jpg2bmp.c jpg2bmp.h
	$(CC) -c $(CFLAGS) -o $@ jpg2bmp.c

bmp2jpg.lo: bmp2jpg.c bmp2jpg.h
	$(CC) -c $(CFLAGS) -o $@ bmp2jpg.c


#face_detection.o: face_detection.cpp face_detection.h
	#arm-xilinx-linux-gnueabi-g++ -Wall -I/home/dlx/opencv/OpenCV-2.3.1/install/include -c face_detection.cpp 
	#$(CC) -c $(CFLAGS) -I/workplace/mjpeg_streamer/jpeg_out/include -L/workplace/mjpeg_streamer/jpeg_out/lib -ljpeg -o $@ jpg2bmp.c
#arm-xilinx-linux-gnueabi-g++ -Wall -fPIC -shared -I/home/dlx/opencv/OpenCV-2.3.1/install/include -L/home/dlx/opencv/OpenCV-2.3.1/install/lib -lopencv_calib3d -lopencv_core -lopencv_features2d -lopencv_flann -lopencv_highgui -lopencv_imgproc -lopencv_ml -lopencv_objdetect -lopencv_video -o libface_detection.so  face_detection.cpp 
