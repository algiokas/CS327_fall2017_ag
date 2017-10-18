#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#include "path.h"
#include "dungeon.h"
#include "pqueue.h"

extern int floor_debug;

//gives an edge weight for our pathfinding algorithm based on the hardness of the destination
inline int pf_weight(int hardness) {
    if (hardness == 255) {
        return INT_MAX;
    }
    return 1 + hardness / 85;
}

int dijkstra_map(struct Floor *floor, tunnel_trait t)
{
    if (floor_debug) { printf("Calculating Distance Map...\n"); }
    int *dist;
    int *prev;
    int *visited;
    struct PQueue pq;
    int current = -1;
    int in;
    int out;
    int iter;
    int sourceIndex = floor->pc_loc;

    dist = (int*) malloc(FWIDTH * FHEIGHT * sizeof(int));
    prev = (int*) malloc(FWIDTH * FHEIGHT * sizeof(int));
    visited = (int*) malloc(FWIDTH * FHEIGHT * sizeof(int));
    init_PQ(&pq, (FWIDTH * FHEIGHT / 2), sizeof(int));

    //dist: set values to "Infinity" (INT_MAX) for all nodes except source, which is set to 0
    //visited: set values to 0 for all nodes except source, which is set to 1
    //prev: set all values to -1
    for (iter = 0; iter < FWIDTH * FHEIGHT; iter++) {
        if (iter == sourceIndex) {
            dist[iter] = 0;
            visited[iter] = 1;
        } else {
            dist[iter] = INT_MAX;
            visited[iter] = 0;
        }
        prev[iter] = -1;
    }
    in = sourceIndex;
    insert(&pq, 0, &in);

    //primary search loop 
    while(pq.size) { // queue is not empty
        remove_min(&pq, &out);
        current = out;
        /*
        int left = current - 1;
        int top = current - FWIDTH;
        int right = current + 1;
        int bottom = current + FWIDTH;
        int topleft = top - 1;
        int topright = top + 1;
        int botright = bottom + 1;
        int botleft = bottom - 1;
        */
        
        //array of neighbors for iteration
        int n[8];
        get_neighbors(current, n);
        int eweight = pf_weight(floor->hard_map[current]);
        
        if (floor_debug) {printf("(%d, %d)", floor->hard_map[current], eweight); }
        int i;
        for (i = 0; i < 8; i++) {
            if (eweight != INT_MAX && (floor->hard_map[n[i]] == 0 || t == tunneling)) {
                if (dist[n[i]] > dist[current] + eweight) {
                    dist[n[i]] = dist[current] + eweight;
                    prev[n[i]] = current;
                    if (!visited[n[i]]) {
                        in = n[i];
                        insert(&pq, dist[n[i]], &in);
                        visited[n[i]] = 1;
                    }
                }
            }
        }
        if(floor_debug) { printf("\n"); }
    }
    if (current == -1) {
        printf("dijkstra_map() error : queue failure\n");
        return -1;
    }
    
    int mapiter;
    int success;
    if (t == non_tunneling) {
        for (mapiter = 0; mapiter < FWIDTH * FHEIGHT; mapiter++) {
            floor->dist_map[mapiter] = dist[mapiter];
        }
        success = 0;
    } else if (t == tunneling) {
        for (mapiter = 0; mapiter < FWIDTH * FHEIGHT; mapiter++) {
            floor->dist_map_tunnel[mapiter] = dist[mapiter];
        }
        success = 0;
    } else {
        success = 1;
    }
    delete_PQ(&pq);
    free(dist);
    free(prev);
    free(visited);

    return success;
}




/*  Function: dijkstra_corridor 
    -----------------------
    Uses Dijkstra's shortest path algorithm to calculate a path between two
    locations in the dungeon. The floor map is treated as a graph where adjacent
    cells are connected, and the edge weights between two cells is equivalent to the
    hardness of the destination cell. Additionally, the HMODVALUE macro in this file
    modifies the behaviour of this algorithm to treat rooms cells as higher than their
    actual hardness which helps make the generated corridors more intersting. This 
	implementation uses the priority queue in pqueue.c. 
    
    floor: pointer to the floor being modified
    source, target: ordered pairs that denote the start and end of the path
    path: a pointer array that is passed to the function and will contain the output path

    returns: 0 if successful
 */
