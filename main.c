#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "graph.h"
#include "dijkstra.h"
#include "utils.h"
/**
 * Trim/Strip function
 */
char *strstrip(char *s) {
  size_t size;
  char *end;

  size = strlen(s);

  if (!size) {
    return s;
  }

  end = s + size - 1;
  while (end >= s && isspace(*end)) {
    end--;
  }
  *(end + 1) = '\0';

  while (*s && isspace(*s)) {
    s++;
  }

  return s;
}


/**
 * Create a graph structure from an input file.
 *
 * @todo Read better the file, to be able to parse large files.
 */
graph_t *parse_input_file (FILE* fp) {
  char line[1024];
  char *token;

  int vertices;
  int i = 0;
  int edge[2];
  double weight;

  graph_t *graph = NULL;

  if (fp != NULL) {
    while (fgets(line, 1024, fp) != NULL) {
      if (i == 0) {
        token = strtok(strstrip(line), " ");
        if (token == NULL) {
          fprintf(stderr, "Unable to parse input.\n");
          exit(EXIT_FAILURE);
        }
        vertices = atoi(token);

        graph = createGraph(vertices);
      }
      else {
        token = strtok(strstrip(line), " ");
        if (token == NULL) {
          fprintf(stderr, "Unable to parse input.\n");
          exit(EXIT_FAILURE);
        }
        edge[0] = atoi(token);

        token = strtok(NULL, " ");
        if (token == NULL) {
          fprintf(stderr, "Unable to parse input.\n");
          exit(EXIT_FAILURE);
        }
        edge[1] = atoi(token);

        if (edge[0] >= vertices || edge[1] >= vertices) {
          fprintf(stderr, "Invalid graph provided.\n");
          exit(EXIT_FAILURE);
        }

        token = strtok(NULL, " ");
        if (token == NULL) {
          fprintf(stderr, "Unable to parse input.\n");
          exit(EXIT_FAILURE);
        }
        weight = atof(token);

        addEdge(graph, edge[0], edge[1], weight);
      }
      i++;
    }
    fclose(fp);
  }
  else {
    fprintf(stderr, "Unable to parse input.\n");
    exit(EXIT_FAILURE);
  }

  return graph;
}


void pretty_print_float (float f) {
  char s[50];
  char *p;

  sprintf(s, "%f", f);
  for (p=s; *p; ++p) {
    if ('.' == *p) {
      while (*++p);
      while ('0' == *--p) {
        *p = '\0';
      }
      if ('.' == *p) {
        *p = '\0';
      }
    }
  }
  printf("%s", s);
}


int main(int argc, char **argv) {
  FILE* in;

  if (argc == 1) {
    in = stdin;
  }
  else if (argc >= 2) {
    if (access(argv[1], R_OK) == 0) {/* file exists */
      in = fopen(argv[1], "r");

      /* try to open the next file */
      if (in == NULL) {
        fprintf(stderr, "Couldn't open %s.\n", argv[1]);
        exit(EXIT_FAILURE);
      }
    }
    else {
      fprintf(stderr, "Invalid input graph.\n\nUsage:\n\t");
      fprintf(stderr, "%s", argv[0]);
      fprintf(stderr, " graph\n");
      exit(EXIT_FAILURE);
    }
  }
  else {
    fprintf(stderr, "Invalid command line.\n\nUsage:\n\t");
    fprintf(stderr, "%s", argv[0]);
    fprintf(stderr, " graph\n");
    exit(EXIT_FAILURE);
  }

  graph_t *graph = parse_input_file(in);

  /* if (!isConnected(graph)) { */
  /*   fprintf(stderr, "Input graph is not connected.\n"); */
  /*   exit(EXIT_FAILURE); */
  /* } */

  int start = 0;
  double *dist = (double*) safe_malloc(sizeof(double)*graph->nb_vertices);
  int *prev = (int*) safe_malloc(sizeof(int)*graph->nb_vertices);
  dijkstra(graph, start, prev, dist);

  for (int i=0; i<graph->nb_vertices; i++) {
    printf("%d\t", i);
    if (dist[i] == INFINITY)
      printf("-\t-\n");
    else {
      pretty_print_float(dist[i]);
      printf("\t");
      if (prev[i] == INT_MAX)           /* unreachable */
        printf("-\n");
      else
        printf("%d\n", prev[i]);
    }
  }

  freeGraph(graph);
  free(dist);
  free(prev);
  exit(EXIT_SUCCESS);
}
