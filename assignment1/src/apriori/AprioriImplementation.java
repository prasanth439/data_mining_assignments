// package apriori;

import java.io.FileInputStream;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Map;
import java.util.Scanner;
import java.util.Set;
import java.util.StringTokenizer;
import java.util.TreeSet;

public class AprioriImplementation {

	String filepath;

	/*
	 * MAIN METHOD
	 * 1. CALL TO FILE PARSING METHOD
	 * 2. CALL TO PRINT FREQUENT ITEM SETS 
	 */
	public static void main(String[] args){
		AprioriImplementation IF = new AprioriImplementation();
		IF.parseFile(System.getProperty("user.dir") + "/sirinput.txt");
		System.out.println(System.getProperty("user.dir"));
		GlobalVars.printFrequentItems();	
	}
	
	/*
	 * CONSTRUCTOR METHOD
	 */
	public AprioriImplementation(){
	}

	/*
	 * OVERLOADED CONSTRUCTOR METHOD
	 */
	public AprioriImplementation(String path){
		this.filepath = path;
	}
	
	/*
	 * PARSE FILE METHOD
	 * THE DO WHILE WILL ENSURE THAT METHOD RUNS UNTIL 
	 * THERE IS AT LEAST ONE KTH LEVEL ITEM
	 * 
	 *  FOR FIRST ITERATION
	 *  1. ITERATE THROUGH FILE LINE BY LINE
	 *  2. MAKE THE FREQUENT_ITEM_COUNT
	 *  
	 *  FOR SECOND LEVEL ONWARDS
	 *  1. PRUNE THE ITEMSET LIST
	 *  2. GENERATE NEW CANDIDATES
	 *  3. FIND MIN SUPPORT COUNT FOR NEW CANDIDATES
	 *  4. REPEAT THE ABOVE THREE STEPS
	 *  
	 */
	public HashMap<String, Integer> parseFile(String path){
		
		int  k = 1;
		do {
			if (k == 1){
				IterateFileLinebyLine(path, k);
				GlobalVars.frequent_itemsets = GlobalVars.frequent_itemset_count.keySet();
				k++;
			} else {
				GlobalVars.frequent_itemsets = pruning();
				GlobalVars.frequent_itemsets = generatenextlevelCandidate(k);
				IterateFileLinebyLine(path, k); // change the update itemwise count 
				k++;
			}
		}while (GlobalVars.frequent_itemsets.size() > 0);
		
		return GlobalVars.frequent_itemset_count;
	}
	
	/*
	 * PRUNING METHOD: 
	 * THIS WILL REMOVE ALL THE FREQUENT ITEM SET FOR 
	 * WHICH MIN-SUPPORT CRITERIAN IS NOT FULFILLED.
	 * 
	 */
	public Set<String> pruning(){
		
		Set<String> pruneditemkeylist = new HashSet<String>();
		for (String item: GlobalVars.frequent_itemsets){
			if (GlobalVars.frequent_itemset_count.containsKey(item) 
					&& GlobalVars.frequent_itemset_count.get(item) >= GlobalVars.MIN_SUPPORT){
				pruneditemkeylist.add(item);
			}
		} 
		return pruneditemkeylist;
	}
	
	/*
	 * GENERATE_NEXT_LEVEL_CANDIDATE
	 * FOR ALL THE PRUNED FREQUENT ITEM SETS,
	 * FOR EACH ITEM:
	 * 1. TAKE THE STRING AT INDEX I
	 * 2. TAKE THE STRING AT INDEX J
	 * 3. TOKENIZE THE STRING AT INDEX J
	 * 4. CHECK IF THE TOKEN ALREADY EXISTS IN THE INDEX I
	 * 5. IF NO, THEN CONCATENATE THE TOKEN WITH THE STRING AT INDEX I 
	 * 6. REPEAT THE SEQUENCE
	 */
	public Set<String> generatenextlevelCandidate(int k){
		
		ArrayList<String> list = new ArrayList<String>(GlobalVars.frequent_itemsets);
		Set<String> tempitemkeylist = new HashSet<String>();
		String tempitem = null;
		
		String s, s1 = null;
		for (int i = 0; i< list.size()-1; i++){
			s = list.get(i);
			
			for (int j = i + 1; j < list.size() ; j++){
				s1 = list.get(j);
				if (  optimizeApriori(s, s1, k)){
					tempitem = s1.substring(s1.lastIndexOf(" ") + 1);
			    	 if (!s.contains(tempitem) ){
			    		 String temp = sortString(s + " " + tempitem);
			    		 if (!tempitemkeylist.contains(temp)){
			    			 tempitemkeylist.add(temp );
			    		 }
			    	 }
				}
			}
		}
		return tempitemkeylist;
	}
	
	/*
	 * Check first k-1 words
	 * if similar then generate next level candidate
	 */
	public boolean optimizeApriori(String s, String s1, int k){
		
		
		 if (k == 2){
			 return true;
		 }
		
		  String s_temp = s.substring(0, s.lastIndexOf(" "));
		  String s1_temp = s1.substring(0, s1.lastIndexOf(" "));
		  
		  if (s_temp.equals(s1_temp)){
			  return true;
		  } else {
			  return false;
		  }
		  //String lastWord = test.substring(test.lastIndexOf(" ") + 1);
	}
	
	/*
	 * SORTSTRING METHOD SO THAT NEW ITEM GENERATED IS 
	 * ARRANGED AS PER LEXICOGRAPHIC ORDER
	 */
	public String sortString(String s){
	    String[] strArray = s.split("\\s+");
	    Arrays.sort(strArray);
	    return String.join(" ", strArray);
	}
	
