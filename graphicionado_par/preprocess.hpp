#ifndef preprocess_hpp
#define preprocess_hpp preprocess_hpp

void readTextFile(const char* filename);

void readTextFileWithLineParsing(const char* filename);

void readNumEdgesFromFile(const char* filename);

void readGTgraphFile(const char* filename);

void setupDSM(unsigned int numVerticies, unsigned int numEdges);

void initializeDSM(unsigned int numVerticies, unsigned int numEdges);

void setupEIT(unsigned int numVertices, unsigned int numEdges, unsigned int* edgeIDTable);

void cleanupPreprocess();


// Global declaration.
extern unsigned int* edgeStreamCounterDst; // Will hold worst case amount of edges for Dst each stream.
extern unsigned int* edgeStreamCounterSrc; // Will hold worst case amount of edges for Src each stream.
extern unsigned int** queueSizes; // Will hold worst case amount of elements for local/output queues.

#endif
