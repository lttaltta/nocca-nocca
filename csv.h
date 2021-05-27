
#ifndef CSV_H_
#define CSV_H_

#include <fstream>
#include <sstream>
#include <vector>
#include <string>

class CSV {
public:
    std::vector< std::vector<std::string> > data;

    CSV () {}
    CSV ( const std::string filename ) { read_file(filename); }

    bool read_file (const std::string filename ) {
        data.clear();

        std::ifstream ifs(filename);
        std::string line;
        while ( std::getline(ifs, line) ) {
            std::istringstream stream(line);
            std::string field;

            std::vector<std::string> tmp;
            while ( getline(stream, field, ',') ) {
                tmp.push_back(field);
            }
            data.push_back(tmp);
        }

        return true;
    }

    std::vector<std::string> operator [] ( const int index ) {
        return data[index];
    }

    int get_rows () const { return data.size(); }

};

#endif
