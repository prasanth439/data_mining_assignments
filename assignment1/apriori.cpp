#include<bits/stdc++.h>
#include<fstream>
using namespace std;

int main(int argc,const char* argv[])
{
    int X;
    string nNum;
    string out_file,in_file;
    if (argc==3)
    {
        out_file = argv[2];
        nNum = argv[1];
        X = stoi(nNum);
    }
    else{
        cerr<<"Insufficient or More arguments\n";
    }
    in_file = "webdocs.dat";
    ifstream ifile(in_file);
    int temp;
    ifile>>temp;
    ifile>>temp;
    ifile>>temp;
    ifile>>temp;
    ifile>>temp;
    ifile>>temp;
    cout<<temp<<" from file\n";
    return 0;
}