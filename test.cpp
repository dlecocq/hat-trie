#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <hash.hpp>
#include <array-hash.hpp>

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
    }
}
