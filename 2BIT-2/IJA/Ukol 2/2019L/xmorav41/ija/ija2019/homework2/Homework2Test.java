/*
 * Zdrojové kódy jsou součástí zadání 2. úkolu pro předmětu IJA v ak. roce 2019/2020.
 * Testovací třída pro JUnit 5.
 * (C) Radek Kočí
 * 
 */
package ija.ija2019.homework2;

import ija.ija2019.homework2.maps.Coordinate;
import ija.ija2019.homework2.maps.Line;
import ija.ija2019.homework2.maps.Stop;
import ija.ija2019.homework2.maps.Street;
import java.util.stream.Collectors;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.Assertions;

/**
 * Testovací třída úkolu 2 v předmětu IJA.
 * Vytvořte rozhraní a třídy, které splňují podmínky definované API dokumentací a touto testovací
 * třídou. Kromě metod, předepsaných rozhraními, mohou třídy implementovat své další metody a konstruktory.
 * Věnujte také pozornost privátním metodám testovací třídy a komentářům.
 * @author koci
 */
public class Homework2Test {

    @Test
    public void testStreet01() {

        Coordinate c1, c2, c3;
        Street s1, s2, s3;

        c1 = Coordinate.create(100, 100);
        c2 = Coordinate.create(100, 200);
        c3 = Coordinate.create(150, 250);        
        s1 = Street.defaultStreet("first", c1, c2, c3);        
        Assertions.assertNull(s1, "Ulice s jinymi nez pravouhlymi zlomy - nelze vytvorit");

        c1 = Coordinate.create(100, 100);
        c2 = Coordinate.create(100, 200);
        c3 = Coordinate.create(150, 200);        
        s1 = Street.defaultStreet("first", c1, c2, c3);        
        Assertions.assertNotNull(s1, "Ulice s pravouhlymi zlomy - lze vytvorit");

        c1 = Coordinate.create(200, 200);
        c2 = Coordinate.create(250, 200);
        s2 = Street.defaultStreet("second", c1, c2);        
        Assertions.assertNotNull(s2, "Ulice second - lze vytvorit");

        c1 = Coordinate.create(150, 200);
        c2 = Coordinate.create(250, 200);
        s3 = Street.defaultStreet("third", c1, c2);        
        Assertions.assertNotNull(s3, "Ulice third - lze vytvorit");
        
        Assertions.assertEquals(s1.begin(), Coordinate.create(100, 100), "Overeni pocatecni souradnice");
        Assertions.assertEquals(s1.end(), Coordinate.create(150, 200), "Overeni koncove souradnice");
        Assertions.assertEquals(s1.getCoordinates().get(1), Coordinate.create(100, 200), "Overeni prostredni souradnice");
        
        Assertions.assertFalse(s1.follows(s2), "Ulice first a second na sebe nenavazuji");
        Assertions.assertTrue(s1.follows(s3), "Ulice first a third na sebe navazuji");
    }

    @Test
    public void testStreet02() {
        Coordinate c1, c2, c3;
        Street s1;
        Stop stop1;

        c1 = Coordinate.create(100, 100);
        c2 = Coordinate.create(100, 200);
        c3 = Coordinate.create(150, 200);        
        s1 = Street.defaultStreet("first", c1, c2, c3);        
        Assertions.assertNotNull(s1, "Ulice first - lze vytvorit");

        c1 = Coordinate.create(170, 200);
        stop1 = Stop.defaultStop("stop1", c1);
        Assertions.assertFalse(s1.addStop(stop1), "Zastavka je mimo ulici");
        Assertions.assertTrue(s1.getStops().isEmpty(), "Ulice je prazdna");

        c1 = Coordinate.create(130, 200);
        stop1 = Stop.defaultStop("stop2", c1);
        Assertions.assertTrue(s1.addStop(stop1), "Zastavka je na ulici");
        Assertions.assertFalse(s1.getStops().isEmpty(), "Ulice neni prazdna");

    }
    
    @Test
    public void testLine() {
        Coordinate c1, c2, c3;
        Street s1, s2, s3;
        Stop stop1, stop3;
        Line line1;

        c1 = Coordinate.create(100, 100);
        c2 = Coordinate.create(100, 200);
        c3 = Coordinate.create(150, 200);        
        s1 = Street.defaultStreet("first", c1, c2, c3);        
        Assertions.assertNotNull(s1, "Ulice first - lze vytvorit");

        c1 = Coordinate.create(150, 200);
        c2 = Coordinate.create(150, 300);
        s2 = Street.defaultStreet("second", c1, c2);        
        Assertions.assertNotNull(s2, "Ulice second - lze vytvorit");

        c1 = Coordinate.create(150, 300);
        c2 = Coordinate.create(50, 300);
        s3 = Street.defaultStreet("third", c1, c2);        
        Assertions.assertNotNull(s3, "Ulice third - lze vytvorit");
        
        c1 = Coordinate.create(130, 200);
        stop1 = Stop.defaultStop("stop1", c1);
        s1.addStop(stop1);
        c1 = Coordinate.create(80, 300);
        stop3 = Stop.defaultStop("stop3", c1);
        s3.addStop(stop3);
        
        line1 = Line.defaultLine("10");
        Assertions.assertTrue(line1.addStop(stop1), "Prvni vlozeni zastavky vzdy uspesne");
        Assertions.assertFalse(line1.addStop(stop3), "Stop3 nelze vlozit - ulice nenavazuji");
        assertRoute(line1, "first:stop(stop1);");
        Assertions.assertTrue(line1.addStreet(s2), "Ulice second navazuje, bez zastavky");
        assertRoute(line1, "first:stop(stop1);second:null;");
        Assertions.assertTrue(line1.addStop(stop3), "Vlozeni stop3, ulice third navazuje");
        assertRoute(line1, "first:stop(stop1);second:null;third:stop(stop3);");
        
        testImmutableRoute(line1);
    }
    
    /* Oveřuje správnou sekvenci trasy linky. Záznamy jsou transformovány do řetězce podle níže uvedeného vzoru.
     * Pro street je využita metoda getId() - reprezentace pouze identifikátorem.
     * Pro zastávku je využita implicitní konverze na String - reprezentace řetězcem "stop(id)" nebo "null" (pokud zastávka není).
     */
    private void assertRoute(Line line, String expected) {
        String res = line.getRoute().stream()
            .map(entry -> entry.getKey().getId() 
                    + ":" 
                    + entry.getValue()
                    + ";")
            .collect(Collectors.joining());
        //System.out.println(res);        
        Assertions.assertEquals(res, expected, "Reprezentace cesty linky.");
    }

    /* Test správného vytvoření defenzivní kopie / nemodifikovatelného seznamu v metodě Line.getRoute().
     * Pokud není vytvořená defenzivní kopie / nemodifikovatelný seznam, je možné trasu modifikovat zvenku, 
     * což je nežádoucí stav.
     * Podobně by mělo být i pro Street.getCoordinates() -- není v úkolu testováno.
     */
    private void testImmutableRoute(Line line) {
        try {
            line.getRoute().remove(0);
        } catch(UnsupportedOperationException ex) {}
        assertRoute(line, "first:stop(stop1);second:null;third:stop(stop3);");
    }
}
