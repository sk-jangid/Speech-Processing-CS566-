// LBG.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <math.h> 
#include <string>
#include<time.h>

using namespace std;

// codebook size
long cb_size = 8;  // cb_size = k
ifstream fin;
ofstream cb;
int m = 0;

// To Store Nodes
class Node{
public:
	vector<long double>ceps;
	int cluster;
};
// To Store Clusters
class Cluster{
public:
	int count;
	vector<long double>centeroid;

};
// Universe
vector<Node*> universe;
vector<Node*>codebook;
vector<Cluster*> clusters;
vector<Cluster*> temp_clusters;

// To Read Universe file and store values
void read_universe(char* universe_file){
	fin.open(universe_file, ios::in); 
	string t, line, cep_value;
	if(!fin)
	{
		cout << "File "<<universe_file<<" not found\n";
		fin.close();
		system("pause");
	}
	while(!fin.eof()){
		getline(fin, line);
		stringstream s(line);
		Node* temp = new Node();
		temp->cluster = 0;
		int i = 0;
		while (getline(s, cep_value, ',')) { 
            (temp->ceps).push_back(stold(cep_value.c_str())); 
			i++;
			
        }
		universe.push_back(temp);
	}
	universe.pop_back();
	fin.close();
	
}

// Initialize Variables and Codebook
void initialization(){

	for(int i = 0; i<cb_size; i++){
		Cluster* temp = new Cluster();
		Cluster* temp2 = new Cluster();
		temp->count = 0;
		temp2->count = 0;
		for(int j = 0; j<12; j++){
			temp->centeroid.push_back(0.0);
			temp2->centeroid.push_back(0.0);
		}
		clusters.push_back(temp);
		temp_clusters.push_back(temp2);
	}



}

// Calculate Tokhura Distance
 long double tokhura_distance(vector<long double>A, vector<long double> B){
	// Initialize tokhura coefficients which are already provided
	long double tokhura_coeffs[] = { 1.0, 3.0, 7.0, 13.0, 19.0, 22.0, 25.0, 33.0, 42.0, 50.0, 56.0, 61.0};
	// Initialize distance to zero
	long double distance = 0.0;

	// Just to check if both vector have same shape
	if(A.size()!=B.size()){
		return 0;
	}
	// Get Distance
	for(unsigned int i=0;i<A.size()&&i<B.size();i++){
		distance+=tokhura_coeffs[i]*(A[i]-B[i])*(A[i]-B[i]);
	}
	// Return distance
	return distance/(long double)12.0;
}

// Classification Step 2
long double classification(){
	
	long double distortion = 0.0;

	for(unsigned int i = 0; i<temp_clusters.size();i++){
		temp_clusters[i]->count = 0;
		for(unsigned int j = 0;j<(temp_clusters[i]->centeroid).size();j++){
			temp_clusters[i]->centeroid[j] = 0.0;
		}
	}
	int c = 0;
	for(unsigned int i=0;i<universe.size();i++){
		int flag = 0;
		long double temp = 0.0;
		int temp_index = 0;
		for(unsigned int j=0; j<m; j++){
			long double tok_distance = (tokhura_distance(universe[i]->ceps, codebook[j]->ceps));
			if(flag==0){
				temp = tok_distance;
				temp_index = j;
				flag = 1;
			}else if(tok_distance< temp){
				temp = tok_distance;
				temp_index = j;
			}
		}
		distortion = distortion + temp;
		universe[i]->cluster = temp_index;
		
		(temp_clusters[temp_index])->count = (temp_clusters[temp_index])->count + 1;
		for(int j=0; j<12;j++){
			(temp_clusters[temp_index])->centeroid[j] = (temp_clusters[temp_index])->centeroid[j] + (universe[i])->ceps[j];
		}

	
	}
	// update Clusters
	for(int i=0;i<m;i++){
		for(int j = 0;j<12;j++){
			if(temp_clusters[i]->count == 0){	// Avoid Empty cell problem
				int max = 0, max_index=0;
				int flag = 0;
				// Get Cluster with maximum vectors
				for(int k=0;k<m;k++) {
					if(flag == 0){
						flag = 1;
						max = temp_clusters[k]->count;
						max_index = k;
						continue;
					}
					if(max<temp_clusters[k]->count){
						max_index = k;
						max = temp_clusters[k]->count;
					}
				}
				// Update Cepstarl value of the Empty cell on the basis of maximum vector cluster 
				for(int k=0;k<12;k++) 
				{
					if(i<m/2)
						clusters[i]->centeroid[j]=(1-2.0*0.01)/(1-0.03)*temp_clusters[max_index]->centeroid[k]/(long double)temp_clusters[max_index]->count;
					else
						clusters[i]->centeroid[j]=(1+2.0*0.01)/(1+0.03)*temp_clusters[max_index]->centeroid[k]/(long double)temp_clusters[max_index]->count;
				}
				break;
			
			}
			// Update Cluster Representor values
			clusters[i]->centeroid[j] = temp_clusters[i]->centeroid[j]/(long double)temp_clusters[i]->count;
		
		}
	
	
	}
	// return distortion
	return distortion;

}


