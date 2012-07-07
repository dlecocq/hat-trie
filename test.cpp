#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <hat-set.hpp>
#include <hat-map.hpp>

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
            ht::Set<int, std::greater<int> > s;
            REQUIRE(s.size() == 0);
            /* Using a comparator, using the same object in constructor */
            std::greater<int> cmp;
            ht::Set<int, std::greater<int> > s1(cmp);
            ht::Set<int, std::greater<int> > s2(cmp);
            REQUIRE(s1.size() == 0);
            REQUIRE(s2.size() == 0);
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
        s.insert("hello");
        s.insert("how");
        s.insert("are");
        s.insert("you");
        s.insert("today");
        
        SECTION("begin/end", "Can traverse in forward order") {
            ht::Set<std::string>::Iterator it(s.begin());
            std::vector<std::string> found;
            for(; it != s.end(); ++it) {
                found.push_back(*it);
            }
            REQUIRE(found[0] == "are");
            REQUIRE(found[4] == "today");
        }
        
        SECTION("rbegin/rend", "Can traverse in reverse order") {
            ht::Set<std::string>::Reverse_Iterator it(s.rbegin());
            std::vector<std::string> found;
            for(; it != s.rend(); ++it) {
                found.push_back(*it);
            }
            REQUIRE(found[0] == "today");
            REQUIRE(found[4] == "are");
        }
    }
}
