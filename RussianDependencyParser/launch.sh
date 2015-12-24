set -x
python generateSentence.py "$1"

export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:TurboParser-2.3.0/deps/local/lib:"

# input the infinitive forms before verbs and gerunds in the dictionary file from opencorpora.org
# python correctDictionary.py ../openCorpora/dict.opcorpora.txt ../openCorpora/good_dict.opcorpora.txt

# train the morphological parser AMorph14
#./genhashtable ../openCorpora/good_dict.opcorpora.txt hash.bin

# train the syntactic parser ASynt14
# ./main --synttrain test1142.xml newmmodel.txt newsyntmodel.txt ../TurboParser-2.2.0/TurboParser

# apply AMorph14 the morphological parser
# ./main --morphmark testSentence.xml morphmark_output.txt newmmodel.txt

#./main --syntmark test1142.xml newmmodel.txt newsyntmodel.txt output.txt /home/C++/TurboParser-2.2.0/TurboParser
./main --syntmark testSentence.xml newmmodel.txt newsyntmodel.txt output_upper.txt TurboParser-2.3.0/TurboParser
head -n 100 output_upper.txt

