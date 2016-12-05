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

// Global variable declaration
//int THREADS = 4; // Set number of threads

Vertex* vertices; // All vertices in the graph
Vertex* activeVertex; 

Edge* edges; // All edges in the graph
unsigned int* edgeIDTable;

VertexProperty* vProperty; // property of nodes.
VertexProperty* vTempProperty; // new vProperty that been changed
VertexProperty* vConst; 

unsigned int totalVertexCount; // Number of nodes in the system.
unsigned int activeVertexCount; // Number of active nodes in the system.
unsigned int totalEdgeCount;

// Make all cleanups needed before closing the program.
void terminateProgram(){
    std::cout << "Shutting down program! \n";
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

void processEdges(Vertex dst){
for (unsigned int i=0; i<activeVertexCount; i++) {
      Vertex src = activeVertex[i]; // Sequential Vertex Read
      unsigned int eID = edgeIDTable[src.ID]; // Edge ID Read
      if(eID == 0) continue; // If the index for the vertex is 0, it has no outgoing edges.
      eID--; // Edges index is shifted by 1 because if the index is 0 it indicates that there are no outgoing edges from this vertex.
      Edge e = edges[eID]; // Edge Read
      while (e.srcID == src.ID) {
        //dst.prop = vProperty[e.dstID]; // [OPT IONAL] Random Vertex Read
        VertexProperty res = processEdge(e.weight, src.prop, dst.prop);
        VertexProperty temp = vTempProperty[e.dstID]; // Random Vertex Read
        temp = reduce(temp, res);
        vTempProperty[e.dstID] = temp; // Random Vertex Write
        e = edges[++eID]; // Edge Read
      }
    }
}

void applyEdges(){
for (unsigned int i=0; i<totalVertexCount; i++) {
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
          activeVertex[activeVertexCount++] = v; // Sequential Vertex Write
        }
      }
    }
}

void graphicionado(){
  Vertex dst; // TODO: Not needed for this algorithm right now? Implement in future if we want to use it
  
  while(activeVertexCount != 0) {
    // Pipeline from graphicionado behavior 
   
    processEdges(dst); //A Process edge Phase
    
    activeVertexCount = 0; // reset activeVertexCount & active vertices.
    
    applyEdges(); //B Apply Phase

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
  //int id = argo::node_id(); // get this node unique index number starting from 0
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
  
  std::chrono::time_point<std::chrono::system_clock> start, end;
  std::time_t end_time;
  std::chrono::duration<double> elapsed_seconds;
  start = std::chrono::system_clock::now();

  graphicionado();
  end = std::chrono::system_clock::now();
  elapsed_seconds = end-start;
  end_time = std::chrono::system_clock::to_time_t(end);
  std::cout << "finished computation at " << std::ctime(&end_time)
              << "elapsed time: " << elapsed_seconds.count() << "s\n";

  // printVerticesProperties(totalVertexCount, vertices, vProperty); //Debug prints too see behavior
  writeVerticesProperties(totalVertexCount, vertices, vProperty);
  terminateProgram(); // Cleanup for this node when program has finished.
  return 0;
}
