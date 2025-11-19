/*
 * Zdrojové kódy josu součástí zadání 1. úkolu pro předmětu IJA v ak. roce 2019/2020.
 * (C) Radek Kočí
 */
package vut.fit.ija.homework1.myMaps;
import vut.fit.ija.homework1.maps.Coordinate;
import vut.fit.ija.homework1.maps.Stop;
import vut.fit.ija.homework1.maps.Street;



/**
 * Reprezentuje zastávku. Zastávka má svůj unikátní identifikátor a dále souřadnice umístění a zná ulici, na které je umístěna.
 * Zastávka je jedinečná svým identifikátorem. Reprezentace zastávky může existovat, ale nemusí mít
 * přiřazeno umístění (tj. je bez souřadnic a bez znalosti ulice). Pro shodu objektů platí, že dvě zastávky jsou shodné, pokud
 * mají stejný identifikátor.
 * @author koci
 */
public class MyStop implements Stop {

    public String stop_id = null;
    public Coordinate stop_coord = null;
    public Street stop_street = null;

    public MyStop(String name){
        this.stop_id = name;
    }

    public MyStop(String name, Coordinate c){
        this.stop_id = name;
        this.stop_coord = c;
    }

    /**
     * Vrátí identifikátor zastávky.
     * @return Identifikátor zastávky.
     */
    public String getId(){
        return this.stop_id;
    }
    
    /**
     * Vrátí pozici zastávky.
     * @return Pozice zastávky. Pokud zastávka existuje, ale dosud nemá umístění, vrací null.
     */
    public Coordinate getCoordinate(){
        return this.stop_coord;
    }

    /**
     * Nastaví ulici, na které je zastávka umístěna.
     * @param s Ulice, na které je zastávka umístěna.
     */
    public void setStreet(Street s){
        this.stop_street = s;
    }

    /**
     * Vrátí ulici, na které je zastávka umístěna.
     * @return Ulice, na které je zastávka umístěna. Pokud zastávka existuje, ale dosud nemá umístění, vrací null.
     */
    public Street getStreet(){
        return this.stop_street;
    }

    @Override 
    public boolean equals(Object o){
        if (o == this) { 
            return true; 
        } 
  
        if (!(o instanceof Stop)) { 
            return false; 
        } 

        Stop s = (Stop) o;

        return stop_id.equals(s.getId());

    }
}
