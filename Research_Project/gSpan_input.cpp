#include<cstdio>
#include<cstring>
#include<iostream>
#include<unordered_map>
typedef long int ll; // defined a shortform for long int as ll
using namespace std;
// argv[1] for  support
// argv[2] for input_filename
// argv[3] for output_filename
int main(int argc, const char*argv[])
{
    FILE * inp_file = nullptr; // input scanner
    FILE * out_file = nullptr; // output scanner
    int support = stoi(argv[1]); // support count
    inp_file = fopen(argv[2],"r"); 
    out_file = fopen(argv[3],"w");
    ll  id,nvert,nedge,i,a1,a2;
    int counter = 0,a3;
    unordered_map<string,int> mp; // for mapping the input labels to integer
    string temp;
    ll graph_id = -1;
    while(fscanf(inp_file,"#%ld\n%ld\n",&id,&nvert)!=EOF){
        counter = 0;
        graph_id++; // graph id
        fprintf(out_file,"t # %ld\n",graph_id); // printing the graph id in required format
        for(i=0;i<nvert;i++) // reading the vertices with labels 
        {                    // storing the labels in map with unique integer 
                             // and printing the vertex number and label in required format
            char temp2[100];
            fscanf(inp_file,"%100s\n",temp2);
            temp = string(temp2);
            if(mp.find(temp)==mp.end())
            {
                mp[temp] = counter++;
            }
            fprintf(out_file,"v %ld %d\n",i,mp[temp]);
        }
        fscanf(inp_file,"%ld",&nedge); // reading the edge count
        for(i=0;i<nedge;i++)
        {
            fscanf(inp_file,"%ld %ld %d\n",&a1,&a2,&a3); // reading the three numbers and 
            fprintf(out_file,"e %ld %ld %d\n",a1,a2,a3); // printing the same as they are in same format 
        }
    }
    return 0;
}
