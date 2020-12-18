// Vowel_recognition.cpp : Defines the entry point for the console application.
/*
Roll Number:- 170101064
Name:- Shubham Kumar
*/

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h> 
#include <string>
#include <conio.h>
#include <sstream>
#include <vector>
#include <windows.h>



using namespace std;


// Store value of P and Q
int P = 12;
int Q = 12;
// frame Size
long frame_size = 320;
// Store value of PI
long double pi = 3.141593;
// stream for reading sile
ifstream infile;


/*
 Function to get Alpha coefficients from durbin Algorithm
 Parameters:- R Values
 Return Value:- Alpha Values from Durbin algo
*/
vector<long double> durbin(vector<long double> R){
	// E Stores values of E's, K Stores Value of K's, old_alpha stores value of alpha that was calculated in loop in just previous iteration . These are intermediate variables used in durbins algo.
	vector<long double> E(13, 0.0), K(13, 0.0), alpha(13, 0.0), old_alpha(13, 0.0);
	long double sum;
	// STE should not be zero
	if(R[0] == 0.0){
		return alpha;
	}
	// Apply Durbin Algorithm
	// Initialize E[0] to R[0]
	E[0] = R[0];
	// Iterate loop for P times
	for(int i=1; i<=P; i++){
		// Calculate intermediate sum
		sum = 0.0;
		for(int j=1; j<=i-1;j++){
			sum = sum + old_alpha[j]*R[i-j];
		}
		// Intermediate step in algo
		K[i] = (R[i] - sum)/E[i-1];
		alpha[i] = K[i];
		// update alpha
		for(int j = 1; j<=i-1; j++){
			alpha[j] = old_alpha[j] - (K[i]*old_alpha[i-j]);
		}
		// update E[i]'s
		E[i] = (1-K[i]*K[i])*E[i-1];
		// Store these alpha values in old_alpha
		for(int j=0; j<=P; j++){
			old_alpha[j] = alpha[j];
		}
	}
	// return alpha[1:P]
	alpha.erase(alpha.begin());
	return alpha;
}

/*
 Function to get Cepstral Coefficients
 Parameters:- Alpha Values
 Return Value:- Cepstral Coefficients
*/
vector<long double> cepstral_coeffs(vector<long double> alpha){
	// We are Asuming Q = P
	Q = P; 

	// Vector to store Cepstral Coeffs
	vector<long double>C(Q,0);

	// loop to negate vaues of A[i]'s before going further.
	// But I am not negating since i am getting good C's values without negating alphas
	for(int i = 0;i<P;i++){
		alpha[i] = alpha[i]*(1);
	}

	// loop to get C's for m<=P
	for(int m=1; m<=P; m++){
		long double sum = 0;

		for(int k=1; k<m; k++){
			sum = sum + ((long double)k/(long double)m)*C[k-1]*alpha[m-k-1];
		}
		C[m-1] = alpha[m-1] + sum;
	}

	// loop to get C's for m>P
	for(int m=P+1; m<=Q; m++){
		long double sum = 0;
		for(int k=m-P; k<m; k++){
			sum = sum + (k/m)*C[k]*alpha[m-k-1];
		}

		C[m-1] = sum;
	}

	// Return Cepstral Coefficient
	return C;
}
/*
 Apply DC Shift
 Parameters:- Sample values
 Return value:- Void
*/
 void DC_Shift(vector<long double>&samples){

	// Sum for getting Arithmetic Sum of samples
	long double sum = 0.0;
	// This is just to avoid error if there are no samples
	if(samples.size()==0){
		return;
	}
	// Get Arithmetic sum of samples
	for(unsigned int i = 0;i<samples.size();i++){
		sum+=samples[i];
	}
	// get average of sample values
	long double avg = sum/(long double)samples.size();
	// Update Each sample. Substract average value from each sample value.
	for(unsigned int i = 0;i<samples.size();i++){
		samples[i] = samples[i] - avg;
	}
	return;
}

