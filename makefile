all:sim
sim:
	g++ -std=c++11 main.cpp var.cpp decode.cpp register.cpp error_detect.cpp alu.cpp -o single_cycle
clean:
	rm -f *.o
