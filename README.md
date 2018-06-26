# 351-project-1

CPSC 351-02

Group Members:

Michael Perna		mperna@csu.fullerton.edu
Stephen Shinn		smshinn@csu.fullerton.edu
Oscar Cheung		cheung.oscar@csu.fullerton.edu
Katherine Davis	        kdavis890892003@csu.fullerton.edu
Jasper Chen		jsprchn2@csu.fullerton.edu

Program Info:
  This program is designed to transfer data from one file to another in 1000 byte increments, this process is synchronized between a 'sender' application and a 'receiver' application.  Linux IPC tools are used to accomplish this.
 Language: C++
  Developed on the GNU/Linux operating system

To Compile:
  A makefile is included.  Type 'make' in the terminal of the directory to execute compile command.  Two(2) files will be generated, one named 'recv' and one named 'sender'.  To execute the program, first  run the receiver by entering './recv' in the command line.  Next, open up another terminal and navigate to the directory where the source is located.  Then open the sender program by typing './send big.txt' where 'big.txt' can be any textfile you wish to transfer.  A file named 'big.txt' is included for testing.

Contributions:
Michael/Stephen:  design and implement the recv program
Oscar/Katherine:  design and implement the sender program
Jasper:  implement the SIGINT function / testing.

Our team was unable to complete the implementation of the bonus assignment.