/*
 Function to Normalize Sample Values
 Parameters:- Sample Values
 Return:- Void
*/
 void normalize(vector<long double>&samples){
	// cinitilize sample max value to zero
	long double max_value = 0;
	// Get Max Absolute value of samples
	for(unsigned int i=0;i<samples.size();i++){
		if(samples[i]>max_value){
			max_value = samples[i];
		}

		if(-1*samples[i] >max_value){
			max_value = samples[i]*-1;
		}
	}

	// Jsut to avoid error. If there is no max value
	if(max_value == 0){
		return;
	}
	// Update each sample with normalized value
	for(unsigned int i=0;i<samples.size();i++){
		samples[i] = samples[i]*10000.0/max_value;
	}

	return;
}
/*
 Function to apply Sine window
 Parameters:- Cepstral Coefficints
 Return Value:- void
*/
void raised_sine_window(vector<long double>&C){
	Q=P;
	for(unsigned int i=0;i<C.size();i++){
		C[i] = 1+(Q/2)*sin(pi*C[i]/2.0);
	
	}
	return;
}
/*
 Function to get tokhura distance between two sets of Cepstral Coefficient values
 Parameters:- A-list of Cepstral Coefficients for vector1. B- A-list of Cepstral Coefficients for vector2.
 Return Value:- Tokhura Distance between two vectors
*/
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
	return distance;
}



/*
 Function to get STE of a frame
 Parameters:- List of frame samples
 Return Value:- STE of the frame
*/
long double STE(vector<long double>frame){
	long double sum = 0.0;
	// Calculate STE
	for(unsigned int i=0;i<frame.size();i++){
		sum+=frame[i];
	}

	return sum/(long double)frame.size();

}


/*
 Function to get Highest STE frame Number
 Parameters:- Samples of given vowel
 Return Value:- Frame number which has highest frame
*/
int get_highest_STE_frame(vector<long double> samples){
	vector<long double>frame;
	// Store highest ste temporarily
	long double highest_ste = 0.0;
	long highest_ste_frame = 5;
	long double temp;
	for(unsigned int i=0;i<samples.size();i++){
		frame.push_back(samples[i]);
		//calculate and compare STE if there are enough samples in frame
		if((i+1)%320==0){
			temp = STE(frame);
			if(temp>highest_ste){
				highest_ste = temp;
				highest_ste_frame = (i+1)/320;
			}
			frame.clear();
		}
	
	}
	// Just to avoid that thee are enough samples on the left side
	if(highest_ste_frame<3){
		return 3;
	}
	return highest_ste_frame;
}


/*
 Function to get frame samples for given frame number
 Parameters:- Samples- Samples of a given vowel; frame- frame number for which samples has to be retrieved
 Return Value:- frame samples
*/
vector<long double>get_frame_samples(vector<long double>samples, int frame){
	vector<long double> frame_samples;
	for(int i = (frame-1)*frame_size; i<(frame)*frame_size;i++){
		frame_samples.push_back(samples[i]);
	}
	return frame_samples;
}
/*
 Function to get R values for the frame
 Parameters:- frame samples
 Return Value:- R values of given frame
*/
vector<long double> get_R(vector<long double> frame){
		vector<long double>R(P+1,0.0);
		// Calculate R Values
		for(int j=0;j<=P;j++){
			long double square_temp = 0;
			for(int t=0;t<320-j;t++){
				square_temp+=frame[t]*frame[t+j];
			}
			R[j] = square_temp/320.0;
		}

		return R;
}


