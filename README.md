# level-based-swarm-optimizer-optimization
We implement Yang et al, " A Level-based Learning Swarm Optimizer For Large-scale Optimization", both using serial and parallel code implementation and compared the results obtained based on time complexity and time of convergence. 


<b>Steps to run the serial code</b>:

(1) gcc -o serial code serial code.c -lm </br>
(2) .\serial </br>
(3) Enter the swarm size, level size and learning parameter </br>
</br>

<b>Steps to run the parallel code</b>:

(1) export OMP NUM THREADS=4 </br>
(2) gcc -fopenmp -o parallel parallel final openmp.c -lm </br>
(3) .\parallel </br>
(4) Enter the swarm size, level size and learning parameter </br>
