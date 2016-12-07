/**
 * Parallel implementation of graphicionado with both threads/Nodes
 */
#include <argo.hpp> // Get access to Argo function calls.
#include <iostream> // Used for output prints.
#include <array>
#include "graphicionado.hpp" // Data structures for graph problems
#include "preprocess.hpp"
#include "loadSettings.hpp"
#include "test_functions.hpp"
#include "tests.hpp"
#include "pipelines.hpp"
#include <chrono>
#include <vector>
#include <thread>
#include <chrono>

#include <pthread.h> // For the barrier

// Global variable declaration
unsigned int THREADS; // Number of threads. Read and set from setting file
unsigned int NODES;
unsigned int NUM_STREAMS;
pthread_barrier_t thread_barrier;

Vertex** vertices; // All vertices in the graph
Vertex** activeVertex;

Edge** edges; // All edges in the graph
unsigned int* edgeIDTable;

VertexProperty** vProperty; // property of nodes.
VertexProperty** vTempProperty; // new vProperty that been changed
VertexProperty** vConst;

unsigned int* totalVertexCount; // Number of nodes in the system.
unsigned int* activeVertexCount; // Number of active nodes in the system.
unsigned int* totalEdgeCount;

std::chrono::duration<double> time_graphicionado, time_init, time_cleanup, time_read, time_src, time_dst, time_merge, time_apply;
std::chrono::time_point<std::chrono::system_clock> start, startGraphicionado;

// Make all cleanups needed before closing the program.
void terminateProgram(){
  std::cout << "Shutting down program! \n";
  cleanupPreprocess(); // Free everything thats allocated in preprocess
  argo::finalize(); // Cleanup for this node when program has finished.
}

// Calculate the total number of streams over all nodes
void setNumberOfStreams() {
  if(singleNodeRunning) {
    NUM_STREAMS = THREADS;
  } else {
    NUM_STREAMS = THREADS * NODES;
  }
}

void printTimeMeasurements(){
  std::cout << "Finished computation with " << NODES << " nodes each running " << THREADS << " threads."  << "\n----------------\n"
	    << "Graphicionado time: \t\t" << time_graphicionado.count() << "\nArgo initialization time: \t" << time_init.count() << "\nData read time: \t\t" << time_read.count() << "\nCleanup time: \t\t\t" << time_cleanup.count() << "\n-----------\n" << std::endl;

  std::cout << "Time in processing src "<< ": \t" << time_src.count() << "\nTime in processing dst " << ": \t" << time_dst.count() << "\nTime spent merging queues " << ": \t" << time_merge.count() << "\nTime spent applying " << ": \t\t" << time_apply.count() << std::endl;  
}

/*
 * readData take data and init all data in the system so graphicionado can run.
 */
int readData(int argc, char *argv[]){
  if(argc>1) {
    const std::string tests = "tests";
    if(argv[1] == tests){
      runTests();
      return 2; // Return 2 when running tests
    }
    //Init all data organization 
    std::cout << "Reading graph from textfile: " << argv[1] << std::endl;
    readGTgraphFile(argv[1]);
  }
  else {
    // Error no argument with filename
    std::cout << "Missing argument: graph file. \n";
    terminateProgram();
    return 1;
  }
  return 0;
}


/*
 * Use this to check if there are any active vertices left to process
 */
bool hasActiveVertices() {
  for(unsigned int i = 0; i < NUM_STREAMS; ++i) {
    if(activeVertexCount[i] != 0) {
      return true;
    }
  }
  return false;
}

// Barrier function adjusting to the mode of the running Graphicionado
void barrier(unsigned int id) {
  if(singleNodeRunning) {
    // Wait for all threads in the single node
    pthread_barrier_wait(&thread_barrier);
  } else {
    argo::barrier(THREADS);
  }
}

// ID is for node/thread
void graphicionado(unsigned int id){
  unsigned int iterations = maxIterations; // maxIterations read from settings file
  while(hasActiveVertices()) {
	

    if(id == 0)
      start = std::chrono::system_clock::now();
    //A process edge
    processingPhaseSourceOriented(id); //
    barrier(id);
    if(id == 0){
      time_src = time_src + (std::chrono::system_clock::now()-start);
      start = std::chrono::system_clock::now();
    }
    mergeQueues(id); // Take all local queues and merge them into one output Queue.
    barrier(id);
    if(id == 0){
      time_merge = time_merge + (std::chrono::system_clock::now()-start);
      start = std::chrono::system_clock::now();
    }
    processingPhaseDestinationOriented(id); //

    barrier(id);
    if(id == 0){
      time_dst = time_dst + (std::chrono::system_clock::now()-start);
      start = std::chrono::system_clock::now();
    }
    //B Apply Phase
    applyPhase(id);
    barrier(id);
    if(id == 0)
      time_apply = time_apply + (std::chrono::system_clock::now()-start);

    //Settings check if we should use max iteration implementation or not
    if(iterations != 0){ //If setting is set to 0 it will use infinity iteration possibility
      iterations--;
      if(iterations == 0){
        std::cout << "All iterations are done!" << std::endl;
        //Only enter this stage if it reached the maximum iteration count
        break; // break the entire loop
      }
    }
  }
}

