I have Generated this code on the basis of input file provided by sir and instruction provided by sir to use ZCR to detect presence of yes.

After Building and Running Here You have to give  2 input files 
	First containing Silence only( So that Words can be extracted from the input file)
	Second Containing various "Yes" and "No" that are to be detected. 

Though i have included a sample silence file "silence.txt" and input file "yes_no.txt".

What Code Does, it first calculate threshold energy based on the silence.txt, which is 5 times of the average STE of Silence.

After this Code Calculate STE and ZCR of of discrete frames ( Here frame means 20 ms or 320 samples of data), and Extract words and store Starting and end points of words based on STE and Threshold energy.

To differentiate "Yes" and "No" Code uses ZCR. First it calculate Average ZCR of firt half of word and then Average ZCR of last 20 % of Word. If Last 20% ZCR is atleast 4 times that of Average ZCR of First half then its a "YES" Otherwise "No"

Algorithm

	For each word in data
		average_zcr_first_half = "Calculate Average ZCR of first half of the word"
		average_zcr_last_20 = "Calculate Average ZCR of last 20 % of the word"

		if(average_zcr_last_20 > 4*average_zcr_first_half):
			print "YES"
		else:
			print "NO"