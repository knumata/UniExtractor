SRC := read.cpp
OBJS := $(SRC:%.cpp=%.o)

TARGET := assetReader

CC := g++
CPP := g++
CPPFLAGS := -O2

all: $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) -o $@ $(OBJS)

%.o : %.cpp
	$(CC) -c $(CPPFLAGS) $< -o $@

clean:
	rm -f *.o
