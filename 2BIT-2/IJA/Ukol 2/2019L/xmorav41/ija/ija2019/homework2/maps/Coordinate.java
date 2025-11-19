/*
 * Zdrojové kódy josu součástí zadání 1. úkolu pro předmětu IJA v ak. roce 2019/2020.
 * (C) Radek Kočí
 */
package ija.ija2019.homework2.maps;

/**
 * Reprezentuje pozici (souřadnice) v mapě. Souřadnice je dvojice (x,y), počátek mapy je vždy na pozici (0,0). 
 * Nelze mít pozici se zápornou souřadnicí.
 * @author koci
 */
public class Coordinate {

    public int x_ax;
    public int y_ax;

    public static Coordinate create(int x, int y) {

        if (x < 0 || y < 0) {
            //System.out.println("Negative coord");
            return null;
        }

        Coordinate coord = new Coordinate();
        coord.x_ax = x;
        coord.y_ax = y;
        return coord;

    };

    public int diffX(Coordinate c){
        return this.x_ax - c.getX();
    };

    public int diffY(Coordinate c){
        return this.y_ax - c.getY();
    }

    /**
     * Vrací hodnotu souřadnice x.
     * @return Souřadnice x.
     */
    public int getX(){
        return x_ax;
    };
    
    /**
     * Vrací hodnotu souřadnice y.
     * @return Souřadnice y.
     */
    public int getY(){
        return y_ax;
    };


    @Override 
    public boolean equals(Object o){
        if (o == this) { 
            return true; 
        } 
  
        if (!(o instanceof Coordinate)) { 
            return false; 
        } 

        Coordinate c = (Coordinate) o;

        return x_ax == c.getX() && y_ax == c.getY();

    }


    
}
