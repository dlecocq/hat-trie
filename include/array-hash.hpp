/**
 * The Array Hash described by Askitis
 *
 * This is meant for character data, but it can be applied to any char *, so 
 * long as a length is provided. In the interface where no length is provided, 
 * it is assumed to be a null-terminated string.
 */

#ifndef HT__ARRAY_HASH_H
#define HT__ARRAY_HASH_T

#include <vector>
#include <string>

#include <hash.hpp>

#include <iostream>

namespace ht {
    template <
        class Value,
        class Hash=crapwow,
        class Allocator=std::allocator<std::pair<std::string,Value> > >
    class ArrayHash {
    public:
        /* Some typedefs */
        typedef size_t                         size_type;
        typedef std::string                    key_type;
        typedef Value                          value_type;
        typedef std::pair<key_type,value_type> pair_type;
        typedef Hash                           hash_type;
        typedef Allocator                      allocator_type;
        
        /* Share the hash function object across all such classes */
        static const hash_type      hasher;
        /* And the allocator common to all of these */
        static       allocator_type allocator;
        
        ArrayHash(size_type b=16384): num_bins(b), bins(NULL) {
            /* Allocate the number of bins that we'll need. For this initial 
             * allocation, we'll actually use malloc, but for the allocations of 
             * the contents of each bin, we'll use the allocator */
            bins = new std::vector<pair_type>[b];
            if (bins == NULL) {
                std::cout << "Could not allocate" << std::endl;
            }
        }
        
        ~ArrayHash() {
            if (bins == NULL) {
                /* Bail out early -- nothing to see here */
                return;
            }
            delete[] bins;
        }
        
        /**
         * Find the key, if it exists
         */
        // iterator find(const char* key);
        // iterator find(const std::string& key);
        
        /* Insertion/Retrieval
         *
         * If the key doesn't exist, create it, and return a reference to where 
         * we'd store it */
        value_type& operator[](const std::string& key) {
            /* We need to hash the function, see if it exists, and if it already 
             * exists in the value, when we'll update the value and return a 
             * reference. If it doesn't, we'll append. If that bin is NULL, then 
             * we'll go ahead and allocate it already */
            size_type position = hasher(key.c_str(), key.length()) % num_bins;
            std::vector<pair_type>& bin(bins[position]);
            /* I _believe_ there is a more C++-y way of doing this, but it's 
             * late, and I'm not entirely sure */
            typename std::vector<pair_type>::iterator it(bin.begin());
            for (; it != bin.end(); ++it) {
                if (it->first == key) {
                    /* Return a reference to the value */
                    return it->second;
                }
            }

            /* If we didn't find one, we should add one */
            bin.push_back(make_pair(key, value_type()));
            return bin.back().second;
        }
        
        /* Insert
         *
         * This interface is provided for both continuity and to support binary 
         * data (where the length of the char* buffer is provided)
         */
        value_type& insert(const std::string& key, const value_type& v) {
            value_type& ref(operator[](key));
            ref = v;
            return ref;
        }
        
        /* Remove
         *
         * If the provided key exists, then it is removed. If the key does not 
         * exist, it has no effect. Normally, it would return a reference, but 
         * since it's possible the key doesn't exist, we can't. */
        void remove(const std::string& key) {
            size_type position = hasher(key.c_str(), key.length()) % num_bins;
            std::vector<pair_type>& bin(bins[position]);
            typename std::vector<pair_type>::iterator it(bin.begin());
            for (; it != bin.end(); ++it) {
                if (it->first == key) {
                    bin.erase(it, it);
                    return;
                }
            }
        }
        
        /* Existence
         *
         * Returns true if the provided key exists, else, false */
        bool exists(const std::string& key) {
            size_type position = hasher(key.c_str(), key.length()) % num_bins;
            std::vector<pair_type>& bin(bins[position]);
            typename std::vector<pair_type>::iterator it(bin.begin());
            for (; it != bin.end(); ++it) {
                if (it->first == key) {
                    return true;
                }
            }
            return false;
        }
    private:
        /* How many bins are we using? */
        size_type  num_bins;
        /* We need an array of char*'s */
        std::vector<pair_type>* bins;
    };
}

#endif
