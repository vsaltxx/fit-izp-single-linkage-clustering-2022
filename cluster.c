/**
 * xsalta01
 * Veranika Saltanava
 *
 * Kostra programu pro 2. projekt IZP 2022/23
 *
 * Jednoducha shlukova analyza: 2D nejblizsi soused.
 * Single linkage
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h> // sqrtf
#include <limits.h> // INT_MAX
#include <string.h>

/*****************************************************************
 * Ladici makra. Vypnout jejich efekt lze definici makra
 * NDEBUG, napr.:
 *   a) pri prekladu argumentem prekladaci -DNDEBUG
 *   b) v souboru (na radek pred #include <assert.h>
 *      #define NDEBUG
 */
#ifdef NDEBUG
#define debug(s)
#define dfmt(s, ...)
#define dint(i)
#define dfloat(f)
#else

// vypise ladici retezec
#define debug(s) printf("- %s\n", s)

// vypise formatovany ladici vystup - pouziti podobne jako printf
#define dfmt(s, ...) printf(" - "__FILE__":%u: "s"\n",__LINE__,__VA_ARGS__)

// vypise ladici informaci o promenne - pouziti dint(identifikator_promenne)
#define dint(i) printf(" - " __FILE__ ":%u: " #i " = %d\n", __LINE__, i)

// vypise ladici informaci o promenne typu float - pouziti
// dfloat(identifikator_promenne)
#define dfloat(f) printf(" - " __FILE__ ":%u: " #f " = %g\n", __LINE__, f)

#endif

/*****************************************************************
 * Deklarace potrebnych datovych typu:
 *
 * TYTO DEKLARACE NEMENTE
 *
 *   struct obj_t - struktura objektu: identifikator a souradnice
 *   struct cluster_t - shluk objektu:
 *      pocet objektu ve shluku,
 *      kapacita shluku (pocet objektu, pro ktere je rezervovano
 *          misto v poli),
 *      ukazatel na pole shluku.
 */

struct obj_t {
    int id;
    float x;
    float y;
};

struct cluster_t {
    int size;
    int capacity;
    struct obj_t *obj;
};

/*****************************************************************
 * Deklarace potrebnych funkci.
 *
 * PROTOTYPY FUNKCI NEMENTE
 *
 * IMPLEMENTUJTE POUZE FUNKCE NA MISTECH OZNACENYCH 'TODO'
 *
 */

/*
 Inicializace shluku 'c'. Alokuje pamet pro cap objektu (kapacitu).
 Ukazatel NULL u pole objektu znamena kapacitu 0.
*/
void init_cluster(struct cluster_t *c, int cap)
{
    assert(c != NULL);
    assert(cap >= 0);

    c->size = 0;
    //initially the cluster size is 1
    c->capacity = cap;

    if (cap != 0)
        c->obj = malloc(cap * sizeof(struct obj_t));

}

/*
 Odstraneni vsech objektu shluku a inicializace na prazdny shluk.
 */
void clear_cluster(struct cluster_t *c)
{
    free(c->obj);
    c->size = 0;
    c->capacity = 0;
}

/// Chunk of cluster objects. Value recommended for reallocation.
const int CLUSTER_CHUNK = 10;

/*
 Zmena kapacity shluku 'c' na kapacitu 'new_cap'.
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap)
{
    // TUTO FUNKCI NEMENTE
    assert(c);
    assert(c->capacity >= 0);
    assert(new_cap >= 0);

    if (c->capacity >= new_cap)
        return c;

    size_t size = sizeof(struct obj_t) * new_cap;

    void *arr = realloc(c->obj, size);
    if (arr == NULL)
        return NULL;

    c->obj = (struct obj_t*)arr;
    c->capacity = new_cap;
    return c;
}

/*
 Prida objekt 'obj' na konec shluku 'c'. Rozsiri shluk, pokud se do nej objekt
 nevejde.
 */
void append_cluster(struct cluster_t *c, struct obj_t obj)
{
    assert(c != NULL);
    if (c->size >= c->capacity)
        c = resize_cluster(c, c->capacity + CLUSTER_CHUNK);

    c->obj[c->size] = obj;
    c->size++;
}

/*
 Seradi objekty ve shluku 'c' vzestupne podle jejich identifikacniho cisla.
 */
