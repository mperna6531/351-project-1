#include <sys/shm.h>
#include <sys/msg.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "msg.h"    /* For the message struct */

/* The size of the shared memory chunk */
#define SHARED_MEMORY_CHUNK_SIZE 1000

/* The name of the keyfile */
#define KEY_FILE "keyfile.txt"

/* The ids for the shared memory segment and the message queue */
int shmid, msqid;

/* The pointer to the shared memory */
void *sharedMemPtr;

/* The name of the received file */
const char *recvFileName = "recvfile";


/**
 * Sets up the shared memory segment and message queue
 * @param shmid - the id of the allocated shared memory 
 * @param msqid - the id of the shared memory
 * @param sharedMemPtr - the pointer to the shared memory
 */

void init(int& shmid, int& msqid, void*& sharedMemPtr) {
  // Create a file called keyfile.txt containing string "Hello world"
  FILE *kf = fopen(KEY_FILE, "w");
  fputs("Hello world", kf);
  fclose(kf);
  
  // use ftok("keyfile.txt", 'a') in order to generate the key
  key_t key = ftok(KEY_FILE, 'a');

  // allocate a piece of shared memory
  shmid = shmget(key, SHARED_MEMORY_CHUNK_SIZE, IPC_CREAT | 0666);
  
  // attach to the shared memory
  sharedMemPtr = shmat(shmid, (void*)0, 0);
  
  // create a message queue
  msqid = msgget(key, 0666 | IPC_CREAT);
}
 
/**
 * The main loop
 */
void mainLoop() {
  /* The size of the mesage */
  int msgSize = 0;
  
  /* Open the file for writing */
  FILE* fp = fopen(recvFileName, "w");
    
  /* Error checks */
  if(!fp) {
    perror("fopen");	
    exit(-1);
  }
  // NOTE: the received file will always be saved into the file called  "recvfile"

  message sndMsg, rcvMsg;
  sndMsg.mtype = RECV_DONE_TYPE;
   
  msgrcv(msqid, &rcvMsg, sizeof(rcvMsg.size), SENDER_DATA_TYPE, 0);
  msgSize = rcvMsg.size;

  /* Keep receiving until the sender set the size to 0, indicating that
    * there is no more data to send
    */	
  while(msgSize != 0) {	
    /* If the sender is not telling us that we are done, then get to work */
    
    /* Save the shared memory to file */
    if(fwrite(sharedMemPtr, sizeof(char), msgSize, fp) < 0) {
      perror("fwrite");
    }

    // tell the sender that we are ready for the next file chunk
    msgsnd(msqid, &sndMsg, sizeof(sndMsg.size), 0);
    msgrcv(msqid, &rcvMsg, sizeof(rcvMsg.size), SENDER_DATA_TYPE, 0);
    msgSize = rcvMsg.size;
  }

  fclose(fp);
}

/**
 * Perfoms the cleanup functions
 * @param sharedMemPtr - the pointer to the shared memory
 * @param shmid - the id of the shared memory segment
 * @param msqid - the id of the message queue
 */

void cleanUp(const int& shmid, const int& msqid, void* sharedMemPtr) {
  // detach from shared memory
  shmdt(sharedMemPtr);

  // deallocate the shared memory chunk 
  shmctl(shmid, IPC_RMID, NULL);

  // deallocate the message queue
  msgctl(msqid, IPC_RMID, NULL);
}

/**
 * Handles the exit signal
 * @param signal - the signal type
 */

void ctrlCSignal(int signal) {
  /* Free system V resources */
  cleanUp(shmid, msqid, sharedMemPtr);
  exit(1);
}

int main(int argc, char **argv) {
  // handle ctrl-c input
  signal(SIGINT, ctrlCSignal);

  /* Initialize */
  init(shmid, msqid, sharedMemPtr);
  
  /* Go to the main loop */
  mainLoop();

  // etach from shared memory segment, and deallocate shared memory and message queue
  cleanUp(shmid, msqid, sharedMemPtr);

  return 0;
}