/**
   Information about graphicionado
   * EdgeIDTable - is constructed and stored in memory. It is an array that store edge id of the first edge of each vertex. Sorted by srcID and then dstID.
   * VProperty - is an array that store vertex property (Unknown for now)
   * VTempProperty - same as VProperty but updated from the function reduce.
   * Edges - array of all edges. Is indexed (sorted) by edge ID E.g of an edge: (srcID, dstID, weight) (NOT SURE VISULISE TOMORROW)
   */
int main(int argc, char *argv[]){
  /* 
     Set up the argo environment, caches and global memory. 
     Init the total space that is shared between all nodes. 
  */
      start = std::chrono::system_clock::now();
      argo::init(256 * 1024 * 1024);
      std::cout << "Argo init done" << std::endl;
  
 
      // Local variable declaration
      unsigned int id = argo::node_id(); // get this node unique index number starting from 0
      if(id==0) time_init += std::chrono::system_clock::now()-start;
      NODES = argo::number_of_nodes(); // return the total number of nodes in the Argo system.

      // Load the configuration settings from file (settings.cfg)
      loadSettings();

      // Set the number of streams across all nodes (saved in NUM_STREAMS)
      setNumberOfStreams();

      argo::barrier();
      if(id==0) start = std::chrono::system_clock::now();
      // readData take input and organize the input
      int code = readData(argc,argv);
      if(code != 0){
	if(code == 2){
	  // test run
	  return 0;
	}
	//Exist program something went wrong with reading of Data.
	return 1;
      }  
  
      argo::barrier(); // Synchronize after node 0 is done with the initialization.
      if(id==0){
	time_read += std::chrono::system_clock::now()-start;
	startGraphicionado = std::chrono::system_clock::now();
      }
      if(!singleNodeRunning) {
	// Create a vector of threads
	std::vector<std::thread> thread_vector(THREADS);
  
	// Create threads and give them function graphicionado to run
	for(unsigned int i = 0; i < THREADS; ++i) {
	  unsigned int streamID = id * THREADS + i; // Give each thread a unique ID based on the Argo node id
	  thread_vector[i] = std::thread(graphicionado, streamID);
	}

	// Wait for all threads to finish the graphicionado function for this node
	for(unsigned int i = 0; i < THREADS; ++i) {
	  thread_vector[i].join();
	}
      } else {
	// Big memory mode. Only one argo node running threads. The rest holds extra memory.
	if(id == 0) {
	  // Setup the local thread barrier
	  pthread_barrier_init(&thread_barrier, NULL, THREADS);

	  // Create a vector of threads
	  std::vector<std::thread> thread_vector(THREADS);
  
	  // Create threads and give them function graphicionado to run
	  for(unsigned int i = 0; i < THREADS; ++i) {
	    unsigned int streamID = id * THREADS + i; // Give each thread a unique ID based on the Argo node id
	    thread_vector[i] = std::thread(graphicionado, streamID);
	  }

	  // Wait for all threads to finish the graphicionado function for this node
	  for(unsigned int i = 0; i < THREADS; ++i) {
	    thread_vector[i].join();
	  }
	}
      }

      argo::barrier(); // Synchronize before cleaning up  
      if(id == 0) {
	time_graphicionado += std::chrono::system_clock::now()-startGraphicionado;
	start = std::chrono::system_clock::now();
	writeTwoDimensionalVerticesProperties(NODES, totalVertexCount, vertices, vProperty);
      }

      terminateProgram(); // Cleanup for this node when program has finished.
      

      //printVerticesProperties(totalVertexCount[id], vertices[id], vProperty[id]); //Debug prints too see behavior
      if(id == 0) {
	time_cleanup += std::chrono::system_clock::now()-start;
	// Node 0 writes the parallel results to file
	printTimeMeasurements();
      }

      
  return 0;
}