// Function to Train Model
void train_vowel_r(){
	// Assuming Q = P
	Q = P;
	// Vars to store some values
	vector<long double>frame_samples;	// Store frame samples Temporarily
	vector<long double> R;				// Store R Values Temporarily
	vector<long double> alpha;			// Store alpha Values Temporarily
	vector<long double> C;				// Store C Values Temporarily

	// Files for which system has to be trained
	string vowel_files[5][10] = {{"a1.txt", "a2.txt", "a3.txt", "a4.txt", "a5.txt", "a6.txt", "a7.txt", "a8.txt", "a9.txt", "a10.txt"}, {"e1.txt", "e2.txt", "e3.txt", "e4.txt", "e5.txt", "e6.txt", "e7.txt", "e8.txt", "e9.txt", "e10.txt"}, {"i1.txt", "i2.txt", "i3.txt", "i4.txt", "i5.txt", "i6.txt", "i7.txt", "i8.txt", "i9.txt", "i10.txt"}, {"o1.txt", "o2.txt", "o3.txt", "o4.txt", "o5.txt", "o6.txt", "o7.txt", "o8.txt", "o9.txt", "o10.txt"}, {"u1.txt", "u2.txt", "u3.txt", "u4.txt", "u5.txt", "u6.txt", "u7.txt", "u8.txt", "u9.txt", "u10.txt"}};
	char vowels[5] = {'a','e','i','o','u'};
	// Open file to dump Average Cepstral Coefficients for 5 Frames
	ofstream outfile("Cepstrals.txt");
	vector<long double> samples;		// Store sample values for input file
	int count;							// Keep count of training files
	string line;
	cout<<"Average Cepstral Coefficients generated for Vowel are as:-\n";
	// Loop For each of the 5 vowels
	for(unsigned int i=0;i<5;i++){
		count = 1;
		cout<<"For Vowel "<<vowels[i]<<"\n";;
		// Store Values of C's for 5 frames temporarily
		vector<vector<long double>> temp(5,vector<long double>(12,0.0));
		// Each vowel should be trained for 10 instances of it.
		while(count<=10){
			samples.clear();
			// Open Training sample file
			ifstream infile("train/"+vowel_files[i][count-1]);

			// Drop Initial 5 samples which correspons to details of the data
			// 5 Samples also not make any changes in the sample if the file dont have those 5 lines
			int drop_samples = 0;
			// initial 5 samples dont make such difference
			while (getline (infile, line)){
				drop_samples++;
				if(drop_samples==5){
					break;
				}
			}

			// get Sample Values and store in vector samples
			while (getline (infile, line)) {
				samples.push_back(stold(line));
			}
			// Apply dc_shift to the samples
			DC_Shift(samples);
			// Apply Normalization to the samples
			normalize(samples);

			// Get frame number with highest STE in the sample
			int f = get_highest_STE_frame(samples);
			// Loop for five frame 2 left, 2 right and one frame with highest STE
			for(int frame = f-2;frame<=f+2;frame++){
				// Get Frame samples for given frame number
				frame_samples = get_frame_samples(samples, frame);
				// Get R Values of Given frame
				R = get_R(frame_samples);
				// Get alpha values of Given frame using R values
				alpha = durbin(R);
				// Get Cepstral Coeffs from values of alpha
				C = cepstral_coeffs(alpha);

				// Apply Raised sine window
				//raised_sine_window(C);
				
				// Store Values of C's and corressponding frame to the temp
				for(int k = 0;k<Q;k++){
					temp[frame-f+2][k]+=C[k];
					if(count == 10){
						// At last count store values to the codebook after taking average
						temp[frame-f+2][k] = temp[frame-f+2][k]/10.0;
						outfile<<temp[frame-f+2][k]<<" ";
						cout<<temp[frame-f+2][k]<<" ";
					}
				}
				if(count == 10){
						outfile<<"\n";
						cout<<"\n";
				}
			}
			count++;
		}
		outfile<<"\n\n";
		cout<<"\n\n";
	}

}

/*
 Function to tokenize a String to its double values
 Parameters:- String containg various float values seprated by ' '
 Return Value:- Tokenized String to double values
*/
vector<long double>tokenize(string line){
	vector<long double> C;
	string token = "";
	for(unsigned int i=0;i<line.length();i++){
		if(line[i] == ' ' && token!=""){
			C.push_back(stold(token));
			token = "";
		}else if(line[i]!=' ' ){
			token = token+line[i];
		}
	}

	if(token!=""){
		C.push_back(stold(token));
	}
	return C;
}
/*
 Function to test 
 Parameters:- input test file name
 Return value:- a ineger representing vowel where
	1 - 'a'
	2 - 'e'
	3 - 'i'
	4 - 'o'
	5 - 'u'
*/
int test(string test_file){
	string line;
	vector<vector<long double>> c_coeffs(25,vector<long double>(12,0.0));\
	vector<long double>samples;
	vector<long double>frame_samples;
	vector<long double> R;
	vector<long double> alpha;
	vector<long double> C;

	// Open file containing Cepstral Coefficients(Codebook)
	ifstream infile("Cepstrals.txt");

	// Get Values of Cepstral Coefficients from Codebook to a vector and store
	for(unsigned int i = 0;i<5;i++){
		for(unsigned int j = 0;j<5;j++){
			getline (infile, line);
			c_coeffs[i*5+j] = tokenize(line);
		}getline (infile, line);getline (infile, line);
	
	}
	infile.close();

	vector<vector<long double>> temp(5,vector<long double>(12,0.0));
	// Open Test Sample file
	ifstream infile_test(test_file);
	// Drop Inital 5 lines from sample file
	// initial 5 samples dont make such difference
	int drop_samples = 0;
	while (getline (infile_test, line)){
		drop_samples++;
		if(drop_samples==5){
			break;
		}
	}

	// Store each sample 
	while (getline (infile_test, line)) {
		samples.push_back(stold(line));
	}

	// Apply DC Shift
	DC_Shift(samples);
	// Normalize samples
	normalize(samples);
	// Get frame number with highet STE
	int f = get_highest_STE_frame(samples);
	// Loop for 5 frames
	for(int frame = f-2;frame<=f+2;frame++){
		frame_samples = get_frame_samples(samples, frame);
		R = get_R(frame_samples);
		alpha = durbin(R);
		C = cepstral_coeffs(alpha);
		//raised_sine_window(C);
		// Store values of C's in a var named temp
		for(int k = 0;k<Q;k++){
			temp[frame-f+2][k] = C[k];
		}
			
	}
	// Store Tokhura Distance of 5 frames
	vector<long double>tokhura_distances(5,0.0);

	// Calculate Tokhura distance from each vowel
	for(unsigned int i =0;i<5;i++){
		for(unsigned int j=0;j<5;j++){
			tokhura_distances[i] = tokhura_distances[i] + tokhura_distance(temp[j], c_coeffs[i*5+j]);
		}
	}

	long double min_distance = tokhura_distances[0];
	long min_distance_index = 0;
	char vowels[5] = {'a','e','i','o','u'};
	// Get Minimum Distance Vowel
	for(int i=0;i<5;i++){
		if(tokhura_distances[i]<min_distance){
			min_distance = tokhura_distances[i];
			min_distance_index = i;
		}
		cout<<"Tokhura Distance between testing vowel and "<<vowels[i]<<" is: "<<tokhura_distances[i]/5.0<<"\n";
	}
	// Return int representing number of that vowel
	return (int)min_distance_index+1;

}

