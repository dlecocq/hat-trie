#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <hash.hpp>
#include <hat-set.hpp>
#include <hat-map.hpp>
#include <array-hash.hpp>

struct cmp_int {
    const char* operator()(const int& i) {
        return reinterpret_cast<const char*>(&i);
    }
};

TEST_CASE("hashes", "Implements hashes") {
    /* Our hash object */
    ht::superfast hash;
    
    SECTION("length", "Provides impl for length-provided char*") {
        int foo = 5;
        REQUIRE(hash(reinterpret_cast<char*>(&foo), sizeof(foo)));
    }
}

TEST_CASE("array-hash", "Implements the Array Hash") {
    ht::ArrayHash<int> hash;
    
    SECTION("insert / retrieve", "Implements insertions") {
        SECTION("bracket operator", "Can insert with the bracket operator") {
            REQUIRE(hash.exists("foo") == false);
            hash["foo"] = 5;
            REQUIRE(hash.exists("foo"));
            REQUIRE(hash["foo"] == 5);
        }
        
        SECTION("no length", "Can insert with a null-terminated char*") {
            REQUIRE(hash.exists("testing") == false);
            hash.insert("testing", 5);
            REQUIRE(hash.exists("testing"));
            /* Simple retrieve */
            REQUIRE(hash["testing"] == 5);
            /* Update */
            hash["testing"] = 10;
            REQUIRE(hash["testing"] == 10);
        }
        
        SECTION("with length", "Can insert with length provided") {
            REQUIRE(hash.exists("foob") == false);
            /* Insert 5 with the word 'test' */
            hash.insert("foobar", 4, 5);
            REQUIRE(hash.exists("foob"));
            /* There seems to be some fishiness going on here, where if I 
             * include `hash["foob"]` in the REQUIRE block, it does not return
             * what it should. However, storing in an integer is fine */
            REQUIRE(hash["foob"] == 5);
        }
        
        SECTION("binary key", "Can insert with a binary key") {
            int   i = 5;
            char* p = reinterpret_cast<char*>(&i);
            REQUIRE(hash.exists(p, sizeof(int)) == false);
            hash.insert(p, sizeof(int), 10);
            REQUIRE(hash.exists(p, sizeof(int)));
            REQUIRE(hash.get(p, sizeof(int)) == 10);
        }
    }
    
    SECTION("remove", "Implements removal") {
        SECTION("char*", "Can remove with char*") {
            REQUIRE(hash.exists("chardel") == false);
            hash["chardel"] = 10;
            REQUIRE(hash.exists("chardel"));
            REQUIRE(hash["chardel"] == 10);
            hash.remove("chardel");
            REQUIRE(hash.exists("chardel") == false);
        }
        
        SECTION("binary", "Can remove with binary data") {
            int   i = 10;
            char *p = reinterpret_cast<char*> (&i);
            REQUIRE(hash.exists(p, sizeof(int)) == false);
            hash.insert(p, sizeof(int), 10);
            REQUIRE(hash.exists(p, sizeof(int)));
            hash.remove(p, sizeof(int));
            REQUIRE(hash.exists(p, sizeof(int)) == false);
        }
        
        SECTION("string", "Can remove using a string") {
            std::string s("string");
            REQUIRE(hash.exists(s) == false);
            hash[s] = 10;
            REQUIRE(hash.exists(s));
            hash.remove(s);
            REQUIRE(hash.exists(s) == false);
        }
    }
}

TEST_CASE("hat-set", "The HAT set implementation") {
    SECTION("constructors", "Implements all the correct constructors") {
        SECTION("default", "Default constructor") {
            ht::Set<int> int_s;
            ht::Set<std::string> string_s;
            REQUIRE(   int_s.size() == 0);
            REQUIRE(string_s.size() == 0);
        }
        
        SECTION("special comparator", "Can provide a special comparator") {
            /* Using a comparator, default constructor */
            ht::Set<int, cmp_int> s;
            REQUIRE(s.size() == 0);
        }
        
        SECTION("iterators", "Can be initialized with iterators") {
            std::vector<int> values;
            for (int i = 0; i < 10; ++i) {
                values.push_back(i);
            }
            
            ht::Set<int> s(values.begin(), values.end());
            REQUIRE(s.size() == 10);
        }
        
        SECTION("copy constructor", "We can copy a set") {
            ht::Set<int> s1;
            for (int i = 0; i < 10; ++i) {
                s1.insert(i);
            }
            ht::Set<int> s2(s1);
            REQUIRE(s2.size() == 10);
        }
    }
    
    SECTION("iterators", "Can traverse it") {
        ht::Set<std::string> s;
        
        /* Let's insert a bunch of words */
        std::cout << "Inserting hello" << std::endl;
        s.insert("hello");
        std::cout << "Inserting how" << std::endl;
        s.insert("how");
        s.insert("are");
        s.insert("you");
        s.insert("today");
        
        SECTION("begin/end", "Can traverse in forward order") {
            ht::Set<std::string>::iterator it(s.begin());
            std::vector<std::string> found;
            for(; it != s.end(); ++it) {
                found.push_back(*it);
                std::cout << "Just got " << *it << std::endl;
            }
            REQUIRE(found[0] == "are");
            REQUIRE(found[4] == "today");
        }
        
        SECTION("rbegin/rend", "Can traverse in reverse order") {
            ht::Set<std::string>::reverse_iterator it(s.rbegin());
            std::vector<std::string> found;
            for(; it != s.rend(); ++it) {
                found.push_back(*it);
            }
            REQUIRE(found[0] == "today");
            REQUIRE(found[4] == "are");
        }
    }
}
