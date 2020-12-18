// k-means.cpp : Defines the entry point for the console application.
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

long cb_size = 8;  // cb_size = k
ifstream fin; 
ofstream cb;

// Structure to store Nodes
class Node{
public:
	vector<long double>ceps;
	int cluster;
};

// Structure to store Cluster information center and number of vectors in it
class Cluster{
public:
	int count;
	vector<long double>centeroid;

};

// Store universe (All vectors)
vector<Node*> universe;
// Codebook Veectors Centeres
vector<Node*>codebook;
// Clusters
vector<Cluster*> clusters;


// Read universe and store 
void read_universe(char* universe_file){
	fin.open(universe_file, ios::in); 
	string t, line, cep_value;
	if(!fin) //display error message if we cant open the file
    {
        cout << "File "<<universe_file<<" not found\n";
	    fin.close();
		system("pause");
    }
	// read csv file line by line
	while(!fin.eof()){
		getline(fin, line);
		stringstream s(line);

		Node* temp = new Node();
		temp->cluster = 0;
		//getline(fin, line); 
		//cout<<line<<"\n";
		int i = 0;
		while (getline(s, cep_value, ',')) { 
            (temp->ceps).push_back(stold(cep_value.c_str())); 
			i++;
			
        }
		//Push value to universe
		universe.push_back(temp);
	}
	universe.pop_back();

	fin.close();
	//return universe;
}

// Function to get random vectors from universe initially
vector<int> get_random_indices(int count, int range){
	vector<int> result;
	srand(time(0));
	int i = 0;
	int val;
	if(range<count){
		for(unsigned int j = 0; j<result.size(); j++){
			val = rand() % range;
			result.push_back(val);
		}
		return result;
	}
	
	while(i<count){
		val = rand() % range;
		int j = 0;
		for(j = 0; j<result.size(); j++){
			if(result[j] == val){
				break;
			}
		}

		if(j == i){
			result.push_back(val);
			i++;
		}
	}
	return result;
}

// Initialize Vectors and cluster
void initialization(){

	vector<int> random_indices = get_random_indices(cb_size, universe.size());
	cout<<"Randomly Choosen vectors indices are: ";
	for(int i = 0; i<cb_size; i++){
		Node* temp = new Node();
		temp->cluster = i;
		cout<<random_indices[i]<<", ";
		for(int j = 0; j<12; j++){
			
			(temp->ceps).push_back((universe[random_indices[i]])->ceps[j]);
		}
		codebook.push_back(temp);
	}cout<<"\n";

	for(int i = 0; i<cb_size; i++){
		Cluster* temp = new Cluster();
		temp->count = 0;
		for(int j = 0; j<12; j++){
			temp->centeroid.push_back(0.0);
		}
		clusters.push_back(temp);
	}



}
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
	return distance/12.0;
}

 // Step 2 Classification
long double classification(){
	
	long double distortion = 0.0;

	for(unsigned int i = 0; i<clusters.size();i++){
		clusters[i]->count = 0;
		for(unsigned int j = 0;j<(clusters[i]->centeroid).size();j++){
			clusters[i]->centeroid[j] = 0.0;
		}
	}
	// Check for each vector in universe
	for(unsigned int i=0;i<universe.size();i++){
		int flag = 0;
		long double temp = 0.0;
		int temp_index = 0;
		// Get minimum Tokhura distance in all the clusters
		for(unsigned int j=0; j<codebook.size(); j++){
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
		// Modify cluster values
		(clusters[temp_index])->count = (clusters[temp_index])->count + 1;
		for(int j=0; j<12;j++){
			(clusters[temp_index])->centeroid[j] = (clusters[temp_index])->centeroid[j] + (universe[i])->ceps[j];
		}

	
	}
	return distortion;

}
// Update Code vectors
void update_code_vectors(){
	for(unsigned int i=0; i<codebook.size(); i++){
		if((clusters[i])->count==0){
			continue;
		}
		for(int j=0; j<12; j++){
			(codebook[i])->ceps[j] = (clusters[i])->centeroid[j]/(long double)clusters[i]->count;
		
		}
	}
}
// Check if termination is to be done
bool is_termination(long double delta, long double old_d, long double new_d){

	if(abs(old_d-new_d)> delta){
		return false;
	}

	return true;
}


// K means algorithm
void k_means(long double delta){
	int flag = 0;
	long double old_distortion = 0.0;
	long double new_distortion = 0.0;
	int m = 0;
	initialization();
	cout<<"Initialization  done\n";
	bool is_terminate = false;
	while(is_terminate == false){

		new_distortion = classification();
		update_code_vectors();
		if(flag == 0){
			old_distortion = new_distortion;
			is_terminate = false;
			flag = 1;
			m = m+1;
			continue;
		}
		cout<<"Iteration "<<m<<"; Distortion: "<<new_distortion<<"\n";
		is_terminate = is_termination(delta, old_distortion, new_distortion);
		old_distortion = new_distortion;
		m = m+1;
	}
	
}
// Display codebook

void display_codebook(){
	cb.open("codebook.txt");
	cb <<"Codebook values for K-Means are:"<<  "\n";
	cout<<"Codebook values for K-Means are(Also Stored in file coebook.txt):"<<  "\n";
	for(unsigned int i=0;i<codebook.size();i++){
		for(unsigned int j =0;j<(codebook[i]->ceps).size(); j++){
			cout<< (codebook[i])->ceps[j]<<" ";
			cb<<(codebook[i])->ceps[j]<<" ";;
		}
		cout<<"\n";
		cb<<"\n";
	}

}


int _tmain(int argc, _TCHAR* argv[])
{
	read_universe("Universe.csv");
	cout<<"Total vectors in universe: "<<universe.size()<<"\n";
	cout<<"universe read done\n";
	k_means(0.03);
	display_codebook();
	return 0;
}

