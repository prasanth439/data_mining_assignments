#include<bits/stdc++.h>
#include<fstream>
using namespace std;

void fn(int i,int end,vector<int> temp,vector<int>& inp)
{
    if(end == inp.size()-1)
    {
        temp.push_back(1);
        int size = temp.size()-1;
        for(int j=i;j<=end;j++)
        {
            temp[size] = inp[j];
            string tempo =  "";
            for(int l=0;l<temp.size()-1;l++)
            {
                tempo+=to_string(temp[l])+" ";
            }
            tempo+=to_string(temp[size]);
            // search in map
        }
        return ;
    }
    temp.push_back(1);
    int size = temp.size()-1;
    for(int j=i;j<=end;j++)
    {

        temp[size] = inp[j];
        fn(j+1,end+1,temp,inp);
    }
}
int main (int argc, const char* argv[])
{
    // string file_name = "Helloworld1.temp";
    // ofstream ofle(file_name, ios::app);
    // ofle<<"Happy Raksha Bandan"<<endl;

    set<string> myset;
    myset.insert("1");
    myset.insert("2");
    myset.insert("1");
    cout<< myset.size();
    return 0;
}