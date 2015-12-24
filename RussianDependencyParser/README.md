RussianDependencyParser
=======================

Morphological disambiguation (C++) and dependency parser of Russian by Alexey Gorelov, supervised by Dr Mstislav Maslennikov

Nickname: ASynt 14

This is a project for the "Mathematical methods for text analysis" course in autumn 2013 in MSU. 

Kindly refer to:
- main1.pdf
- vk.com/mathtexts -> achievements of listeners (http://vk.com/topic-50553132_28815796)

Compulsory citation:
Alexey Gorelov and Mstislav Maslennikov, 2014. "Syntactic analysis of texts". In "Mathematical methods for text analysis" speckours, vk.com/mathtexts.


Installation
============

(1) Clone RussianDependencyParser into some directory, for example /home/user/RussianDependencyParser
   $ cd /home/user/RussianDependencyParser
   $ git clone https://github.com/maslenni/RussianDependencyParser.git

(2) Install TurboParser v2.1.0 from the page http://www.ark.cs.cmu.edu/TurboParser/ into /home/user/RussianDependencyParser by following their INSTALL.txt. You can also install into another directory and symlink this directory;  

(3) Download the plain text dictionary from opencorpora.org/dict.php, eg into the file /home/user/RussianDependencyParser/openCorpora/dict.opcorpora.txt.;

(4) Install cmake and qt
    $ sudo apt-get install cmake qt-sdk
    
(5) Launch the installer
    $ ./install.sh

Installation on Max OS X
========================

The following amendments to step (2) were found necessary for OS X version 10.10 with gcc 4.9:
(2b) Please change LD_LIBRARY_PATH to DYLD_LIBRARY_PATH in ... . 
scripts/run_parser.sh
scripts/run_tagger.sh
scripts/train_test_parser.sh
scripts/train_test_tagger.sh
install_deps.sh

You may also need to use "sudo" to launch TurboParser;

Running
=======

$ ./launch.sh "Мама мыла раму"

License
=======

   (C) Copyright 2014 Moscow State University.

   All rights reserved. This program and the accompanying materials
   are made available under the terms of the Eclipse Public License v1.0
   which accompanies this distribution, and is available at
   
   http://www.eclipse.org/legal/epl-v10.html
  
   Contributors:
       Alexey Gorelov, Elena Sokolova (dependency corpus extension), Mstislav Maslennikov
