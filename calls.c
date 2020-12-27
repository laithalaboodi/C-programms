/*Author:Laith Alaboodi*/
/*
 * creating phone call graph and performing analysis on it
 * Note about data structures.
 * Graph is represented as linked list of nodes of type Node (below),
 * and from each of them there is an adjacent list of edges Edge (below).
 * Also, there is linked-list based queue for BFS (which is common for both visited nodes
 * and unvisited queue, we just have visited nodes head pointing to its start,
 * and unvisited queue head moving forward through it, and unvisited queue end is the same
 * as end of visited queue, that means of whole list).
 * All are freed in whatever way the program ends.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* ------------------- PART I -- GRAPH --------------------- */

typedef struct Node {      // nodes in graph
    char phone[13];        // xxx-xxx-xxxx + '\0'
    struct Node *next;     // another node in graph (not necessarily connected)
    struct Edge *adjList;  // edges connected to this node
} Node;

// edges in graph (start node of edge is the one from which adj. list we get this edge)
// NOTE: graph is undirected, so to satisfy start-to pattern there are 2 edges, 1 in list
// for both start and to nodes
typedef struct Edge {
    struct Node *to;     // destination node of edge
    int nCalls;          // calls count to that node from start node
    struct Edge *next;   // next node start is connected to
} Edge;

typedef struct {        // graph is wrapped around to be able to change firstNode in functions
    Node *firstNode;    // and not pass double pointer there
} Graph;


/*
 * allocation of new graph, returns it
 */
Graph *allocGraph()
{
    Graph *graph = malloc(sizeof *graph);
    if (graph != NULL)
        graph->firstNode = NULL;
    return graph;
}

/*
 * allocation of new node, returns it
 */
static Node *allocNode(char *phone)
{
    Node *node = malloc(sizeof *node);
    if (node == NULL)
        return NULL;
    strcpy(node->phone, phone);
    node->next = NULL;
    node->adjList = NULL;
    return node;
}

/*
 * allocation of new edge, returns it
 */
static Edge *allocEdge(Node *node)
{
    Edge *edge = malloc(sizeof *edge);
    if (edge==NULL)
        return NULL;
    edge->to = node;
    edge->nCalls = 1;
    edge->next = NULL;

    return edge;
}

/*
 * find the node for the given phone,
 * returns node structure
 */
static Node *findNode(Graph *graph, char *phone)
{
    for (Node *node = graph->firstNode; node != NULL; node=node->next)
        if (strcmp(node->phone, phone)==0)
            return node;
    return NULL;
}

/*
 * add node with the given phone
 * returns:
 * -1 if fail (means it already exists, or there is memory error)
 * 0 if OK
 */
static int addNode(Graph *graph, char *phone)
{
    if (findNode(graph, phone) != NULL)
        return -1;
    Node *node = allocNode(phone);
    if (node == NULL)
        return -1;
    node->next = graph->firstNode;
    graph->firstNode = node;
    return 0;
}

/* 
 * find edge between two nodes in the graph
 * returns edge if found or null if fail
 */
static Edge *findEdge(Graph *graph, Node *from, Node *to)
{
    for (Edge *edge = from->adjList; edge!=NULL; edge=edge->next)
        if (edge->to == to)
            return edge;
    return NULL;   
}

/*
 * adding new edge to the graph, for two given phones
 * NOTE: if they do not exist, then add them, to easily call only this function in main
 *
 * returns 0 if OK, -1 if either this is the same node (cannot connect with itself),
 * or nodes were not added due to memory error (when adding new nodes)
 */
int addEdge(Graph *graph, char *phone1, char *phone2)
{
    // some not exist -- add them
    if (findNode(graph,phone1)==NULL && addNode(graph, phone1)==-1) 
        return -1;

    if (findNode(graph,phone2)==NULL && addNode(graph, phone2)==-1) 
        return -1;

    Node *node1=findNode(graph, phone1);
    Node *node2=findNode(graph, phone2);

    // also cannot add edge to itself (e.g. call to itself)
    if (node1 == node2)
        return -1;

    // edges can either BOTH exist, or BOTH not exist:
    // it is ensured by design, so no check separately
    Edge *edge1 = findEdge(graph, node1, node2);
    Edge *edge2 = findEdge(graph, node2, node1);

    // already exist    
    if (edge1 != NULL && edge2 != NULL) {
        edge1->nCalls++;
        edge2->nCalls++;
        return 0;
    }

    // new (adj. lists are updated only so that either BOTH
    //  are added, or BOTH are not added if error)
    edge1 = allocEdge(node2);
    if (edge1 == NULL)
        return -1;
    edge2 = allocEdge(node1);
    if (edge2 == NULL) {
        free(edge1);
        return -1;
    }

    // 1->2
    edge1->next = node1->adjList;
    node1->adjList = edge1;

    // 2->1
    edge2->next = node2->adjList;
    node2->adjList = edge2;

    return 0;
}