int dijkstra_corridor(struct Floor *floor, struct Duo source, struct Duo target, int *path)
{
    if (floor_debug) { printf("Calculating Dijkstra Path...\n"); }
    int *dist;
    int *prev;
    int *visited;
    struct PQueue pq;
    int current = -1;
    int in;
    int out;
    int iter;
    int sourceIndex = INDEX2D(source.x, source.y);
    int targetIndex = INDEX2D(target.x, target.y);

    dist = (int*) malloc(FWIDTH * FHEIGHT * sizeof(int));
    prev = (int*) malloc(FWIDTH * FHEIGHT * sizeof(int));
    visited = (int*) malloc(FWIDTH * FHEIGHT * sizeof(int));
    init_PQ(&pq, (FWIDTH * FHEIGHT / 2), sizeof(int));

    if (floor_debug) { printf("DPATH: source=%d, target=%d\n" ,sourceIndex, targetIndex); }

    //dist: set values to "Infinity" (INT_MAX) for all nodes except source, which is set to 0
    //visited: set values to 0 for all nodes except source, which is set to 1
    //prev: set all values to -1
    for (iter = 0; iter < FWIDTH * FHEIGHT; iter++) {
        if (iter == sourceIndex) {
            dist[iter] = 0;
            visited[iter] = 1;
        } else {
            dist[iter] = INT_MAX;
            visited[iter] = 0;
        }
        prev[iter] = -1;
    }
    in = sourceIndex;
    insert(&pq, 0, &in);

    //primary search loop 
    while(pq.size) { // queue is not empty
        remove_min(&pq, &out);
        current = out;
        if (current == INDEX2D(target.x, target.y)) {
            break;
        }
        //Left neighbor
        if ((current % FWIDTH) > 1) {
            int left = current - 1;
            int hmod = HMODVAL * (floor->type_map[left] == room_c);
            if (dist[left] > dist[current] + floor->hard_map[left] + hmod) {
                dist[left] = dist[current] + floor->hard_map[left] + hmod;
                prev[left] = current;
                if (!visited[left]) {
                    in = left;
                    insert(&pq, dist[left], &in);
                    visited[left] = 1;
                }
            }
        }
        //top neighbor
        if ((current / FWIDTH) > 1) {
            int top = current - FWIDTH;
            int hmod = HMODVAL * (floor->type_map[top] == room_c);
            if (dist[top] > dist[current] + floor->hard_map[top] + hmod) {
                dist[top] = dist[current] + floor->hard_map[top] + hmod;
                prev[top] = current;
                if (!visited[top]) {
                    in = top;
                    insert(&pq, dist[top], &in);
                    visited[top] = 1;
                }
            }
        }
        //right neighbor
        if ((current % FWIDTH) < FWIDTH - 1) {
            int right = current + 1;
            int hmod = HMODVAL * (floor->type_map[right] == room_c);
            if (dist[right] > dist[current] + floor->hard_map[right] + hmod) {
                dist[right] = dist[current] + floor->hard_map[right] + hmod;
                prev[right] = current;
                if (!visited[right]) {
                    in = right;
                    insert(&pq, dist[right], &in);
                    visited[right] = 1;
                }
            }
        }
        //bottom neighbor
        if ((current / FWIDTH) < FHEIGHT - 1) {
            int bottom = current + FWIDTH;
            int hmod = HMODVAL * (floor->type_map[bottom] == room_c);
            if (dist[bottom] > dist[current] + floor->hard_map[bottom] + hmod) {
                dist[bottom] = dist[current] + floor->hard_map[bottom] + hmod;
                prev[bottom] = current;
                if (!visited[bottom]) {
                    in = bottom;
                    insert(&pq, dist[bottom], &in);
                    visited[bottom] = 1;
                }
            }
        }
    }
    if (current == -1) {
        printf("dijkstra_corridor() error : queue failure\n");
        return -1;
    }
    if (floor_debug) { printf("Constructing path from %d to %d\n", current, sourceIndex); }
    int pathIter = 1;
    path[0] = current;
    while (prev[current] != sourceIndex && pathIter < 100) {
        path[pathIter] = prev[current];
        current = prev[current];
        pathIter++;
    }
    
    delete_PQ(&pq);
    free(dist);
    free(prev);
    free(visited);

    return pathIter+1;
}