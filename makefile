CC = mpic++
FLAGS = -c -O3
LINK_FLAGS = #-std=c++11
OBJS = main.o InitFunctions.o constants.o ColumnBlock.o
EXECUTABLE_NAME = parallel-gauss-jordan


# Compile
all: $(OBJS)
	$(CC) $(OBJS) $(LINK_FLAGS) -o $(EXECUTABLE_NAME)
	rm $(OBJS)

main.o: main.cpp
	$(CC) $(FLAGS) $(LINK_FLAGS) main.cpp

InitFunctions.o: InitFunctions.cpp
	$(CC) $(FLAGS) $(LINK_FLAGS) InitFunctions.cpp

constants.o: constants.cpp
	$(CC) $(FLAGS) $(LINK_FLAGS) constants.cpp

ColumnBlock.o: ColumnBlock.cpp
	$(CC) $(FLAGS) $(LINK_FLAGS) ColumnBlock.cpp


# Clean-up
clean:
	rm -f $(EXECUTABLE_NAME)
	rm -f $(OBJS)

