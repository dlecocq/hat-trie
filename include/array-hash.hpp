/**
 * The Array Hash described by Askitis
 *
 * This is meant for character data, but it can be applied to any char *, so 
 * long as a length is provided. In the interface where no length is provided, 
 * it is assumed to be a null-terminated string.
 */

#ifndef HT__ARRAY_HASH_H
#define HT__ARRAY_HASH_T

#include <hash.hpp>

#include <cstdlib>
#include <cstring>
#include <iostream>

namespace ht {
    template <
        class Value,
        class Hash=superfast,
        class Allocator=std::allocator<char> >
    class ArrayHash {
    public:
        /* Some typedefs */
        typedef size_t    size_type;
        typedef Value     value_type;
        typedef Hash      hash_type;
        typedef Allocator allocator_type;
        
        /* Share the hash function object across all such classes */
        static const hash_type      hasher;
        /* And the allocator common to all of these */
        static       allocator_type allocator;
        
        ArrayHash(size_type b=16384): num_bins(b), bins(NULL) {
            /* Allocate the number of bins that we'll need. For this initial 
             * allocation, we'll actually use malloc, but for the allocations of 
             * the contents of each bin, we'll use the allocator */
            std::allocator<char*> tmp;
            try {
                bins = tmp.allocate(num_bins);
                for (size_type i = 0; i < num_bins; ++i) {
                    bins[i] = NULL;
                }
            } catch(std::bad_alloc e) {
                std::cout << "Could not allocate" << std::endl;
                bins = NULL;
            }
        }
        
