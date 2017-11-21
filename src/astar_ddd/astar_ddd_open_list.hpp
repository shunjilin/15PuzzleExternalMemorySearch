#ifndef ASTAR_DDD_OPEN_LIST_HPP
#define ASTAR_DDD_OPEN_LIST_HPP

#include "../utils/memory.hpp"
#include "../utils/named_fstream.hpp"
#include "../utils/errors.hpp"
#include "../hash_functions/tabulation_hash.hpp"

#include <utility>
#include <vector>
#include <string>
#include <memory>
#include <limits>
#include <unordered_set>
#include <sstream>
#include <cmath>

// for constructing directory
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;

namespace astar_ddd {
    
    enum class BucketType { open, next, closed };
    
    template<class Entry>
    class AstarDDDOpenList {

        int n_buckets = 10; // tune outside?

        bool reopen_closed;
        
        int min_f = 0;
        int next_f = numeric_limits<int>::max();
        void remove_duplicates();
        bool first_insert = true; // to initialize min_f
        int current_bucket = 0; // current bucket being expanded
        
        vector<unique_ptr<named_fstream> > open_buckets;
        vector<unique_ptr<named_fstream> > next_buckets;
        vector<unique_ptr<named_fstream> > closed_buckets;

        unique_ptr<named_fstream> recursive_bucket; // for recursive expansion

        void create_bucket(int bucket_index, BucketType bucket_type);
        string get_bucket_string(int bucket_index, BucketType bucket_type) const;

        TabulationHash<Entry> bucket_hasher;
        TabulationHash<Entry> dd_hasher; // duplicate detection

        // for temp logging
        size_t max_bucket_size_in_bytes = 0;
        
    public:
        AstarDDDOpenList(bool reopen_closed);
        ~AstarDDDOpenList() = default;

        void push(const Entry& entry);
        Entry pop();
        void clear();
        
        Entry trace_parent(const Entry &entry);
    };
    
    template<class Entry>
    AstarDDDOpenList<Entry>::AstarDDDOpenList(bool reopen_closed) :
        reopen_closed(reopen_closed),
        open_buckets(n_buckets),
        next_buckets(n_buckets),
        closed_buckets(n_buckets)
    {
        // create directory for open list files if not exist
        mkdir("open_list_buckets", 0744);
        
        recursive_bucket = memory::make_unique<named_fstream>
            ("open_list_buckets/recursive.bucket");

        // create buckets
        for (int i = 0; i < n_buckets; ++i) {
            create_bucket(i, BucketType::open);
            create_bucket(i, BucketType::next);
            create_bucket(i, BucketType::closed);
        }
        dfpair(stdout, "number of hash buckets", "%d", n_buckets);
    }

    template<class Entry>
    void AstarDDDOpenList<Entry>::
    remove_duplicates() {
        min_f = numeric_limits<int>::max();
        
        for (int i = 0; i < n_buckets; ++i) { // for each bucket

            unordered_set<Entry, decltype(dd_hasher) > hash_table;

            // hash next list entries
            next_buckets[i]->clear();
            next_buckets[i]->seekg(0, ios::beg);
            Entry next_entry;
            next_entry.read(*next_buckets[i]);
            while (!next_buckets[i]->eof()) {
                auto it = hash_table.find(next_entry);
                if (it != hash_table.end()) {
                    if (it->g > next_entry.g) {
                        hash_table.erase(it);
                        hash_table.insert(next_entry);
                    }
                } else {
                    hash_table.insert(next_entry);
                }
                next_entry.read(*next_buckets[i]);
            }
            
            size_t bucket_size_in_bytes = hash_table.size() * sizeof(Entry);
            if (bucket_size_in_bytes > max_bucket_size_in_bytes)
                max_bucket_size_in_bytes = bucket_size_in_bytes; 
            next_buckets[i].reset(nullptr);
            create_bucket(i, BucketType::next);
            next_buckets[i]->clear();
            next_buckets[i]->seekg(0, ios::beg);

            // hash closed list entries against next list entries, deleting
            // duplicates
            closed_buckets[i]->clear();
            closed_buckets[i]->seekg(0, ios::beg);
            Entry closed_entry;
            closed_entry.read(*closed_buckets[i]);
            while (!closed_buckets[i]->eof()) {
                auto it = hash_table.find(closed_entry);
                if (it != hash_table.end()) {
                    hash_table.erase(it);
                }
                closed_entry.read(*closed_buckets[i]);
            }
            closed_buckets[i]->clear();
            closed_buckets[i]->seekg(0, ios::end);

            open_buckets[i].reset(nullptr); // erase old open bucket
            create_bucket(i, BucketType::open);
            open_buckets[i]->clear();
            open_buckets[i]->seekg(0, ios::beg);
            
            for (auto& entry : hash_table) {
                if (entry.f < min_f) min_f = entry.f;
                entry.write(*open_buckets[i]);
            }
            // reset open
            open_buckets[i]->clear();
            open_buckets[i]->seekg(0, ios::beg);
        }
        if (min_f ==  numeric_limits<int>::max()) {
            throw OpenListEmpty();
        }
    }

