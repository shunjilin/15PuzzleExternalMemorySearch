#include "../tabulation_hash.hpp"

struct Test {
    static int get_n_var() { return 1; }
    static int get_n_val(int i) { return 1; }
};

int main(int argc, char *argv[])
{
    TabulationHash<Test> tabulation_hash;
    
    return 0;
}
