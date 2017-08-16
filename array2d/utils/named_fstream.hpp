#ifndef NAMED_FSTREAM_HPP
#define NAMED_FSTREAM_HPP

#include <fstream>
#include <string>
#include <cstdio>
#include <vector>

constexpr int BUFFER_SIZE = 4096 * 2; // test different buffer sizes
/*                                                                           \
| Keeps file_names with their respective fstream for convenient destruction. |
| Also provides custom size buffer.                                          |
\===========================================================================*/

using namespace std;

class named_fstream : public fstream {
    string file_name;
    vector<char> buffer = vector<char>(BUFFER_SIZE);
 public:
    named_fstream() = default;
    named_fstream(const string file_name,
               ios_base::openmode mode = ios_base::in | ios_base::out
               | ios_base:: trunc | ios_base:: binary);
    
    ~named_fstream();

    named_fstream(const named_fstream &other) = delete;
    named_fstream& operator = (const named_fstream &other) = delete;
};


#endif