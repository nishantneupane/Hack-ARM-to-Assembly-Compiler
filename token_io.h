
#ifndef FILE_IO_H_
#define FILE_IO_H_

#include <string>
#include <istream>
using namespace std;


string getNextLine(istream& input);


string getFirstToken(string input);


string getSecondToken(string input);


string takeToken(string& input);


string takeTokenFromEnd(string& input);


void removeChars(string& str, const string& symbols);

#endif
