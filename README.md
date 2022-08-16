# Demonstration of Deadlock in SST 

[Structural Simulation Toolkit](https://sst-simulator.org/) (SST) is a Discrete Event Simulation (DES) framework written in C++ and Python. Although SST has historically focused on simulation of High Performance Computing, the DES framework can be used in other scenarios. 

In this repo, [deadlock](https://en.wikipedia.org/wiki/Deadlock) is modeled using SST. 


This demonstration of deadlock relies only on [SST-Core](https://github.com/sstsimulator/sst-core)


# Objectives
- [x] literature search for instances of this problem
- [x] identify underlying mathematical or logical conditions that trigger this problem in distributed systems
- [x] identify minimum features for C++ components needed to create this problem
- [x] identify minimum size of graph (links, components) to observe the issue
- [x] identify metrics needed to identify the existence of the problem
- [x] measure the metrics and demonstrate the detection of the problem
- [ ] use the metrics to identify the problem at various scales (e.g., 10 nodes, 100 nodes, 1000 nodes)

# Context

The intended audience for this documentation has basic familiar with use of SST, C++, Python, Makefiles.

Documentation should explain how the concept of deadlock is mapped to the SST model.


# Literature Review

## "Deadlock Detection in Distributed Systems"  
Ajay Kshemkalyani and Mukesh Singhal  
<https://www.cs.uic.edu/~ajayk/Chapter10.pdf>

## "Distributed Deadlock Detection Algorithm"
R. Obermarck  
<https://personal.utdallas.edu/~praba/obermarck.pdf>

## A Survey Of Distributed Deadlock Detection Algorithms
<https://dl.acm.org/doi/pdf/10.1145/15833.15837>

## Distributed Deadlock Detection 
<https://dl.acm.org/doi/pdf/10.1145/357360.357365>

<http://www.inf.ufsc.br/~frank.siqueira/BDD/Artigos/A%20One-Phase%20Algorithm%20to%20Detect%20Distributed%20Deadlocks%20in%20Replicated%20Databases.pdf>
