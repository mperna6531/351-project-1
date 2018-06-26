all: sender recv
 
sender: sender.cpp msg.h
	g++ -Wall -o sender sender.cpp

recv: recv.cpp msg.h
	g++ -Wall -o recv recv.cpp

bonus: bonus_sender bonus_recv

bonus_sender: bonus_sender.cpp
	g++ -g -o bonus_sender bonus_sender.cpp

bonus_recv: bonus_recv.cpp
	g++ -g -o bonus_recv bonus_recv.cpp

clean:
	rm sender recv keyfile.txt recvfile bonus_sender bonus_recv