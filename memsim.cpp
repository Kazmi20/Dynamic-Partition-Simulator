// -------------------------------------------------------------------------------------
// this is the only file you need to edit
// -------------------------------------------------------------------------------------
//
// (c) 2022, Pavol Federl, pfederl@ucalgary.ca
// Do not distribute this file.

#include "memsim.h"
#include <cassert>
#include <iostream>
#include <list>
#include <set>
#include <unordered_map>
#include <vector>
#include <unistd.h>





struct Node {
    int64_t address; // page id
    int64_t pid; // -1 free processor id
    bool free;
    int64_t size;
};




using LI = std::list<struct Node>::iterator;

bool cmp(const LI & a, const LI & b) {
    if (a->size == b->size) return a->address < b->address;
    else{
    return a->size > b->size;
    }
}
using SN = std::set<LI, decltype(&cmp)>;


// I recommend you implement the simulator as a class. This is only a suggestion.
// If you decide not to use this class, please remove it before submitting it.


std::list<struct Node> chunks;
std::unordered_map<int64_t, std::vector<LI>> pro_chunks;
SN free_chunks(&cmp);

    
int64_t page_count=0;
int64_t page_size=0;

void print_set(SN & set) {
    std::cout << "\nFree chunks\n";
    for (auto n : set) {
        std::cout << "Index " << n->address << " " << n->size << std::endl;
    }
}
struct Simulator {
  
   
  Simulator(int64_t page_siz)
  {
    

   
    page_size=page_siz;
    

    // constructor
  }
  void allocate(int64_t tag, int64_t size)
  {
    // Pseudocode for allocation request:
    // - search through the list of partitions from start to end, and
    //   find the largest partition that fits requested size
    //     - in case of ties, pick the first partition found
    // - if no suitable partition found:
    //     - get minimum number of pages from OS, but consider the
    //       case when last partition is free
    //     - add the new memory at the end of partition list
    //     - the last partition will be the best partition
    // - split the best partition in two if necessary
    //     - mark the first partition occupied, and store the tag in it
    //     - mark the second partition free
    
    //int largest= largest_free->size;
    int64_t addr=0;
    LI largest_free;

    //Case 1: prog just started.
    if(chunks.empty() && free_chunks.empty()){
      int64_t pages_to_add_empty=0;
      int64_t new_page_size_empty=0;

      
      if( size % page_size==0){
        pages_to_add_empty = (size / page_size);

      }
      else{
        pages_to_add_empty = (size / page_size)+1;

      }

      
            
      new_page_size_empty = pages_to_add_empty * page_size;
      chunks.push_back(Node{0, -1, true,new_page_size_empty });
      free_chunks.insert(std::prev(chunks.end()));
      largest_free= *free_chunks.begin();
      page_count+=pages_to_add_empty;
    }

    //Case 2 : free_chunks empty and chunks not empty.

      else if (free_chunks.empty() && !chunks.empty()){

        int64_t pages_to_add_case2=0;
        int64_t new_page_size_case2=0;

        if( size % page_size==0){
        pages_to_add_case2 = (size / page_size);

        }
        else{
          pages_to_add_case2 = (size / page_size)+1;

        }
        new_page_size_case2 = pages_to_add_case2 * page_size;

      
      
      
        LI last_element_case2 = std::prev(chunks.end());
        addr = last_element_case2->address + last_element_case2->size;
        chunks.push_back(Node{addr, -1, true,new_page_size_case2 });
        free_chunks.insert(std::prev(chunks.end()));
        largest_free= *free_chunks.begin();
        page_count+=pages_to_add_case2;

      }

    
    // Case 3: free chunks not empty and and chunks not empty.

    else if (!free_chunks.empty() && !chunks.empty()){

        largest_free= *free_chunks.begin();
        
        if (size > largest_free->size ){
          LI last_element = std::prev(chunks.end());
          int64_t pages_to_add=0;
          int64_t new_page_size=0;
          
          if (last_element->free == true){   
            
            // pages_to_add = (size / page_size);
            
            // new_page_size = pages_to_add * page_size;                            // correct.

            // if (new_page_size + last_element->size < size){
            //   new_page_size+=page_size;
            //   pages_to_add+=1;
            // }

            int64_t total_size=last_element->size;
            int64_t requested_size= size - last_element->size;

            if( requested_size % page_size==0){
            pages_to_add = (requested_size / page_size);

            }
            else{
            pages_to_add = (requested_size / page_size)+1;

            }
            total_size += pages_to_add*page_size;
            free_chunks.erase(last_element);
            //total_size = last_element->size + new_page_size;
            addr=last_element->address;
            //chunks.erase(last_element);
            //chunks.push_back(Node{addr, -1, true,total_size });
            last_element->size = total_size;
            //std:: cout<< total_size << std::endl;
            
            free_chunks.insert(last_element);
            largest_free=*free_chunks.begin();
            page_count+=pages_to_add;

          }

          // last element is occupied 
          else{

              if( size % page_size==0){
                  pages_to_add = (size / page_size);

                }
              else{
                pages_to_add = (size / page_size)+1;

              }
            
            new_page_size = pages_to_add * page_size;                            // correct.
            addr=last_element->address+last_element->size;
            
            
            
            chunks.push_back(Node{addr, -1, true,new_page_size });     //correct.
            free_chunks.insert(std::prev(chunks.end()));
            largest_free=*free_chunks.begin();
            page_count+=pages_to_add;

          }


        }

        
        
    }
    int64_t old_size=largest_free->size;
    free_chunks.erase(largest_free);

    

    largest_free->pid=tag;
    largest_free->free=false;
    largest_free->size=size;

    pro_chunks[tag].push_back(largest_free);

    int64_t size_left = old_size - largest_free->size;
    int64_t address_left= largest_free->address + largest_free->size;
    if (size_left!=0){
      Node f = Node{address_left,-1,true,size_left};

    chunks.insert(std::next(largest_free),f);
    free_chunks.insert(std::next(largest_free));

    }

    







  }
  void deallocate(int64_t tag)
  {
    // Pseudocode for deallocation request:
    // - for every partition
    //     - if partition is occupied and has a matching tag:
    //         - mark the partition free
    //         - merge any adjacent free partitions

    if (pro_chunks.count(tag)==0){
    }
    else{


    for (auto n : pro_chunks[tag]) {
        n->pid = -1;
        n->free = true;

        // check if prev is free
        if (n != chunks.begin() && std::prev(n)->free==true) {
            // merge
            n->size+=std::prev(n)->size;
            n->address = std::prev(n)->address;
            free_chunks.erase(std::prev(n));
            chunks.erase(std::prev(n));
        }

        // check if next is free
        if (n != std::prev(chunks.end()) && std::next(n)->free==true) {
            // merge
            n->size+=std::next(n)->size;
            free_chunks.erase(std::next(n));
            chunks.erase(std::next(n));
        }

        free_chunks.insert(n);
    }
    pro_chunks[tag].clear();

    }


  }
  MemSimResult getStats()
  {
    // let's guess the result... :)
    MemSimResult result;
    
    if(free_chunks.size()==0){
      result.max_free_partition_size = 0;
      result.max_free_partition_address = 0;


    }
    else{
      LI max_free= *free_chunks.begin();

    
    result.max_free_partition_size = max_free->size;
    result.max_free_partition_address = max_free->address;
    
    }
    result.n_pages_requested = page_count ;
    return result;
  }
  void check_consistency()
  {
    // you do not need to implement this method at all - this is just my suggestion
    // to help you with debugging

    // mem_sim() calls this after every request to make sure all data structures
    // are consistent. Since this will probablly slow down your code, you should
    // disable calling this in the mem_sim() function below before submitting
    // your code for grading.

    // here are some suggestions for consistency checks (see appendix also):

    // make sure the sum of all partition sizes in your linked list is
    // the same as number of page requests * page_size

    // make sure your addresses are correct

    // make sure the number of all partitions in your tag data structure +
    // number of partitions in your free blocks is the same as the size
    // of the linked list

    // make sure that every free partition is in free blocks

    // make sure that every partition in free_blocks is actually free

    // make sure that none of the partition sizes or addresses are < 1
     //print_set(free_chunks);
    //  std::cout << "Chunks\n";
    //  for (auto n : chunks) {
    //      std::cout << "Index " << n.address << " pid(" << n.pid << ") " << n.size << std::endl;
    //  }
    //  int64_t process=0;
    //  std::cout << "\nProcess 1:\n";
    //  for (auto n : pro_chunks[process]) {
    //      std::cout << "Index " << n->address << " " << n->size << std::endl;
    //      process++;
    //  }






  }
};

// re-implement the following function
// ===================================
// parameters:
//    page_size: integer in range [1..1,000,000]
//    requests: array of requests
// return:
//    some statistics at the end of simulation
MemSimResult mem_sim(int64_t page_size, const std::vector<Request> & requests)
{
  // if you decide to use the simulator class, you probably do not need to make
  // any changes to the code below, except to uncomment the call to check_consistency()
  // before submitting your code
  Simulator sim(page_size);
  for (const auto & req : requests) {
    if (req.tag < 0) {
      sim.deallocate(-req.tag);
    } else {
      sim.allocate(req.tag, req.size);
    }
    sim.check_consistency();
  }
  return sim.getStats();
}
