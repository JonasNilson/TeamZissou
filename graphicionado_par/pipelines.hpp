#ifndef pipelines_hpp
#define pipelines_hpp pipelines_hpp

#include <argo.hpp>
#include <synchronization/cohort_lock.hpp>

struct DataCrossbar;

// Global visible variables
extern DataCrossbar** outputQueue; // outputQueue[][]: Crossbar output to the destination oriented part
extern unsigned int* outputCount; // outputCount[]: Counter for the number of elements in the outputQueue


// Methods
void initPipelines(unsigned int numVertices);
void cleanupPipelines();
void processingPhaseSourceOriented(unsigned int ID);
void processingPhaseDestinationOriented(unsigned int ID);
void applyPhase(unsigned int ID);
void mergeQueues(unsigned int ID);

// Structs
struct DataCrossbar{
	unsigned int dstID;
	double weight;
	VertexProperty srcProp; // To not have to make a random vertex read from vertices thats not in the scope.
};


#endif
