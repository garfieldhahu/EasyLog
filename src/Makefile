
objects = Util.o LogBuffer.o EasyLog.o

test : $(objects) test.cpp
	g++ -g -o test test.cpp  -lpthread $(objects) 
EasyLog.o : EasyLog.h EasyLog.cpp
	g++ -g -c EasyLog.cpp
LogBuffer.o : LogBuffer.h LogBuffer.cpp
	g++ -g -c LogBffer.cpp
Util.o : Util.h Util.cpp
	g++ -g -c Util.cpp
clean :
	rm test $(objects)
