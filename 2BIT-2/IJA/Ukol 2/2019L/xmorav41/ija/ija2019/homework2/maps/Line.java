package ija.ija2019.homework2.maps;
import java.util.*;



public interface Line{

    public boolean addStop(Stop stop);

    public boolean addStreet(Street street);

    public List<java.util.AbstractMap.SimpleImmutableEntry<Street,Stop>> getRoute();

    public static Line defaultLine(String id){
        MyLine l = new MyLine();
        l.line_id = id;

        return l;
    };
    

}