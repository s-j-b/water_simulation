#include <iostream>
#include <fstream>

using namespace std;

/* A sample program that shows how to parse solarData.txt and print contents */

int main(){
  ifstream infile;
  string name;
  float rad, a, tau, ecc, incl, day, tilt;
  char buf[1024];
  infile.open("solarData.txt");
  //read header lines
  infile.getline(buf,1024);
  cout << buf << endl; 
  infile.getline(buf,1024);
  cout << buf << endl;
  name = ""; 
  while(!infile.eof()){
    infile >> name >> rad >> a >> tau >> ecc >> incl >> day >> tilt;
    if(!infile.eof()){
      printf("%10s %8.2f %6.0f %7.2f %5.2f %5.2f %8.2f %8.2f\n", 
					name.c_str(), rad, a, tau, ecc, incl, day, tilt);
    }
  }
  return 0;
}
