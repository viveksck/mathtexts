set -x
export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:TurboParser-2.3.0/deps/local/lib:"

cmake .
make

# input the infinitive forms before verbs and gerunds in the dictionary file from opencorpora.org
python correctDictionary.py openCorpora/dict.opcorpora.txt openCorpora/good_dict.opcorpora.txt

# train the morphological parser AMorph14
./genhashtable openCorpora/good_dict.opcorpora.txt hash.bin

# train the syntactic parser ASynt14
./main --synttrain test1142.xml newmmodel.txt newsyntmodel.txt TurboParser-2.3.0/TurboParser