void sort_cluster(struct cluster_t *c);

/*
 Do shluku 'c1' prida objekty 'c2'. Shluk 'c1' bude v pripade nutnosti rozsiren.
 Objekty ve shluku 'c1' budou serazeny vzestupne podle identifikacniho cisla.
 Shluk 'c2' bude nezmenen.
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c2 != NULL);

    //add objects one by one
    for (int i = 0; i < c2->size; i++){
        append_cluster(c1,c2->obj[i]);
    }

    sort_cluster(c1);
}

/**********************************************************************/
/* Prace s polem shluku */

/*
 Odstrani shluk z pole shluku 'carr'. Pole shluku obsahuje 'narr' polozek
 (shluku). Shluk pro odstraneni se nachazi na indexu 'idx'. Funkce vraci novy
 pocet shluku v poli.
*/
int remove_cluster(struct cluster_t *carr, int narr, int idx)
{
    assert(idx < narr);
    assert(narr > 0);

    int new_arr = narr - 1;
    clear_cluster(&carr[idx]);
    for (int i = idx; i < new_arr; i++)
    {
        carr[i] = carr[i + 1];
    }

    return new_arr;
}

/*
 Pocita Euklidovskou vzdalenost mezi dvema objekty.
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2)
{
    assert(o1 != NULL);
    assert(o2 != NULL);

    //just a formula
    return sqrtf(pow(o1->x - o2->x, 2)
                 + pow(o1->y - o2->y, 2));
}

/*
 Pocita vzdalenost dvou shluku.
*/
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c1->size > 0);
    assert(c2 != NULL);
    assert(c2->size > 0);

    float dist_cl_min = INFINITY;
    float dist_cl;

    //the distance between each object from one cluster and each object
    //from another cluster, finding the minimum value
    for (int i = 0; i < c1->size; i++)
        for (int j = 0; j < c2->size; j++){

            dist_cl = obj_distance(&c1->obj[i],&c2->obj[j]);
            if (dist_cl_min > dist_cl) {
                dist_cl_min = dist_cl;
            }

        }
    //return the minimum distance between two clusters
    return dist_cl_min;
}

/*
 Funkce najde dva nejblizsi shluky. V poli shluku 'carr' o velikosti 'narr'
 hleda dva nejblizsi shluky. Nalezene shluky identifikuje jejich indexy v poli
 'carr'. Funkce nalezene shluky (indexy do pole 'carr') uklada do pameti na
 adresu 'c1' resp. 'c2'.
*/
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2)
{
    assert(narr > 0);

    float dist_min = INFINITY;
    float dist;

    for (int i = 0; i < narr; i++) {
        for (int j = i + 1; j < narr; j++) {
            //there can be several objects in a cluster
            dist = cluster_distance(&carr[i], &carr[j]);
            if (dist_min > dist) {
                dist_min = dist;
                *c1 = i;
                *c2 = j;
            }
        }
    }
}

// pomocna funkce pro razeni shluku
static int obj_sort_compar(const void *a, const void *b)
{
    // TUTO FUNKCI NEMENTE
    const struct obj_t *o1 = (const struct obj_t *)a;
    const struct obj_t *o2 = (const struct obj_t *)b;
    if (o1->id < o2->id) return -1;
    if (o1->id > o2->id) return 1;
    return 0;
}

/*
 Razeni objektu ve shluku vzestupne podle jejich identifikatoru.
*/
void sort_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    qsort(c->obj, c->size, sizeof(struct obj_t), &obj_sort_compar);
}

/*
 Tisk shluku 'c' na stdout.
*/
void print_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    for (int i = 0; i < c->size; i++)
    {
        if (i) putchar(' ');
        printf("%d[%g,%g]", c->obj[i].id, c->obj[i].x, c->obj[i].y);
    }
    putchar('\n');
}


