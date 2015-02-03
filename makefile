CC:=g++
LIBS:=`pkg-config --libs opencv`
INCLUDE:=`pkg-config --cflags opencv`
OBJECTS:=webcam-template.o

cam-stream:$(OBJECTS) 
	$(CC) $(OBJECTS) -o cam-stream $(LIBS)

webcam-template.o: webcam-template.cpp
	$(CC) $(INCLUDE) -c webcam-template.cpp -o webcam-template.o

clean:
	rm -f *.o cam-stream
