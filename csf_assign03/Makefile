CXX = g++
CPPFLAGS = -g -Wall -Wextra -pedantic -std=c++17
RM = rm -f

SRCS = csim.cpp cache_structure.cpp
OBJS = $(subst .cpp,.o,$(SRCS))

all : csim

csim : $(OBJS)
	$(CXX) -o csim $(OBJS) $(CPPFLAGS)

csim.o : csim.cpp cache_structure.h
	$(CXX) -c csim.cpp $(CPPFLAGS)

cache_structure.o : cache_structure.h cache_structure.cpp
	$(CXX) -c cache_structure.cpp $(CPPFLAGS)

clean :
	$(RM) $(OBJS) csim
