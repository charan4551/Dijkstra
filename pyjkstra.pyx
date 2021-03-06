from libc.stdlib cimport malloc, free
from cpython cimport array
from pyjkstra cimport dijkstra as c_dijkstra
from pyjkstra cimport graph_t, createGraph, freeGraph
from pyjkstra cimport printGraph, printNode, addEdge, INT_MAX

cdef class c_Graph:
    '''Cython class that exposes a graph'''
    cdef graph_t * thisptr

    def __cinit__(self, int n):
        ''' Initialises a C pointer to the graph structure.'''
        self.thisptr = createGraph(n)
        if self.thisptr is NULL:
            raise MemoryError

    def __dealloc__(self):
        ''' Free the malloced memory. '''        
        if self.thisptr is not NULL:
            freeGraph(self.thisptr)
        
    def __str__(self):
        ''' Print a representation of self.'''
        # Bad hack, it prints but returns an empty string …
        printGraph(self.thisptr)
        return ""

    @property
    def nb_vertices(self):
        return self.thisptr.nb_vertices

    def get(self, int n):
        printNode(self.thisptr, n)

    def addEdge(self, int src, int dest, double weight):
        addEdge(self.thisptr, src, dest, weight)

    def dijkstra (self, int s):
        ''' Converts the Python objects to and from C variables and
        call the algorithm. '''

        cdef int l = self.thisptr.nb_vertices

        # Malloc arrays for return values of the dijkstra algorithm 
        cdef int* prev_arg = <int*>malloc(sizeof(int)*l)
        cdef double* dist_arg = <double*>malloc(sizeof(double)*l)

        # Call the algorithm
        c_dijkstra(self.thisptr, s, prev_arg, dist_arg)

        prev_out = []
        dist_out = []
        # Convert back from C-types to python object
        for i in range(l):
            if (prev_arg[i] == INT_MAX):
                val = None
            else:
                val = prev_arg[i]
                
            prev_out.append(val)

            
            if (dist_arg[i] == INT_MAX):
                val = None
            else:
                val = dist_arg[i]
                
            dist_out.append(val)

        # Free C arrays
        free(dist_arg)
        free(prev_arg)
        return (prev_out, dist_out)

# This is the pure Python class that implements all "methods" of our graph
class Graph:
    ''' A graph represented as an adjacency list.'''
    c_graph = None
    def __init__(self, int n):
        ''' n is the number of vertices.'''
        self.c_graph = c_Graph(n)
        
    def __del__(self):
        del self.c_graph
        
    def __str__(self):
        return self.c_graph.__str__()
    
    @property
    def nb_vertices(self):
        return self.c_graph.nb_vertices

    def addEdge(self, int src, int dest, double weight):
        ''' Adds an edge to the graph from `src` to `dest` with weight `weight`.'''
        self.c_graph.addEdge(src, dest, weight)

    def get(self, int n):
        return self.c_graph.get(n)

    def dijkstra (self, int s):
        ''' Implement the dijkstra path-finding algorithm.
        Parameters:
        G    dijkstra.Graph    a python representation of a graph
        s    int               the starting point of the algorithm
    
        Returns:
        (prev, dist) with prev the antecedent in the path and dist the distance of
        each node from the start
        '''
        return self.c_graph.dijkstra(s)
        
