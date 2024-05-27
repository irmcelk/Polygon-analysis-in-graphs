#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_VERTICES 26  // Maximum 26 karakterle etiket verilebilir

/* Bu projede aðýrlýklý graflarda çokgen analizi yapýlmýþtýr. Dosyadan okunan adjaceny listeleri ile oluþturulan graflar DFS algoritmasý kullanýlarak içlerinde
cycle var mý kontrol edilmiþ, cycle bulunmasý durumunda cyclelar kend iclerinde alfabetik olarak sýralanarak bir dizide tutulmustur. Bulunan çokgenlerin 
uzunluklarý hesaplanmýþ ve yazdýrýlarak kullanýcýya bilgi verilmiþtir.
*/

//@brief Node yapisi
struct node {
    char vertex; //node adý
    int weight; //agýrlýk
    struct node* next;
};

//@brief Graf yapisi
struct Graph {
    int numVertices; //dugum sayisi
    struct node** adjLists; //komsuluk listesi
    int* visited; //dfs icin olusturulan liste
};

//@brief Cycle yapisi
struct Cycle {
    int length; // cycle uzunlugu
    int edge; // cycle kenar sayisi
    char* path; // cycleý olusturan dugumler
    struct Cycle* next;
};

/*@brief Node olusturma
@param1 Node adý
@param2 Agýrlýk
@return return olusturulan node
*/
struct node* createNode(char v, int weight) {
    struct node* newNode = malloc(sizeof(struct node)); //yer ayrimi
    if (newNode == NULL) {
        fprintf(stderr, "Error allocating memory for new node.\n");
        exit(EXIT_FAILURE);
    }
    newNode->vertex = v;
    newNode->weight = weight;
    newNode->next = NULL;
    return newNode;
}

/*@brief Graf olusturma
@param1 dugum sayisi
@return return olusturulan graf
*/
struct Graph* createAGraph(int vertices) {
    int i;
    struct Graph* graph = malloc(sizeof(struct Graph));//Yer ayrimi
    if (graph == NULL) {
        fprintf(stderr, "Error allocating memory for new graph.\n");
        exit(EXIT_FAILURE);
    }
    graph->numVertices = vertices;
    
    graph->adjLists = malloc(vertices * sizeof(struct node*));
    if (graph->adjLists == NULL) {
        fprintf(stderr, "Error allocating memory for new list.\n");
        exit(EXIT_FAILURE);
    }
    graph->visited = malloc(vertices * sizeof(int));
    if (graph->visited == NULL) {
        fprintf(stderr, "Error allocating memory for new list.\n");
        exit(EXIT_FAILURE);
    }
    for (i = 0; i < vertices; i++) {
        graph->adjLists[i] = NULL; //listenin tum elemanlarini NULL yapma
        graph->visited[i] = 0; //listenin tum elemanlarini sifirlama
    }

    return graph;
}

/*@brief Node adýný inte donusturme
@param1 Node adý
@return return int olarak char degeri
*/
int charToIndex(char vertex) {
    return vertex - 'A'; // Dugum isimlerini 'A' ile baþlatýyoruz, ASCII tablosunda 'A' 65'e esit
}

/*@brief Cycle olusturma
@param1 Cycle uzunlugu
@param2 Cycle kenar sayisi
@param3 Cycleý olusturan dugumler
@return return olusturulan Cycle
*/
struct Cycle* createACycle(int length, int edge, const char* path) {
    struct Cycle* newCycle = (struct Cycle*)malloc(sizeof(struct Cycle));
    if (newCycle == NULL) {
        fprintf(stderr, "Error allocating memory for new cycle.\n");
        exit(EXIT_FAILURE);
    }

    newCycle->length = length;
    newCycle->edge = edge;

    newCycle->path = (char*)malloc((strlen(path) + 1) * sizeof(char));
    if (newCycle->path == NULL) {
        fprintf(stderr, "Error allocating memory for cycle path.\n");
        free(newCycle);
        exit(EXIT_FAILURE);
    }
    strcpy(newCycle->path, path);//pathi cycleýn pathine kopyalama
    newCycle->next = NULL;

    return newCycle;
}

/*@brief Cycle daha once olusturulmus mu kontrolu
@param1 Cycle listesi
@param2 Cycleý olusturan dugumler
@return return daha once varsa 1 ,yoksa 0
*/
int cycleExists(struct Cycle* head, const char* path) {
    struct Cycle* current = head;
    while (current != NULL) {
        if (strcmp(current->path, path) == 0) {
            return 1;
        }
        current = current->next;
    }
    return 0;
}

/*@brief Linked listeye cycle ekleme
@param1 Cycle listesi
@param2 Cycle uzunlugu
@param3 Cycle kenar sayisi
@param4 Cycleý olusturan dugumler
*/
void addCycle(struct Cycle** head, int length, int edge, const char* path) {
    // Cycle daha once eklenmis mi
    if (cycleExists(*head, path)) {
        return;
    }
    //daha once eklenmemisse listeye ekleme
    struct Cycle* newCycle = createACycle(length, edge, path);
    newCycle->next = *head;
    *head = newCycle;
}

