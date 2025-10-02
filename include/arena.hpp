/*MIT License

Copyright (c) 2025 Tomas Prerovsky

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once
#include <stddef.h>
#include <cstdlib>
#include <cstring>

template<int arena_count>
class Arena{
    struct arena_header{
     arena_header* next{};
     arena_header* prev{};
     char* available{};
     char* limit{};
     size_t counter{};
    };

    arena_header* arenahds{};
    arena_header** arenatails{};
    arena_header* freeblocks{};
    double resize_factor = 1.1;
    public:
    Arena() {
     arenahds = (arena_header*)memset(malloc(arena_count * sizeof(arena_header)),0,arena_count * sizeof(arena_header));
     arenatails = (arena_header**)memset(malloc(arena_count * sizeof(arena_header*)),0,arena_count * sizeof(arena_header*));
     freeblocks = nullptr;
     for (size_t i{}; i < arena_count; ++i){
        arenatails[i] = &arenahds[i]; 
     }
    }
    char* allocate(size_t n, size_t arena){
        if (arena >= arena_count) return nullptr;
        auto ap = arenatails[arena];
        while(ap->available + n > ap->limit) {
                        
            if(!ap->next){
                if (freeblocks) { 
                    ap->next = freeblocks;
                    freeblocks->prev = ap; 
                    freeblocks = freeblocks->next;
                    if (freeblocks) freeblocks->prev = nullptr;
                    ap = arenatails[arena] = ap->next;
                    ap->next = nullptr;
                    ap->available = (char*)ap + sizeof(arena_header);
                    ap->counter = 0;
                    continue; 
                } else {
                 size_t m = n * resize_factor + sizeof(arena_header);
                 auto ap_prev = ap;
                 ap = ap->next = (arena_header*) malloc(m);
                 if (!ap) return nullptr;
                 ap->prev = ap_prev;
                 ap->next = nullptr;
                 ap->counter = 0;
                 ap->limit = (char*)ap + m;
                 ap->available = (char*)ap + sizeof(arena_header);
                 arenatails[arena] = ap;
                 continue;
               }
            }

        }
        if (ap){
            ap->available += n;
            ++ap->counter;
            return ap->available - n;
        }
        return nullptr;
    }
    arena_header* find_page(char* mem, size_t arena){
        for(auto p = &arenahds[arena]; p; p = p->next){
            if (p->limit > mem && (char*)p < mem) return p;
        }
        return nullptr;
    }

    void free(arena_header& page, size_t arena){
        if (arenatails[arena] == &page) arenatails[arena] = page.prev;
        if (page.next) page.next->prev = page.prev;
        if (page.prev) page.prev->next = page.next; 
        page.next = freeblocks;
        page.prev = nullptr;
        if(freeblocks) freeblocks->prev = &page;
        freeblocks = &page;
        
    }

    char* reallocate(char* mem, size_t n_old, size_t n_new, size_t arena){
        auto new_mem = allocate(n_new, arena); //(*)
        if (!new_mem) return nullptr;
        memcpy(new_mem,mem, n_old);   
        auto page = find_page(mem,arena);
        if (page && 0 == --page->counter){
            //INVARIANT: page is not referenced && last allocation (*) hit another page
            //=> page is neither head nor tail of page list
            free(*page, arena);            
        }
        return new_mem;
    }
    void free(size_t arena){
        if (!arenahds[arena].next) return; // nothing to free
        //INVARIANT: &arenahds[arena] != arenatails[arena]
        auto t = freeblocks;
        freeblocks = arenahds[arena].next; 
        if(arenahds[arena].next) arenahds[arena].next->prev = nullptr;
        arenatails[arena]->next = t;
        if (t) t->prev = arenatails[arena];
        arenatails[arena] = &arenahds[arena];
        arenahds[arena].next = arenahds[arena].prev = nullptr;
    }
    void free(size_t arena, char* mem){
        auto page = find_page(mem,arena);
        if (page && page != &arenahds[arena] && 0 == --page->counter) free(*page, arena);    
    }
    ~Arena(){
        for (size_t i{}; i < arena_count; ++i ) free(i);
    }
};