CC:=g++
LIBS:=`pkg-config --libs opencv`
INCLUDE:=`pkg-config --cflags opencv`
SRCS:=motion-estimation.cpp motion-compensation.cpp vidstab.cpp

vidstab:$(SRCS) 
	$(CC) -o vidstab -ggdb $(SRCS) $(LIBS)

clean:
	rm -f *.o vidstab