/*@brief Cyclelarý kenar uzunluklarýna gore sýralama
@param1 Cycle listesi
*/
void sortCyclesByEdgeCount(struct Cycle** head) {
    if (*head == NULL || (*head)->next == NULL)
        return;//dizi bossa
        
    struct Cycle *sorted = NULL;
    struct Cycle *current = *head;
    
    while (current != NULL) {
        struct Cycle *next = current->next;
        if (sorted == NULL || sorted->edge >= current->edge) {
            current->next = sorted;
            sorted = current;
        } else {
            struct Cycle *temp = sorted;
            while (temp->next != NULL && temp->next->edge < current->edge) {
                temp = temp->next;
            }
            current->next = temp->next;
            temp->next = current;
        }
        current = next;
    }
    *head = sorted;
}

/*@brief Linked listedeki cyclelari yazdirma
@param1 Cycle listesi
*/
void printCycles(struct Cycle* head) {
    struct Cycle* current = head;
    int index = 1;
    while (current != NULL) {
        printf("Cycle %d: %d kenar, uzunluk: %d,  %s\n",
               index++, current->edge, current->length, current->path);
        current = current->next;
    }
    printf("Sekil sayisi=%d\n", index - 1);
}

/*@brief Linked listedeki cycle türlerini ve sayýlarýný yazdirma
@param1 Cycle listesi
*/
void printCycleCounts(struct Cycle* head) {
    int triangleCount = 0, quadrilateralCount = 0, pentagonCount = 0;
	int moreThan5=0; //5 ten fazla kenari olan cokgen
    struct Cycle* current = head;
    while (current != NULL) {
    	if(current->edge==3){
    		triangleCount++;
		}
		else if(current->edge==4){
			quadrilateralCount++;
		}
        else if(current->edge==5){
        	 pentagonCount++;
		 }
		 else{
		 	moreThan5++;
		 }
        current = current->next;
    }
    printf("3'gen sayisi: %d\n", triangleCount);
    printf("4'gen sayisi: %d\n", quadrilateralCount);
    printf("5'gen sayisi: %d\n", pentagonCount);
    printf("5 ten fazla kenari olan cokgen sayisi: %d\n", moreThan5);
}

/*@brief Linked listeyi free etme
@param1 Cycle listesinin headi
*/
void freeCycles(struct Cycle* head) {
    struct Cycle* current = head;
    while (current != NULL) {
        struct Cycle* next = current->next;
        free(current->path);
        free(current);
        current = next;
    }
}

/*@brief Grafa egde ekleme
@param1 Graf
@param2 Source olan node adý
@param3 Destiantion olan node adý
@param4 Nodelar arasýndaki agýrlýk
*/
void addEdge(struct Graph* graph, char s, char d, int weight) {
    int sIndex = charToIndex(s);
    int dIndex = charToIndex(d);

    // s den d ye edge ekleme
    struct node* newNode = createNode(d, weight);
    newNode->next = graph->adjLists[sIndex];
    graph->adjLists[sIndex] = newNode;
    graph->adjLists[sIndex]->weight=weight;

    //d den s ye edge ekleme
    newNode = createNode(s, weight);
    newNode->next = graph->adjLists[dIndex];
    graph->adjLists[dIndex] = newNode;
    graph->adjLists[dIndex]->weight=weight;
}

/*@brief Stringi alfabetik olarak sýralama
@param1 sýralanacak string
*/
void sortString(char* str) {
    int len = strlen(str);
    int i, j;
    for (i = 0; i < len - 1; i++) {
        for (j = i + 1; j < len; j++) {
            if (str[i] > str[j]) {
                char temp = str[i];
                str[i] = str[j];
                str[j] = temp;
            }
        }
    }
}

