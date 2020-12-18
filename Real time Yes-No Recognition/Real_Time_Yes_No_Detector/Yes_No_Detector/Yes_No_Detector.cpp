// Yes_No_Detector.cpp : Defines the entry point for the console application.
//

#include "StdAfx.h"
#include <iostream>
#include <string>
#include <fstream>	
#include <stdlib.h> 
#include <conio.h>
#include <sstream>
#include <vector>
#include <windows.h>



using namespace std;
std::ostringstream oss;

// dc shift
long double dc_shift = 0;


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

	for(unsigned int i = 0;i<ste_values.size()-2;i++){
		if(ste_values[i]>threshold_energy && ste_values[i+1]>threshold_energy && ste_values[i+2]>threshold_energy){			// Marks Starting of a word
			
			unsigned int j,start = i,end;
			for(j=i+1;j<ste_values.size()-2;j++){
				
				if(ste_values[j]<threshold_energy && ste_values[j+1] < threshold_energy && ste_values[j+2] < threshold_energy){
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
long double get_threshold_energy(){

	string filename = "noise.txt";
	ifstream file(filename);
	string line;
	int i = 0;
	long double X2 = 0;
	i=0;
	dc_shift = 0;
	// Take input and calculate STE
	while(getline(file,line)){
		i++;
		X2 = X2 + stof(line) * stof(line);
		dc_shift = dc_shift + stof(line);
	}
	dc_shift = dc_shift/i;
	return 10*(X2/i);				// Threshold is 10 times of noise

}


int main(int argc, _TCHAR* argv[])
{
	
	long double threshold_energy;

	oss << "start Recording_Module/Recording_Module.exe 3 input_w.wav input_t.txt"; //define the string manually, because macro cant expand inside string
	system(oss.str().c_str());
	//sleep(6000);
	//_sleep(10000);
	Sleep(5000);
	//printf("Input Silence File Name to Capture Silence: ");
	
	// Get Threshold Energy
	threshold_energy = get_threshold_energy();
	cout<<"Threshold Energy: "<<threshold_energy<<"\n";
	cout<<"DC Shift: "<<dc_shift<<"\n";
	

	// get input from stored file	
	string filename ="input_t.txt";
	ifstream file(filename);
	string line;
	int i = 0;
		
	int frames = 0,drop_samples = 1000;	// drop some initial samples to avoid electrical impulse
	long double max_amp = 0,sample_value;
	vector<long double>sample_values;
	vector<long double>ste_values;
	vector<int>zcr_values;

	// Take input and store STE and ZCR of frames
	// Frame size : 20ms or 320 Samples
	while(getline(file,line)){
		
		if(line==""){
			continue;
		}
		i++;
		if(i<drop_samples){continue;}
		sample_value = stof(line) - dc_shift;
		sample_values.push_back(sample_value);
		if(sample_values.size()==320){
			ste_values.push_back(STE(sample_values));
			zcr_values.push_back(ZCR(sample_values));
			frames++;
			sample_values.clear();
		}
		if(max_amp<sample_value){
			max_amp = sample_value;
		}

		if(max_amp <-1*sample_value){
			max_amp = -1*sample_value;
		}
	}

	if(ste_values.size()==0){
		printf("File Not Present. Try Again!\n");
		return 0;
	}

	// Normalize samples to 10000, means we only have to do it for ste's since only these values are affected by taking ratio of the samples
	for(unsigned int j=0;j<ste_values.size();j++){
		ste_values[j]  = ste_values[j]*((10000.0*10000.0)/(max_amp*max_amp));
	}
	threshold_energy = threshold_energy*((10000.0*10000.0)/(max_amp*max_amp));
	cout<<"Max Amplitude: "<< max_amp<<"\n";

		
	// Extract words and get Starting and ending frames of words
	vector<pair<int,int>>words = extract_words(ste_values,threshold_energy);
		
	// Loop for Each word and check if it is a yes or no
	for(unsigned int i=0;i<words.size();i++){
			
		long int ave_initial_zcr = 0,ave_20_zcr = 0;
		int word_size = words[i].second - words[i].first + 1;

		// For Checking a word if it is a yes or no i am taking average ZCR of first half of the word and comparing it with last 20 % of ZCR
		for( int j=words[i].first;j<words[i].first+word_size/2;j++){
				ave_initial_zcr+=zcr_values[j];
				
		}
		ave_initial_zcr = ave_initial_zcr/(word_size/2);

		// Average ZCR of last 20 % of Word
		for(int j=words[i].first+((4*word_size)/5);j<=words[i].second;j++){
				ave_20_zcr+=zcr_values[j];
		}
			
		ave_20_zcr = ave_20_zcr/(word_size/5);
			
		// if Last 20 % ZCR is more than 3 times that of first half then its a "YES" otherwise "NO"
		
		if(ave_20_zcr >= 3*ave_initial_zcr){
			printf("Word Detected: Yes | Statrted at time %.2lf ms | End at Time %.2lf ms\n\n",(double)(words[i].first)*20.0,(double)(words[i].second)*20.0);
		}else{
			printf("Word Detected: No  | Statrted at time %.2lf ms | End at Time %.2lf ms\n\n",(double)(words[i].first)*20.0,(double)(words[i].second)*20.0);
		}
	} 
	
	return 0;
}



