/*  IMS PROJEKT
 *  Téma č. 1: Epidemiologické modely pomocí celulárních automatů 
 *  Autori: Tomáš Moravčík, Jaromír Homolka
 *  Login: xmorav41, xhomol21
 *  2020
*/

#include <iostream>
#include <fstream>
#include <stdlib.h>     
#include <time.h> 
#include <string.h>
#include <string>


#include "functions.h"

using namespace std;

#define F 0
#define M 1

#define HET 0
#define HOM 1

#define REC_AN 0.014
#define INS_AN 0.0011 
#define REC_VA 0.0008   // chlap HIV
#define INS_VA 0.0004   // zena HIV


// priemerny vek v cz = 42.5 https://www.czso.cz/csu/stoletistatistiky/prumerny-vek-obyvatel-ceske-republiky
/*
Specifically, by the end of the study, a 21 year old with HIV was predicted to live to the age of 77, 
whereas a 21 year old without HIV would live to the age of 86. ===> Assuming HIV patients has 89.5% of normal lifespan
https://www.aidsmap.com/news/mar-2020/yes-same-life-expectancy-hiv-negative-people-far-fewer-years-good-health

https://www.ssa.gov/oact/STATS/table4c6.html#fn1
*/


person generate_person(int seed, int pop_size, Locality l){
    struct person p;
    p.sex = seed % 2;
    (p.sex == M) ? p.age = (rand() % l.MALE ) : p.age = (rand() % l.FEMALE);  // males have lower age average
    (rand() % 100 < 4) ? p.orientation = HOM : p.orientation = HET;
    (((float)pop_size*l.gen_person) >= rand() % pop_size ) ? p.HIV_pos = true : p.HIV_pos = false;                 /// 0.000314
    if (p.HIV_pos){
        (rand() % 100 < 66) ? p.orientation = HOM : p.orientation = HET;    // if HIV then 66% being HOM
    }
    return p;
}