/*@brief Graftaki cyclelarý bulan dfs algoritmasý
@param1 Graf
@param2 Ziyaret edilen dugum
@param3 parent dugum
@param4 cycle
@param5 cycle olusturan dugumler
@param6 cycle uzunlugu
@param7 cycle var mý kontrol eden degisken
*/
void DFS(struct Graph* graph, int vertex, int* parent, struct Cycle** cycles, char* path, int pathLen, int* hasCycle) {
    graph->visited[vertex] = 1; //visited 1 
    path[pathLen++] = 'A' + vertex; //yeni dugumun etiketi listeye eklenir
    path[pathLen] = '\0'; //dizinin sonuna NULL karakter eklenir

    struct node* adjList = graph->adjLists[vertex];
    while (adjList != NULL) {
        int connectedVertex = charToIndex(adjList->vertex); //mevcut dugumun indeksi

        if (!graph->visited[connectedVertex]) //dugum daha once ziyaret edilmemisse
		{
            parent[connectedVertex] = vertex; //parent dizisine dugumun parenti kaydedilir
            DFS(graph, connectedVertex, parent, cycles, path, pathLen, hasCycle);
        }
		  //Eger connectedVertex daha once ziyaret edilmisse ve bu dugum, mevcut dugumun parenti degilse
		 else if (connectedVertex != parent[vertex])
		 {
            *hasCycle = 1;//cycle bulundu
            int cycleLength = 0;
            int edgeCount = 0;
            char cyclePath[MAX_VERTICES * 2] = "";
            int start = 0;
            int i;
            while (path[start] != 'A' + connectedVertex) start++;
            for (i = start; i < pathLen; i++) {
                if (i > start) {
                    struct node* temp = graph->adjLists[charToIndex(path[i-1])];
                    while (temp && temp->vertex != path[i]) 
					temp = temp->next;
                    if (temp) 
					cycleLength += temp->weight;
                }
                edgeCount++; //kenar sayisi arttirir
                strncat(cyclePath, &path[i], 1);
            }

            // Son edge aðýrlýðýný ekleme
            struct node* temp = graph->adjLists[charToIndex(path[pathLen-1])];
            while (temp && temp->vertex != path[start]) 
			temp = temp->next;
            if (temp) 
			cycleLength += temp->weight;

            sortString(cyclePath);  // Alfabetik sekilde sirala
            addCycle(cycles, cycleLength, edgeCount, cyclePath); //cycle ý ekle
        }

        adjList = adjList->next;
    }
    graph->visited[vertex] = 0; // tum dugumlerin ziyaret edilme durumlarýný resetle
}

/*@brief Graftaki cycle var mi kontrolu
@param1 Graf
@param2 cycle
@return cycle var mi yok mu kontrol eden degisken
*/
int isCyclic(struct Graph* graph, struct Cycle** cycles) {
    int hasCycle = 0;
    int i;
    int* parent = malloc(graph->numVertices * sizeof(int)); //yer ayrimi
    if (parent == NULL) {
        fprintf(stderr, "Error allocating memory for new list.\n");
        exit(EXIT_FAILURE);
    }
    for (i = 0; i < graph->numVertices; i++) {
        parent[i] = -1;
    }

    char path[MAX_VERTICES * 2] = "";
    for (i = 0; i < graph->numVertices; i++) {
        if (!graph->visited[i]) {
            DFS(graph, i, parent, cycles, path, 0, &hasCycle);
        }
    }
    free(parent);
    return hasCycle;
}

/*@brief Grafý yazdýran fonksiyon
@param1 Graf
*/
void printGraph(struct Graph* graph) {
    int v;
    for (v = 0; v < graph->numVertices; v++) {
        if (graph->adjLists[v] != NULL) {
            struct node* temp = graph->adjLists[v];
            while (temp) {
                printf("%c %c (weight: %d) -> ", 'A' + v, temp->vertex, temp->weight);
                temp = temp->next;
            }
            printf("\n");
        }
    }
}

/*@brief Dosyadan komsuluk listesini okuyup graf olusturan fonksiyon
@param1 okunacak dosya adý
@return olusan graf
*/
struct Graph* readGraphFromFile(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Dosya acilamadi");
        exit(EXIT_FAILURE);
    }
    struct Graph* graph = createAGraph(MAX_VERTICES);
    char s, d;
    int weight;
    while (fscanf(file, " %c %c %d", &s, &d, &weight) == 3) {
        addEdge(graph, s, d, weight);
    }

    fclose(file);
    return graph;
}

int main() {
    int i;
    int v;
    const char* filenames[] = {"Sample.txt", "Sample2.txt", "Sample3.txt", "Sample4.txt","Sample5.txt"};
    int numFiles = sizeof(filenames) / sizeof(filenames[0]);
    for (i = 0; i < numFiles; i++) {
        printf("%s\n", filenames[i]);
        struct Cycle* head = NULL;
        struct Graph* graph = readGraphFromFile(filenames[i]);
        printGraph(graph);
        if (isCyclic(graph, &head)) {
            sortCyclesByEdgeCount(&head);
            printCycleCounts(head); // Cycle sayýlarý ve türlerini yazdýrma
            printCycles(head); // Tüm cycle'larý yazdýrma
        } else {
            printf("Grafta cycle yok\n");
        }
        freeCycles(head);

        // Free allocated memory
        for (v = 0; v < MAX_VERTICES; v++) {
            struct node* adjList = graph->adjLists[v];
            while (adjList != NULL) {
                struct node* temp = adjList;
                adjList = adjList->next;
                free(temp);
            }
        }
        free(graph->adjLists);
        free(graph->visited);
        free(graph);
        printf("--------------\n");
    }

    return 0;
}

