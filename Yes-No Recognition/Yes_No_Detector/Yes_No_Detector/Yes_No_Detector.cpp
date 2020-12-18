// Yes_No_Detector.cpp : Defines the entry point for the console application.
//

#include "StdAfx.h"
#include <iostream>
#include <string>
#include <fstream>	
#include <vector>
using namespace std;

// Function to get STE of a frame
long double STE(vector<long double> frame){
	long double X2_sum = 0;

	for(unsigned int i=0;i<frame.size();i++){
		X2_sum+=(frame[i]*frame[i]);
	}

	return X2_sum/(long double)frame.size();

}

// Function to get ZCR of a frame
long int ZCR(vector<long double> frame){
	int zcr = 0;
	int  sign;

	if(frame.size()==0){
		return 0;
	}

	if(frame[0]<0.0){
		sign = -1;
	}else{
		sign = 1;
	}

	for(unsigned int i=1;i<frame.size();i++){
		if(sign==1 && frame[i]<0.0){
			zcr++;
			sign=-1;
		}else if(sign==-1 && frame[i] >0.0){
			zcr++;
			sign = 1;
		}
	}

	return zcr;

}

// Function to extract words from the waveform. Return starting and end point of word
vector<pair<int,int>> extract_words(vector<long double>ste_values, long double threshold_energy){

	vector<pair<int,int>> words; // store start and end of words

	for(unsigned int i = 0;i<ste_values.size()-1;i++){
		if(ste_values[i]>threshold_energy && ste_values[i+1]>threshold_energy){			// Marks Starting of a word
			unsigned int j,start = i,end;
			for(j=i+1;j<ste_values.size()-1;j++){
				if(ste_values[j]<threshold_energy && ste_values[j+1] < threshold_energy){
					break;
				}
			}
			end= j-1;																	// Marks End of a word
			i=end;				
		
			words.push_back(make_pair(start,end));										// Store Starting and ending frame of a word
		}
	}
	return words;
}

// Function to get threshold energy based on a silence.txt file stored in the directory
long double get_threshold_energy(string fileName){

	long double temp_ste = 0;

	ifstream file(fileName);
	string line;
	int i = 0;

	// Ignoring first 6 lines
	while(getline(file,line)){
		i++;
		if(i==6){break;}
	}
	
	long double X2 = 0;
	i=0;
	// Take input and calculate STE
	while(getline(file,line)){
		i++;
		X2 = X2 + stof(line) * stof(line);
	}
	return 5*(X2/i);				// Threshold is 5 times of noise

}


int main(int argc, _TCHAR* argv[])
{
	
	long double threshold_energy;

	while(1){ 
		printf("Input Silence File Name to Capture Silence: ");
		string silenceFileName ="";
		cin>>silenceFileName;
	// Get Threshold Energy
		threshold_energy = get_threshold_energy(silenceFileName);
		if(threshold_energy!=0.0){
			break;
		}
	}
	// Loop to get Input file
	while(1){ 
		printf("Input File Name to Detect Words: ");
		string fileName ="";
		cin>>fileName;
		ifstream file(fileName);
		string line;int a = 0;
	
		//Ignore first 6 lines
		int i = 0;
		while(getline(file,line)){
			i++;
			if(i==6){break;}
		}
		
		int frame = 1;
		vector<long double>sample_values;
		vector<long double>ste_values;
		vector<int>zcr_values;

		// Take input and store STE and ZCR of frames
		// Frame size : 20ms or 320 Samples
		while(getline(file,line)){
			i++;
			sample_values.push_back(stof(line));
			if(sample_values.size()==320){
				
				ste_values.push_back(STE(sample_values));
				zcr_values.push_back(ZCR(sample_values));
				sample_values.clear();
			}
		}
		if(ste_values.size()==0){
			printf("File Not Present. Try Again!\n");
			continue;
		}


		
		
		// Extract words and get Starting and ending frames of words
		vector<pair<int,int>>words = extract_words(ste_values,threshold_energy);
		
		// Loop for Each word and check if it is a yes or no
		for(unsigned int i=0;i<words.size();i++){
			
			long int ave_initial_zcr = 0,ave_20_zcr = 0;
			int word_size = words[i].second - words[i].first + 1;

			// For Checking a word if it is a yes or no i am taking average ZCR of first half of the word and comparion it with last 20 % of ZCR
			for( int j=words[i].first;j<words[i].first+word_size/2;j++){
					ave_initial_zcr+=zcr_values[j];
					//printf("%ld,",zcr_values[j]);
			}
			ave_initial_zcr = ave_initial_zcr/(word_size/2);

			// Average ZCR of last 20 % of Word
			for(int j=words[i].first+((4*word_size)/5);j<=words[i].second;j++){
					ave_20_zcr+=zcr_values[j];
			}
			
			ave_20_zcr = ave_20_zcr/(word_size/5);
			
			// if Last 20 % ZCR is more than 4 times that of first half then its a "YES" otherwise "NO"
			if(ave_20_zcr >= 4*ave_initial_zcr){
				printf("Word Detected: Yes | Statrted at time %.2lf ms | End at Time %.2lf ms\n\n",(double)(words[i].first)*20.0,(double)(words[i].second)*20.0);
			}else{
				printf("Word Detected: No  | Statrted at time %.2lf ms | End at Time %.2lf ms\n\n",(double)(words[i].first)*20.0,(double)(words[i].second)*20.0);
			}
		} 
	}
	return 0;
}



