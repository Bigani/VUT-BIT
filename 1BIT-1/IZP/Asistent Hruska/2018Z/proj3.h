/**
 * @mainpage Dokumnetacia zdrojoveho kodu ku 3. projektu z IZP 2018/19
 *
 *
 * @brief Vypracoval: Tomas Moravcik (xmorav41)
 */


/**
 * @brief Struktura so suradnicami objektu
 */
struct obj_t {
    /// Unikatne ID
    int id;
    /// Suradnica X
    float x;
    /// Suradnica Y
    float y;
};

/**
 * @brief Struktura zhluku obsahujuca objekty
 */
struct cluster_t {
    ///Pocet objektov v zhluku
    int size;
    ///Pocet rezervovanych miest pre objekty
    int capacity;
    ///Ukazatel na pole objektov
    struct obj_t *obj;
};


/**
 * @defgroup fun1 Zakladne funkcie
 *
 * @{
 */

/**
 * Inicializace shluku 'c'. Alokuje pamet pro cap objektu (kapacitu).
 * Ukazatel NULL u pole objektu znamena kapacitu 0.
 *
 * @post Vytvori sa novo naalokovany zhluk
 *
 * @param c Pointer na strukturu
 * @param cap Ziadana kapacita
 */
void init_cluster(struct cluster_t *c, int cap);

/**
 * Odstraneni vsech objektu shluku a inicializace na prazdny shluk.
 *
 * @pre Existuje dosazitelny zhluk
 * @post Prazdny zhluk bez objektov
 *
 * @param c Pointer na strukturu
 */
void clear_cluster(struct cluster_t *c);

/**
 * Odstranenie dosud alokovanych clusterov, vid 'clear_cluster'
 *
 * @param carr Pointer pole pointerov clusterov
 * @param pocet Pocet alokovanych clusterov
 */
void clear_every_cluster(struct cluster_t *carr, int pocet);

/**
 * Hodnota vhodna pre realokaciu
 */
extern const int CLUSTER_CHUNK;

/**
 * Zmena kapacity shluku c na kapacitu new_cap. V pripade rovnej/mensej novej kapacity nerobi nic
 *
 * @pre Nova kapacita je rovna alebo vacsia nez stara
 * @post Zhluk s novou kapacitou
 *
 * @param c Pointer na strukturu
 * @param new_cap Nova kapacita
 * @return Struktura s novou kapacitou
 */

struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap);

/**
 * Prida objekt obj na konec shluku c. Rozsiri shluk, pokud se do nej objekt nevejde.
 *
 * @pre Objekt s platnymi datami
 * @post Cluster s novym objektom na jeho konci
 *
 * @param c Pointer na strukturu
 * @param obj Pridavany objekt
 */

void append_cluster(struct cluster_t *c, struct obj_t obj);

/**
 * Do shluku c1 prida objekty c2. Shluk c1 bude v pripade nutnosti rozsiren.
 * Objekty ve shluku c1 budou serazeny vzestupne podle identifikacniho cisla.
 * Shluk c2 bude nezmenen.
 *
 * @pre Platne pointre na zhluky
 * @post Prvy zhluk rozsireny o ten druhy
 *
 * @param c1 Pointer na prvy zhluk
 * @param c2 Pointer na druhy zhluk
 */

void merge_clusters(struct cluster_t *c1, struct cluster_t *c2);

/** @} */

/**
 * @defgroup fun2 Funkcie nad polom zhlukov
 *
 * @{
 */

/**
 * Odstrani shluk z pole shluku 'carr'. Pole shluku obsahuje 'narr' polozek
 *(shluku). Shluk pro odstraneni se nachazi na indexu 'idx'.
 *
 * @pre Platny pointer na pole
 * @post Pole o zhluk mensie
 *
 * @param carr Pointer na pole zhlukov
 * @param narr Pocet zhlukov v poli
 * @param idx Index zhluku na odstranenie
 * @return Pocet zhlukov v poli
 */

int remove_cluster(struct cluster_t *carr, int narr, int idx);

/**
 * Pocita Euklidovskou vzdalenost mezi dvema objekty.
 *
 * @pre Platne pointre na zhluky
 * @post Vzdialenost medzi bodmi, ktora je >=0
 *
 * @param o1 Pointer na 1. objekt
 * @param o2 Pointer na 2. objekt
 * @return Vzdialenost objektov
 */

float obj_distance(struct obj_t *o1, struct obj_t *o2);

/**
 * Pocita vzdalenost dvou shluku.
 *
 * @pre Platne pointre na zhluky
 * @post Vzdialenost medzi zhlukmi, ktora je >=0
 *
 * @param c1 Pointer na 1. zhhluk
 * @param c2 Pointer na 2. zhhluk
 * @return Vzdialenost 2 najblizsich objektov zhlukov
 */

float cluster_distance(struct cluster_t *c1, struct cluster_t *c2);

/**
 * Funkce najde dva nejblizsi shluky. V poli shluku 'carr' o velikosti 'narr'
 * hleda dva nejblizsi shluky. Nalezene shluky identifikuje jejich indexy v poli
 * 'carr'.
 *
 * @pre Platne pointre a nezaporna velkost narr
 * @post Najdene zhluky (ich indexy) sa ulozia na adresu c1 resp. c2
 *
 * @param carr Pointer na pole zhlukov
 * @param narr Velkost pola
 * @param c1 Uchovava index 1. zhluku
 * @param c2 Uchovava index 2. zhluku
 */

void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2);

/**
 * Razeni objektu ve shluku vzestupne podle jejich identifikatoru.
 *
 * @param c Pointer na zhluk
 */

void sort_cluster(struct cluster_t *c);

/**
 * Tisk shluku 'c' na stdout.
 *
 * @param c Pointer na zhluk
 */

void print_cluster(struct cluster_t *c);

/**
 * Ze souboru 'filename' nacte objekty. Pro kazdy objekt vytvori shluk a ulozi
 * jej do pole shluku. Alokuje prostor pro pole vsech shluku a ukazatel na prvni
 * polozku pole (ukalazatel na prvni shluk v alokovanem poli) ulozi do pameti,
 * kam se odkazuje parametr 'arr'. Funkce vraci pocet nactenych objektu (shluku).
 * V pripade nejake chyby uklada do pameti, kam se odkazuje 'arr', hodnotu NULL.
 *
 * @pre Existuje subor so suradnicami objektov
 * @post Uvedene udaje su v spravnom tvare a uspesne ulozene
 *
 * @param filename Pointer na subor
 * @param arr Pointer na pole zhlukov
 * @return Pocet uspesne vytvorenych zhlukov
 */

int load_clusters(char *filename, struct cluster_t **arr);

/**
 * Tisk pole shluku. Parametr 'carr' je ukazatel na prvni polozku (shluk).
 * Tiskne se prvnich 'narr' shluku.
 *
 * @param carr Pointer na prvy zhluk pola
 * @param narr Pocet zhlukov v poli
 */

void print_clusters(struct cluster_t *carr, int narr);

/** @}*/