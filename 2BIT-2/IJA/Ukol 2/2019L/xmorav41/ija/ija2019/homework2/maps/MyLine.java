package ija.ija2019.homework2.maps;

import java.util.*;
import java.util.AbstractMap.SimpleImmutableEntry;


import ija.ija2019.homework2.maps.Line;

public class MyLine implements Line{

    public String line_id = null;
    public List< AbstractMap.SimpleImmutableEntry<Street,Stop> > line = new ArrayList<AbstractMap.SimpleImmutableEntry<Street,Stop>>() ;



    /**
     * Vloží zastávku do linky. Pořadí vložení určuje pořadí zastávek, první vložená je první zastávka linky. 
     * Zastávky leží na ulici, pokud ulice nově vkládané zastávky nenavazuje na poslední vloženou, nelze vložit. 
     * První (výchozí) zastávku lze vložit vždy. Současně se zastávkou je vložena i ulice, kudy linka jede.
     */
    public boolean addStop(Stop stop){

        if (line.isEmpty()){
            line.add(new SimpleImmutableEntry<Street, Stop>(stop.getStreet(), stop));
            return true;
        }

        else if (line.get(line.size()-1).getKey().getId() == stop.getStreet().getId()){
            line.add(new SimpleImmutableEntry<Street, Stop>(null, stop));
            return true;
        }


        else if ( line.get(line.size()-1).getKey().follows(stop.getStreet()) ){
            line.add(new SimpleImmutableEntry<Street, Stop>(stop.getStreet(), stop));
            return true;
        }

        else
            return false;
    };


    
    /**
     * Vloží ulici bez zastávky do linky. Pořadí vložení určuje pořadí průjezdu. 
     * Pokud vkládaná //zastávka// ulice nenavazuje na poslední vloženou, nelze vložit. 
     * Ulici bez zastávky nelze vložit jako první (první je vždy výchozí zastávka).
     */
    public boolean addStreet(Street street){

        if (line.isEmpty() ){
            return false;
        }

        else if (line.get(line.size()-1).getKey().follows(street) ){
            line.add(new SimpleImmutableEntry<Street, Stop>(street, null)); 
            return true;
        }
        else
            return false;
    };

    public List<java.util.AbstractMap.SimpleImmutableEntry<Street,Stop>> getRoute(){
        return new ArrayList<>(this.line);
    }





}