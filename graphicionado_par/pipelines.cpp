#include "graphicionado.hpp" // Data structures for graph problems
#include "loadSettings.hpp"
#include "pipelines.hpp"
#include "test_functions.hpp"
#include <math.h>

//Global variables
DataCrossbar*** outputQueue;
unsigned int** outputCount;

DataCrossbar*** localQueue; // Local queue to store the crossbar data before synchronization/merging the local lists into output queue.
unsigned int** localCounter; // Counter for localQueue how many element is in it currently.

/* 
 * Initialize the data structures required by the pipeline
 */
void initPipelines(unsigned int numEdges) {
  std::cout << "Init pipelines..." << std::endl;
	// Init localQueue
	localQueue = new DataCrossbar**[NUM_STREAMS];
	for(unsigned int l = 0; l < NUM_STREAMS; ++l){
		localQueue[l] = new DataCrossbar*[NUM_STREAMS];
		for(unsigned int stream = 0; stream < NUM_STREAMS; ++stream){
			localQueue[l][stream] = new DataCrossbar[numEdges];
		}
	}
	

	// Init Local counter for local queue
	localCounter = new unsigned int*[NUM_STREAMS];
	for (unsigned int stream = 0; stream < NUM_STREAMS; ++stream){
		localCounter[stream] = new unsigned int[NUM_STREAMS];
		
		for(unsigned int i = 0; i < NUM_STREAMS; ++i) {
			localCounter[stream][i] = 0; // Init all counters to 0
		}
	}


	//Init output queue
	outputQueue = argo::conew_array<DataCrossbar**>(NUM_STREAMS);
	for(unsigned int i = 0; i < NUM_STREAMS; ++i){
		outputQueue[i] = argo::conew_array<DataCrossbar*>(NUM_STREAMS);
		for(unsigned int j = 0; j < NUM_STREAMS; ++j){
		  outputQueue[i][j] = argo::conew_array<DataCrossbar>(numEdges);
		}
	}

	//Init output Count (number of element in outputQueue)
	outputCount = argo::conew_array<unsigned int*>(NUM_STREAMS);
	for(unsigned int i = 0; i < NUM_STREAMS; ++i){
	  outputCount[i] = argo::conew_array<unsigned int>(NUM_STREAMS);
	}

	std::cout << "Init pipelines done." << std::endl;
}

/*
 * Clean up the data structures initialized for the pipelines
 */
void cleanupPipelines() {
	// Free localQueue
	for(unsigned int i = 0; i < NUM_STREAMS; ++i) {
		for(unsigned int j = 0; j < NUM_STREAMS; ++j) {
			delete localQueue[i][j];
		}
	}
	for(unsigned int i = 0; i < NUM_STREAMS; ++i){
		delete localQueue[i];
	}
	delete localQueue;

	// Free local counter
	for(unsigned int i = 0; i < NUM_STREAMS; ++i){
		delete localCounter[i];
	}
	delete localCounter;

	//Free output Queue
	for(unsigned int i = 0; i < NUM_STREAMS; ++i){
	  for(unsigned int j = 0; j < NUM_STREAMS; ++j){
		argo::codelete_array(outputQueue[i][j]);
	  }
	  argo::codelete_array(outputQueue[i]);
	}
	argo::codelete_array(outputQueue);
	
	// Free output count
	for(unsigned int i = 0; i < NUM_STREAMS; ++i){
	  argo::codelete_array(outputCount[i]);
	}
	argo::codelete_array(outputCount);
}

/*The crossbar switch routes edge data by matching the
destination vertex id of the edge. To maximize the throughput
of the switch, standard techniques such as virtual output
queues [47] are implemented.

Read Edges is directly connected to the virtual output queue
of the crossbar switch.
*/

// Crossbar should switch this edge to correct pipeline. It take in a pointer to an edge. 
// ID of who is running.
void crossbar(unsigned int ID, Edge e, VertexProperty srcProp){
	unsigned int stream = e.dstID % NUM_STREAMS; // Check which pipeline to go to

	DataCrossbar data;
	data.dstID = e.dstID;
	data.weight = e.weight;
	data.srcProp = srcProp;
	localQueue[ID][stream][localCounter[ID][stream]] = data;
	localCounter[ID][stream]++;
}

/*
* Merge all local queues from crossbar to outputQueue. 
*/
void mergeQueues(unsigned int ID){
	for(unsigned int i = 0; i < NUM_STREAMS; ++i) {
		for(unsigned int j = 0; j < localCounter[ID][i]; ++j){
			outputQueue[i][ID][outputCount[i][ID]+j] = localQueue[ID][i][j]; //outputCount where to put it in, where j is the fill in from 
		}
		outputCount[i][ID] = outputCount[i][ID] + localCounter[ID][i];
	}
}


