TARGET = GIS.cpp
GIS = GIS

CC = g++
CFLAGS = -Wall -W -pedantic
LIBS = 

#$(GIS)

all:
	$(CC) $(CFLAGS) $(TARGET) $(LIBS) -o $(GIS)


clean:
	rm -rf $(GIS) core *.o

#$(GIS):
#	$(CC) $(CFLAGS) $(TARGET) $(LIBS) -o $@
	
