/* S***y bilateral projects puzzle by tony@lazarew.me */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_INPUT_BUFFER 16
#define ALLOCATEZERO(buffer, size, block) \
    if (((buffer) = malloc((size))) == NULL) \
        { block } \
    memset((buffer), 0, (size))
#define INF -1
#define INITIAL_QUEUE_BLOCKS 32


typedef struct _node_t {
    unsigned int id;
    struct _node_t **neighbors;
    struct _node_t *match;
    int dist;
    unsigned int covered;
} node_t;


int parse_input(const char * /* in */, size_t /* in */, unsigned int * /* out */);

/* Node methods */
node_t *node_create(unsigned int /* in */, size_t /* in */, node_t * /* in */);
void node_add_neighbor(node_t * /* in/out */, node_t * /* in */);
void node_destroy_all(node_t * /* in/out */);

/* Queue methods */
int queue_init_or_extend(node_t *** /* out */, size_t /* in */);
void queue_push(node_t *** /* in/out */, node_t * /* in */);
node_t *queue_pop(node_t ** /* in/out */);
void queue_destroy(node_t ** /* in */);

/* Algorithm methods */
unsigned int hopcroft_karp(node_t * /* in/out*/);
int bfs(node_t * /* in/out */, node_t * /* in/out */);
int dfs(node_t * /* in */, node_t * /* in/out */, node_t * /* in/out */);
void koenig(node_t * /* in/out*/);
void vertex_cover(node_t * /* in/out*/, node_t * /* in */);


int main()
{
    char
        buf[MAX_INPUT_BUFFER + 1] = "",
        chunk[2] = "";
    unsigned int
        m = 0, ids[2] = {0, 0},
        i = 0, j = 0,
        matches = 0;
    node_t
        *nodes = NULL,
        *ptr = NULL,
        *node[2] = {NULL, NULL};

    /* Reading stdin. First off, getting the amount of projects */
    while (read(0, chunk, 1) != 0) {
        if (*chunk == '\n' || *chunk == 0 || strlen(buf) == MAX_INPUT_BUFFER)
            break;
        strncat(buf, chunk, 1);
    }

    if (!parse_input(buf, 1, &m))
        return 1;

    ALLOCATEZERO(nodes, sizeof(node_t) * (m * 2 + 1), {return 1;});

    /* For every project getting a pair of employees and creating nodes
       for them. */
    for (i = 0; i < m; i++) {
        buf[0] = 0;
        while (read(0, chunk, 1) != 0) {
            if (*chunk == '\n' || *chunk == 0 || strlen(buf) == MAX_INPUT_BUFFER)
                break;
            strncat(buf, chunk, 1);
        }

        if (!parse_input(buf, 2, ids))
            return 1;

        for (j = 0; j < 2; j++) {
            if((node[j] = node_create(ids[j], m, nodes)) == NULL) {
                node_destroy_all(nodes);
                free(nodes);
                return 1;
            }
        }

        node_add_neighbor(node[0], node[1]);
        node_add_neighbor(node[1], node[0]);
    }

    matches = hopcroft_karp(nodes);
    koenig(nodes);

    printf("%u\n", matches);
    for (ptr = nodes; ptr->id != 0; ptr++)
        if (ptr->covered)
            printf("%u\n", ptr->id);

    node_destroy_all(nodes);
    free(nodes);

    return 0;
}


/*
 * Process the data from stdin.
 */
int parse_input(const char *buf_in, size_t limit, unsigned int *numbers)
{
    char *tok = NULL,
         buf[MAX_INPUT_BUFFER + 1] = "";
    size_t i = 0;

    memset(buf, 0, MAX_INPUT_BUFFER + 1);
    strncpy(buf, buf_in, strlen(buf_in));
    for (tok = strtok(buf, " ");
         i < limit && tok;
         i++, numbers++, tok = strtok(NULL, " ")) {
        if (sscanf(tok, "%u", numbers) != 1 || *numbers > 10000 )
            return 0;
    }

    if (i < limit)
        return 0;
    return 1;
}


/*
 * Create a new graph node and add it to node vector.  If a node with the same
 * id exists, just return it.
 */
node_t *node_create(unsigned int id, size_t neighbors, node_t *nodes)
{
    for (; nodes->id != 0 && nodes->id != id; nodes++);

    if (nodes->id == 0) {
        nodes->id = id;
        ALLOCATEZERO(nodes->neighbors, sizeof(node_t *) * (neighbors + 1),
            {return (node_t *)NULL;});
        nodes->dist = INF;
        nodes->match = NULL;
        nodes->covered = 0;
    }

    return nodes;
}


void node_add_neighbor(node_t *node, node_t *neighbor)
{
    node_t **ptr = node->neighbors;
    for (; *ptr != NULL; ptr++);
    *ptr = neighbor;
}


/*
 * Destroy all nodes in vector.
 */
void node_destroy_all(node_t *nodes)
{
    for(; nodes->id != 0; nodes++)
        free(nodes->neighbors);
}


/*
 * Finds the maximum matching for the bipartite graph.
 */