int main(int argc, char *argv[]) {

    int GRIDSIZE = 0, PERIOD = 0;
    Locality loc;
    float mutation = 1;     //how strong/weak hiv is    
    
    for (int i = 1; i < argc; i++){

        try{            
            if (strcmp(argv[i], "-g") == 0){
                if (strcmp(argv[i+1], "CZ") == 0)
                    GRIDSIZE = 3271;
                else if (strcmp(argv[i+1], "SK") == 0){
                    loc.loc= "SK";
                    loc.MALE=71;
                    loc.FEMALE=80;
                    loc.root_grid = 2336;
                    loc.gen_person = 0.000178;
                    loc.loc_rate = 1.6;
                }
                else if (strcmp(argv[i+1], "HG") == 0){
                    loc.loc= "HG";
                    loc.MALE=69;
                    loc.FEMALE=78;
                    loc.root_grid = 3126;
                    loc.gen_person = 0.000388;
                    loc.loc_rate = 1.0;
                }
                else if (strcmp(argv[i+1], "AU") == 0){
                    loc.loc= "AU";
                    loc.MALE=76;
                    loc.FEMALE=82;
                    loc.root_grid = 2976;
                    loc.gen_person = 0.001116;
                    loc.loc_rate = 0.4;
                }
                else if (strcmp(argv[i+1], "HR") == 0){
                    loc.loc= "HR";
                    loc.MALE=72;
                    loc.FEMALE=79;
                    loc.root_grid = 2019;
                    loc.gen_person = 0.0004;
                    loc.loc_rate = 0.9;
                }
                else if (strcmp(argv[i+1], "TEST") == 0)
                    loc.root_grid = 1000;
                else{
                    loc.loc= "CZ";
                    GRIDSIZE = 3271; 
                }
               
                //GRIDSIZE = stoi(argv[i+1]);
                i++;            
            }
            else if (strcmp(argv[i], "-p") == 0){
                PERIOD = stoi(argv[i+1]);
                i++;            
            }
            else if (strcmp(argv[i], "-m") == 0){
                mutation = stoi(argv[i+1]);
                i++;            
            }
            else if (strcmp(argv[i], "-h") == 0) {
                printf("\n*  IMS PROJEKT\n*  Téma č. 1: Epidemiologické modely pomocí celulárních automatů\n");  
                printf("*  Autori: Tomáš Moravčík, Jaromír Homolka\n");
                printf("*  Login: xmorav41, xhomol21\n");
                printf("*  2020\n\n");
                printf("   Params:\n");
                printf("                -g  [CZ | SK | HG | AU | HR] - Vyber lokalitu simulace\n");
                printf("                -p  (integer) - Doba trvania simulace v rokoch\n");
                printf("                -m  (integer) - Mutace HIV genu, implicitne 1\n\n");
                return 0;
            }
            else{
                printf("Unkown arg: %s\nUse \"-h\" for help\n", argv[i]);
                return 1;
            } 
            
        }
        catch(exception const & e) {
            cout<<"error : " << e.what() << " > Use -h for help\n" <<endl;
            return 1;
        }
            
        
    }    

    srand (time(NULL));
    int seed;
    GRIDSIZE = loc.root_grid;


    person **grid = (person **)calloc(GRIDSIZE, sizeof(GRIDSIZE * sizeof(person *)));
    for (int i = 0; i < GRIDSIZE; ++i){
        grid[i] = (person *)calloc(GRIDSIZE, sizeof(person));
    }

    person **tmpG = (person **)calloc(GRIDSIZE, sizeof(GRIDSIZE * sizeof(person *)));


    int start_pos = 0;
    int end_pos = 0;
    int end_pos_total = 0;
    int start_hiv_hom = 0;
    int end_hiv_hom = 0;
    int start_hiv_het = 0;
    int end_hiv_het = 0;
    int death_count = 0;
    int hiv_hom_total = 0;
    int hiv_het_total = 0;

    printf("\nEPIDEMIC HIV SIMULATION FOR %s:\n",loc.loc.c_str());

    for (int i = 0; i < GRIDSIZE; i++){
        for (int j = 0; j < GRIDSIZE; j++){

            seed = rand() % 10 + 1;
            person Q = generate_person(seed, GRIDSIZE*GRIDSIZE, loc);
            grid[i][j] = Q;
            
            if (Q.HIV_pos){
               (Q.orientation == HOM) ? start_hiv_hom++ : start_hiv_het++;
               start_pos++;
            } 
        }
    }

    hiv_hom_total = start_hiv_hom;
    hiv_het_total = start_hiv_het;
    end_pos_total = start_pos;

    int cur_year_HIV_up = 0;
    int pos_end_curr = start_pos;
    for (int r = 0; r < PERIOD; r++){

        //memcpy(tmpG,grid,(GRIDSIZE * sizeof(GRIDSIZE * sizeof(person *)))); //working
        memcpy(tmpG,grid,(GRIDSIZE * sizeof(GRIDSIZE * sizeof(person *))));

        for (int i = 0; i < GRIDSIZE; i++){
            for (int j = 0; j < GRIDSIZE; j++){
                person &P1 = grid[i][j];

                if (death( grid[i][j] ,loc)) {
                    death_count++;      // someone died
                    tmpG[i][j] = grid[i][j];
                    continue;
                }

                int partner_count = rand() % 2 + 1;
                if (P1.orientation == HOM){
                    partner_count = partner_count *  2;
                }
                if ((rand() % (10*1000*1000)) < 15 && !P1.HIV_pos) {  // drugs [208 *0,08 /  10 699 142]

                    tmpG[i][j].HIV_pos = true;
                    (P1.orientation == HOM) ? hiv_hom_total++ : hiv_het_total++;
                    cur_year_HIV_up++;
                    pos_end_curr++;                    
                }


                for (int k = 0; k < partner_count && P1.max_partners > 0; k++){    // pocet partnerov za rok avg. 1.5

                    if (P1.age < 16) continue;

                    int x = rand() % GRIDSIZE;    //destination seed
                    int y = rand() % GRIDSIZE;    //destination seed

                    if (x == i && y == j) continue; // no sex for u
                    
                    person P2 = tmpG[x][y];

                    if (partner_compatibility(P1, P2)){
                        P1.max_partners--;
                        P2.max_partners--;
                    
                        if ((P1.HIV_pos || P2.HIV_pos) && !(P1.HIV_pos && P2.HIV_pos)){      // jeden alebo druhy

                            int sex_by_age = 0; // https://www.refinery29.com/en-us/2017/08/168733/sex-frequency-age-average
                            if (P1.age > 17 && 30 < P1.age ) sex_by_age = 112;
                            else if (P1.age >= 30 && 40 < P1.age ) sex_by_age = 86;
                            else if (P1.age >= 40 && 50 < P1.age ) sex_by_age = 69;
                            else sex_by_age = 10;

                            for (int p = 0; p < (sex_by_age / partner_count); p++){ //pocet stykov
                            
                                bool rate = false;
                                if (P1.sex == M && P2.sex == M){ // gays
                                    int act = rand() % 4;
                                    if (act == 0) rate = infection_rate(REC_AN, mutation, loc);
                                    else if (act == 1) rate = infection_rate(INS_AN, mutation, loc);
                                    else rate = infection_rate(INS_AN + REC_AN, mutation, loc);

                                    if (rate){
                                        ((P1.orientation == P2.orientation) && P1.orientation == HOM) ? hiv_hom_total++ : hiv_het_total++;
                                    }
                                }
                                else if ((P1.sex == M && P1.HIV_pos) || (P2.sex == M && P2.HIV_pos) ){ //VAG RES chlap hiv
                                    rate = infection_rate(REC_VA, mutation, loc);
                                    if ((p % 5) == 0 && !rate){
                                        rate = infection_rate(REC_AN, mutation, loc);
                                    }
                                    if (rate) hiv_het_total++;
                                }
                                else if ((P1.sex == F && P1.orientation != HOM) || (P2.sex == F && P2.orientation != HOM) ){      //VAG INS zena HIV
                                    rate = infection_rate(INS_VA, mutation, loc);
                                    if ((p % 5) == 0 && !rate){
                                        rate = infection_rate(INS_AN, mutation, loc);
                                    }
                                    if (rate) hiv_het_total++;
                                }
                                // if else 2 zeny => prakticky nemozne prenosu

                                if (rate){      //ak hiv sex tak obaja pos
                                    tmpG[i][j].HIV_pos = true;
                                    tmpG[x][y].HIV_pos = true;
                                    cur_year_HIV_up++;
                                    pos_end_curr++;
                                    break;
                                }
                            }
                        }
                    }    
                    else if (P1.age < 16) { //nezletili
                        break;
                    }
                    else
                        k--;               // bol nekompatibilny partner
                }

                person &Test = tmpG[i][j];
                Test.age++; 
            }

        }
        int alive_hiv = 0;
        for (int i = 0; i < GRIDSIZE; i++){
            for (int j = 0; j < GRIDSIZE; j++){
                person &p1 = tmpG[i][j];
                p1.max_partners = 4;
                if(p1.HIV_pos) alive_hiv++;
            }        
        } 

    memcpy(grid,tmpG,(GRIDSIZE * sizeof(GRIDSIZE * sizeof(person *))));
        
        printf("\n[YEAR: %02d] [NEW CASES: %3d] [ALIVE HIV: %3d] [TOTAL HIV: %3d]\n", r+1, cur_year_HIV_up, alive_hiv, pos_end_curr);
        end_pos_total = end_pos_total + cur_year_HIV_up;
        cur_year_HIV_up = 0;
    }


    int hom_count = 0, het_count = 0;
    for (int i = 0; i < GRIDSIZE; i++){
        for (int j = 0; j < GRIDSIZE; j++){
            person Q = grid[i][j];
            (Q.orientation == HOM) ? hom_count++ : het_count++;
            
            if (Q.HIV_pos){
               (Q.orientation == HOM) ? end_hiv_hom++ : end_hiv_het++;
               end_pos++;
            } 
        }
    }

    for (int i = 0; i < GRIDSIZE; ++i){
        delete [] grid[i];
    }
    free(grid);
    free(tmpG);

    printf("\n### RESULTS ###\n\n");
    printf("YEARS SIMULATED: %d\n",PERIOD);
    printf("POPULATION: %d \nHET: %d HOM: %d \n", GRIDSIZE*GRIDSIZE, het_count, hom_count);
    printf("[HIV START: %4d pos] [%4d HET] [%4d HOM]\n[HIV END  : %4d pos] [%4d HET] [%4d HOM]\n",start_pos, start_hiv_het, start_hiv_hom, end_pos, end_hiv_het, end_hiv_hom);
    printf("[HIV INC  : %4d pos] [%4d HET] [%4d HOM]\n",end_pos-start_pos,end_hiv_het-start_hiv_het, end_hiv_hom-start_hiv_hom); 
    printf("-------------------------------------\n");
    printf("[HIV TOTAL: %4d pos] [%4d HET] [%4d HOM]\n",end_pos_total, hiv_het_total, hiv_hom_total);
    printf("ALL DEATHS: %d\n\n", death_count);
    printf("ALIVE:\n");
    printf("HIV HET: %.2f %%\nHIV HOM: %.2f %%\n", ((double)end_hiv_het / (double)end_pos)*100, (((double)end_hiv_hom) / (double)end_pos)*100);
    printf("\nTOTAL:\n");
    printf("HIV HET: %.2f %%\nHIV HOM: %.2f %%\n", ((double)hiv_het_total / (double)end_pos_total)*100, ((double)hiv_hom_total / (double)end_pos_total)*100);
    printf("\n\n");
    printf("HIV IN POPULATION: \n");
    printf("ON START  : %.4f %%\n", (double)start_pos/((double)GRIDSIZE*GRIDSIZE)*100);
    printf("IN THE END: %.4f %%\n\n", (double)end_pos/((double)GRIDSIZE*GRIDSIZE)*100);

    return 0;
} 