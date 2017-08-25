#ifndef NODE_HPP
#define NODE_HPP

#include "utils/named_fstream.hpp"
#include <cstring>
#include <fstream>

template<class D>
struct Node {
    char f, g, pop;
    typename D::PackedState parent_packed;
    typename D::PackedState packed;

    Node() {}
    
    Node(typename D::PackedState packedState) : packed(packedState) {} // for hashing parent

    const typename D::PackedState &key() { return packed; }

    bool write(fstream& file) const {
        file.write(reinterpret_cast<const char *>(&f), sizeof(f));
        file.write(reinterpret_cast<const char *>(&g), sizeof(g));
        file.write(reinterpret_cast<const char *>(&pop), sizeof(pop));
        file.write(reinterpret_cast<const char *>(&parent_packed), sizeof(parent_packed));
        file.write(reinterpret_cast<const char *>(&packed), sizeof(packed));
        return !file.fail();
    }

    void write(char* ptr) const {
        memcpy(ptr, &f, sizeof(f));
        ptr += sizeof(f);
        memcpy(ptr, &g, sizeof(g));
        ptr += sizeof(g);
        memcpy(ptr, &pop, sizeof(pop));
        ptr += sizeof(pop);
        memcpy(ptr, &parent_packed, sizeof(parent_packed));
        ptr += sizeof(parent_packed);
        memcpy(ptr, &packed, sizeof(packed));
    }

    bool read(fstream& file) {
        file.read(reinterpret_cast<char *>(&f), sizeof(f));
        file.read(reinterpret_cast<char *>(&g), sizeof(g));
        file.read(reinterpret_cast<char *>(&pop), sizeof(pop));
        file.read(reinterpret_cast<char *>(&parent_packed), sizeof(parent_packed));
        file.read(reinterpret_cast<char *>(&packed), sizeof(packed));
        return !file.fail();
    }

    void read(char *ptr) {
        memcpy(&f, ptr, sizeof(f));
        ptr += sizeof(f);
        memcpy(&g, ptr, sizeof(g));
        ptr += sizeof(g);
        memcpy(&pop, ptr, sizeof(pop));
        ptr += sizeof(pop);
        memcpy(&parent_packed, ptr, sizeof(parent_packed));
        ptr += sizeof(parent_packed);
        memcpy(&packed, ptr, sizeof(packed));
    }
        
    static size_t get_size_in_bytes() {
        return sizeof(f) + sizeof(g) + sizeof(pop) +
            sizeof(packed) + sizeof(parent_packed);
    }

    static int get_n_var() {
        return D::PackedState::get_n_var();
    }

    static int get_n_val(int i) {
        return D::PackedState::get_n_val(i);
    }

    bool operator==(const Node& other) const {
        return this->packed == other.packed;
    }

    bool operator<(const Node& other) const {
        return this->packed < other.packed;
    }

    bool operator>(const Node& other) const {
        return this->packed > other.packed;
    }

    int operator[](int i) const {
        return this->packed[i];
    }
};

#endif
