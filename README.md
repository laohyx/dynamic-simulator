dynamic-simulator
=================
Usage:

    Download the git reopsitory, and type `make` to compile.
  
You can edit param.conf to modity settings.
    
The program will generate a series files like network_*.dat and community_*.dat.

1) network_*.dat contains the list of edges (nodes are labelled from ZERO; the edges are ordered and repeated twice, i.e. source-target and target-source).

2) community_*.dat contains a list of the nodes and their membership (memberships are labelled by integer numbers from ZERO).
