#ifndef pipelines_hpp
#define pipelines_hpp pipelines_hpp

#include <argo.hpp>
#include <synchronization/cohort_lock.hpp>

struct DataCrossbar;

// Global visible variables
extern DataCrossbar** outputQueue;
extern unsigned int* outputCount;

// Argo variables.


// Methods
void initPipelines();
void cleanupPipelines();
void processingPhaseSourceOriented(unsigned int ID);
void processingPhaseDestinationOriented(unsigned int ID);

// Struct
struct DataCrossbar{
	unsigned int dstID;
	double weight;
	VertexProperty srcProp; // To not have to make a random vertex read from vertices thats not in the scope.
};


#endif