/*
 * this is for debugging: print the whole graph
 */
void printGraph(Graph *graph)
{
    for (Node *node = graph->firstNode; node!=NULL; node=node->next) {
        printf("%s: ", node->phone);
        for (Edge *edge=node->adjList; edge != NULL; edge=edge->next)
            printf("%s(%d) ", edge->to->phone, edge->nCalls);
        printf("\n");
    }
}

/*
 * free memory used by graph
 */
void removeGraph(Graph *graph)
{
    Node *nextNode;
    for (Node *node = graph->firstNode; node!=NULL; node=nextNode) {
        Edge *nextEdge;
        for (Edge *edge=node->adjList; edge!=NULL; edge=nextEdge) {
            nextEdge = edge->next;
            free(edge);
        }
        nextNode = node->next;
        free(node);
    }
    free(graph);
}

/* 
 * determine count of talks between phones phone1 and phone2
 * returns 
 * - count of talks if they are directly connected, count >= 1
 * - 0 if they are not directly connected -- means have to use BFS after
 * - -1 if any error (only when not existing nodes are asked)
 */
int talkedTimes(Graph *graph, char *phone1, char *phone2)
{
    Node *node1 = findNode(graph, phone1);
    Node *node2 = findNode(graph, phone2);
    if (node1 == NULL || node2==NULL)
        return -1;    // incorrect input
 
    Edge *edge = findEdge(graph, node1, node2);
    if (edge==NULL)
        return 0;
    return edge->nCalls;
}




/* -------------------- PART II -- BFS AND QUEUE ---------------------- */


// node in both visited list and queue
typedef struct qNode {
    Node *node;            // node in graph which is placed in queue
    int level;            // level of node (for BFS)
    struct qNode *next;    // next in this queue
} qNode;

/*
 * check if queue contains node  (either as visited, or as ready to
 * be visited later) -- this is to avoid adding it second time, in BFS
 */
static int containsQueue(qNode *qHead, Node *node)
{
    for (qNode *cur = qHead; cur != NULL; cur = cur->next)
        if (cur->node == node)
            return 1;
    return 0;
}

/*
 * add graph node to BFS queue, setting the given level
 * queue is passed as double pointer, to allow changing it easily if it 
 * was NULL on first call
 *
 * returns -1 if error of allocating queue node
 * 0 otherwise (will always be OK)
 */
static int entailQueue(qNode **qHead, Node *node, int level)
{
    // alloc
    qNode *tail = malloc(sizeof *tail);
    if (tail == NULL)
        return -1;

    // fill
    tail->node = node;
    tail->level = level;
    tail->next = NULL;

    // entail
    qNode **current;
    for (current = qHead; *current != NULL; current=&((*current)->next) )
        ;
    *current = tail;
    return 0;
}

/*
 * free BFS queue
 */
static void freeQueue(qNode *qHead)
{
    qNode *cur, *next;
    for (cur = qHead; cur != NULL; cur = next) {
        next = cur->next;
        free(cur);
    }
}

/*
 * BFS algorithm,
 * returns -2 if error (main handles)
 * returns -1 if no path (disjoint nodes)
 * returns number of edges on shortest path between otherwise (joint nodes),
 * this also includes 0 if same node, it is handled by main as well
 */
int BFS(Graph *graph, char *startPhone, char *targetPhone)
{
    Node *startNode, *targetNode;

    // check existence
    startNode = findNode(graph, startPhone);
    targetNode = findNode(graph, targetPhone);
    if (startNode == NULL || targetNode == NULL)
        return -2;

    // prepare start
    qNode *visitedHead, *unvisitedHead;
    visitedHead = NULL;
    if (entailQueue(&visitedHead, startNode, 0) == -1)
        return -2;
    unvisitedHead = visitedHead;

    // work
    while (unvisitedHead != NULL) {
        Node *curNode = unvisitedHead->node;
        int curLevel = unvisitedHead->level;
        if (curNode == targetNode) {   // done
            freeQueue(visitedHead);
            return curLevel;
        }
        // get all children
        for (Edge *edge = curNode->adjList; edge!=NULL; edge=edge->next) {
            Node *linkNode = edge->to;
            if (!containsQueue(visitedHead, linkNode)) {
                if (entailQueue(&unvisitedHead, linkNode, curLevel+1)==-1) {
                    freeQueue(visitedHead);
                    return -2;
                }
            }
        }
        unvisitedHead = unvisitedHead->next;
    }

    freeQueue(visitedHead);
    return -1;
}



/* ----------------------- PART III -- INPUT PARSING AND MAIN() --------- */