unsigned int hopcroft_karp(node_t *nodes)
{
    unsigned int match = 0;
    node_t
        *ptr = NULL,
        null_node = {0, NULL, NULL, INF, 0};

    while (bfs(nodes, &null_node))
        for (ptr = nodes; ptr->id != 0; ptr++)
            if (ptr->id < 2000 && ptr->match == NULL
                && dfs(ptr, nodes, &null_node))
                match++;

    return match;
}


/*
 * Breadth-first search.
 */
int bfs(node_t *nodes, node_t *null_node)
{
    node_t
        *ptr = NULL,
        *match = NULL,
        **neighbor = NULL,
        **queue = NULL;

    if (!queue_init_or_extend(&queue, INITIAL_QUEUE_BLOCKS))
        return 0;

    for (ptr = nodes; ptr->id != 0; ptr++)
        if (ptr->id < 2000) {
            if (ptr->match == NULL) {
                ptr->dist = 0;
                queue_push(&queue, ptr);
            } else {
                ptr->dist = INF;
            }
        }

    null_node->dist = INF;

    while ((ptr = queue_pop(queue)) != NULL) {
        if (ptr->id == 0)
            continue;

        for (neighbor = ptr->neighbors; *neighbor != NULL; neighbor++) {
            if ((match = (*neighbor)->match) == NULL)
                match = null_node;

            if (match->dist == INF) {
                match->dist = ptr->dist + 1;
                queue_push(&queue, match);
            }
        }
    }

    queue_destroy(queue);
    return null_node->dist != INF;
}


/*
 * Depth-first search.
 */
int dfs(node_t *node, node_t *nodes, node_t *null_node)
{
    node_t
        *match = NULL,
        **neighbor = NULL;

    if (node->id == 0)
        return 1;

    for (neighbor = node->neighbors; *neighbor != NULL; neighbor++) {
        if ((match = (*neighbor)->match) == NULL)
            match = null_node;

        if (match->dist == node->dist + 1 && dfs(match, nodes, null_node)) {
            node->match = (*neighbor);
            (*neighbor)->match = node;
            return 1;
        }
    }

    node->dist = INF;
    return 0;
}


/*
 * Returns the minimum vertex cover based on the matching found by
 * Hopcroft-Karp algorithm.
 *
 * In order to promote our swedish friend into the list of invitees we just
 * start off from free vertices in Sthlm's half of the graph.  This way in
 * situations when both Ldn and Sthlm employees are appropriate we choose
 * a Sthlm's one, whether it is our friend or not.  That is the only legal
 * way to get him an invitation.
 */
void koenig(node_t *nodes)
{
    node_t *ptr = NULL;

    for (ptr = nodes; ptr->id != 0; ptr++)
        if (ptr->id < 2000 && ptr->match == NULL) {
            ptr->covered = 1;
            vertex_cover(ptr, nodes);
        }

    /* Inverting covered state for Sthlm's employees */
    for (ptr = nodes; ptr->id != 0; ptr++)
        if (ptr->id < 2000)
            ptr->covered = !ptr->covered;
}


void vertex_cover(node_t *node, node_t *nodes)
{
    node_t **neighbor = NULL;

    for (neighbor = node->neighbors; *neighbor != NULL; neighbor++) {
        if ((*neighbor)->covered)
            continue;

        if ((*neighbor)->match != NULL && (*neighbor)->match != node) {
            (*neighbor)->covered = (*neighbor)->match->covered = 1;
            vertex_cover((*neighbor)->match, nodes);
        }
    }
}


/*
 * Simple FIFO queue implementation.
 */
int queue_init_or_extend(node_t ***queue, size_t min_size)
{
    static size_t
        size = 0,
        used = 0;
    size_t prev_size = 0;
    node_t **prev_queue = NULL;

    if (size == 0 && min_size)
        size = min_size;
    if (!size)
        return 0;  /* No initial size was set? */

    if (*queue && used < size) {
        /* Enough room to fit one more element */
        used++;
        return 1;
    }

    prev_queue = *queue;
    if (prev_queue) {
        /* Doubling the space for reallocation */
        prev_size = size;
        size *= 2;
    }

    ALLOCATEZERO(*queue, sizeof(node_t *) * (size + 1), {return 0;});

    if (prev_queue == NULL)
        queue_pop(NULL);

    if (prev_queue) {
        memcpy(*queue, prev_queue, sizeof(node_t *) * prev_size);
        free(prev_queue);
    }

    return 1;
}


void queue_push(node_t ***queue, node_t *node)
{
    node_t **ptr = NULL;

    queue_init_or_extend(queue, 0);
    for(ptr = *queue; *ptr != NULL; ptr++);
    *ptr = node;
}


node_t *queue_pop(node_t **queue)
{
    static size_t top = 0;

    if (queue == NULL) {
        /* Resetting internal top index.  This is done once, when initializing
           the queue. */
        top = 0;
        return NULL;
    }

    return queue[top++];
}


void queue_destroy(node_t **queue)
{
    free(queue);
}
