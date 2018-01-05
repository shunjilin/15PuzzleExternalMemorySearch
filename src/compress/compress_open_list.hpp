// Copyright 2017 Shunji Lin. All rights reserved.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file.
#ifndef COMPRESS_OPEN_LIST_HPP
#define COMPRESS_OPEN_LIST_HPP

#include "../utils/errors.hpp"
#include "../utils/named_fstream.hpp"

#include <utility>
#include <map>
#include <set>
#include <string>
#include <sstream>

// for constructing directory
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
using namespace std;

namespace compress {
    template<class Entry>
    class CompressOpenList  {

        map<int, map<int, named_fstream> > fg_buckets;
        
        int size;

        string get_bucket_string(int f, int g) const;
        bool exists_bucket(int f, int g) const;
        void create_bucket(int f, int g);

    public:
        CompressOpenList();
        ~CompressOpenList();

        Entry pop();
        void push(const Entry &entry);
        void clear();
        bool isempty() const;
        int get_current_h_value() const;
        
    };

    
    template<class Entry>
    CompressOpenList<Entry>::CompressOpenList()
    {
        // create directory for open list files if not exist
        mkdir("open_list_buckets", 0744);
    }

    template<class Entry>
    void CompressOpenList<Entry>::
    push(const Entry &entry) {
        auto f = entry.f;
        auto g = entry.g;

        if (!exists_bucket(f, g)) create_bucket(f, g);
        if (!entry.write(fg_buckets[f][g]))
            throw IOException("Fail to write state to fstream.");
        ++size;
    }

    template<class Entry>
    Entry CompressOpenList<Entry>::pop() {
        assert(size > 0);
        Entry min_entry;

        //TODO: Forloop unecessary? Always returns on first iteration.
        // tiebreak by lowest f value
        for (auto f_bucket = fg_buckets.begin();
             f_bucket != fg_buckets.end(); ++f_bucket) {
            // tiebreak by highest g value
            for (auto g_bucket = f_bucket->second.rbegin();
                 g_bucket != f_bucket->second.rend(); ++g_bucket) {
                // FIFO 
                g_bucket->second.seekp(-Entry::get_size_in_bytes(), ios::cur);
                min_entry.read(g_bucket->second);
                g_bucket->second.seekp(-Entry::get_size_in_bytes(), ios::cur);

                // remove files if empty
                if (g_bucket->second.tellp() == 0) {
                    auto g = g_bucket->first;
                    f_bucket->second.erase(g);
                    if (f_bucket->second.empty()) {
                        auto f = f_bucket->first;
                        fg_buckets.erase(f);
                    }
                }
                --size;
                return min_entry;
            }
        }
        return min_entry;
    }

    template<class Entry>
    bool CompressOpenList<Entry>::isempty() const {
        return size == 0;
    }

    template<class Entry>
    void CompressOpenList<Entry>::clear() {
        fg_buckets.clear();
        size = 0;
        rmdir("open_list_buckets");
    }
    
    template<class Entry>
    string CompressOpenList<Entry>::
    get_bucket_string(int f, int g) const {
        std::ostringstream oss;
        oss << "open_list_buckets/" <<  f << "_" << g << ".bucket";
        return oss.str();
    }

    template<class Entry>
    bool CompressOpenList<Entry>::
    exists_bucket(int f, int g) const {
        auto f_bucket = fg_buckets.find(f);
        if (f_bucket == fg_buckets.end()) return false;
        auto g_bucket = f_bucket->second.find(g);
        if (g_bucket == f_bucket->second.end()) return false;
        return true;
    }

    template<class Entry>
    void CompressOpenList<Entry>::
    create_bucket(int f, int g) {
        // to prevent copying of strings, in-place construction
        fg_buckets[f].emplace(piecewise_construct,
                              forward_as_tuple(g),
                              forward_as_tuple(get_bucket_string(f, g)));
        
        if (!fg_buckets[f][g].is_open())
            throw IOException("Fail to open open list fstream.");
    }

    template<class Entry>
    int CompressOpenList<Entry>::
    get_current_h_value() const {
        auto f_bucket = fg_buckets.begin();
        auto g_bucket = f_bucket->second().rbegin();
        return f_bucket->first - g_bucket->first;
    }

}

#endif
        
