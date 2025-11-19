/*
 * Zdrojové kódy josu součástí zadání 1. úkolu pro předmětu IJA v ak. roce 2019/2020.
 * (C) Radek Kočí
 */
package ija.ija2019.homework2.maps;
import ija.ija2019.homework2.maps.Street;



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

    /**
     * Vrátí identifikátor ulice.
     * 
     * @return Identifikátor ulice.
     */
    public String getId() {
        return this.street_id;
    };

    /**
     * Vrátí seznam souřadnic definujících ulici. První v seznamu je vždy počátek a
     * poslední v seznamu konec ulice.
     * 
     * @return Seznam souřadnic ulice.
     */
    public List<Coordinate> getCoordinates() {
        return this.street_list;
    };

    /**
     * Testuje, zda ulice navazuje na zadanou ulici. Ulice na sebe navazují, pokud
     * dva libovolné konce ulice this a s mají stejné souřadnice.
     */
    public boolean follows(Street s) {
        if (this.begin().equals(s.end())|| this.end().equals(s.begin())) {
            return true;
        } else
            return false;
    };

    /**
     * Vrací souřadnice začátku ulice.
     */
    public Coordinate begin() {
        return street_list.get(0);
    };

    /**
     * Vrací souřadnice konce ulice.
     */
    public Coordinate end() {
        return street_list.get(street_list.size() - 1);
    };

    /**
     * Vytvoří ulici (instance implicitní implementace). Tato implementace umožňuje pouze pravoúhlé zlomy ulice.
     * @param id názov ulice
     * @param coordinates koordinácie ulice
     * @return Street s
     */
    public static Street defaulStreet(String id, Coordinate... coordinates) {

        MyStreet s = new MyStreet();
        s.street_id = id;
        for (Coordinate c : coordinates){

            if (s.street_list == null || s.end().diffX(c) == 0 || s.end().diffY(c) == 0){
                s.street_list.add(c); 
            }
            else 
                return null;
        }
        return s;
    };

    /**
     * Vrátí seznam zastávek na ulici.
     * @return Seznam zastávek na ulici. Pokud ulize nemá žádnou zastávku, je seznam prázdný.
     */
    public List<Stop> getStops(){
        return this.street_stops;
    };
    

    /**
     * Přidá do seznamu zastávek novou zastávku. Při vkládání testuje, zda zastávka 
     * skutečně leží na ulici. Pokud zastávka neleží na ulici, nedělá nic.
     * Skrze coords
     */
    public boolean addStop(Stop stop){
        
        for (int i = 0; i < this.street_list.size()-1; i++){

            int A_X = this.street_list.get(i).getX();
            int A_Y = this.street_list.get(i).getY();
            int B_X = this.street_list.get(i+1).getX();
            int B_Y = this.street_list.get(i+1).getY();
            int STOP_X = stop.getCoordinate().getX();
            int STOP_Y = stop.getCoordinate().getY();

            if (A_X == B_X && A_X == STOP_X){
                if ((A_Y <= STOP_Y && STOP_Y <= B_Y)||(B_Y <= STOP_Y && STOP_Y <= A_Y)){
                    this.street_stops.add(stop);
                    stop.setStreet(this);
                    return true;
                }
            }
            else if (A_Y == B_Y && A_Y == STOP_Y){
                if ((A_X <= STOP_X && STOP_X <= B_X)||(B_X <= STOP_X && STOP_X <= A_X)){
                    this.street_stops.add(stop);
                    stop.setStreet(this);
                    return true;
                }
            }
            else
                continue;
        }
        return false;
    }
}