        ~ArrayHash() {
            if (bins == NULL) {
                /* Bail out early -- nothing to see here */
                return;
            }
            
            for (size_type i = 0; i < num_bins; ++i) {
                if (bins[i] == NULL) {
                    continue;
                }
                /* Now, we have to deallocate the char * we were given */
                free(static_cast<void*>(bins[i]));
                bins[i] = NULL;
            }
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
        value_type& operator[](const char* key) {
            value_type& ref(get(key, strlen(key)));
            return ref;
        }
        
        value_type& operator[](const std::string& key) {
            return get(key.c_str(), key.length());
        }
        
        /* Retrieve only
         *
         * Supports binary keys by providing a length, too. Missing keys are 
         * automatically inserted, and a reference to the new item is returned 
         */
        value_type& get(const char* k, size_type len) {
            /* We need to hash the function, see if it exists, and if it already 
             * exists in the value, when we'll update the value and return a 
             * reference. If it doesn't, we'll append. If that bin is NULL, then 
             * we'll go ahead and allocate it already */
            size_type position = hasher(k, len) % num_bins;
            char*  bin = bins[position];
            if (bin == NULL) {
                /* Allocate some space, set, return early. We need enough for a 
                 * size_type for how many items are in the list, enough for the 
                 * key, and then enough for a copy of the value */
                size_type new_len = sizeof(size_type) + sizeof(size_type) + 
aligned(len) + aligned(sizeof(value_type));
                bin = bins[position] = static_cast<char*>(malloc(new_len));
                //std::cout << "  Allocating " << new_len << " for " << position << " into " << static_cast<void*>(bin) << std::endl;
                /* Now, we'll advance bin as we add more to it. First, set the 
                 * count to be just 1 */
                *(reinterpret_cast<size_type*>(bin)) = 1;
                bin += sizeof(size_type);
                return set_record(bin, k, len);
            }
            
            /* If we've gotten this far, the bin existed -- we need to do a 
             * linear scan to see if it's already in here. First, let's see how 
             * many items are actually in this array */
            if (find_in_bin(bin, k, len)) {
                //std::cout << "Found record in bin" << std::endl;
                return get_value(bin);
            }
            
            /* If we've gotten this far, the key doesn't exist in its table, in
             * which case we'll have to append it to the end of the character 
             * array that we created. This will involve a reallocation.
             *
             * Apparently the std::allocator doesn't actually use the hint 
             * provided by `allocate`, but we'll give it a shot anyways. We may
             * eventually want to use this with an allocator that is aware of 
             * realloc
             *
             * In order to determine how many items we need to declare, we have 
             * to compare to the original pointer, and then add how much space 
             * we'll need for this appended item.
             */
            //std::cout << "New record in existing bin" << std::endl;
            size_type old_len = bin - bins[position];
            size_type new_len = old_len + sizeof(size_type) + aligned(len)
                + aligned(sizeof(value_type));
            /* We'll make use of hint */
            //std::cout << "Deallocating " << old_len << " for " << position << " out of " << static_cast<void*>(bins[position]) << std::endl;
            //std::cout << "  Allocating " << new_len << " for " << position << " into " << static_cast<void*>(bin) << std::endl;
            bin = static_cast<char*>(realloc(bins[position], new_len));
            // if (bin != bins[position]) {
            //     //std::cout << "Realloc failed! " << std::endl;
            //     /* Now, we'll copy, deallocate, swap */
            //     // memcpy(bin, bins[position], old_len);
            //     // free(static_cast<void*>(bins[position]));
            // } else {
            //     //std::cout << "Realloc to the rescue!" << std::endl;
            // }
            //std::cout << "New position: " << reinterpret_cast<void*>(bin) << " old: " << reinterpret_cast<void*>(bins[position]) << std::endl;
            bins[position] = bin;
            /* Now increment the count by one, and then add the new value */
            ++(*bin);
            bin += old_len;
            return set_record(bin, k, len);
        }
        
        /* Insert
         *
         * This interface is provided for both continuity and to support binary 
         * data (where the length of the char* buffer is provided)
         */
        value_type& insert(const char* key, const value_type& value) {
            return insert(key, strlen(key), value);
        }
        
        value_type& insert(const char* k, size_type len, const value_type& v) {
            return (get(k, len) = v);
        }
        
        value_type& insert(const std::string& key, const value_type& value) {
            return insert(key.c_str(), key.length(), value);
        }
        
        /* Remove
         *
         * If the provided key exists, then it is removed. If the key does not 
         * exist, it has no effect. Normally, it would return a reference, but 
         * since it's possible the key doesn't exist, we can't. */
        void remove(const char* k) {
            remove(k, strlen(k));
        }
        
        void remove(const char* k, size_type len) {
            size_type position = hasher(k, len) % num_bins;
            char* bin = bins[position];
            if (bin == NULL) { return; }
            
            if (!find_in_bin(bin, k, len)) { return; }
            
            /* Otherwise, we've got just a little bit of work to do. First 
             * things first, we need to decrement the number of items we have 
             * in the bin */
            //--(*reinterpret_cast<size_type*>(bin));
        }
        
        void remove(const std::string& key) {
            remove(key.c_str(), key.length());
        }
        
        /* Existence
         *
         * Returns true if the provided key exists, else, false */
        bool exists(const char* k) {
            return exists(k, strlen(k));
        }
        
        bool exists(const char* k, size_type len) {
            size_type position = hasher(k, len) % num_bins;
            char* bin = bins[position];
            if (bin == NULL) { return false; }
            
            return find_in_bin(bin, k, len);
        }
        
        bool exists(const std::string& key) {
            return exists(key.c_str(), key.length());
        }
        
    private:
        /* How many bins are we using? */
        size_type  num_bins;
        /* We need an array of char*'s */
        char ** bins;
        
        /* This is for byte alignment purposes. Turns out we have to align 
         * things well */
        size_type aligned(size_type len, size_type multiple=8) {
            size_t remainder = len % multiple;
            if (remainder) {
                return len + multiple - remainder;
            }
            return len;
        }
        
        /* Return a reference to the value at the provided record as returned by 
         * find_in_bin */
        value_type& get_value(char* record) {
            size_type *len  = reinterpret_cast<size_type* >(record);
            value_type* cpy = reinterpret_cast<value_type*>(record + 
                sizeof(size_type) + aligned(*len));
            return *cpy;
        }
        
        /* Fill in a record starting at the provided pointer, and return a 
         * reference to the value_type stored in it */
        value_type& set_record(char* r, const char* k, size_type len) {
            /* Now, set the length of the string key that we're inserting */
            *(reinterpret_cast<size_type*>(r)) = len;
            r += sizeof(size_type);
            /* Now, copy the string into the new memory */
            memcpy(reinterpret_cast<void*>(r),
                reinterpret_cast<const void*>(k), len);
            r += aligned(len);
            /* Lastly, we have have to create a new value type */
            value_type* cpy = new (r) value_type();
            return *cpy;
        }
        
        /* Find the start of a record in the provided bin. Returns NULL if not 
         * found and returns a pointer starting where the length is encoded */
        bool find_in_bin(char*& bin, const char* k, size_type len) {
            size_type* count = reinterpret_cast<size_type*>(bin);
            /* Let's advance the pointer as we're moving along */
            bin += sizeof(size_type);
            for (size_type i = 0; i < *count; ++i) {
                /* If the lengths of the two keys aren't equal, then they can't 
                 * be equal */
                size_type* l = reinterpret_cast<size_type*>(bin);
                if (*l == len && !strncmp(bin + sizeof(size_type), k, len)) {
                    return true;
                }
                /* Advance the pointer to just past this item */
                bin += (sizeof(size_type) + aligned(*l) + 
                    aligned(sizeof(value_type)));
            }
            return false;
        }
    };
}

#endif
