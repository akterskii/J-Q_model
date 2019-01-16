# J-Q_model

Pipeline to calculate properties of the singlet spectrum of the J-Q model of the 2D Heisenberg antiferromagnetic with spin 1/2 on the square lattice.
Notation corresponds to the paper [Anders W. Sandvik](https://arxiv.org/abs/cond-mat/0611343). 
General description of the approach could be found in this [paper](https://arxiv.org/abs/1508.00744)

Stage 0:
Generate ordered set of all possible states with k spins 1/2 (in total 2^k states). 
Accodring to the generated order fill the matrix elements that correspond to the lefthand side of the equations that use to define effective Hamiltonian states.
Matrix generated in Wolfram Mathematica format, additionally program output the list of variables that are used in the matrix.

Stage 1:
Generate all possible configurations that can give nonzero terms of the "effective Hamiltonian". 
Each term could be represented as connected graph on the square lattice vith diagonals. 
Term could give nonzero solution only if the term represented by connected graph. 
Each addend in the perturbation operator contains two pairs of scalar products of spin operators and therefore provide nonezero matrix elments for transitions between states with zero total spins of the plaquet.
Consequently the graph could be not connected

Stage 2:
Evaluate each generated term and output the corresponding matrix elements for transitions between all possible ground states. Output is represented im Wolfram Mathematica matrix format.

Stage 3:
Generate Wolfram Mathematica files that contain systems to solve with fixed parameters of J and Q. Result of this stage is the coefficiets of the "effective Hamiltonian" in the corresponding order by corresponding term.

Stage 4:
Sum all terms of the same type. Nodes have numbers in spiral order (generated by class in SharedFiles\matrixNodes.cpp)

Stage 4.5:
Sum all translationaly invariant terms that contain 2 or less spin operators.