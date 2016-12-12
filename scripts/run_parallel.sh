#!/bin/bash -l

#SBATCH -A g2016032
#SBATCH -p devel
#SBATCH -n 1
#SBATCH -t 00:15:00 --qos=short
#SBATCH -J par_2node_random_mega
#SBATCH --output=graph_par_2n_bfs_random_mega.txt

module load gcc/6.2.0
module unload openmpi
module load openmpi/2.0.1
export ARGODSM=/proj/g2016032/argodsm-master

cd /proj/g2016032/TeamZissou/graphicionado_par/
make
mpirun bfs data/random_mega.gr
