# encoding: utf8
from __future__ import print_function
import sys
import re

filename = str(sys.argv[1])
fi = open(filename, 'r')
fo = open('testSentence.xml', 'w')

print ('<?xml version="1.0" encoding="utf-8" standalone="yes"?>', file=fo)
print ('<sentences><p>', file=fo)

for line in fi:
  sentences = re.split(r'[\.\?!]', line.strip())
  for sentence in sentences:
    words = sentence.split(' ')
    flag = False
    for word in words:
      if len(word) > 0:
        flag = True
        break
    if flag:
      print ('<se>', file=fo)
      id = 0
      for word in sentence.split(' '):
        if len(word) > 0:
          id += 1
          print ('<w id="%d">' % id + word + '<rel id_head="" type=""/></w>', file=fo)
      print ('</se>', file=fo)

print ('</p>', file=fo)
print ('</sentences>', file=fo)

fi.close()
fo.close()