/*
 * remove and leading and trailing spaces in input string
 */
static char *trim(char *s)
{
    int i;
    for (i = 0; isspace(s[i]); i++)    // skip leading
        ;
    int from = i;    // start here
    
    for (; s[i]; i++)    // move until end
        ;

    // move backwards, skip trailing
    for (i--; i>from && isspace(s[i]); i--)
        ;
    s[i+1] = '\0';    // stop here

    strcpy(s, &s[from]);    // copy this part to beginning
            // (it is ok as we copy from higher to upper addresses always)
    return s;
}

/*
 * check correct phone format in trimmed string
 *
 * s must be of format:
 * xxx-xxx-xxxx (any space/tab count) xxx-xxx-xxxx
 * return 0 if OK
 * return -1 if fail
 */
static int check_string(const char *s)
{
    const char *phone = "xxx-xxx-xxxx";    // template to compare 2 parts with

    // first part
    int i;
    for (i = 0; phone[i]; i++)
        if ((phone[i]=='x' && !isdigit(s[i])) || (phone[i]=='-' && s[i]!='-'))
            return -1;

    // skip spaces between parts
    while (isspace(s[i]))
        i++;

    // second part
    for (int j = 0; phone[j]; j++, i++)
        if ((phone[j]=='x' && !isdigit(s[i])) || (phone[j]=='-' && s[i]!='-'))
            return -1;
    if (s[i])
        return -1;

    return 0;
}



int main(int argc, char *argv[])
{
    int return_status=0;

    // check CLI
    if (argc < 2) {
        fprintf(stderr, "Usage: ./calls <file1> [file2] [file3] [...]\n");
        exit(1);
    }

    // make graph
    Graph *graph = allocGraph();
    if (graph == NULL) {
        fprintf(stderr, "Cannot create graph\n");
        exit(1);
    }

    // get lines and input
    int at_least_one_opened = 0;
    while (--argc) {
        // open the next one
        FILE *fp = fopen(*++argv, "r");
        if (fp == NULL) {
            fprintf(stderr, "Cannot open file %s\n", *argv);
            return_status = 1;    // nonfatal error
            continue;
        }
        at_least_one_opened = 1;

        // read it
        char buf[200];
        while (fgets(buf, sizeof buf, fp) != NULL) {
            trim(buf);
            if (*buf=='\0')    // empty string, skip
                continue;
            if (check_string(buf) == -1) { // incorrect format, nonfatal error
                fprintf(stderr, "reading %s: incorrect format\n", *argv);
                return_status = 1;
                continue;
            }

            // get 2 phones from line, build edge on them
            char phone1[13], phone2[13];
            sscanf(buf, "%s %s", phone1, phone2);
            if (addEdge(graph, phone1, phone2)==-1) {
                fprintf(stderr, "reading %s: fail to add (%s,%s) call\n", 
                        *argv, phone1, phone2);
                return_status = 1;    // nonfatal error
            }
        }
        fclose(fp);
    }

    // fatal error -- no input files opened
    if (!at_least_one_opened) {
        fprintf(stderr, "No input file opened\n");
        removeGraph(graph);
        exit(1);
    }


    // now read stdin
    char buf[200];
    while (fgets(buf, sizeof buf, stdin) != NULL) {
        trim(buf);
        if (*buf=='\0')    // skip empty
            continue;
        if (check_string(buf) == -1) {    // incorrect phone format
            fprintf(stderr, "incorrect format\n");
            return_status = 1;    // nonfatal error
            continue;
        }

        // get 2 phones to analyze
        char phone1[13], phone2[13];
        sscanf(buf, "%s %s", phone1, phone2);
        
        // try to get direct connection 
        int nTalk = talkedTimes(graph, phone1, phone2);
        if (nTalk == -1) {    // no such phones
            fprintf(stderr, "One/both do not exist: %s, %s\n", phone1, phone2);
            return_status = 1;    // nonfatal error
        } else if (nTalk == 0) {    // nondirected -> use BFS
            int nConnected = BFS(graph, phone1, phone2);
            switch (nConnected) {
            case -2:    // fatal error (memory error), but consider notfatal as not tested
                fprintf(stderr, "Error determining connected number: %s, %s\n", phone1, phone2);
                return_status = 1;
                break;
            case -1:    // dijoint
                printf("Not connected\n");
                break;
            case 0:    // same -- nonfatal error
                fprintf(stderr, "Phones are same: %s, %s\n", phone1, phone2);
                return_status = 1;
                break;
            default:    // normal case for indirect, print n-1
                printf("Connected through %d numbers\n", nConnected-1);
            }
        } else        // directed -- print the result
            printf("Talked %d times\n", nTalk);
    }
         

    removeGraph(graph);
    exit(return_status);
}
