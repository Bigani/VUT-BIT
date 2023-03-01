/**
 * Kostra programu pro 3. projekt IZP 2018/19
 *
 * Jednoducha shlukova analyza: 2D nejblizsi soused.
 * Single linkage
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h> // sqrtf
#include <limits.h> // INT_MAX

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

#define MAXD 1500.0 //presahuje maximalnu moznu vzdialenost voci porovnavanim

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
    if (cap > 0)
    {
        if ((c->obj = (struct obj_t*) malloc(sizeof(struct obj_t) * cap)))
        {
            c->capacity = cap;
            return;
        }
    }

    c->capacity = 0; //pripad prazdneho zhluku alebo nezdarenia mallocu
    c->obj = NULL;

}

/*
 Odstraneni vsech objektu shluku a inicializace na prazdny shluk.
 */
void clear_cluster(struct cluster_t *c)
{
    free(c->obj);
    init_cluster(c, 0);
}

/*
 * Odstranenie vsetkych zhlukov s ich objektami (++prehladnost)
*/
void clear_every_cluster(struct cluster_t *carr, int pocet)
{
    for (int i = 0; i < pocet; i++)
        clear_cluster(&carr[i]);

    free(carr);
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
    if (c->capacity <= c->size)
    {
        resize_cluster(c, c->size+CLUSTER_CHUNK);
    }

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




    for (int i = 0; i < c2->size; i++)
    {
        append_cluster(c1, c2->obj[i]);
    }

    sort_cluster(c1);

}
void print_cluster(struct cluster_t *c);
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

    // odstrani zhluk na indexu idx,
    // zmensi pocet poloziek v zhluku
    // a presunie zlozky na vyplnenie prazdneho miesta
    clear_cluster(&carr[idx]);

    int decreased_narr = narr - 1;
    for (int i = idx; i < decreased_narr; i++) {
        carr[i] = carr[i + 1];
    }

    return decreased_narr;
}

