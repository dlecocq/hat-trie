/**
 * A HAT-Trie set
 *
 * Much like std::set, in the sense that it contains unique items and maintains 
 * them in sorted order. */

namespace ht {
    template <
        class Key,
        class Compare=std::less<Key>,
        class Allocator=std::allocator<Key> >
    class Set {
    public:
        class Iterator {
        public:
            Iterator();
            Iterator(const Iterator& other);
            ~Iterator();
            
            const Iterator& operator=(const Iterator& other);
            
            /* Incrementers */
            const Iterator& operator++(int); // postfix
                  Iterator& operator++();    // prefix
            const Iterator& operator--(int); // postfix
                  Iterator& operator--();    // prefix
            
            /* Dereference */
            Key& operator*();
            Key* operator->();
            
            /* Comparison */
            bool operator==(const Iterator& other) const;
            bool operator!=(const Iterator& other) const;
        };
        
        class Const_Iterator {
            
        };
        
        class Reverse_Iterator {
        public:
            Reverse_Iterator();
            Reverse_Iterator(const Reverse_Iterator& other);
            ~Reverse_Iterator();
            
            const Reverse_Iterator& operator=(const Reverse_Iterator& other);
            
            /* Incrementers */
            const Reverse_Iterator& operator++(int); // postfix
                  Reverse_Iterator& operator++();    // prefix
            const Reverse_Iterator& operator--(int); // postfix
                  Reverse_Iterator& operator--();    // prefix
            
            /* Dereference */
            Key& operator*();
            Key* operator->();
            
            /* Comparison */
            bool operator==(const Reverse_Iterator& other) const;
            bool operator!=(const Reverse_Iterator& other) const;
        };
        
        class Const_Reverse_Iterator {
            
        };
        
        /* Traits */
        typedef size_t size_type;
        typedef Key    key_type;
        
        /* Constructors, destructors */
        Set(const Compare& comp = Compare(),
            const Allocator& alloc = Allocator()):
            comparator(comp), allocator(alloc) {};
        
        template <class InputIterator>
        Set(InputIterator first, InputIterator last,
            const Compare& comp = Compare(),
            const Allocator& alloc = Allocator()):
            comparator(comp), allocator(alloc) {
            /* With this set up, we can just insert the above components */
            insert(first, last);
        }
        
        /* Copy constructor */
        Set(const Set<Key,Compare,Allocator>& other): 
            comparator(other.comparator), allocator(other.allocator) {
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
              Iterator begin();
        Const_Iterator begin() const;
              Iterator end();
        Const_Iterator end() const;
        
        /* Reverse Iterators */
              Reverse_Iterator rbegin();
        Const_Reverse_Iterator rbegin() const;
              Reverse_Iterator rend();
        Const_Reverse_Iterator rend() const;
        
        /* Capacity */
        size_type size() const;
        size_type max_size() const;
        bool      empty() const;
        
        /* Insertion */
        template <class InputIterator>
                             void insert(InputIterator f, InputIterator l);
        std::pair<Iterator, bool> insert(const Key& x);
                         Iterator insert(Iterator position, const Key& x);
        
        /* Erase */
             void erase(Iterator position);
        size_type erase(const Key& x);
             void erase(Iterator first, Iterator last);
        
        /* Swap, clear */
        void swap( Set<Key,Compare,Allocator>& other);
        void clear();
        
        /* Operations */
        Iterator  find(const key_type& x) const;
        size_type count(const key_type& x) const;
        Iterator  lower_bound(const key_type& x) const;
        Iterator  upper_bound(const key_type& x) const;
        std::pair<Iterator,Iterator> equal_range(const key_type& x) const;
    private:
        Compare   comparator;
        Allocator allocator;
    };
}
