you can repurpose it to anything for which ip input is required. for example you can make a C code that takes an ip and do some inspection on it. 
then based on that inspection the C code makes an action like using an exploit script. 
the concept written above can be used for scanning mass ip for vulnerability.

the main goal of this script is to make the work faster using multi tasking.
if your code is taking 5 second for one ip to scan this script will help your code to scan 160
ip at a time. 

it's like this project:
https://github.com/alexjhon483b1/OpenPort/tree/4th

this commands will come to use to execute this script 

sudo apt-get install parallel

./run.sh | tee -a output.txt

cat output.txt | grep "keyword"


