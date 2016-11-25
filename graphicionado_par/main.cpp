/**
 * Sequential implementation of graphicionado
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

// Global variable declaration
int THREADS = 4; // Set number of threads
unsigned int NODES = 2;

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

// Make all cleanups needed before closing the program.
void terminateProgram(){
    std::cout << "Shutting down program! \n";
	cleanupPipelines(); // Cleanup the allocations from pipelines
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

// ID is for node/thread
void graphicionado(unsigned int id){
    
  while(activeVertexCount != 0) {
    //A process edge
    processingPhaseSourceOriented(id); //
    argo::barrier(); // Synchronization
    processingPhaseDestinationOriented(id); //

	/*
    //B Apply Phase
    for (unsigned int i=start; i<end; i++) {
      VertexProperty vprop = vProperty[i]; // Sequential Vertex Read
      VertexProperty temp = vTempProperty[i]; // Sequential Vertex Read
      VertexProperty vconst = vConst[i];
      temp = apply(vprop, temp, vconst);
    
      vProperty[i] = temp; // Sequential Vertex Write
      if(isAllVerticesActive) { // Setting to check if all vertices should be active.
        if(temp.property != vprop.property) { // No need to write all if no changed made
          Vertex v;
          v.ID = i;
          v.prop = temp;
          activeVertex[i] = v; // Sequential Vertex Write
        }
      }
      else { // If not all vertices is active.
        if(temp.property != vprop.property) { 
          Vertex v;
          v.ID = i;
          v.prop = temp;
          //Active synch for this
          activeVertex[activeVertexCounterLocalForNow++] = v; // Sequential Vertex Write
        }
      }
    }
	*/

    //Settings check. If isAllVerticesActive = true then all vertices should be active over all iterations.
    if(isAllVerticesActive){
      activeVertexCount = totalVertexCount;//Set active Vertex count to be the total number of vertices.
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
  argo::init(128 * 1024 * 1024); 

  // Load the configuration settings from file (settings.cfg)
  loadSettings();
 
  // Local variable declaration
  unsigned int id = argo::node_id(); // get this node unique index number starting from 0
  //int nodes = argo::number_of_nodes(); // return the total number of nodes in the Argo system.
 
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
  
  graphicionado(id);

  //printVerticesProperties(totalVertexCount, vertices, vProperty); //Debug prints too see behavior
  terminateProgram(); // Cleanup for this node when program has finished.
  return 0;
}
