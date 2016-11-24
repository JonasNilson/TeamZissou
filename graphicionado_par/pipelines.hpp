#ifndef pipelines_hpp
#define pielines_hpp pipelines_hpp

struct DataCrossbar;

// Global visible variables
extern unsigned int NODES;
extern DataCrossbar** outputQueue;
extern unsigned int* outputCount;

// Argo variables.
argo::globallock::cohort_lock primelock;




// Methods
void sourceOriented(unsigned int ID);
void destinationOriented(unsigned int ID)

// Struct
struct DataCrossbar{
	unsigned int dstID;
	double weight;
	VertexProperty srcProp; // To not have to make a random vertex read from vertices thats not in the scope.
}


#endif