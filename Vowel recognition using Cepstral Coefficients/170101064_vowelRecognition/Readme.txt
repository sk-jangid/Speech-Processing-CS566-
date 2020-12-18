Roll Number: 170101064
Name: Shubham Kumar

Vowel Recognition System

How to Run Code:
	You can use 'Vowel_recognition.exe' in the Debug folder to run my program or you can run code from visual studio using Debug option.
	I have Removed Debug Folder to Minimize submission size. So you actually have to build it again in visual studio. If you use ".exe" file to run program then dont forget to add test and train folders from sample datas and recording module in the Debug folder because path of test and train files are hard coded.
	Though each of them need test and train files to go furher. If these both folders are not present code will abort after giving error.
	test and train files are stored in "test" and "train" named folders respectively(These are hard coded), So there will be error if these are not present. Also file names are also hardcoded for testing files and traing files. You can see my testing and training sample file names in the solution folder.
	
	file name for training and testing files should be as-
	<vowel_name><count>.txt
	for example 'a1.txt', 'e10.txt'
	where vowel name can be 'a','e','i','o',u' and count can be between 1 and 10 inclusive.

Problem Statement: Generate a vowel recognition system that can detect different vowels spoken by me.

Solution: 
	I applied DC Shift and Normalization for training and testing samples both.

	Training(Generation of Codebook):-
	Followed instructions given. I have taken 10 utterances of each vowel and calculated cepstral coefficints for 5 frames that have high STE's. I used simple approach to choose 5 frames, i am selecting highest STE frame and then 2 from its left and 2 from the right of it. After calculating average Cepstral coefficients for 5 frames and for each vowel i am storing these values in the reference file "Cepstrals.txt".

Testing:
	For testing I am taking cepstral coefficients values stored in the reference file. After calculating cepstral coefficients for the testing file ci am comparing tokhura distance of test file to the each of the referance file and returning vowel which has least average distance for each frame. Also dispaying average tokhura distance from each vowel for 5 frames.
	Real time testing: After testing for some of the standard files code will ask for an option from the user where we can do real time testing. Options for the input are:-
		'1'- for Real time testing
		'2'- for exit
	Here system waits for 3 seconds for iteration and store samples in file "real_time_testing_test.txt". Though i have not included functionality to check whether a vowel or any sound is spoken or not. My function will give some random vowel if there is no sound or if there is no vowel actually spoken.

Accuracy:- I have not negated the signs of the Alpha's for calculating Cepstral Coefficients and also not applied raised sine window though i was getting similar accuracy after applying raised sine window but cepstral coefficients were not that accurate.

I have tested it for many samples of trainig and testing data and the accuracy was aroung 90-95% in each of the cases. In this assignemnt i have added two sets of training and testing datas. Where in one the accuracy is 100% (50/50) and 98%(49/50) if i use test data as training data and training data as testing data.

After Applying Raised sine window - Accuracy was 98% and 96% after reversing training and test data.

I was getting average Accuracy of different vowel in each test:
	'a':- Around 98%
	'e':- Around 90%
	'i':- Around 90%
	'o':- Around 95%
	'u':- Around 98%
In Real time testing 'e' and 'i' was not that accurate they are giving accuracy of aroud 90%.

It was quite good in recognizing vowels i have tested for some other persons voices also, It was almost 85% correct.



