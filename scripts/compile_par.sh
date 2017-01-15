#!/bin/bash -l

# This is meant to be run directly, not with sbatch
# It's meant to make it easier with module loading

#SBATCH -A g2016032
#SBATCH -p devel
#SBATCH -n 1
#SBATCH -t 00:10:00 --qos=short
#SBATCH -J seq_1node_small
#SBATCH --output=graph_seq_bfs_large.txt

module load gcc/6.2.0
module unload openmpi
module load openmpi/2.0.1
export ARGODSM=/proj/g2016032/argodsm-master

cd /proj/g2016032/TeamZissou/graphicionado_par/
make
#mpirun bfs data/large.gr
