**********************Real time yes-no detector***************************

I have Generated this code on the basis of my voice i have included few run on my program in directory some tests.
I used recording module provided already to record voice.
******How to Run***********
	Execute program, program waits for 3 seconds for detecting voice, after that it processes on the files.

I have included recording module in the yes-no detector. Recording module is stored in the directory "Recording _Module". It records speech and stores wav file as "input_w.wav" and sample values in text file "input_t.txt". Code stops for 5 seconds for speech.
After speaking, code calculates threshold frequency( 10 times of average STE of noise) and DC shift using a file "noise.txt". Then code normalizes all values about 10000. After that code calculates ZCR and STE for all discrete frames ( Here frame means 20 ms or 320 samples of data) based on STE and threshold energy.

To differentiate "Yes" and "No" Code uses ZCR. First it calculate Average ZCR of first half of word and then Average ZCR of last 20 % of Word. If Last 20% ZCR Average is atleast 3 times that of Average ZCR of First half then its a "YES" Otherwise "No"

Algorithm

	For each word in data
		average_zcr_first_half = "Calculate Average ZCR of first half of the word"
		average_zcr_last_20 = "Calculate Average ZCR of last 20 % of the word"

		if(average_zcr_last_20 > 3*average_zcr_first_half):
			print "YES"
		else:
			print "NO"