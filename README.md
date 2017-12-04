# 15 Puzzle External Memory Search

External Memory Optimal Search for the 15 Puzzle Domain.

This is the source code used in 'Revisiting Immediate Duplicate Detection in
External Memory Search' by Shunji Lin, Alex Fukunaga, accepted for the 32nd
AAAI Conference on Artificial Intelligence (AAAI-18).

Includes A*-IDD, which implements immediate duplicate detection via open
address hashing (instead of delayed duplicate detection), for use with solid
states drives as secondary memory.

This source code is built upon the [15 Puzzle solver by Ethan
Burns](http://www.cs.unh.edu/~eaburns/socs12/), as used in
``Implementing Fast Heuristic Search Code,'' by Burns, Hatem, Leighton, and
Ruml, in Proceedings of the Fifth Annual Symposium on Combinatorial Search
(SoCS-12), 2012.

The src/ directory contains the most optimized versions of both
IDA* and A* (Burns et al. 2012), as well as the external search algorithms,
A*-IDD, A*-DDD and External A*

## Files/Subdirectories:

+ src/astar.hpp
  - Most optimized version of A* (Burns et al. 2012), implementation by Ethan
    Burns

+ src/idastar.hpp
  - Most optimized version of IDA* (Burns et al. 2012), implementation by
    Ethan Burns

+ src/compressing
  - A*-IDD (Lin, Fukunaga 2018)

+ src/astar_ddd
  - A*-DDD (Korf 2004; Hatem 2014)

+ src/external_astar
  - External A* (Edelkamp, Jabbar, and Schrodl 2004)

+ Korf100/  
  - Korf's 100 random instances (Korf 1985)

## Usage
To compile:
```
./makeall
```

To run:
```
./src/tiles <search algorithm>  
<width height>  
<goal positions>  
<intial positions>  
```
Where  
search algorithm = [astar, idastar, astar\_idd, astar\_ddd,
external\_astar]   
width height = 4 4  
initial positions = 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15

Alternatively, run on Korf's instances, e.g.:
```
./src/tiles astar_idd < Korf100/prob001
```

## Disclaimer
This has only been tested on a linux system. Furthermore, the use of mmap in
A*-IDD requires a POSIX-compliant operating system.

## References
+ Burns, E. A.; Hatem, M.; Leighton, M. J.; and Ruml, W. 2012. Implementing
fast heuristic search code. In Fifth Annual Symposium on
Combinatorial Search.
+ Edelkamp, S.; Jabbar, S.; and Schrödl, S. 2004. External A*. KI 4:226–240.
+ Hatem, M. 2014. Heuristic search with limited memory. Ph.D. Dissertation,
University of New Hampshire.
+ Korf, R. E. 1985. Depth-first iterative-deepening. Artificial Intelligence
27(1):97 – 109.
+ Lin, S.; Fukunaga, A. 2018. Revisiting Immediate Duplicate Detection in
External Memory Search. To appear in the 32nd
AAAI Conference on Artificial Intelligence (AAAI-18).