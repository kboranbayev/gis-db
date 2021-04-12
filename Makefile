TARGET = GIS.cpp
GIS = GIS

CC = g++
CFLAGS = -Wall -W -pedantic -pthread
LIBS = 

all:
	$(CC) $(CFLAGS) $(TARGET) $(LIBS) -o $(GIS)


clean:
	rm -rf $(GIS) core *.o

