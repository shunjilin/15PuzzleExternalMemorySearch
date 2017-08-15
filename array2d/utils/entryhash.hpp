#ifndef ENTRY_HASH_HPP
#define ENTRY_HASH_HPP

#include <cstddef>

template<class Entry>
struct EntryHash {
    std::size_t operator()(const Entry& entry) const {
        return entry.packed.hash();
    }
};

#endif
