import java.io.FileInputStream;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Scanner;
import java.util.Set;
import java.util.StringTokenizer;
import java.util.TreeSet;

public class IterateFile {

	String filepath;
	public static HashMap<String, Integer> itemwisecount = new HashMap<String, Integer>();
	public static Set<String> itemkeylist ;
	public static int MIN_SUPPORT = 4;
	
	public static void main(String[] args){
		IterateFile IF = new IterateFile();
		IF.parseFile(System.getProperty("user.dir") + "/webdocs.dat");
	}
	
	public IterateFile(){
	}
	
	public IterateFile(String path){
		this.filepath = path;
	}
	
	/*
	 * It will scan through the entire file
	 */
	public HashMap<String, Integer> parseFile(String path){
		
		int  k = 1;
		do {
			if (k == 1){
				IterateFileLinebyLine(path, k);
				itemkeylist = itemwisecount.keySet();
				k++;
			} else {
			    itemkeylist = pruning();
			    itemkeylist = generatenextlevelCandidate(k);
				IterateFileLinebyLine(path, k); // change the update itemwise count 
				k++;
			}
		}while (itemkeylist.size() > 0);
		
		return itemwisecount;
	}
	
	/*
	 * It will prune the list.
	 */
	
	public Set<String> pruning(){
		
		Set<String> pruneditemkeylist = new HashSet<String>();
		
		for (String item: itemkeylist){
			if (itemwisecount.containsKey(item) && itemwisecount.get(item) >= MIN_SUPPORT){
				pruneditemkeylist.add(item);
			}
		} 
		System.out.println("Pruned list is as following:" + pruneditemkeylist);
		return pruneditemkeylist;
	}
	
	/*
	 * Need to refine this algorithm
	 */
	public Set<String> generatenextlevelCandidate(int k){
		
		ArrayList<String> list = new ArrayList<String>(itemkeylist);
		Set<String> tempitemkeylist = new HashSet<String>();
		
		String s = null;
		for (int i = 0; i<= list.size()-k; i++){
			s = list.get(i);
			for (int j = i + 1; j < list.size() ; j++){
				StringTokenizer st = new StringTokenizer(list.get(j)," ");  
			     while (st.hasMoreTokens()) {  
			    	 String tempitem = st.nextToken();
			    	 if (!s.contains(tempitem) ){
			    		 tempitemkeylist.add(s + " " + tempitem );
			    	 }
			    }  
			}
			System.out.println("kth level candidates are as following:"+ tempitemkeylist);	
		}
		return tempitemkeylist;
	}
	
	public void updateItemwisecount(String line, int k){
		
		  String itemkey; 
		
		 Iterator<String> iter_itemkey = itemkeylist.iterator();		    	
		  while (iter_itemkey.hasNext()){
			  itemkey = iter_itemkey.next();
			  if (line.contains(itemkey)){
				  if ( itemwisecount.containsKey(itemkey) ) {
					  itemwisecount.put(itemkey, itemwisecount.get(itemkey) + 1);	
				  } else {
					  itemwisecount.put(itemkey, 1);		
				  }			  
			  }
		    }
		    System.out.println("Full HashMap looks like following " + itemwisecount);
		  return;  
	}
	
	/*
	 * This will update the itemwisecount i.e. hashmap
	 * of string and values
	 */
	public void updateItemwisecount(Set<String> set){
		
		 String itemkey; 
		
		  Iterator<String> iter = set.iterator();
		    while (iter.hasNext()) {
		    	itemkey = iter.next();
		        if ( itemwisecount.containsKey(itemkey) ) {
		        	itemwisecount.put(itemkey, itemwisecount.get(itemkey) + 1);	
		        } else {
		        	itemwisecount.put(itemkey, 1);		
		        }
		    }
		    System.out.println("Full HashMap looks like following " + itemwisecount);
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
	     System.out.println("Set generated after parsing the line: "+ set); 
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
			        System.out.println("Line Read is following" + line);
			        System.out.println("Set of element is following" + set);
			        updateItemwisecount(set);
		        }else {
		        	/*
		        	 * Take each line, 
		        	 * Check whether the pruneditemwiselist item matches with substring
		        	 * but for this, first sort the substring i.e. line of the file
		        	 * if yes then update the count in the itemwisecount
		        	 */
		        	updateItemwisecount( line, k);
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



