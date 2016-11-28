#include "graphicionado.hpp" // Data structures for graph problems
#include "loadSettings.hpp"
#include "pipelines.hpp"

//Global variables
DataCrossbar** outputQueue;
unsigned int* outputCount;
argo::globallock::cohort_lock* primelock; // Cohort lock for the Argo nodes

/* 
 * Initialize the data structures required by the pipeline
 */
void initPipelines() {
	outputCount = argo::conew_array<unsigned int>(NODES);
	primelock = new argo::globallock::cohort_lock;
}

/*
 * Clean up the data structures initialized for the pipelines
 */
void cleanupPipelines() {
	argo::codelete_array(outputCount);
	delete primelock;
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
	
	unsigned int stream = e.dstID % NODES; // Check which pipeline to go to

	DataCrossbar data;
	data.dstID = e.dstID;
	data.weight = e.weight;
	data.srcProp = srcProp;

	primelock->lock();
	argo::backend::acquire(); // TODO: Local counter and synchronize them in the end.
	unsigned int count = outputCount[stream];
	outputCount[stream] = outputCount[stream] + 1;
	argo::backend::release();
	primelock->unlock();
	outputQueue[stream][count] = data;
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
	for(unsigned int i = 0; i < outputCount[ID]; ++i){
		unsigned int position = (outputQueue[ID][i].dstID/NODES);// TODO: Check if correct position //This get position in the [][] array for dstID
    	//dst.prop = vProperty[e.dstID]; // [OPT IONAL] Random Vertex Read
    	VertexProperty res = processEdge(outputQueue[ID][i].weight, outputQueue[ID][i].srcProp, dst.prop);
    	VertexProperty temp = vTempProperty[ID][position]; // Random Vertex Read     
	    temp = reduce(temp, res);
	    vTempProperty[ID][position] = temp; // Random Vertex Write 
	}

	// Reset ActiveVertex and ActiveVertexCount
	activeVertexCount[ID] = 0; // reset activeVertexCount & active vertices.
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
	      		v.ID = ID + NODES * i; // Calculate the ID of the vertex in that position id + nodes * i.
	      		v.prop = temp;
	      		activeVertex[ID][i] = v; // Sequential Vertex Write
	    	}
	  	}
	  	else{ 
	  		// If not all vertices is active.
	    	if(temp.property != vprop.property) { 
	      		Vertex v;
	      		v.ID = ID + NODES * i; // Calculate the ID of the vertex in that position id + nodes * i.
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
