CXX = g++
CXXFLAGS = -g -Wall -Werror -ggdb -O2
OBJECTS = PQueueTest.o 

dungeon : $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o PQueueTest

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $<

.PHONY : clean

clean :
	rm $(OBJECTS)