/*
 Pocita Euklidovskou vzdalenost mezi dvema objekty.
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2)
{
    assert(o1 != NULL);
    assert(o2 != NULL);

    //Dlzku vypocita ako c×c = root of (a×a + b×b) podla Pytagorovej vety

    float length;
    float X,Y;
    X = powf(o1->x - o2->x,2);
    Y = powf(o1->y - o2->y,2);
    length = sqrtf(X+Y);

    return length;
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


    // porovna navzajom vsetky body,
    // ak najde kratsiu vzdialenost nez MAXD tak ju ulozi
    float distance = MAXD, new_distance;

    for (int i = 0; i < c1->size; i++)
        for (int j = 0; j < c2->size; j++)
        {
            new_distance = obj_distance(&c1->obj[i], &c2->obj[j]);
            if (new_distance < distance)
                distance = new_distance;
        }

    return (distance != MAXD) ? distance : 0;
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

    //pre 1 zhluk
    if (narr == 1)
    {
        *c1 = 0;
        *c2 = 0;
        return;
    }

    float distance = MAXD, new_distance;

    for (int i = 0; i < narr - 1; i++)
    {
        for (int j = i + 1; j < narr; j++)
        {
            new_distance = cluster_distance(&carr[i], &carr[j]);
            if (new_distance < distance)
            {
                distance = new_distance;
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

    FILE *file = fopen(filename,"r");
    if (file == NULL)
    {
        fprintf(stderr,"Subor sa nepodarilo otvorit\n");
        return -1;
    }

    int line_count = 0, object_num = 0;
    int obj_ID, obj_X, obj_Y;
    char line[30]; //Riadok so suradnicami by nemal presahovat 30 znakov

    struct obj_t obj;
    struct cluster_t *cluster;
    while (fgets(line,30,file))
    {
        line_count++;
        if(line_count == 1)
        {
            //kontrola udajov v txt

            if (sscanf(line,"count=%d^\n", &object_num) != 1)
            {
                fprintf(stderr,"Nespravny zapis na prvom riadku\n");
                fclose(file);
                return -1;
            }

            //kontrola N na prvom riadku
            if (object_num <= 0)
            {
                fprintf(stderr,"Count = N nesmie byt mensia nez 0\n");
                fclose(file);
                return -1;
            }

            //alokuje miesto pre pole pointerov
            *arr = malloc(sizeof(struct cluster_t) * object_num);

            if (*arr == NULL)
            {
                fprintf(stderr, "Chyba alokacie pamate\n");
                return -1;
            }

            for (int i = 0; i < object_num; i++) {
                init_cluster(&(*arr)[i],0);

                if (&(*arr)[i] == NULL)
                {
                    fprintf(stderr,"Chyba alokacie pamate\n");
                    fclose(file);
                    return -1;
                }
            }
            continue;
        }

        //pozrie ci sme nacitali ziadany pocet riadkov
        if (line_count > object_num + 1)
        {
            break;
        }


        //kontrola suradnic
        if (sscanf(line,"%d %d %d",&obj_ID,&obj_X,&obj_Y) != 3)
        {
            fprintf(stderr,"Chyba zapisu riadka\n");

            //cyklicky vycisti dosud alokovane zhluky
            clear_every_cluster(*arr, object_num);
            *arr = NULL;

            fclose(file);
            return -1;
        }

        if (obj_X < 0 || obj_X > 1000 || obj_Y < 0 || obj_Y > 1000)
        {
            fprintf(stderr,"Chyba rozmedzia suradnice bodu na riadku: %d\n",line_count);

            //cyklicky vycisti dosud alokovane zhluky
            clear_every_cluster(*arr, object_num);
            *arr = NULL;

            fclose(file);
            return -1;
        }

        obj.id = obj_ID;
        obj.x = obj_X;
        obj.y = obj_Y;

        cluster = &(*arr)[line_count - 2]; //oznacenie zhlukov zacina od 0
        append_cluster(cluster, obj);      //priradi zhluku objekt

        if (cluster->size != 1) {
            fprintf(stderr,"Chyba alokacie pamate\n");

            clear_every_cluster(*arr, object_num);
            *arr = NULL;

            fclose(file);
            return -1;
        }

    }

    fclose(file);
    return object_num;
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

int main(int argc, char *argv[])
{
    //kontrola poctu arg
    if (argc < 1 || argc > 3)
    {
        fprintf(stderr,"Chybny pocet argumentov\n");
        return -1;
    }

    struct cluster_t *clusters;
    int size = load_clusters(argv[1], &clusters);
        if(size == -1)
        {
            return -1;
        }


    int cluster_sets = 1; //pozadovany pocet vyslednych zhlukov
    if (argc == 3)
        if (sscanf(argv[2],"%d",&cluster_sets) != 1)
        {
            fprintf(stderr, "Chybne zadane [N]\n");
            return -1;
        }

    if ((size < cluster_sets) || (cluster_sets <= 0))
    {
        fprintf(stderr,"Ziadany pocet zhlukov nevyhovuje poctu objektov v subore\n");
        return -1;
    }

    for (int i = 0; i < size - 1; i++)
        for (int j = i + 1; j < size; j++)
        {
            if (clusters[i].obj->id == clusters[j].obj->id )
            {
                fprintf(stderr,"Chybne zadanie objektov\n");
                for (int i = 0; i < size; i++)
                {
                    clear_cluster(&clusters[i]);
                }
                free(clusters);
                return -1;
            }
        }


    int c1,c2,new_size;

    //usporiadanie zhlukov
    while (size > cluster_sets)
    {
        find_neighbours(clusters,size,&c1,&c2);
        new_size = clusters[c1].size;
        merge_clusters(&clusters[c1], &clusters[c2]);

        if (clusters[c2].size > 0 && clusters[c1].size != new_size + clusters[c2].size) {
            fprintf(stderr,"Chyba alokace pameti\n");
            return -1;
        }
        size = remove_cluster(clusters,size,c2);
    }

    print_clusters(clusters,size);

    for (int i = 0; i < size; i++)
    {
        clear_cluster(&clusters[i]);
    }
    free(clusters);

    return 0;
}