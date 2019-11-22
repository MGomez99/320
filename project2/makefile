file1= cache-sim
target= cache-sim
OBJECTS= $(file1).o 
CFLAGS= -std=c++14 -g

all: $(OBJECTS)
	g++ $(CFLAGS) $(OBJECTS) -o $(target)


$(file1).o: $(file1).h $(file1).cpp
	g++ -c $(CFLAGS) $(file1).cpp

clean:
	rm -rf *.o
	rm $(target)
	rm output*
run: all
	./$(target)

easy_run: all
	./cache-sim traces/trace1.txt output1.txt &
	./cache-sim traces/trace2.txt output2.txt &
	./cache-sim traces/trace3.txt output3.txt &
easy_diff:
	diff output1.txt correct_outputs/trace1_output.txt
	diff output2.txt correct_outputs/trace2_output.txt
	diff output3.txt correct_outputs/trace3_output.txt
memcheck: all
	valgrind ./$(target) -v --leak-check=full

winclean:
	del *.o
	del $(target).exe
	del output.txt
