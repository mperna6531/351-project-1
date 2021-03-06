#include <sys/shm.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "msg.h"    /* For the message struct */

/* The size of the shared memory chunk */
#define SHARED_MEMORY_CHUNK_SIZE 1000
#define KEY_FILE "keyfile.txt"

/* The ids for the shared memory segment and the message queue */
int shmid, msqid;

/* The pointer to the shared memory */
void *sharedMemPtr;

/**
 * Sets up the shared memory segment and message queue
 * @param shmid - the id of the allocated shared memory 
 * @param msqid - the id of the shared memory
 */

void init(int& shmid, int& msqid, void*& sharedMemPtr) {
  // use ftok("keyfile.txt", 'a') in order to generate the key
  key_t key = ftok(KEY_FILE, 'a');
  
  
  // get the id of the shared memory segment
  shmid = shmget(key, SHARED_MEMORY_CHUNK_SIZE, IPC_CREAT | 0666);

  // attach to the shared memory
  sharedMemPtr = shmat(shmid, (void*)0, 0);

  // attach to the message queue
  msqid = msgget(key, 0666 | IPC_CREAT);	
}

/**
 * Performs the cleanup functions
 * @param sharedMemPtr - the pointer to the shared memory
 * @param shmid - the id of the shared memory segment
 * @param msqid - the id of the message queue
 */

void cleanUp(const int& shmid, const int& msqid, void* sharedMemPtr) {
  // detach from shared memory
  shmdt(sharedMemPtr);
}

/**
 * The main send function
 * @param fileName - the name of the file
 */
void send(const char* fileName) {
  /* Open the file for reading */
  FILE* fp = fopen(fileName, "r");

  /* A buffer to store message we will send to the receiver. */
  message sndMsg;
  sndMsg.mtype = SENDER_DATA_TYPE;
  
  /* A buffer to store message received from the receiver. */
  message rcvMsg;
  
  /* Was the file open? */
  if(!fp) {
    perror("fopen");
    exit(-1);
  }
  
  /* Read the whole file */
  while(!feof(fp)) {
    /* Read at most SHARED_MEMORY_CHUNK_SIZE from the file and store them in shared memory. 
      * fread will return how many bytes it has actually read (since the last chunk may be less
      * than SHARED_MEMORY_CHUNK_SIZE).
      */
    if((sndMsg.size = fread(sharedMemPtr, sizeof(char), SHARED_MEMORY_CHUNK_SIZE, fp)) < 0) {
      perror("fread");
      exit(-1);
    }

    // send a message to the receiver telling him that the data is ready 
    msgsnd(msqid, &sndMsg, sizeof(sndMsg.size), 0);
    /* TODO: Wait until the receiver sends us a message of type RECV_DONE_TYPE telling us 
      * that he finished saving the memory chunk. 
      */

    msgrcv(msqid, &rcvMsg, sizeof(rcvMsg.size), RECV_DONE_TYPE, 0);
  }

  // tell the receiver that we have nothing more to send. We will do this by sending a message of type SENDER_DATA_TYPE with size field set to 0. 
  sndMsg.size = 0;	
  msgsnd(msqid, &sndMsg, sizeof(sndMsg.size), 0);
    
  /* Close the file */
  fclose(fp);
  
}


int main(int argc, char** argv) {
  
  /* Check the command line arguments */
  if(argc < 2) {
    fprintf(stderr, "USAGE: %s <FILE NAME>\n", argv[0]);
    exit(-1);
  }
    
  /* Connect to shared memory and the message queue */
  init(shmid, msqid, sharedMemPtr);
  
  /* Send the file */
  send(argv[1]);
  
  /* Cleanup */
  cleanUp(shmid, msqid, sharedMemPtr);
    
  return 0;
}
