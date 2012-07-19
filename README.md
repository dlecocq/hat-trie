HAT-Trie
========
I've been wanting to play around with the
[HAT-Trie (pdf)](http://crpit.com/Vol62.html) described by
[Dr. Askitis](http://www.naskitis.com/). The jist of it is that it is supposed 
to have all the wonderful performance characteristics of a trie -- it's 
sorted, it's got a bit of compression built into the thing, etc. -- but in a 
cache-conscious way. In particular, much of this strength comes from the fact 
that the tree doesn't branch all the way down to hold individual words in 
leaves, but leaves actually contain a cache-conscious unsorted associative data
structure, the `array-hash`.

There are a few reasons for this repository:

1. Askitis has licensed his code in such a way to prevent commercial use.
1. I'd like to gain an appreciation for this and similar data structures
1. I'd like to benchmark this relative to other data structures, in absence of 
	the implementation from the paper (worrying about license pollution)

As a __goal__ I'd like to see a canonical open source (and commercially usable)
implementation emerge. May not be this code base, of course. In fact, there are
a couple of implementations, in [C](https://github.com/dcjones/hat-trie) and in
[C++](https://github.com/chris-vaszauskas/hat-trie) that have served as 
inspiration and good conversation. This is in hopes of continuing that 
conversation.

Array Hash
==========
Before the HAT-Trie can be implemented, we must have the `array-hash`. It's 
very similar to traditional hash table, except that bins are not linked lists,
but are contiguous chunks of memory with the keys and values interleaved. The 
hope is that since there are fewer levels of indirection, cache performance may
be improved.

I tried two implementations: 1) using a `char*` directly and encoding the keys and values manually and 2) using `std::vector` which internally uses the same 
technique, but has the added bonus of being _much_ easier to deal with. The 
`char*` version is available under the `char` branch, and the (more complete) 
`std::vector` approach can be found under the `vector` branch.

In terms of __performance__, `std::tr1::unordered_map` reliably beat both 
implementations after about 10-15k keys are inserted, and both `std::vector`
and `char*` generally perform better for fewer than about 4k keys. 
Interestingly enough, however, `std::vector<std::pair<std::string,T> >` 
performed comparably well when compared to the `char*` version. It also makes
the code significantly more readable and _much_ easier to deal with.

For this test, I used `std::tr1::unordered_map<std::string,size_t>`, and for 
keys I used the `distinct_t` dataset hosted on Askitis' site. I tested with 
both `g++ 4.2.2` and `clang 2.1` on a 2011-ish MacBook Pro. I encourage you to 
run `make bench` yourself to compare this implementation of `array-hash`, 
`std::map` (for fun) and `std::tr1::unordered_map`.

