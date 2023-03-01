/*
 * 1. Projekt
 * Praca s textom
 * Tomáš Moravčík (xmorav41)
 * Oktober 2018
 *
 *
 * Tento program je riadkovy editor s neiteraktivnymi prikazmi
 * Editovany text bude upraveny po riadkoch zo standardneho
 * vstupu
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX 1001
#define MAXROW 101


void checkEOL(char *str_ll){
   long int k = strlen(str_ll);

    if (str_ll[k-1] != '\n'){   //zaisti spravnu funkciu ak posledny prikaz nema EOL sign
        str_ll[k] = '\n';
        str_ll[k+1] = '\0';
    }
}

void beforeContent(char *str_p, char *str_l) {
//prida pred input string
    char str_answer[MAX];
    long int k,m;
    int j;

    memset(str_answer, 0, sizeof(str_answer));
    k = strlen(str_p)- 2;//Minus 2 lebo odstranujem 'b' a '/0' znak
    m = strlen(str_l) - 1;//Taktiez pre '/0'

    for (j = 0; j < k; j++)
        str_answer[j] = str_p[j + 1]; //b nepocitame

    for (j = 0; j <= m; j++)
        str_answer[k + j] = str_l[j];

    for (j=0;j<=(int)strlen(str_answer);j++)
        str_l[j]=str_answer[j];//skopiruje vysledny string na dalsie editacie
}

void appendContent(char *str_p, char *str_l){
//prida za input string
    char str_answer[MAX];
    long int j,k,m;

    memset(str_answer, 0, sizeof(str_answer));
    k = strlen(str_p) - 1; //Minus 1 lebo odstranujem [i][0] znak
    m = strlen(str_l) - 1; //Minus 1 lebo odstranujem /0

    for (j = 0; str_l[j] != '\n'; j++)
        str_answer[j] = str_l[j];

    for (j = 0; j < k; j++)
        str_answer[m + j] = str_p[j + 1];

    for (j=0;j<=(int)strlen(str_answer);j++)
        str_l[j]=str_answer[j];
}

void insertContent(char *str_p){
    printf("%s",str_p+1);
}

void removeEOL(char *str_l){
    long int a = strlen(str_l) - 1;
    str_l[a] = 0;
}

void substituteContent(char *str_p, char *str_l) {
    char pat[MAX]; //pattern
    char rep[MAX]; //replacement
    char str_answer[MAX];
    char x;
    char *p;
    long int j,k;

    k = strlen(str_p) - 1;
    x = str_p[1];

    //ziska pattern
    for (j=0; str_p[j+2] != x ; j++){
        pat[j] = str_p[j+2];
    }
    pat[j+1] = '\0';

    //ziska replacement
    long int pp = j; //ulozisko s originalnou hodnotou premennej j
    for (; j<k; j++){
        rep[j-pp] = str_p[j+2];
    }
    for (j=0; (rep[j] != ('\n' - 1)) && j<MAX; j++){ //bez tohto by nacitanie do pamati mohlo pretiect a zle zapisat rep
        rep[j]=rep[j+1];
    }

    j = strlen(rep);
    rep[j-1] = '\0';

    //hlada pattern v stringu, ak ho najde tak ho nahradi
    if (!(p = strstr(str_l,pat)))
        ;
    else {
        strncpy(str_answer,str_l,p-str_l);
        str_answer[p-str_l]='\0';
        sprintf(str_answer+(p-str_l), "%s%s", rep, p+strlen(pat));
        strcpy(str_l,str_answer);
    }
}

void uniquePattern(char *str_p, char *str_l) {
    char pat[MAX];
    char rep[MAX];
    char str_answer[MAX];
    char x;
    char *p;
    long int j,k;

    k = strlen(str_p) - 1;
    x = str_p[1];

    //ziska pattern
    for (j=0; str_p[j+2] != x ; j++){
        pat[j] = str_p[j+2];
    }
    pat[j] = '\0';

    //ziska replacement
    long int pp = j; //ulozisko s originalnou hodnotou premennej j
    for (; j<k; j++) {
        rep[j - pp] = str_p[j + 2];
    }
    for (j=0; (rep[j] != ('\n' - 1)) && j<MAX; j++){
        rep[j]=rep[j+1];
    }

    j = strlen(rep);
    rep[j-1] = '\0';
    char uniq_pat[] = "----ôô.§.ňň§...-.ň"; //unique pattern pouzivame v pripade ze replacement obsahuje pattern, teda vyhneme sa zacykleniu

    for(int o = 0; o<MAX; o++){
        if (!(p = strstr(str_l,pat)))
            break;
        else {
            strncpy(str_answer, str_l, p - str_l);
            str_answer[p - str_l] = '\0';
            sprintf(str_answer + (p - str_l), "%s%s", uniq_pat, p + strlen(pat));
            strcpy(str_l, str_answer);
        }
    }

    //nahradi unique pattern už chcenym stringom (uz hlada unique pattern a nie len pattern)
    for(int o = 0; o<MAX; o++){
        if (!(p = strstr(str_l,uniq_pat)))
            break;
        else {
            strncpy(str_answer, str_l, p - str_l);
            str_answer[p - str_l] = '\0';
            sprintf(str_answer + (p - str_l), "%s%s", rep, p + strlen(uniq_pat));
            strcpy(str_l, str_answer);
        }
    }
}

void nextLineN(FILE *fr, char *str_p, char *str_l){
    int j;

    //Skontroluje a nacita pripadne N
    if (str_p[1] != '\n' && (str_p[1] >='1' && str_p[1] <='9' )){
        int x = str_p[1] - '0';
        if (str_p[2] != '\n' && (str_p[2] >='0' && str_p[2] <='9' ))
            x = x*10 + str_p[2] - '0';

        for (j = 0; j < x; j++){
            printf("%s",str_l);
            memset(str_l, 0, sizeof(*str_l));
            fgets(str_l, MAX, fr);
        }
    }
    else {
        printf("%s", str_l);
        memset(str_l, 0, sizeof(*str_l));
        fgets(str_l, MAX, fr);
    }
}

void deleteLine(FILE *fr, char *str_p, char *str_l){
    int j,x;

    if (str_p[1] != '\n' && (str_p[1] >= '0' && str_p[1] <= '9' )){
        x = str_p[1] - '0';
        if (str_p[2] != '\n' && (str_p[2] >= '0' && str_p[2] <= '9' ))
            x = x*10 + str_p[2] - '0';

        for (j = 0; j < x; j++){
            memset(str_l, 0, sizeof(*str_l));
            fgets(str_l, MAX, fr);
        }
    }
    else{
        memset(str_l, 0, sizeof(*str_l));
        fgets(str_l, MAX, fr);
    }
}

int goToX(int doot, int z, char *str_p) {
    int  x;

    if (!doot) {
        if (str_p[1] >= '0' && str_p[1] <= '9') {
            x = str_p[1] - '0';
            if (str_p[2] >= '0' && str_p[2] <= '9')
                x = x * 10 + str_p[2] - '0';
            z = x -1;
        }
    }
    return z;
}


int main(int argc, char* argv[]) {
    char str_prikaz[MAXROW][MAX];
    char str_line[MAX];
    int j;
    int bing = 0; //pri goto funkcii proti zacykleniu
    int c;

    if (argc != 2) {
        printf("Prilis vela alebo malo argumentov, koniec programu \n"); // skontroluje pocet argumentov
        return 1;
    }

    FILE *commands = fopen(argv[1], "r");
    FILE *stdin_file;
    stdin_file = stdin;

    if ((commands == NULL) || (stdin_file == NULL)) {
        printf("Subor sa nepodaril otvorit\n");
        return 1;
    }

    int rowCount = 1;

    //zisti pocet riadkov v prikazoch
    do {
        c = getc(commands);
        if (c == '\n') {
            rowCount++;
        }
    } while (c != EOF);

    if (rowCount >MAXROW){
        printf("Pocet prikazov presiahol stanoveny pocet 100\nKoniec programu\n");
        return 1;
    }

    rewind(commands); //restartne prikazy na nacitanie do pola

    // nacita prikazy do 2d pola
    int i = 1;
    while (fgets(str_line, MAX, commands) != NULL && i <= rowCount) {
        if (strlen(str_line) >= MAX) {
            printf("%ld\n", strlen(str_line));
            printf("Pocet znakov presiahol stanoveny pocet 1000\nKoniec programu\n");
            return 1;
        }

        for (j = 0; j < (int)(strlen(str_line)); j++) //vytvori 2d pole prikazov
            str_prikaz[i][j] = str_line[j];
        i++;
    }

    long int y = strlen(str_prikaz[rowCount]);
    if (str_prikaz[rowCount][y-1] != '\n'){   //zaisti spravnu funkciu ak posledny prikaz nema EOL sign
        str_prikaz[rowCount][y] = '\n';
        str_prikaz[rowCount][y+1] = '\0';
    }


    fgets(str_line,MAX,stdin_file);
    for (i = 1; i <= rowCount; i++) {

        if (strlen(str_line) >= MAX) {
            printf("Pocet znakov presiahol stanoveny pocet 1000\nKoniec programu\n");
            return 1;
        }

        switch (str_prikaz[i][0]) {

            case 'b':
                beforeContent(str_prikaz[i], str_line);
                break;

            case 'i':
                insertContent(str_prikaz[i]);
                break;

            case 'a':
                appendContent(str_prikaz[i], str_line);
                break;

            case 'd':
                deleteLine(stdin_file,str_prikaz[i], str_line);
                break;

            case 'r':
                removeEOL(str_line);
                break;

            case 's':
                substituteContent(str_prikaz[i], str_line);
                break;

            case 'S':
                uniquePattern(str_prikaz[i], str_line);
                break;

            case 'n':
                nextLineN(stdin_file,str_prikaz[i],str_line); //nN
                break;

            case 'q':
                if (str_prikaz[i-1][0] == 'n' && str_prikaz[i-1][1] == '\n') //skontroluje či niesme už na dalsom riadku, co by inak sposobilo ukoncenie bez EOL
                    if (str_prikaz[i-2][0] == 'r')//skontroluje ci predchadzajuci neprisiel o EOL
                      printf("%c",'\n');
                checkEOL(str_line);
                return 0;

            case 'g':
                i=goToX(bing,i,str_prikaz[i]);
                bing = 1;
                break;

            default:
                ;
        }
    }

    if (i - 1 == rowCount && feof(stdin_file) == 0) { //skontroluje ci koniec a a vypise zvysok
        checkEOL(str_line);
        printf("%s",str_line);

        while (fgets(str_line, MAX, stdin)) {
            checkEOL(str_line);
            printf("%s", str_line);
        }
    }

    if (fclose(commands) == EOF){
        printf("Subor sa %s sa nepodarilo zatvorit",argv[1]);
    }

    fclose(stdin_file);

    return 0;
}