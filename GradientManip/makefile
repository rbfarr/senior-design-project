CC:=g++
LIBS:=`pkg-config --libs opencv`
INCLUDE:=`pkg-config --cflags opencv`
OBJECTS:=sobel-gradients.o

sobel-gradients:$(OBJECTS) 
	$(CC) $(OBJECTS) -o sobel-gradients $(LIBS)

sobel-gradients.o: sobel-gradients.cpp
	$(CC) $(INCLUDE) -c sobel-gradients.cpp -o sobel-gradients.o

clean:
	rm -f *.o sobel-gradients
