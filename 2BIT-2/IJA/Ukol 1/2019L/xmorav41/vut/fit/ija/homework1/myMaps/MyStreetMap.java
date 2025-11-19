/*
 * Zdrojové kódy josu součástí zadání 1. úkolu pro předmětu IJA v ak. roce 2019/2020.
 * (C) Radek Kočí
 */
package vut.fit.ija.homework1.myMaps;

import java.util.*;

import vut.fit.ija.homework1.maps.Street;
import vut.fit.ija.homework1.maps.StreetMap;


/**
 * Reprezentuje jednu mapu, která obsahuje ulice.
 * @author koci
 */
public class MyStreetMap implements StreetMap{

    public List<Street> map = new ArrayList<Street>();
    /**
     * Přidá ulici do mapy.
     * @param s Objekt reprezentující ulici.
     */
    public void addStreet(Street s){
        this.map.add(s);
    };
    
    /**
     * Vrátí objekt reprezentující ulici se zadaným id.
     * @param id Identifikátor ulice.
     * @return Nalezenou ulici. Pokud ulice s daným identifikátorem není součástí mapy, vrací null.
     */
    public Street getStreet(String id){
        for (Street street : map){
            if(street.getId().equals(id)){
                return street;
            }
        }
        return null;
    };
}
