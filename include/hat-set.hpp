/**
 * A HAT-Trie set
 *
 * Much like std::set, in the sense that it contains unique items and maintains 
 * them in sorted order. */

#ifndef HT__HAT_SET_H
#define HT__HAT_SET_H

#include <set>

namespace ht {
    /* This is a GetKey class that converts any provided argument to a char* 
     * directly */
    struct PassThruGetKey {
        template<class Key>
        const char* operator()(const Key& k) const {
            return reinterpret_cast<const char*>(&k);
        }
    };
    
    /**
     * The `Key` class is the class this should hold
     *
     * The `GetKey` class must implement `const char* operator(const Key& k)`, 
     *    which must return a char* that is representative of the key being 
     *    inserted. It will be used for both generating a hash, and for trie 
     *    traversal, and it must be null-terminated. I'm currently considering 
     *    how to make this work best for data types that would not have a null-
     *    terminated representation.
     *
     * The `Allocator` class has the same meaning as in the STL
     *
     * The `Alphabet` parameter defines the anticipated alphabet size (as 
     *    described in the paper. This is essentially the branching factor for
     *    the tries. It defaults to 128, which is what is prescribed in the 
     *    paper
     */
    template <
        class  Key,
        class  GetKey=PassThruGetKey,
        class  Allocator=std::allocator<Key>,
        size_t Alphabet=128>
    class Set {
    public:
        /**
         * Some constants */
        static const size_t BUCKET_CAP    = 16384; // # Items in a bucket
        static const size_t trie_mask     = 0x1; // Is node a trie or bucket?
        static const size_t pure_mask     = 0x2; // Is the node pure or hybrid?
        static const size_t term_mask     = 0x4; // Is this a terminal word?
        
        /* A shared copy of the getter */
        static const GetKey key_getter;
        
        /* Traits */
        typedef size_t             size_type;
        typedef Key                key_type;
        typedef Set                node_type;
        typedef std::set<key_type> bucket_type; // Class for buckets
        
        class iterator : public std::iterator<
            std::bidirectional_iterator_tag, key_type> {
        public:
            typedef iterator  self_type;
            typedef key_type  value_type;
            typedef key_type& reference;
            typedef key_type* pointer;
            typedef int       difference_type;
            typedef std::forward_iterator_tag iterator_category;
            
            iterator(): it(), node(NULL), value(0) {};
            iterator(const iterator& other): it(), node(NULL), value(0) {};
            ~iterator() {};
            
            const self_type& operator=(const self_type& other);
            
            /* Postfix incrementer */
            self_type operator++(int) {
                self_type it_(*this);
                operator++();
                return it_;
            }
            
            /* Prefix incrementer */
            self_type& operator++() {
                return *this;
            }
            
            /* Postfix decrementer */
            self_type operator--(int) {
                self_type it_(*this);
                operator--();
                return it_;
            }
            
            /* Prefix decrementer */
            self_type& operator--() {
                return *this;
            }
            
            /* Dereference */
            const reference operator*() {
                return value;
            }
            
            const pointer operator->() { return *it; }
            
            /* Comparison */
            bool operator==(const self_type& other) const {
                return false;
            }
            
            bool operator!=(const self_type& other) const {
                return false;
            }
        private:
            /* We need an iterator to the bucket type, and we need to know what 
             * node we're on */
            typename bucket_type::iterator it;
            node_type* node;
            key_type value;
        };
        
        /* And the reverse iterator */
        typedef std::reverse_iterator<iterator> reverse_iterator;
        
        Set(): bucket(), parent(NULL), c(0) {
            for (size_t i = 0; i < Alphabet; ++i) {
                children[i] = NULL;
            }
        }
        
        template <class InputIterator>
        Set(InputIterator first, InputIterator last):
            bucket(), parent(NULL), c(0) {
            /* With this set up, we can just insert the above components */
            insert(first, last);
        }
        
        /* Copy constructor */
        Set(const Set<Key,GetKey,Allocator,Alphabet>& other): 
            bucket(), parent(NULL), c(0) {
            /* Now we must copy all the items from the other */
            insert(other.begin(), other.end());
        }
        
