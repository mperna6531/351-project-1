make: sender recv

sender: sender.cpp msg.h
	g++ -Wall -o sender sender.cpp

receiver: recv.cpp msg.h
	g++ -Wall -o recv recv.cpp

clean:
	rm sender recv