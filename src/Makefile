CPP = g++ 
CPPFLAGS = -g
LDFLAGS = -lpthread
#ARLIB = ar -t $^
COMPILEBIN = $(CPP) -o $@ $^ $(LDFLAGS)

SRCS=$(wildcard *.cpp)
OBJS=$(SRCS:.cpp=.o)
DEPS=$(SRCS:.cpp=.d)

all : test

.PHONY: all clean

%.d : %.cc
    $(CPP) -MM %.cpp

%.o : %.cc %.d 
    $(CPP) $(CPPFLAGS) -o $@ -c $<

test : $(OBJS)
    $(COMPILEBIN)

clean :
    rm *.o test
