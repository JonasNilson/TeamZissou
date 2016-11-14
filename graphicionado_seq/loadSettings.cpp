#include <iostream>     // std::cout
#include <fstream>      // std::ifstream

//Settings
bool isAllVerticiesActive;
unsigned int maxIteration;
bool edgeDuplicate;

void loadSettings(){
	std:String filename = "settings.cfg"; // filename of settings to be read.
	readSettings();
	std::cout << "Settings have been loaded.\n"
}

void readSettings(){

	// read a file into memory

  std::ifstream file("settings.cfg", std::ifstream::binary); // File open stream
  if (file) {
    // get length of file:
    file.seekg(0, file.end); //get end of file location
    int length = file.tellg(); // get length
    file.seekg(0, file.beg); // set to beginning again

    char * buffer = new char [length];

    std::cout << "Read setting file... \n";
    
    // read data as a block:
    file.read (buffer,length);

    if (file)
      std::cout << "Load settings...";
    else
      std::cout << "error: could not read setting file. Only " << file.gcount() << " could be read";

    file.close(); //Close file stream

    //buffer contains the entire file.
    initSettings(buffer,index+1);

    delete[] buffer;
  }

/*
* length - size of buffer
* buffer - stored setting text buffer
*/
void initSettings(char * buffer, int lenght){
	char c;
	std::String str; // String we use to get 
	for(int i = 0; i < lenght; ++i){
		if(c == '$'){
			setGlobalVariables(buffer, i+1);
		}
	}
}

/**
* Get the variable name
* Input: Index of first character of variable from the buffer.
*/
void setGlobalVariables(char * buffer, int index){
	//Read variable name
	std::String vName; // String we use to store the variable name.
	char c = buffer[index];
	while(c != ':') {
		vName += c;
		c = buffer[++index];
	}
	
	//Read variable value
	index++; // to skip ":" character
	c = buffer[index];
	std::String value; // String we use to store variable value.
	while(c != ']'){
		value = c;
		c = buffer[++index];
	}

	//Set global setting variables
	switch(vName){
		case 'isAllVerticiesActive':
			isAllVerticiesActive = (value == 'y' ? true : false);
		case 'maxIteration':
			maxIteration = (unsigned int) value;
		case 'edgeDuplicate':
			edgeDuplicate = (value == 'y' ? true : false);
		default:
			std::cout << "Something went wrong with the reading of " << vName << "\n";
	}
	std::cout << "Setting of " << vName << " has read the option [" << value <<  "] successfully.\n"
}