// package apriori;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Map;
import java.util.Set;

public class GlobalVars {
	
	public static HashMap<String, Integer> frequent_itemset_count = new HashMap<String, Integer>();
	public static Set<String> frequent_itemsets = new HashSet<>();
	public static int MIN_SUPPORT = 1;
	
	public static void printFrequentItems(){
		Iterator hmIterator = frequent_itemset_count.entrySet().iterator();   
        while (hmIterator.hasNext()) { 
            Map.Entry mapElement = (Map.Entry)hmIterator.next(); 
            System.out.println(mapElement.getKey() + " => " + mapElement.getValue()); 
        } 	
	}

}
