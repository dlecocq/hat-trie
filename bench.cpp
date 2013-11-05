#include <iostream>

/* Include the libraries we actually want to bench */
#include <array-hash.hpp>

/* Include the unordered map, the array-hash's competitor */
#include <map>
#include <vector>
#include <cassert>
#include <fstream>
#include <algorithm>
#include <tr1/unordered_map>

/* Insert each items into the map with value 1 */
template <class m, class iterator>
long int insertion(m& mp, iterator first, iterator last) {
    long int result(CLOCKS_PER_SEC);
    for (size_t i = 0; i < 10; ++i) {
        clock_t start = clock();
        for (iterator it = first; it != last; ++it) {
            mp[*it
                ] = 1;
        }
        clock_t end = clock();
        result = std::min(result, static_cast<long int>(end - start));
    }
    return result;
}

/* Query each of the items in the list, ensuring that it exists */
template <class m, class iterator>
long int query(m& mp, iterator first, iterator last) {
    long int result(CLOCKS_PER_SEC);
    for (size_t i = 0; i < 10; ++i) {
        clock_t start = clock();
        for (iterator it = first; it != last; ++it) {
            assert(mp[*it] == 1);
        }
        clock_t end = clock();
        result = std::min(result, static_cast<long int>(end - start));
    }
    return result;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Provide a path" << std::endl;
        exit(1);
    }

    /* This will hold our results */
    std::map<std::string,std::map<std::string,double> > insertion_times;
    std::map<std::string,std::map<std::string,double> > query_times;
    std::map<std::string,std::map<std::string,double> > rquery_times;

    /* Read in the data */
    std::cerr << "Reading " << argv[1] << "..." << std::endl;
    std::vector<std::string> words;
    std::ifstream ifs(argv[1], std::ifstream::in);
    while (ifs.good() && words.size() < 100000) {
        std::string s;
        ifs >> s;
        words.push_back(s);
    }
    ifs.close();
    std::cerr << "Done reading." << std::endl;

    /* Run this for each of the number of elements provided */
    size_t counts[] = {
           10,    20,    30,    40,    50,    60,    70,    80,    90,
          100,   150,   200,   250,   300,   350,   400,   450,   500,
          550,   600,   650,   700,   750,   800,   850,   900,   950,
         1000,  1500,  2000,  2500,  3000,  3500,  4000,  4500,  5000,
         5500,  6000,  6500,  7000,  7500,  8000,  8500,  9000,  9500,
        10000, 15000, 20000, 25000, 30000, 35000, 40000, 45000, 50000,
        55000, 60000, 65000, 70000, 75000, 80000, 85000, 90000, 95000
    };

    std::cerr << "       |          ArrayHash          |           std::map          |          tr1::umap          |" << std::endl;
    std::cerr << " Count | Insert  |  Query  | Random  | Insert  |  Query  | Random  | Insert  |  Query  | Random  |" << std::endl;
    for (size_t i = 0; i < 63; ++i) {
        std::vector<std::string>::iterator start = words.begin();
        std::vector<std::string>::iterator end   = words.begin() + counts[i];

        /* Declare our contestsants */
        ht::ArrayHash<                      size_t> htah;
        std::map<std::string,               size_t> stdmap;
        std::tr1::unordered_map<std::string,size_t> tr1umap;

        /* Make a copy, and shuffle it */
        std::vector<std::string> shuffled(start, end);
        std::random_shuffle(shuffled.begin(), shuffled.end());

        /* First, insertion times */
        printf("%6lu", counts[i]);
        printf("   %7lu", insertion(htah, start, end));
        printf("   %7lu", query(htah, start, end));
        printf("   %7lu", query(htah, shuffled.begin(), shuffled.end()));

        printf("   %7lu", insertion(stdmap, start, end));
        printf("   %7lu", query(stdmap, start, end));
        printf("   %7lu", query(stdmap, shuffled.begin(), shuffled.end()));

        printf("   %7lu", insertion(tr1umap, start, end));
        printf("   %7lu", query(tr1umap, start, end));
        printf("   %7lu", query(tr1umap, shuffled.begin(), shuffled.end()));
        std::cout << std::endl;
    }
    
    return 0;
}
