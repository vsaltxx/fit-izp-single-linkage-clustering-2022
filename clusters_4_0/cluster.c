/**
 * xsalta01
 * Veranika Saltanava
 *
 * Programme skeleton for the 2nd IZP project 2022/23 
 *
 * Simple cluster analysis: 2D nearest neighbour.
 * Single linkage
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h> // sqrtf
#include <limits.h> // INT_MAX
#include <string.h>

/*****************************************************************
 * Debugging macro. You can turn off their effect by defining the macro
 * NDEBUG, for ex.:
 *   a) in translation by the argument of the translator -DNDEBUG
 *   b) in the file (on the line before #include <assert.h>
 *      #define NDEBUG
 */
#ifdef NDEBUG
#define debug(s)
#define dfmt(s, ...)
#define dint(i)
#define dfloat(f)
#else

// prints the debug chain
#define debug(s) printf("- %s\n", s)

// prints formatted debug output - use similar to printf 
#define dfmt(s, ...) printf(" - "__FILE__":%u: "s"\n",__LINE__,__VA_ARGS__)

// prints debugging information about the variable - use dint(identifier_variable)
#define dint(i) printf(" - " __FILE__ ":%u: " #i " = %d\n", __LINE__, i)

// prints debugging information about variable of float type - usage
// dfloat(identifier_variable)
#define dfloat(f) printf(" - " __FILE__ ":%u: " #f " = %g\n", __LINE__, f)

#endif

/*****************************************************************
 * Declaration of the necessary data types:
 *
 * DON'T CHANGE THIS DECLARATION
 *
 *   struct obj_t - object structure: identifier and coordinates
 *   struct cluster_t - a cluster of objects:
 *       number of objects in the cluster,
 *       the capacity of the cluster (the number of objects for which it is reserved
 *       space in the array),
 *       pointer to the array of the cluster.
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
 * Declaration of the necessary functions.
 *
 * FUNCTION PROTOTYPES ARE NOT REPLACED
 *
 * ONLY IMPLEMENT FUNCTIONS IN PLACES MARKED 'TODO'.
 *
 */

/*
 Initialization of cluster 'c'. Allocates memory for the object cap (capacity).
 The NULL pointer to the object array indicates a capacity of 0.
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
 Remove all cluster objects and initialize to an empty cluster.
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
 Change the capacity of cluster 'c' to the capacity of 'new_cap'.
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap)
{
  // DO NOT CHANGE THIS FUNCTION

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
 Adds object 'obj' to the end of the 'c' stack. Expand the heap if the object
 does not fit.
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
 Serializes the objects in cluster 'c' in ascending order by their identification number.
 */
void sort_cluster(struct cluster_t *c);

/*
 Adds the 'c2' objects to the 'c1' cluster. The 'c1' cluster will be expanded if necessary.
 The objects in the 'c1' cluster will be sorted in ascending order by the identification number.
 Cluster 'c2' will be unchanged.
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
/* Working with a cluster array */

/*
 Remove a cluster from the 'carr' cluster field. The cluster array contains 'narr' entries
 (cluster). The cluster to remove is located at index 'idx'. The function returns a new
 number of clumps in the array.
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
 It calculates the Euclidean distance between two objects.
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
 It calculates the distance between  two clusters.
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
        for (int j = 0; j < c2->size; j++){ //???

            dist_cl = obj_distance(&c1->obj[i],&c2->obj[j]);
            if (dist_cl_min > dist_cl) {
                dist_cl_min = dist_cl;
            }

        }
    //return the minimum distance between two clusters
    return dist_cl_min;
}

/*
 The function finds the two closest clusters. In a cluster array 'carr' of size 'narr'
 searches for the two nearest clusters. It identifies the found clusters by their indices in the array
 'carr'. The function stores the found clusters (indices in the 'carr' array) in memory for
 address 'c1' and 'c2' respectively.
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

// auxiliary function for cluster sort
static int obj_sort_compar(const void *a, const void *b)
{
    // DON'T CHANGE THIS FUNCTION
    const struct obj_t *o1 = (const struct obj_t *)a;
    const struct obj_t *o2 = (const struct obj_t *)b;
    if (o1->id < o2->id) return -1;
    if (o1->id > o2->id) return 1;
    return 0;
}

/*
 Sort the objects in the cluster in ascending order according to their identifier.
*/
void sort_cluster(struct cluster_t *c)
{
    // DON'T CHANGE THIS FUNCTION
    qsort(c->obj, c->size, sizeof(struct obj_t), &obj_sort_compar);
}

/*
 Print cluster 'c' to stdout.
*/
void print_cluster(struct cluster_t *c)
{
    // DON'T CHANGE THIS FUNCTION
    for (int i = 0; i < c->size; i++)
    {
        if (i) putchar(' ');
        printf("%d[%g,%g]", c->obj[i].id, c->obj[i].x, c->obj[i].y);
    }
    putchar('\n');
}


/*
 Retrieves objects from the file 'filename'. For each object it creates
 a cluster and saves it into the cluster array. Allocates space for the 
 array of all clusters and a pointer to the first position of the array 
 (the pointer to the first cluster in the allocated array) is stored in 
 memory, where the parameter 'arr' is referenced. The function returns 
 the number of loaded objects (clusters). In case of an error, it stores 
 a NULL value in the memory referenced by 'arr'.
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
 Printing the cluster field. The parameter 'carr' is a pointer to the first item (cluster).
 The first 'narr' of the cluster is printed.
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

    //free up space allocated for the array of casters itself
    free(clusters);
}

