#ifndef loadSettings_hpp
#define loadSettings_hpp loadSettings_hpp

void loadSettings();

//Global settings variables
extern bool isAllVerticesActive;
extern unsigned int maxIterations;
extern bool edgeDuplicates;
extern unsigned int* startingNodes;
extern unsigned int numberOfStartingNodes;

extern bool singleNodeRunning;
extern unsigned int THREADS;


#endif
