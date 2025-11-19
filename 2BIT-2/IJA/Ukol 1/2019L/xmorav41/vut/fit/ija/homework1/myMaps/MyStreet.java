/*
 * Zdrojové kódy josu součástí zadání 1. úkolu pro předmětu IJA v ak. roce 2019/2020.
 * (C) Radek Kočí
 */
package vut.fit.ija.homework1.myMaps;
import vut.fit.ija.homework1.maps.Coordinate;
import vut.fit.ija.homework1.maps.Street;
import vut.fit.ija.homework1.maps.Stop;



import java.util.*;

/**
 * Reprezentuje jednu ulici v mapě. Ulice má svůj identifikátor (název) a je definována souřadnicemi. Pro 1. úkol
 * předpokládejte pouze souřadnice začátku a konce ulice.
 * Na ulici se mohou nacházet zastávky.
 * @author koci
 */
public class MyStreet implements Street{

    public String street_id = null; 
    public List<Coordinate> street_list = new ArrayList<Coordinate>();
    public List<Stop> street_stops = new ArrayList<Stop>();

    public MyStreet(String name, Coordinate start, Coordinate end){

        this.street_id = name;
        this.street_list.add(start);
        this.street_list.add(end);
    }

    /**
     * Vrátí identifikátor ulice.
     * 
     * @return Identifikátor ulice.
     */
    public String getId() {
        return this.street_id;
    };
    
    /**
     * Vrátí seznam souřadnic definujících ulici. První v seznamu je vždy počátek a poslední v seznamu konec ulice.
     * @return Seznam souřadnic ulice.
     */
    
    public List<Coordinate> getCoordinates(){
        return this.street_list;
    };
    
    /**
     * Vrátí seznam zastávek na ulici.
     * @return Seznam zastávek na ulici. Pokud ulize nemá žádnou zastávku, je seznam prázdný.
     */
    public List<Stop> getStops(){
        return this.street_stops;
    };
    
    /**
     * Přidá do seznamu zastávek novou zastávku.
     * @param stop Nově přidávaná zastávka.
     */
    public void addStop(Stop stop){
        this.street_stops.add(stop);
        stop.setStreet(this);
    }



}