void processingPhaseSourceOriented(unsigned int ID){
	//Vertex dst; // TODO: Not needed for this algorithm right now? Implement in future if we want to use it

    // Pipeline from graphicionado behavior 
    //A Process edge Phase
    for (unsigned int i = 0; i < activeVertexCount[ID]; ++i) { //if all activeSrcID mod THREADS is fitting to their own section
  
      	Vertex src = activeVertex[ID][i]; // Sequential Vertex Read
      	/*TODO :Future check*/unsigned int eID = edgeIDTable[src.ID]; // Edge ID Read
      	if(eID == 0) continue; // If the index for the vertex is 0, it has no outgoing edges.
      	eID--; // Edges index is shifted by 1 because if the index is 0 it indicates that there are no outgoing edges from this vertex.
      	Edge e = edges[ID][eID]; // Edge Read

      	while (e.srcID == src.ID) {
	        // Crossbar should switch this edge e to correct pipeline 
	        crossbar(ID,e,src.prop); // TODO check if correct value is sent. 
	        e = edges[ID][++eID]; // Edge Read // TODO: make sure this does not go out of bounds
      	}
    }
}

//outputQueue with current ID's of edges thats gonna be used. //Double array
//outputQueue [][] put in IDs on edges thats gonna be used.
// edgeID
void processingPhaseDestinationOriented(unsigned int ID){
	Vertex dst; // [OPTIONAL]
	for(unsigned int source = 0; source < NUM_STREAMS; ++source){
	  for(unsigned int i = 0; i < outputCount[ID][source]; ++i){
	    unsigned int position = (outputQueue[ID][source][i].dstID/NUM_STREAMS); // This get position in the [][] array for dstID
		
	    //dst.prop = vProperty[e.dstID]; // [OPT IONAL] Random Vertex Read
	    VertexProperty res = processEdge(outputQueue[ID][source][i].weight, outputQueue[ID][source][i].srcProp, dst.prop);
	    VertexProperty temp = vTempProperty[ID][position]; // Random Vertex Read     
	    temp = reduce(temp, res);
	    vTempProperty[ID][position] = temp; // Random Vertex Write 
	  }
	}

	//TODO Reset them after they been used so we do not to read them again here. E.g. outputCount can be put inside the first loop above. Just after its done with it.
	//TODO Optimization maybe? Check with times if it do anything.
	// Reset ActiveVertex and ActiveVertexCount
	activeVertexCount[ID] = 0; // reset activeVertexCount & active vertices.
	for(unsigned int k = 0; k < NUM_STREAMS; ++k) {
	  outputCount[ID][k] = 0; // reset output queue count for each of this streams queues
	  localCounter[ID][k] = 0; // reset the local output queue counter
	}
}

/*
* Update phase for active vertices
* ID - is the node ID which is the stream in what array that is being used.
*/
void applyPhase(unsigned int ID){
    for (unsigned int i = 0; i < totalVertexCount[ID]; i++) {
	    VertexProperty vprop = vProperty[ID][i]; // Sequential Vertex Read
		VertexProperty temp = vTempProperty[ID][i]; // Sequential Vertex Read
		VertexProperty vconst = vConst[ID][i];
		temp = apply(vprop, temp, vconst);

		vProperty[ID][i] = temp; // Sequential Vertex Write
	  	if(isAllVerticesActive) { // Setting to check if all vertices should be active.
	    	if(temp.property != vprop.property) { // No need to write all if no changed made
	      		Vertex v;
	      		v.ID = ID + NUM_STREAMS * i; // Calculate the ID of the vertex in that position id + nodes * i.
	      		v.prop = temp;
	      		activeVertex[ID][i] = v; // Sequential Vertex Write
	    	}
	  	}
	  	else{ 
	  		// If not all vertices is active.
	    	if(temp.property != vprop.property) { 
	      		Vertex v;
	      		v.ID = ID + NUM_STREAMS * i; // Calculate the ID of the vertex in that position id + nodes * i.
	      		v.prop = temp;
	      		//Active synch for this
	      		activeVertex[ID][activeVertexCount[ID]++] = v; // Sequential Vertex Write
	    	}
	  	}
	}


	//Settings check. If isAllVerticesActive = true then all vertices should be active over all iterations.
    if(isAllVerticesActive){
      	activeVertexCount[ID] = totalVertexCount[ID];//Set active Vertex count to be the total number of vertices in their node id stream.
    }
}
