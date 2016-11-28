//

#include "graphicionado.hpp" // Data structures for graph problems
#include "pipelines.hpp"

//Global variables
DataCrossbar** outputQueue; // Crossbar output for destination oriented part.
unsigned int* outputCount;

argo::globallock::cohort_lock* primelock;

void initPipelines() {
	outputCount = argo::conew_array<unsigned int>(NODES);	
	primelock = new argo::globallock::cohort_lock;
}

void cleanupPipelines() {
	delete primelock;
}

// TODO: data structure
//Edges = double array list
//Vertices = double array list
//ActiveVertices = double array list
//EdgeIDTable: Unsure maybe not 


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
	        crossbar(ID,e,src.prop); // TODO check if correct value is send. 
	        e = edges[ID][++eID]; // Edge Read
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
