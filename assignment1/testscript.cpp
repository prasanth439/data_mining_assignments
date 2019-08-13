#include <iostream>
#include <fstream>
using namespace std;

int main (int argc,const char* argv[]) {

  	ofstream myfile;
	string filename;
	int num_of_trans,range;
  	if (argc==4)
	{
		filename = argv[1];
		num_of_trans = atof(argv[2]);
		range = atof(argv[3]);
	}
	else{
		cout<<"arg error"<<endl;
	}
  	myfile.open (filename);
  
    while(num_of_trans--){
		int count = rand()%range;
		myfile << rand()%range;
		for(int i =0;i<count+2;i++)
			myfile << " ";
			myfile << rand()%range;
		myfile<<endl;
	}
  	myfile.close();
  	return 0;
}