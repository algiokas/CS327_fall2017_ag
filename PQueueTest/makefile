CXX = g++
CXXFLAGS = -g -Wall -Werror -ggdb -O2
OBJECTS = PQueueTest.o PQueue.o

dungeon : $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o dungeon

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $<

.PHONY : clean

clean :
	rm $(OBJECTS)