	/*
	 * UPDATE_FREQUENT_ITEMSET METHOD
	 * IF THE ITEM SET STRING IS PRESENT AS A SUBSEQUENCE IN THE LINE
	 * THEN UPDATE THE COUNT ELSE DO NOT UPDATE THE COUNT
	 * 
	 * ASSUMPTION: LINE IS SORTED INTERNALLY IN LEXICOGRAPHIC ORDER
	 * IF LINE IS NOT SORTED THEN WE SHOULD FIRST SORT THE LINE IN LEXICOGRAPHIC ORDER
	 * 
	 * FOR EACH LINE, CHECK ALL THE FREQUENT ITEMSETS PRESENCE 
	 */
	public void updateFrequentItemSet(String line, int k){
		
		  String itemkey; 
		
		 Iterator<String> iter_itemkey = GlobalVars.frequent_itemsets.iterator();		    	
		  while (iter_itemkey.hasNext()){
			  itemkey = iter_itemkey.next();
			 // if (line.contains(itemkey)){
			if (isSubSequence( itemkey, line,  itemkey.length(), line.length())) {	
			  if ( GlobalVars.frequent_itemset_count.containsKey(itemkey) ) {
					  GlobalVars.frequent_itemset_count.put(itemkey, GlobalVars.frequent_itemset_count.get(itemkey) + 1);	
				  } else {
					  GlobalVars.frequent_itemset_count.put(itemkey, 1);		
				  }			  
			  }
		    }
		    //System.out.println("Full HashMap looks like following " + GlobalVars.frequent_itemset_count);
		  return;  
		}
	
		/*
		 * SUBSEQUENCE METHOD TO CHECK IF STRING 1 IS SUBSEQUENCE OF STRING 2
		 */
	   static boolean isSubSequence(String str1, String str2, int m, int n) 
	    { 
	        // Base Cases 
	        if (m == 0)  
	            return true; 
	        if (n == 0)  
	            return false; 
	              
	        // If last characters of two strings are matching 
	        if (str1.charAt(m-1) == str2.charAt(n-1)) 
	            return isSubSequence(str1, str2, m-1, n-1); 
	  
	        // If last characters are not matching 
	        return isSubSequence(str1, str2, m, n-1); 
	    } 
	
	
	/*
	 * THIS METHOD IS USEFUL FOR FIRST TIME GENERATION OF
	 * FREQUENT ITEM SETS 
	 */
	public void updateFrequentItemSet(Set<String> set){
		
		 String itemkey; 
		
		  Iterator<String> iter = set.iterator();
		    while (iter.hasNext()) {
		    	itemkey = iter.next();
		        if ( GlobalVars.frequent_itemset_count.containsKey(itemkey) ) {
		        	GlobalVars.frequent_itemset_count.put(itemkey, GlobalVars.frequent_itemset_count.get(itemkey) + 1);	
		        } else {
		        	GlobalVars.frequent_itemset_count.put(itemkey, 1);		
		        }
		    }
		    //System.out.println("Full HashMap looks like following " + GlobalVars.frequent_itemset_count);
		  return;  
	}
	
	
	/*
	 * This method will take a line
	 * Tokenize it on the space 
	 * Sort all the values
	 * and put them in tree ordered set and return
	 * 
	 */
	public Set<String> parseLine(String line){
	
		Set<String> set = new HashSet<String>();

		StringTokenizer st = new StringTokenizer(line," ");  
	     while (st.hasMoreTokens()) {  
            set.add(st.nextToken());		      
	     }  
	     //System.out.println("Set generated after parsing the line: "+ set); 
	     TreeSet<String> sortedSet = new TreeSet<String>(set);
	    // System.out.println("The sorted list is:");
	    // System.out.println(sortedSet);

		return sortedSet;
	}
	
	/*
	 * This function will read the file line by line without 
	 * keeping all the lines in main memory at once.
	 * For big GB size files also, it will cost hardly 100-150 MBs  
	 * 
	 * https://www.baeldung.com/java-read-lines-large-file
	 */
	public void IterateFileLinebyLine(String path, int k){
		
		FileInputStream inputStream = null;
		Scanner sc = null;
		Set<String> set = null;
		
		try {
		    inputStream = new FileInputStream(path);
		    sc = new Scanner(inputStream, "UTF-8");
		    while (sc.hasNextLine()) {
		        String line = sc.nextLine();
		        // System.out.println(line);
		        /*
		         * CALL THE PARSER TO PARSE THE LINE
		         */
		        if (k == 1){
			        set = this.parseLine(line);
			        //System.out.println("Line Read is following" + line);
			        //System.out.println("Set of element is following" + set);
			        updateFrequentItemSet(set);
		        }else {
		        	/*
		        	 * Take each line, 
		        	 * Check whether the pruneditemwiselist item matches with substring
		        	 * but for this, first sort the substring i.e. line of the file
		        	 * if yes then update the count in the GlobalVars.frequent_itemset_count
		        	 */
		        	updateFrequentItemSet( line, k);
		        }
		    }
		    
		    /*
		     * This is the set corresponding to one line
		     * Now we need to do the same for all the lines
		     * And then we should merge this
		     */
		    
		    // note that Scanner suppresses exceptions
		    if (sc.ioException() != null) {
		        throw sc.ioException();
		    }
		}catch (Exception e) 
		{
			System.out.println("Exception occurred");
			e.printStackTrace();
		}
		finally {
		    if (inputStream != null) {
		    	try{
			    	inputStream.close();		    		
		    	} catch (Exception e){
					System.out.println("Exception occurred");
					e.printStackTrace();		    		
		    	}
		    }
		    if (sc != null) {
		        sc.close();
		    }
		}
	}	
}
