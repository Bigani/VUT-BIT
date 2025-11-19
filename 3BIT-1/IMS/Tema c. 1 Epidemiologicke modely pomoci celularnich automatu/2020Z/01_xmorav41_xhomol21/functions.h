#include <iostream>
#include <fstream>
#include <stdlib.h>     
#include <time.h> 
#include <string.h>
#include <string>
using namespace std;

#define F 0
#define M 1

#define HET 0
#define HOM 1

struct person{
    int age, sex, orientation;
    bool HIV_pos;
    int max_partners = 4;
};

struct Locality {
   string loc= "CZ";
   int MALE=76;
   int FEMALE=82;
   int root_grid = 3271;
   float gen_person = 0.000314;
   float loc_rate = 1.0;
} CZ;



bool death(struct person &p, Locality l){
    // chlap 76 zena 82
    // https://www.czso.cz/csu/xb/jakeho-veku-se-pravdepodobne-dozijeme
    // https://www.healthline.com/health/hiv-aids/life-expectancy

    //printf("VEK %d\n", p.age );
    if (((p.age >= l.MALE) && (p.sex == M)) || ((p.age >= l.FEMALE) && (p.sex == F))){
        p.age = 0;
        p.HIV_pos = false;
        (rand() % 100 < 4) ? p.orientation = HOM : p.orientation = HET;
        //printf("NEG DIED\n");
        return true;
    }
    else if ( (((p.age >= (l.MALE*0.89)) && (p.sex == M)) && (p.HIV_pos)) || (((p.age >= (l.FEMALE*0.89)) && (p.sex == F)) && (p.HIV_pos)) ){
        p.age = 0;
        p.HIV_pos = false;
        (rand() % 100 < 4) ? p.orientation = HOM : p.orientation = HET;
        //printf("POS DIED\n");
        return true;
    }
    else 
        ;//printf("%d ",p.age);
    return false;
}


bool partner_compatibility(person p1, person p2){
    if(p1.age < 16 || p2.age < 16) return false;

     
    if (p1.sex == p2.sex && (rand() % 100 < 1)){    // bisexualny nahodny styk
        return true;
    }

    if (p1.sex == p2.sex){
        return (p1.orientation == HET || p2.orientation == HET) ? false : true;
    }
    else{
        return (p1.orientation == HOM || p2.orientation == HOM) ? false : true;
    }
}

bool infection_rate(float activity, float mutation, Locality l){
    // 
    // https://www.ncbi.nlm.nih.gov/pmc/articles/PMC5795598/

    int ochrana = rand() % 100;
    float with_condom,treated_HIV;
    treated_HIV = 1;
    float preven_HIV = 1;

    (ochrana < 80) ? with_condom = 0.3 : with_condom = 1;   // 80% ze pouzili kondom 

    int trt_chance = rand() % 100;
    if (trt_chance < 40) treated_HIV = 0.0001;  //  liecba u choreho  //90% ze su na HIV liecbe
    else if (trt_chance < 65) preven_HIV = 0.02;  // Prevencia u zdraveho 
    else if (trt_chance < 80) { treated_HIV = 0.000001; preven_HIV = 0.02; } // Oboje
                                                // else nic nepouzivaju

    float rate = activity * with_condom * treated_HIV * preven_HIV * mutation * l.loc_rate;

    float transmission = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/1));
    bool chance;
    (transmission < rate) ? chance = true : chance = false;
    
    return chance;
}




