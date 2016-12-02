/**
 * Parallel implementation of graphicionado
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

// Global variable declaration
unsigned int THREADS; // Number of threads. Read and set from setting file
unsigned int NODES;

unsigned int numberOfStreams; // Streams that is how many parts running graphicionado on its own set of vertices.

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

std::chrono::duration<double> time_graphicionado, time_init, time_cleanup, time_read;
std::chrono::duration<double> time_src;
std::chrono::duration<double> time_dst;
std::chrono::duration<double> time_merge;
std::chrono::duration<double> time_apply;
std::chrono::time_point<std::chrono::system_clock> start;
std::chrono::time_point<std::chrono::system_clock> end;
std::chrono::time_point<std::chrono::system_clock> startG;

// Make all cleanups needed before closing the program.
void terminateProgram(){
    std::cout << "Shutting down program! \n";
    cleanupPreprocess(); // Free everything thats allocated in preprocess
    argo::finalize(); // Cleanup for this node when program has finished.
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
	for(unsigned int i = 0; i < NODES; ++i) {
		if(activeVertexCount[i] != 0) {
			return true;
		}
	}
	return false;
}

// ID is for node/thread
void graphicionado(unsigned int id){
  while(hasActiveVertices()) {
    //A process edge
    if(id == 0){
      start = std::chrono::system_clock::now();
    }
    processingPhaseSourceOriented(id); //
    argo::barrier();
    if(id == 0){
      time_src += std::chrono::system_clock::now()-start;
      start = std::chrono::system_clock::now();
    }
    mergeQueues(id); // Take all local queues and merge them into one output Queue.
    argo::barrier(); // Synchronization
    if(id == 0){
      time_merge += std::chrono::system_clock::now()-start;
      start = std::chrono::system_clock::now();
    }
    processingPhaseDestinationOriented(id); //
    argo::barrier();
    if(id == 0){
      time_dst += std::chrono::system_clock::now()-start;
      start = std::chrono::system_clock::now();
    }
    //B Apply Phase
    applyPhase(id);
    argo::barrier();
    if(id == 0){
      time_apply += std::chrono::system_clock::now()-start;
    }

    //Settings check if we should use max iteration implementation or not
    if(maxIterations != 0){ //If setting is set to 0 it will use infinity iteration possibility
      maxIterations--;
      if(maxIterations == 0){
        std::cout << "All iterations are done!" << std::endl;
        //Only enter this stage if it reached the maximum iteration count
        break; // break the entire loop
      }
    }
  }
}

void printTimeMeasurements(){
    std::cout << "Finished computation with " << NODES << " nodes." << "\n----------------\n"
	      << "Graphicionado time: \t\t" << time_graphicionado.count() << "\nArgo initialization time: \t" << time_init.count() << "\nData read time: \t\t" << time_read.count() << "\nCleanup time: \t\t\t" << time_cleanup.count() << "\n-----------\n" << std::endl;
    std::chrono::duration<double> srctime, dsttime, mergetime, applytime;

    std::cout << "Time in processing src "<< ": \t" << time_src.count() << "\nTime in processing dst " << ": \t" << time_dst.count() << "\nTime spent merging queues " << ": \t" << time_merge.count() << "\t(Lock waiting time: " << time_lock.count() << ")\nTime spent applying " << ": \t\t" << time_apply.count() << std::endl;  
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
  argo::init(128 * 1024 * 1024); 
  time_init = std::chrono::system_clock::now()-start;

  // Local variable declaration
  unsigned int id = argo::node_id(); // get this node unique index number starting from 0
  NODES = argo::number_of_nodes(); // return the total number of nodes in the Argo system.

  // Load the configuration settings from file (settings.cfg)
  loadSettings();
  argo::barrier();
  // readData take input and organize the input
  if(id == 0)
    start = std::chrono::system_clock::now();
  int code = readData(argc,argv);
  if(id == 0)
    time_read = std::chrono::system_clock::now()-start;
  if(code != 0){
      if(code == 2){
		  // test run
		  return 0;
      }
      //Exist program something went wrong with reading of Data.
      return 1;
  }  
  
  argo::barrier(); // Synchronize after node 0 is done with the initialization.
  
  
  if(id == 0){
    startG = std::chrono::system_clock::now();
  }
  graphicionado(id);
  argo::barrier(); // Synchronize before cleaning up
  //printVerticesProperties(totalVertexCount[id], vertices[id], vProperty[id]); //Debug prints too see behavior
  if(id == 0) {
    time_graphicionado = std::chrono::system_clock::now()-startG;
    start = std::chrono::system_clock::now();
  }
  terminateProgram(); // Cleanup for this node when program has finished.
  if(id == 0) {
    time_cleanup = std::chrono::system_clock::now()-start;
    // Node 0 writes the parallel results to file
  writeTwoDimensionalVerticesProperties(NODES, totalVertexCount, vertices, vProperty); 
  printTimeMeasurements();
  }
   
  return 0;
}