// main function
int _tmain(int argc, _TCHAR* argv[])
{
	cout<<"Name: Shubham Kumar\n";
	cout<<"Roll Number: 170101064\n\n";
	cout<<"----------------------Vowel Recognition System----------------------\n\n";
	cout<<"Generation of codebook in progress...............\n";
	// Generate Codebook
	train_vowel_r();
	cout<<"Codebook Generated successfully\n";
	// Test for given 50 files
	cout<<"Testing for some Test Files\n";
	string vowel_files[5][10] = {{"a1.txt", "a2.txt", "a3.txt", "a4.txt", "a5.txt", "a6.txt", "a7.txt", "a8.txt", "a9.txt", "a10.txt"}, {"e1.txt", "e2.txt", "e3.txt", "e4.txt", "e5.txt", "e6.txt", "e7.txt", "e8.txt", "e9.txt", "e10.txt"}, {"i1.txt", "i2.txt", "i3.txt", "i4.txt", "i5.txt", "i6.txt", "i7.txt", "i8.txt", "i9.txt", "i10.txt"}, {"o1.txt", "o2.txt", "o3.txt", "o4.txt", "o5.txt", "o6.txt", "o7.txt", "o8.txt", "o9.txt", "o10.txt"}, {"u1.txt", "u2.txt", "u3.txt", "u4.txt", "u5.txt", "u6.txt", "u7.txt", "u8.txt", "u9.txt", "u10.txt"}};
	char vowel[5] = {'a','e','i','o','u'};
	long wrong_count = 0;
	for(int i=0;i<5;i++){
		for(int j=0;j<10;j++){
			char ide = vowel[test("test/"+vowel_files[i][j])-1];
			cout<<"Test file"<<10*i+j+1<<" | Test Vowel: "<<vowel[i]<<"; Identified as: "<<ide;

			if(ide != vowel[i]){
				cout<<" ; Incorrectly Identified\n\n";
				wrong_count++;
			}else{
				cout<<" ; Correctly Identified\n\n";
			}
		}
	
	}

	cout<<"Incorrect identified vowels: "<<wrong_count<<"\n";
	//test("test6.txt");
	int option;
	// loop for real time testing
	while(1){
		cout<<"Type 1 for Real time teting. Type 2 to exit system\n";
		cin>>option;
		if(option==1){
			std::ostringstream oss;
			cout<<"Waiting 5 seconds for your speech\n";
			oss << "start Recording_Module/Recording_Module.exe 3 real_time_testing_test.wav real_time_testing_test.txt"; //define the string manually, because macro cant expand inside string
			system(oss.str().c_str());
			// sleep for some time to avoid error with recording module
			Sleep(6000);
			char recogized_vowel = test("real_time_testing_test.txt");
			// print recognized value
			cout<<"Recognized Vowel: "<<vowel[recogized_vowel-1]<<"\n";
		}else if(option == 2){
			break;
		}else{
			cout<<"Not Valid choice! Try Again\n";
		}

	}
	return 0;
}

