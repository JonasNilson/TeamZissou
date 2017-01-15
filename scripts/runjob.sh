#!/bin/bash -l

#SBATCH -A g2016032
#SBATCH -p devel
#SBATCH -n 1
#SBATCH -t 00:10:00 --qos=short
#SBATCH -J seq_1node_small

module load gcc/6.2.0
module unload openmpi
module load openmpi/2.0.1
export ARGODSM=/proj/g2016032/argodsm-master

cd /proj/g2016032/TeamZissou/graphicionado_seq/
make
mpirun bfs data/small.gr
