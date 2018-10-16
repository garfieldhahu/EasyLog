CPP = g++
CPPFLAGS = -g
LDFLAGS = 
#ARLIB = ar -t $^
COMPILEBIN = $(CPP) -o $@ $^ $(LDFLAGS)

CPPFLAGS += -I ~/project/RecSys/thirdparty/protobuf-3.5.0/include
LDFLAGS += -L ~/project/RecSys/thirdparty/protobuf-3.5.0/lib64_release/ -lprotobuf

%.pb.cc : %.proto
	~/project/RecSys/thirdparty/protobuf-3.5.0/bin/protoc --cpp_out=. $<

%.d : %.cc
	$(CPP) -MM %.cc

%.o : %.cc %.d
	$(CPP) $(CPPFLAGS) -o $@ -c $<

test : Person.pb.o test.o
	$(COMPILEBIN)

all : Person.pb.cc

clean :
	rm *.o test
