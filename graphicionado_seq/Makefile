# Make sure to set the ARGODSM environment variable:
# e.g. export ARGODSM=/home/jonas/argodsm

# Compilers and flags
CXX := mpic++
LDFLAGS := -L$(ARGODSM)/build/lib
LDLIBS := -largo -largobackend-mpi -lrt -lnuma
CFLAGS := -O3 -g -Wall -pthread -I$(ARGODSM)/src
CPPFLAGS := $(CFLAGS) -std=c++11

# A list of the executable files
EXECUTABLES := main bfs sssp

.PHONY: check-env all

all: check-env $(EXECUTABLES)

main: main.cpp preprocess.cpp test_functions.cpp loadSettings.cpp

bfs: main.cpp bfs.cpp preprocess.cpp test_functions.cpp loadSettings.cpp

sssp: main.cpp sssp.cpp preprocess.cpp test_functions.cpp loadSettings.cpp

clean:
	rm $(EXECUTABLES)

# Check if the environment variable is defined
check-env:
ifndef ARGODSM
	$(error Environment variable ARGODSM is undefined. Set it using: export ARGODSM=/path/to/argodsm/)
endif