        ~Set() {};
        
        /* Copy constructor, assignment take O(n) time */
        
        const Set& operator=(const Set& other) {
            /* Clear out the current set, and insert all the items from other */
            clear();
            insert(other.begin(), other.end());
        }
        
        /* Forward Iterators */
        iterator begin() const { return iterator(); }
        iterator   end() const { return iterator(); }
        
        /* Reverse Iterators */
        reverse_iterator rbegin() const { return reverse_iterator(  end()); }
        reverse_iterator   rend() const { return reverse_iterator(begin()); }
        
        /* Capacity */
        size_type size() const { return 0; }
        size_type max_size() const { return 0; }
        bool      empty() const { return false; }
        
        /* Insertion */
        template <class InputIterator>
        void insert(InputIterator first, InputIterator last) {
            for (; first != last; ++first) { insert(*first); }
        }
        
        std::pair<iterator, bool> insert(const key_type& x) {
            return insert(key_getter(x), x);
        }
        
        iterator insert(const iterator& position, const key_type& x) {
            /* In the future, we should use the provided iterator as a hint */
            insert(x);
            return iterator();
        }
        
        /* Erase */
             void erase(const iterator& position);
        size_type erase(const key_type& x);
             void erase(const iterator& first, const iterator& last);
        
        /* Swap, clear */
        void swap( Set<Key,GetKey,Allocator,Alphabet>& other);
        void clear();
        
        /* Operations */
        iterator  find(const key_type& x) const;
        size_type count(const key_type& x) const;
        iterator  lower_bound(const key_type& x) const;
        iterator  upper_bound(const key_type& x) const;
        std::pair<iterator,iterator> equal_range(const key_type& x) const;
    
    private:
        /* Like the original implementation, we'll allocate Alphabet + 
         * 1, using the extra pointer as housekeeping and flags */
        node_type* children[Alphabet];
        /* My suspicion is that we'll have to play with the byte alignment
         * here at some point */
        bucket_type bucket;
        /* Our parent node, if any */
        node_type* parent;
        /* The character in our parent that we represent, if any */
        char c;
        /* Flags about this current node */
        size_t flags;
        
        /*
         * Helper functions
         */
        
        /* Returns whether or not this node is a trie / bucket */
        bool is_trie()   { return flags & trie_mask; }
        bool is_bucket() { return !is_trie(); }
        /* Is this particular node pure, or hybrid? */
        bool is_pure()   { return flags & pure_mask; }
        bool is_hybrid() { return !is_pure(); }
        /* Is this a terminal word? */
        bool is_term()   { return flags & term_mask; }
        /* Is this full? */
        bool is_full()   { return bucket.size() >= BUCKET_CAP; }
        
        bool exists(const Key& k);
        
        /**
         * Core
         *
         * The methods below implement the core functionality */
        
        /* Split this bucket up */
        void split() {
            std::cout << "Splitting..." << std::endl;
            return;
        }
        
        /* Insert k based on its characteristic const char* */
        std::pair<iterator, bool> insert(const char* x, const key_type& k) {
            std::cout << "In insert: " << x << std::endl;
            
            /* If this is a bucket, then we should insert directly into it.
             * Otherwise, we should find the appropriate branch and insert 
             * it there */
            if (is_trie()) {
                if (x[0] != '\0') {
                    std::cout << "I'm a trie." << std::endl;
                    return children[static_cast<size_t>(x[0])]->insert(x+1, k);
                } else {
                    /* We've exuasted the string. We should mark this as 
                     * begin terminal */
                    std::cout << "I'm terminal" << std::endl;
                }
            }
            
            /* If the bucket is overly full, then we have to split it */
            if (is_full()) {
                split();
            }
            
            /* Now, independent of whether we were full before, we have 
             * space. Therefore, put it into the bucket */
            bucket.insert(k);
            return std::pair<iterator,bool>(iterator(), false);
        }
        
        /* Remove an item */
        void remove(const key_type& x) {
            
        }
    };
}

#endif
