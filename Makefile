TARGET = GIS.cpp
GIS = GIS

CC = g++
CFLAGS = -Wall -W -pedantic
LIBS = 

all: $(GIS)

clean:
	rm -rf $(GIS) core *.o

$(GIS):
	$(CC) $(CFLAGS) $(TARGET) $(LIBS) -o $@
