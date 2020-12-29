#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_MAX_LEN 64

struct link;

struct page
{
    char value[BUFFER_MAX_LEN];
    int visited;
    int numLinks;
    struct link *links;
};

struct link
{
    struct page* toPage;
    struct link * next;
};
  
struct graph
{
  int numVertices;
  struct page** adjList;  
};

struct page* createPage(char val[BUFFER_MAX_LEN])
{
  struct page* newPage = malloc(sizeof(struct page));
  strcpy(newPage->value, val);
  newPage->visited = 0;
  newPage->numLinks = 0;
  newPage->links = NULL;
  return newPage;  
}

/** inserts a page into the graph */
void addPage(struct graph* aGraph, char val[BUFFER_MAX_LEN])
{
    aGraph->numVertices++;
	aGraph->adjList = (struct page**)realloc(aGraph->adjList,
    	                (aGraph->numVertices) * sizeof(struct page*));

    aGraph->adjList[ aGraph->numVertices-1] = createPage(val);
}

/** inserts a link between the source page and target page. */
void addLink(struct graph* aGraph, char source[BUFFER_MAX_LEN],
        char target[BUFFER_MAX_LEN], int *status)
{ 
    size_t i = -1, sourcePos = -1, targetPos = -1;
    for (i=0; i < aGraph->numVertices; i++)
    {
        if (strcmp(aGraph->adjList[i]->value, source) == 0)
            sourcePos = i;
        
        if (strcmp(aGraph->adjList[i]->value, target) == 0)
            targetPos = i;
        
        if (targetPos != -1 && sourcePos != -1)
            break;
    }
    
    int tempStatus = *status;
    if (sourcePos == -1)
    {
        tempStatus = 1;
        status = &tempStatus;
        
        printf("Error. Specified source page does not exist.\n");
        return;
    }
    
    if (targetPos == -1)
    {
        tempStatus = 1;
        status = &tempStatus;
        
        printf("Error. Specified link-to page does not exist.\n");
        return;
    }
    
    struct page* current =  aGraph->adjList[sourcePos];
    if (current->links == NULL)
    {
        current->links = (struct link*)malloc(sizeof(struct link));
        current->links->toPage = aGraph->adjList[targetPos];
        current->links->next = NULL;   
    }
    else
    {

        while (current->links->next != NULL)
        {
            current->links = current->links->next;   
        }    
        current->links->next = (struct link*)malloc(sizeof(struct link));
        current->links->next->toPage = aGraph->adjList[targetPos];
        current->links->next->next = NULL;
    }    
 
}

void addPages(struct graph* aGraph, char line[BUFFER_MAX_LEN])
{
    size_t pos = 0;
    char page[BUFFER_MAX_LEN];
    while (sscanf(line+pos, "%s", page) != -1)
    {
        addPage(aGraph, page);
        pos += strlen(page)+1;
    }
}

void addLinks(struct graph* aGraph, char line[BUFFER_MAX_LEN], int *status)
{
    int tempStatus = *status;
    size_t pos = 0;
    char sourcePage[BUFFER_MAX_LEN];
    if (sscanf(line+pos, "%s", sourcePage) == -1)
    {
        printf("Error. Source page is not specified directive\n");
        tempStatus = 1;
        status = &tempStatus;
        return;
    }
    pos += strlen(sourcePage)+1;
    
    char page[BUFFER_MAX_LEN];    
    while (sscanf(line+pos, "%s", page) != -1)
    {
        addLink(aGraph, sourcePage, page, status);
        pos += strlen(page)+1;
    }

}

/* dfs(fromPage, toPage) -- returns true if there is a path from
    fromPage to toPage.
    To find whether there is a path from page A to page B:
    1) mark every page as "not visited"
    2) compute dfs(A, B)
*/
int dfs(struct page* fromPage, struct page* toPage) {
    if (fromPage->value == toPage->value)
        return 1;
    
    if (fromPage->visited == 1)
    {
        return 1;
    }
    
    fromPage->visited = 1;
    
    //for each page midPage linked to by fromPage do     
    struct link* currentLink = fromPage->links;
    while (currentLink != NULL) 
    {
        struct page* midPage = currentLink->toPage;
        if (dfs(midPage, toPage)) return 1;
        
        currentLink = currentLink->next; 
    }
    return 0;
}