/*
 Ze souboru 'filename' nacte objekty. Pro kazdy objekt vytvori shluk a ulozi
 jej do pole shluku. Alokuje prostor pro pole vsech shluku a ukazatel na prvni
 polozku pole (ukalazatel na prvni shluk v alokovanem poli) ulozi do pameti,
 kam se odkazuje parametr 'arr'. Funkce vraci pocet nactenych objektu (shluku).
 V pripade nejake chyby uklada do pameti, kam se odkazuje 'arr', hodnotu NULL.
*/
int load_clusters(char *filename, struct cluster_t **arr)
{
    assert(arr != NULL);

    FILE *file = fopen(filename, "r");

    //in case of file opening mistake
    if (file == NULL)
        return -3;

    //I read the first line from the file. T
    // The line contains the total number of objects to be read
    int size;
    if (fscanf(file, "count=%d", &size) != 1){
         printf("Error: incorrect file opening");
     }

    //allocating space for the array -> size the number of objects
    *arr = malloc(size * sizeof(struct cluster_t));

    for (int i = 0; i < size; i++)
    {
        //Initialization of structures - elements of the cluster array before saving the data there
        init_cluster(&(*arr)[i], 1);
        struct obj_t obj;
        //I read the data from the file line by line
        int check = fscanf(file, "%d %f %f", &obj.id, &obj.x, &obj.y);
        //check that the object parameters are numbers of a certain format
        if (check != 3) {
            return -2;
        }

        //check that the object parameters are integers
        if (!((obj.id == (int)obj.id) && (obj.x == (int)obj.x) && (obj.y == (int)obj.y))) {
            for (int error = 0; error <= i; error++){
                clear_cluster(&(*arr)[error]);
            }
            size = -2;
            break;
        }

        //check the object parameters for a range of values
        if (obj.x < 0 || obj.x > 1000 || obj.y < 0 || obj.y > 1000) {
            for (int error = 0; error < i; error++) {
                //in the case of an error, free the memory allocated to the
                // already saved objects and terminate the program
                clear_cluster(&(*arr)[i]);
            }
            size = -2;
        }
        //I add objects one by one to the cluster
        append_cluster(&(*arr)[i], obj);
    }
    //I close the file after reading all the objects
    fclose(file);

    return size;
}

/*
 Tisk pole shluku. Parametr 'carr' je ukazatel na prvni polozku (shluk).
 Tiskne se prvnich 'narr' shluku.
*/
void print_clusters(struct cluster_t *carr, int narr)
{
    printf("Clusters:\n");
    for (int i = 0; i < narr; i++)
    {
        printf("cluster %d: ", i);
        print_cluster(&carr[i]);
    }
}

//a separate function to check and process the arguments
int parse_args(int argc, char *argv[], char **filename, int *N)
{
    *filename = argv[1];

    if (argc == 2) {
        *N = 1; //from the task condition
    }
    else if (argc < 2 || argc > 3){
        return -1; //
    }
    // Three arguments must be set in order for the program to work correctly
    else //if (argc == 3)
    {
        //arguments set on the command line are originally of type string
        //convert it into integer
        *N = atoi(argv[2]);

        //integer check
        float f_N = atof(argv[2]);
        if (f_N != *N){
            return -1;
        }
        //negative number check
        if (*N < 0)
            return -1;
        //zero check
        if (*N == 0){
            return -2;
        }

    }

    return 0;
}


int main(int argc, char *argv[])
{
    struct cluster_t *clusters;

    char *filename;
    int N;
    int check = parse_args(argc, argv, &filename, &N);

    //check the given arguments, in case of an error the program is terminated
    if (check == -1){
        fprintf(stderr, "Error: Invalid arguments.\n");
        return 1;
    }
    else if (check == -2){
        fprintf(stderr, "Zero clusters.\n");
        return 1;
    }



    int size = load_clusters(filename, &clusters);

    if (size == -1) {
        fprintf(stderr, "Error: Invalid arguments.\n");
        return 1;
    }
    else if (size == -2) {
        fprintf(stderr, "Error: Incorrect object parameters.\n");
        free(clusters);
        return 1;
    }
    else if (size == -3){
        fprintf(stderr, "Error: file opening mistake\n");
        return 1;
    }

    int c1, c2;
    while (size > N)
    {
        find_neighbours(clusters, size, &c1, &c2);
        merge_clusters(&clusters[c1], &clusters[c2]);
        remove_cluster(clusters, size, c2);

        size--;
    }

    print_clusters(clusters, size);

    //release the memory of the obj structure elements in each cluster structure element
    for (int i = 0; i < size; i++) {
        free(clusters[i].obj);
    }

    //free up space allocated for the array of clusters itself
    free(clusters);
}

