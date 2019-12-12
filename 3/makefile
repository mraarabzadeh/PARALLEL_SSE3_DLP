CC=g++

CFLAGS=-c

INCDIR=-I/opt/intel/ipp/include -I/usr/local/Cellar/opencv3/3.3.0_3/include

LIBDIR=-L/opt/intel/ipp/lib/intel64 -L/usr/local/Cellar/opencv3/3.3.0_3/lib -L/usr/local/opt/jpeg/lib/

LIBFILE=-lippcore -lopencv_ccalib -lopencv_calib3d -lopencv_core -lopencv_features2d -lopencv_flann -lopencv_highgui -lopencv_imgproc -lopencv_ml -lopencv_objdetect -lopencv_photo -lopencv_stitching -lopencv_superres -lopencv_video -lopencv_videostab -lopencv_imgcodecs -ljpeg 

all: main

main: main.o 
	$(CC) $(LIBDIR) $(LIBFILE)  main.o -o main `pkg-config --cflags --libs opencv`

main.o: main.cpp
	$(CC) $(CFLAGS) -msse3  $(INCDIR) main.cpp

clean:
	rm -f *o main


	  
