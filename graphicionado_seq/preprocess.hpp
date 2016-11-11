#ifndef preprocess_hpp
#define preprocess_hpp preprocess_hpp

void readTextFile(char * filename);

void setupDSM(unsigned int numVerticies, unsigned int numEdges);

void initializeDSM(unsigned int numVerticies, unsigned int numEdges);

  bool edgeIDCompare(Edge e1, Edge e2);
  
#endif
