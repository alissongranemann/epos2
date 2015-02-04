all: 
	g++ -I/usr/local/bin/Calypto_Design_Systems/Catapult_Synthesis_2011a.187/Mgc_home/shared/include -o aes aes_toplevel.cpp 128_aes.cpp

run:
	./aes

warning:
	g++ -o aes aes_ram.cpp -Wall

gprof:
	g++ -pg -o aes aes_ram.cpp
	./aes
	gprof aes gmon.out > analisys.txt

clean:
	rm -rf *o aes