// Function to get first vector 
// Codebook of size 1
void first_centroid()
{
	Cluster* temp = new Cluster();
	clusters[m] = temp;
	temp_clusters[m] = temp;
	for(int j = 0 ;j<12;j++){
		clusters[m]->count = 0;
		clusters[m]->centeroid.push_back(0.0);
		temp_clusters[m]->count = 0;
		temp_clusters[m]->centeroid.push_back(0.0);
	}

	clusters[0]->count = 0;

	for(int i = 0; i<12;i++){
		(clusters[0])->centeroid[i] = 0;
	}

	for(unsigned int i=0; i<universe.size();i++){
		(clusters[0])->count = (clusters[0])->count +1;
		for(int j=0;j<12;j++){
			(clusters[0])->centeroid[j] = (clusters[0])->centeroid[j]+universe[i]->ceps[j];
		}
	}

	// Get Average of all the Cepstrals to get centroid of the universe
	for(int j=0;j<12;j++){
		(clusters[0])->centeroid[j] = (clusters[0])->centeroid[j] / (long double)(clusters[0])->count;
	}

	cout<<"Initial Centroid\n";
	for(int j=0;j<12;j++){
		cout<<(clusters[0])->centeroid[j]<<" ";
	}
	Node* temp2 = new Node();
	for(int j=0;j<12;j++){
		temp2->ceps.push_back((clusters[0])->centeroid[j]);
	}
	codebook.push_back(temp2);
	cout<<"\n\n";
	cout<<"Centroid generated\n\n";
	// Increase codebook size
	m = m+1;
}

// Function to split codebook
void split_codebook(){
	int old_size = m;
	// Doube the codebook size
	m = m*2;

	cout<<"\nCodebook Splitted to size: "<<m<<"\n";
	for(int i=0;i<old_size;i++)
	{
		clusters[old_size+i]=new Cluster();
		temp_clusters[old_size+i]=new Cluster();
		for(int j = 0 ;j<12;j++){
			clusters[old_size+i]->count = 0;
			(clusters[old_size+i]->centeroid).push_back(0.0);
			temp_clusters[old_size+i]->count = 0;
			(temp_clusters[old_size+i])->centeroid.push_back(0.0);
		}
		Node* temp = new Node();
		temp->cluster = codebook.size();
		for(unsigned int j=0;j<12;j++){
			temp->ceps.push_back(clusters[old_size+i]->centeroid[j]);
		}
		codebook.push_back(temp);
	}
	// Update the values in the codebook values which are added new
	for(int i=0;i<old_size;i++){
		for(int j=0;j<12;j++){
			clusters[old_size+i]->centeroid[j] = (1+0.03)*(clusters[i]->centeroid[j]);
			codebook[old_size+i]->ceps[j] =  (1+0.03)*(codebook[i]->ceps[j]);
		}
		for(int j=0;j<12;j++){
			clusters[i]->centeroid[j] = (1-0.03)*(clusters[i]->centeroid[j]);
			codebook[i]->ceps[j] =  (1-0.03)*(codebook[i]->ceps[j]);
		}
	}
}

// Update Codebook according to the value of cluster
void update_codebook(){
	for(int i = 0;i<codebook.size();i++){
		for(int j = 0;j<12;j++){
			codebook[i]->ceps[j] = clusters[i]->centeroid[j];
		}
	
	}

}

// LBG algorithm
void LBG(long double delta){
	int M = cb_size;
	initialization();
	// Step 1
	first_centroid();
	long double new_distortion = 0.0;
	long double old_distortion = 0.0;
	int i;
	do
	{
		i = 0;
		new_distortion = 0.0;
		// split codebook
		split_codebook();
		// Apply k means untill to get optimal codebook of size m
		do
		{
			old_distortion = new_distortion;
			new_distortion = classification();
			cout<<"Iteration: "<<i+1<< "; Distortion: " << new_distortion << "\n";
			update_codebook();
			i++;

		}while(abs(new_distortion - old_distortion) > delta);
	}while(m < M);

	
}

// Display and store codebook.
void display_codebook(){
	cb.open("codebook.txt");
	cb <<"Codebook values for LBG are:"<<  "\n";
	cout <<"Codebook values for LBG are:"<<  "\n";
	for(unsigned int i=0;i<codebook.size();i++){
		for(unsigned int j =0;j<(codebook[i]->ceps).size(); j++){
			cout<< (codebook[i])->ceps[j]<<" ";
			cb << (codebook[i])->ceps[j]  <<  " ";
		}
		cout<<"\n";
		cb << "\n";
	}

}


int _tmain(int argc, _TCHAR* argv[])
{
	// First Read universe
	read_universe("Universe.csv");
	cout<<"Total Cepstarl values in the Universe are: "<<universe.size()<<"\n";
	cout<<"universe read done\n";
	// Aplly LBG
	LBG(0.03);
	// Display Codebook
	display_codebook();
	return 0;
}


