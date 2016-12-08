#ifndef preprocess_hpp
#define preprocess_hpp preprocess_hpp

void readTextFile(const char* filename);

void readTextFileWithLineParsing(const char* filename);

void readGTgraphFile(const char* filename);

void setupDSM(unsigned int numVerticies, unsigned int numEdges);

void initializeDSM(unsigned int numVerticies, unsigned int numEdges);

void setupEIT(unsigned int numVertices, unsigned int numEdges, unsigned int* edgeIDTable);

void cleanupPreprocess();


// Global declaration.
extern edgeStreamCounterDst; // Will hold worst case amount of edges for Dst each stream.
extern edgeStreamCounterSrc; // Will hold worst case amount of edges for Src each stream.

#endif