    template<class Entry>
    void AstarDDDOpenList<Entry>::
    push(const Entry &entry) {
        if (!first_insert &&
            entry.f <= min_f) {
            entry.write(*recursive_bucket);
            return;
        }
        
        int bucket_index = bucket_hasher(entry) % n_buckets;
    
        if (first_insert) {
            min_f = entry.f;
            entry.write(*open_buckets[bucket_index]);
            open_buckets[bucket_index]->clear();
            open_buckets[bucket_index]->seekg(0, ios::beg);
            first_insert = false;
            return;
        }
        
        
        if (!entry.write(*next_buckets[bucket_index]))
            throw IOException("Fail to write state to fstream.");

    }

    template<class Entry>
    Entry AstarDDDOpenList<Entry>::pop() {
        //cout << "removing min" << endl;
        Entry min_entry;
        if (recursive_bucket->tellg() != 0) {
            recursive_bucket->seekg(-Entry::get_size_in_bytes(), ios::cur);
            min_entry.read(*recursive_bucket);
            recursive_bucket->seekg(-Entry::get_size_in_bytes(), ios::cur);
            min_entry.write(*closed_buckets[bucket_hasher(min_entry) % n_buckets]);
            return min_entry;
        }
    
        while (current_bucket != n_buckets) {
            // attempt read from current bucket
            min_entry.read(*open_buckets[current_bucket]);
            //cout << " reading entry " << endl;
            if (open_buckets[current_bucket]->eof()) { // exhausted current bucket
                ++current_bucket;
                continue;
            }
            
            if (min_entry.f == min_f) {
                min_entry.write(*closed_buckets[current_bucket]);
                return min_entry;
            } else {
                // transfer unexpanded to next bucket
                min_entry.write(*next_buckets[current_bucket]);
            }
        }

        // exhausted all buckets
        remove_duplicates();
        current_bucket = 0;
        recursive_bucket.reset(nullptr);
        recursive_bucket =
            memory::make_unique<named_fstream>
            (string("open_list_buckets/recursive.bucket"));
         
        return pop();
    }


    template<class Entry>
    void AstarDDDOpenList<Entry>::clear() {
        open_buckets.clear();
        next_buckets.clear();
        closed_buckets.clear();
        recursive_bucket = nullptr;
        // remove empty directory, this fails if directory is not empty
        rmdir("open_list_buckets");
        dfpair(stdout, "max bucket size (bytes)", "%lu", max_bucket_size_in_bytes);
    }

    template<class Entry>
    string AstarDDDOpenList<Entry>::
    get_bucket_string(int bucket_index, BucketType bucket_type) const {
        string bucket_type_str;
        if (bucket_type == BucketType::open) bucket_type_str = "open";
        if (bucket_type == BucketType::next) bucket_type_str = "next";
        if (bucket_type == BucketType::closed) bucket_type_str = "closed";
        std::ostringstream oss;
        oss << "open_list_buckets/" <<  bucket_index << "_" << bucket_type_str << ".bucket";
        return oss.str();
    }

    template<class Entry>
    void AstarDDDOpenList<Entry>::
    create_bucket(int bucket_index, BucketType bucket_type) {
        if (bucket_type == BucketType::open) {
            open_buckets[bucket_index] =
                memory::make_unique<named_fstream>
                (get_bucket_string(bucket_index, bucket_type));
            if (!open_buckets[bucket_index]->is_open())
                throw IOException("Fail to open open list fstream.");
        }
        if (bucket_type == BucketType::next) {
            next_buckets[bucket_index] =
                memory::make_unique<named_fstream>
                (get_bucket_string(bucket_index,bucket_type));
            if (!next_buckets[bucket_index]->is_open())
                throw IOException("Fail to open open list fstream.");
        }
        if (bucket_type == BucketType::closed) {
            closed_buckets[bucket_index] =
                memory::make_unique<named_fstream>
                (get_bucket_string(bucket_index, bucket_type));
            if (!closed_buckets[bucket_index]->is_open())
                throw IOException("Fail to open open list fstream.");
        }
    }

    template<class Entry>
    Entry AstarDDDOpenList<Entry>::
    trace_parent(const Entry &entry) {
        // This is only works with unit cost domain (no zero cost actions)
        // For zero cost actions, buckets with g and g-1 both have to be checked
        
        // check parent hash bucket only
        int bucket_index = bucket_hasher(entry.parent_packed) % n_buckets;
        closed_buckets[bucket_index]->clear();
        closed_buckets[bucket_index]->seekg(0, ios::beg);

        Entry closed_entry;
        closed_entry.read(*closed_buckets[bucket_index]);
        while (!closed_buckets[bucket_index]->eof()) {
            if (closed_entry.packed ==
                entry.parent_packed) {
                return closed_entry;
            }
            closed_entry.read(*closed_buckets[bucket_index]);
        }
        return Entry();
    }
}       

#endif