int isConnected(struct graph* aGraph, char line[BUFFER_MAX_LEN], int *status)
{    
    int tempStatus = *status;
    size_t pos = 0;
    char toPage[BUFFER_MAX_LEN];
    char fromPage[BUFFER_MAX_LEN];
    
    if (sscanf(line+pos, "%s", fromPage) == -1)
    {
        printf("Error. Source page is not specified directive\n");
        tempStatus = 1;
        status = &tempStatus;
        return 0;
    }
    
    pos += strlen(fromPage)+1;
    
    if (sscanf(line+pos, "%s", toPage) == -1)
    {
        printf("Error. Link page is not specified directive\n");
        tempStatus = 1;
        status = &tempStatus;
        return 0;
    }    
    
    size_t i, fromPos = -1, toPos = -1;
    for (i=0; i < aGraph->numVertices; i++)
    {
        aGraph->adjList[i]->visited = 0;
        
        if (strcmp(aGraph->adjList[i]->value, fromPage) == 0)
            fromPos = i;

        if (strcmp(aGraph->adjList[i]->value, toPage) == 0)
            toPos = i;
    }


    if (fromPos == -1)
    {
        tempStatus = 1;
        status = &tempStatus;
        printf("Error. Source page is not specified directive\n");
        return 0;
    } 
    
    if (toPos == -1)
    {
        tempStatus = 1;
        status = &tempStatus;
        printf("Error. Link page is not specified directive\n");
        return 0;
    }      
        
   return dfs(aGraph->adjList[fromPos], aGraph->adjList[toPos]);
}

void destroy(struct graph* aGraph)
{
	int i;
	for (i=0; i < aGraph->numVertices; i++)
	{
		struct link *current = aGraph->adjList[i]->links;

		while (current != NULL)
		{
			struct link *temp = current;			
			current = current->next;
			free(temp);
			temp=NULL;
		}
		if (aGraph->adjList[i] != NULL)
		{
			free(aGraph->adjList[i]);
			aGraph->adjList[i] = NULL;
		}
	}
	free(aGraph->adjList);
	free(aGraph);
}

void readInput(FILE* fp, struct graph* aGraph, int *status)
{    
    int tempStatus = *status;

    ssize_t numCharsRead = 0;
    char *line = NULL;
    size_t lineSize = BUFFER_MAX_LEN;
  
    while ((numCharsRead = getline(&line, &lineSize, fp)) != -1)
    {
        if (fp == stdin && ( strcmp(line, "EOF\n") == 0 || strcmp(line, "eof\n") == 0) )
            break;

        if (numCharsRead < 9)
        {
          printf("Error. invalid directive\n");
          tempStatus = 1;
          continue;
        }
        
        char op[BUFFER_MAX_LEN];
        int numFilled = sscanf(line, "%s", op);
        if (numFilled < 1)
        {
          printf("Error. invalid directive\n");
          tempStatus = 1;
          continue;
        }
       
        if (strcmp(op, "@addPages") == 0)
            addPages(aGraph, line+strlen(op)+1);
        else if (strcmp(op, "@addLinks") == 0)
            addLinks(aGraph, line+strlen(op)+1, status);
        else if (strcmp(op, "@isConnected") == 0)
        {
            int pathExists = isConnected(aGraph, line+strlen(op)+1, status);
            printf("%d\n", pathExists);
        }
        else
        {
            tempStatus = 1;
            printf("Error. Invalid directive\n");
        }
		free(line);
        line = NULL;
    }
    
    status = &tempStatus;
    fclose(fp);
}


int main(int argc, char * argv[])
{
    int status = 0;
    FILE* fp = NULL;

    // if a command-line argument (name of a file) is specified  
    //  read input from the file. Otherwise read from stdin.
    if (argc > 1)
    {
        fp = fopen(argv[1], "r");
        if (fp == NULL)
        {
            fprintf(stderr, "Fatal error: input file does not exist.\n");
            exit(1);
        }
    }
    else
    {
        fp = stdin;
        printf("Enter your directive in the form operation args. Only three operations allowed:\n");
        printf("operation 1: @addPages Name_1 Name_2 . . . Name_n\n");
        printf("operation 2: @addLinks sourcePage Page_1 Page_2 . . . Page_n\n");
        printf("operation 3: @isConnected Page_1 Page_2\n");
        printf("Type EOF to mark the end of all directives.\n");
    }

    
    struct graph* aGraph = malloc(sizeof(struct graph));
    aGraph->numVertices = 0;
    
    readInput(fp, aGraph, &status);
    destroy(aGraph);

    return status;
}
