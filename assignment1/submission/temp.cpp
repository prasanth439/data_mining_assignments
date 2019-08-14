#include<bits/stdc++.h>
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
int main(){
    vector<int> input = {1,2,3,4,5,6,7,8};
    vector<int> temp;
    int k;
    fn(0,input.size()-k,temp,input);
}