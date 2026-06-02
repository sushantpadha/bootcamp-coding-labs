# Report

Initially compiled with `-O2 -g [-fno-omit-frame-pointer]`:

## Baselne - 128 cols
**perf stat -d**

        Performance counter stats for './main':

        46,447,962      task-clock                       #    0.971 CPUs utilized             
                 4      context-switches                 #   86.118 /sec                      
                 3      cpu-migrations                   #   64.588 /sec                      
             2,706      page-faults                      #   58.259 K/sec                     
       210,062,906      instructions                     #    1.03  insn per cycle            
                                                  #    0.18  stalled cycles per insn     (69.91%)
       203,613,025      cycles                           #    4.384 GHz                         (69.84%)
        36,790,874      stalled-cycles-frontend          #   18.07% frontend cycles idle        (70.35%)
        31,734,409      branches                         #  683.225 M/sec                       (72.50%)
         3,227,451      branch-misses                    #   10.17% of all branches             (74.12%)
        55,138,245      L1-dcache-loads                  #    1.187 G/sec                       (72.66%)
        22,400,228      L1-dcache-load-misses            #   40.63% of all L1-dcache accesses   (70.61%)

       0.047814885 seconds time elapsed

       0.039824000 seconds user
       0.007964000 seconds sys

**perf record** (compiled with `-g -fno-omit-frame-pointer`)
```
    99.38%     0.00%  main     main                  [.] main
            |
            ---main
               |          
               |--90.65%--run_epoch(std::vector<Packet, std::allocator<Packet> >&, std::vector<int, std::allocator<int> > const&, std::vector<int, std::allocator<int> > const&, std::vector<int, std::allocator<int> > const&, std::vector<int, std::allocator<int> >&, int)
               |          |          
               |          |--69.33%--process_packets(std::vector<Packet, std::allocator<Packet> > const&, std::vector<int, std::allocator<int> > const&, std::vector<int, std::allocator<int> > const&, std::vector<int, std::allocator<int> > const&)
               |          |          |          
               |          |          |--43.69%--chase_dependency(int, int, std::vector<int, std::allocator<int> > const&, std::vector<int, std::allocator<int> > const&)
               |          |          |          |          
               |          |          |          |--6.31%--std::vector<int, std::allocator<int> >::operator[](unsigned long) const
               |          |          |          |          
               |          |          |           --1.40%--std::vector<int, std::allocator<int> >::size() const
               |          |          |          
               |          |          |--10.53%--branchy_score(Packet const&, std::vector<int, std::allocator<int> > const&)
               |          |          |          
               |          |          |--0.65%--std::vector<int, std::allocator<int> >::size() const
               |          |          |          
               |          |           --0.60%--std::vector<int, std::allocator<int> >::operator[](unsigned long) const
               |          |          
               |          |--14.38%--refresh_history(std::vector<int, std::allocator<int> >&, std::vector<Packet, std::allocator<Packet> > const&, int)
               |          |          |          
               |          |           --3.20%--std::vector<int, std::allocator<int> >::operator[](unsigned long)
               |          |          
               |          |--5.76%--cold_column_probe(std::vector<int, std::allocator<int> > const&, int, int, int)
               |          |          |          
               |          |           --1.77%--std::vector<int, std::allocator<int> >::operator[](unsigned long) const
               |          |          
               |           --0.64%--std::vector<int, std::allocator<int> >::operator[](unsigned long)
               |          
               |--3.52%--build_dependency_next(int)
               |          |          
               |          |--1.02%--std::vector<int, std::allocator<int> >::operator[](unsigned long)
               |          |          
               |           --0.79%--std::vector<int, std::allocator<int> >::vector(unsigned long, std::allocator<int> const&)
               |                     |          
               |                      --0.76%--std::vector<int, std::allocator<int> >::_M_default_initialize(unsigned long)
               |                                int* std::__uninitialized_default_n_a<int*, unsigned long, int>(int*, unsigned long, std::allocator<int>&)
               |                                int* std::__uninitialized_default_n<int*, unsigned long>(int*, unsigned long)
               |                                int* std::__uninitialized_default_n_1<true>::__uninit_default_n<int*, unsigned long>(int*, unsigned long)
               |                                int* std::fill_n<int*, unsigned long, int>(int*, unsigned long, int const&)
               |                                int* std::__fill_n_a<int*, unsigned long, int>(int*, unsigned long, int const&, std::random_access_iterator_tag)
               |                                void std::__fill_a<int*, int>(int*, int*, int const&)
               |                                __gnu_cxx::__enable_if<std::__is_scalar<int>::__value, void>::__type std::__fill_a1<int*, int>(int*, int*, int const&)
               |          
               |--2.92%--build_packets(int, int, int)
               |          |          
               |           --1.53%--std::vector<Packet, std::allocator<Packet> >::vector(unsigned long, std::allocator<Packet> const&)
               |                     std::vector<Packet, std::allocator<Packet> >::_M_default_initialize(unsigned long)
               |                     Packet* std::__uninitialized_default_n_a<Packet*, unsigned long, Packet>(Packet*, unsigned long, std::allocator<Packet>&)
               |                     Packet* std::__uninitialized_default_n<Packet*, unsigned long>(Packet*, unsigned long)
               |                     Packet* std::__uninitialized_default_n_1<true>::__uninit_default_n<Packet*, unsigned long>(Packet*, unsigned long)
               |                     Packet* std::fill_n<Packet*, unsigned long, Packet>(Packet*, unsigned long, Packet const&)
               |                     Packet* std::__fill_n_a<Packet*, unsigned long, Packet>(Packet*, unsigned long, Packet const&, std::random_access_iterator_tag)
               |                     void std::__fill_a<Packet*, Packet>(Packet*, Packet*, Packet const&)
               |                     __gnu_cxx::__enable_if<!std::__is_scalar<Packet>::__value, void>::__type std::__fill_a1<Packet*, Packet>(Packet*, Packet*, Packet const&)
               |                     |          
               |                      --1.30%--asm_exc_page_fault
               |                                |          
               |                                 --1.21%--exc_page_fault
               |                                           |          
               |                                            --1.17%--do_user_addr_fault
               |                                                      |          
               |                                                       --1.09%--handle_mm_fault
               |                                                                 |          
               |                                                                  --1.06%--__handle_mm_fault
               |                                                                            |          
               |                                                                             --0.97%--handle_pte_fault
               |                                                                                       |          
               |                                                                                        --0.91%--do_anonymous_page
               |                                                                                                  |          
               |                                                                                                   --0.73%--alloc_anon_folio
               |          
               |--0.96%--build_dependency_value(int)
               |          
                --0.80%--std::vector<int, std::allocator<int> >::vector(unsigned long, int const&, std::allocator<int> const&)
                          std::vector<int, std::allocator<int> >::_M_fill_initialize(unsigned long, int const&)
                          int* std::__uninitialized_fill_n_a<int*, unsigned long, int, int>(int*, unsigned long, int const&, std::allocator<int>&)
                          int* std::uninitialized_fill_n<int*, unsigned long, int>(int*, unsigned long, int const&)
                          int* std::__uninitialized_fill_n<true>::__uninit_fill_n<int*, unsigned long, int>(int*, unsigned long, int const&)
                          int* std::fill_n<int*, unsigned long, int>(int*, unsigned long, int const&)
                          int* std::__fill_n_a<int*, unsigned long, int>(int*, unsigned long, int const&, std::random_access_iterator_tag)
                          void std::__fill_a<int*, int>(int*, int*, int const&)
                          __gnu_cxx::__enable_if<std::__is_scalar<int>::__value, void>::__type std::__fill_a1<int*, int>(int*, int*, int const&)
```

**perf record -e cache-misses** (compiled with `-g -fno-omit-frame-pointer`)
```
    99.68%     0.00%  main     main                  [.] main
            |
            ---main
               |          
               |--98.11%--run_epoch(std::vector<Packet, std::allocator<Packet> >&, std::vector<int, std::allocator<int> > const&, std::vector<int, std::allocator<int> > const&, std::vector<int, std::allocator<int> > const&, std::vector<int, std::allocator<int> >&, int)
               |          |          
               |          |--67.55%--process_packets(std::vector<Packet, std::allocator<Packet> > const&, std::vector<int, std::allocator<int> > const&, std::vector<int, std::allocator<int> > const&, std::vector<int, std::allocator<int> > const&)
               |          |          |          
               |          |          |--51.91%--chase_dependency(int, int, std::vector<int, std::allocator<int> > const&, std::vector<int, std::allocator<int> > const&)
               |          |          |          |          
               |          |          |          |--5.07%--std::vector<int, std::allocator<int> >::operator[](unsigned long) const
               |          |          |          |          
               |          |          |          |--0.87%--std::vector<int, std::allocator<int> >::size() const
               |          |          |          |          
               |          |          |           --0.56%--asm_sysvec_apic_timer_interrupt
               |          |          |                     |          
               |          |          |                      --0.55%--sysvec_apic_timer_interrupt
               |          |          |          
               |          |          |--10.30%--branchy_score(Packet const&, std::vector<int, std::allocator<int> > const&)
               |          |          |          
               |          |           --0.66%--std::vector<Packet, std::allocator<Packet> >::size() const
               |          |          
               |          |--21.08%--cold_column_probe(std::vector<int, std::allocator<int> > const&, int, int, int)
               |          |          |          
               |          |           --6.15%--std::vector<int, std::allocator<int> >::operator[](unsigned long) const
               |          |          
               |          |--8.14%--refresh_history(std::vector<int, std::allocator<int> >&, std::vector<Packet, std::allocator<Packet> > const&, int)
               |          |          |          
               |          |           --0.54%--std::vector<Packet, std::allocator<Packet> >::size() const
               |          |          
               |           --0.63%--std::vector<int, std::allocator<int> >::operator[](unsigned long) const
               |          
                --0.93%--build_dependency_next(int)
```

## Baselne - 2048 cols

**perf stat -d**

        Performance counter stats for './main':

       537,713,460      task-clock                       #    0.998 CPUs utilized             
                 5      context-switches                 #    9.299 /sec                      
                 1      cpu-migrations                   #    1.860 /sec                      
            10,387      page-faults                      #   19.317 K/sec                     
       857,691,777      instructions                     #    0.33  insn per cycle            
                                                  #    0.09  stalled cycles per insn     (71.35%)
     2,594,488,121      cycles                           #    4.825 GHz                         (71.38%)
        78,850,528      stalled-cycles-frontend          #    3.04% frontend cycles idle        (71.38%)
       129,470,521      branches                         #  240.780 M/sec                       (71.36%)
         5,155,744      branch-misses                    #    3.98% of all branches             (71.43%)
       234,882,629      L1-dcache-loads                  #  436.817 M/sec                       (71.59%)
        74,198,976      L1-dcache-load-misses            #   31.59% of all L1-dcache accesses   (71.51%)

       0.538686597 seconds time elapsed

       0.523617000 seconds user
       0.015017000 seconds sys


**perf report** (compiled with `-g -fno-omit-frame-pointer`)

```
    99.92%     0.00%  main     main                  [.] main
            |
            ---main
               |          
               |--96.62%--run_epoch(std::vector<Packet, std::allocator<Packet> >&, std::vector<int, std::allocator<int> > const&, std::vector<int, std::allocator<int> > const&, std::vector<int, std::allocator<int> > const&, std::vector<int, std::allocator<int> >&, int)
               |          |          
               |          |--63.55%--cold_column_probe(std::vector<int, std::allocator<int> > const&, int, int, int)
               |          |          |          
               |          |           --6.73%--std::vector<int, std::allocator<int> >::operator[](unsigned long) const
               |          |          
               |          |--21.23%--refresh_history(std::vector<int, std::allocator<int> >&, std::vector<Packet, std::allocator<Packet> > const&, int)
               |          |          |          
               |          |           --5.89%--std::vector<int, std::allocator<int> >::operator[](unsigned long)
               |          |          
               |          |--10.55%--process_packets(std::vector<Packet, std::allocator<Packet> > const&, std::vector<int, std::allocator<int> > const&, std::vector<int, std::allocator<int> > const&, std::vector<int, std::allocator<int> > const&)
               |          |          |          
               |          |          |--6.90%--chase_dependency(int, int, std::vector<int, std::allocator<int> > const&, std::vector<int, std::allocator<int> > const&)
               |          |          |          |          
               |          |          |           --1.17%--std::vector<int, std::allocator<int> >::operator[](unsigned long) const
               |          |          |          
               |          |           --1.33%--branchy_score(Packet const&, std::vector<int, std::allocator<int> > const&)
               |          |          
               |          |--0.68%--std::vector<int, std::allocator<int> >::operator[](unsigned long) const
               |          |          
               |           --0.57%--std::vector<int, std::allocator<int> >::operator[](unsigned long)
               |          
               |--1.85%--std::vector<int, std::allocator<int> >::vector(unsigned long, int const&, std::allocator<int> const&)
               |          std::vector<int, std::allocator<int> >::_M_fill_initialize(unsigned long, int const&)
               |          int* std::__uninitialized_fill_n_a<int*, unsigned long, int, int>(int*, unsigned long, int const&, std::allocator<int>&)
               |          int* std::uninitialized_fill_n<int*, unsigned long, int>(int*, unsigned long, int const&)
               |          int* std::__uninitialized_fill_n<true>::__uninit_fill_n<int*, unsigned long, int>(int*, unsigned long, int const&)
               |          int* std::fill_n<int*, unsigned long, int>(int*, unsigned long, int const&)
               |          int* std::__fill_n_a<int*, unsigned long, int>(int*, unsigned long, int const&, std::random_access_iterator_tag)
               |          void std::__fill_a<int*, int>(int*, int*, int const&)
               |          __gnu_cxx::__enable_if<std::__is_scalar<int>::__value, void>::__type std::__fill_a1<int*, int>(int*, int*, int const&)
               |          |          
               |           --1.20%--asm_exc_page_fault
               |                     |          
               |                      --1.12%--exc_page_fault
               |                                |          
               |                                 --1.09%--do_user_addr_fault
               |                                           |          
               |                                            --1.04%--handle_mm_fault
               |                                                      |          
               |                                                       --1.01%--__handle_mm_fault
               |                                                                 |          
               |                                                                  --0.96%--handle_pte_fault
               |                                                                            |          
               |                                                                             --0.89%--do_anonymous_page
               |                                                                                       |          
               |                                                                                        --0.70%--alloc_anon_folio
               |          
                --0.51%--build_dependency_next(int)
```

**perf report -e cache-misses** (compiled with `-g -fno-omit-frame-pointer`)
```
    99.88%     0.00%  main     main                  [.] main
            |
            ---main
               |          
                --98.94%--run_epoch(std::vector<Packet, std::allocator<Packet> >&, std::vector<int, std::allocator<int> > const&, std::vector<int, std::allocator<int> > const&, std::vector<int, std::allocator<int> > const&, std::vector<int, std::allocator<int> >&, int)
                          |          
                          |--68.78%--cold_column_probe(std::vector<int, std::allocator<int> > const&, int, int, int)
                          |          |          
                          |           --21.85%--std::vector<int, std::allocator<int> >::operator[](unsigned long) const
                          |          
                          |--24.12%--process_packets(std::vector<Packet, std::allocator<Packet> > const&, std::vector<int, std::allocator<int> > const&, std::vector<int, std::allocator<int> > const&, std::vector<int, std::allocator<int> > const&)
                          |          |          
                          |          |--18.39%--chase_dependency(int, int, std::vector<int, std::allocator<int> > const&, std::vector<int, std::allocator<int> > const&)
                          |          |          |          
                          |          |           --2.02%--std::vector<int, std::allocator<int> >::operator[](unsigned long) const
                          |          |          
                          |           --3.61%--branchy_score(Packet const&, std::vector<int, std::allocator<int> > const&)
                          |          
                          |--4.97%--refresh_history(std::vector<int, std::allocator<int> >&, std::vector<Packet, std::allocator<Packet> > const&, int)
                          |          |          
                          |           --0.52%--asm_sysvec_apic_timer_interrupt
                          |                     |          
                          |                      --0.52%--sysvec_apic_timer_interrupt
                          |          
                           --0.95%--std::vector<int, std::allocator<int> >::operator[](unsigned long) const
```

---

## Observations
This already shows significant performance issues
- High L1D thrashing especially high in 2048 case
- IPC around 1.0 and 0.33 resp. is very low
- Significant frontend stalls and high branch miss rate

From perf record three hotspots were identified (2) `chase_dependency`, (1) `cold_column_probe` and (3) `branchy_score`.

From record on cache misses we see that 1 and 2 generated most cache misses.

Size has an interesting effect here, at 128 columns (2) causes more cache misses how to inherent nature of work, but at 2048 columns stride length of `2048*4` bytes is becomes the bigger problem and (1) becomes bigger contributor of cache misses.

## Inferences
This suggests:
- poor cache locality due to col major traversal
- heavy pointer chasing
- unpredictable branching in scoreing func

---



---

## Phase 1
### Changes (for 2048)
- Change from col to row major traversal
- Unroll pointer chasing loop by 7

```
 Performance counter stats for './main':

       116,246,828      task-clock                       #    0.992 CPUs utilized             
                 0      context-switches                 #    0.000 /sec                      
                 0      cpu-migrations                   #    0.000 /sec                      
            10,387      page-faults                      #   89.353 K/sec                     
       808,954,576      instructions                     #    1.45  insn per cycle            
                                                  #    0.08  stalled cycles per insn     (70.70%)
       557,841,645      cycles                           #    4.799 GHz                         (70.77%)
        66,888,052      stalled-cycles-frontend          #   11.99% frontend cycles idle        (70.77%)
       129,032,271      branches                         #    1.110 G/sec                       (71.47%)
         4,723,711      branch-misses                    #    3.66% of all branches             (72.34%)
       226,874,037      L1-dcache-loads                  #    1.952 G/sec                       (72.39%)
        27,254,589      L1-dcache-load-misses            #   12.01% of all L1-dcache accesses   (71.56%)

       0.117164502 seconds time elapsed

       0.098144000 seconds user
       0.019028000 seconds sys
```

```
    99.81%     0.00%  main     main                  [.] main
            |
            ---main
               |          
               |--92.76%--run_epoch(std::vector<Packet, std::allocator<Packet> >&, std::vector<int, std::allocator<int> > const&, std::vector<int, std::allocator<int> > const&, std::vector<int, std::allocator<int> > const&, std::vector<int, std::allocator<int> >&, int)
               |          |          
               |          |--48.22%--refresh_history(std::vector<int, std::allocator<int> >&, std::vector<Packet, std::allocator<Packet> > const&, int)
               |          |          |          
               |          |           --10.60%--std::vector<int, std::allocator<int> >::operator[](unsigned long)
               |          |          
               |          |--22.12%--process_packets(std::vector<Packet, std::allocator<Packet> > const&, std::vector<int, std::allocator<int> > const&, std::vector<int, std::allocator<int> > const&, std::vector<int, std::allocator<int> > const&)
               |          |          |          
               |          |          |--14.93%--chase_dependency(int, int, std::vector<int, std::allocator<int> > const&, std::vector<int, std::allocator<int> > const&)
               |          |          |          |          
               |          |          |          |--2.55%--std::vector<int, std::allocator<int> >::operator[](unsigned long) const
               |          |          |          |          
               |          |          |           --0.72%--std::vector<int, std::allocator<int> >::size() const
               |          |          |          
               |          |           --3.10%--branchy_score(Packet const&, std::vector<int, std::allocator<int> > const&)
               |          |          
               |          |--21.42%--cold_column_probe(std::vector<int, std::allocator<int> > const&, int, int, int)
               |          |          |          
               |          |           --9.25%--std::vector<int, std::allocator<int> >::operator[](unsigned long) const
               |          |          
               |           --0.90%--std::vector<int, std::allocator<int> >::operator[](unsigned long)
               |          
               |--4.09%--std::vector<int, std::allocator<int> >::vector(unsigned long, int const&, std::allocator<int> const&)
               |          std::vector<int, std::allocator<int> >::_M_fill_initialize(unsigned long, int const&)
               |          int* std::__uninitialized_fill_n_a<int*, unsigned long, int, int>(int*, unsigned long, int const&, std::allocator<int>&)
               |          int* std::uninitialized_fill_n<int*, unsigned long, int>(int*, unsigned long, int const&)
               |          int* std::__uninitialized_fill_n<true>::__uninit_fill_n<int*, unsigned long, int>(int*, unsigned long, int const&)
               |          int* std::fill_n<int*, unsigned long, int>(int*, unsigned long, int const&)
               |          int* std::__fill_n_a<int*, unsigned long, int>(int*, unsigned long, int const&, std::random_access_iterator_tag)
               |          void std::__fill_a<int*, int>(int*, int*, int const&)
               |          __gnu_cxx::__enable_if<std::__is_scalar<int>::__value, void>::__type std::__fill_a1<int*, int>(int*, int*, int const&)
               |          |          
               |           --2.52%--asm_exc_page_fault
```

### Observations
- Runtime decreased dramatically
- IPC increased by a lot
- L1 miss rate dropped substantially
- `chase_dependency` becomes more substantial
- Runtime `~110ms`

---

## Phase 2
### Changes (2048)
- Remove (dumb) unnatural loop in `cold_column_probe` with more vectorizable version
- Specialize the functions based on `history_cols` values by templating
    - This can make loop unrolling, modulos etc. easier for the compiler
- `p.stamp % history_cols` -> `i & (history_cols - 1)` under the assumption history_cols is 2^k
- `branchy_score` is replaed with a linear sequence of branchless computations
    - I also explored writing a massive switch statment (64 cases) - did not help
- Precomputed 7-step jump sum since its in hot loop
    - Led to major gain
- Make the condition in `process_packets` branchless
- Compiler hints - `__restrict__`, `always_inline`, using pointers (rarely though)
- Compiler flags!
    - `-O3 -march=native -funroll-loops` to optimize and really force the compiler to unroll loops*
    - `-fopt-info-loop -fopt-info-vec` print useful info


> \* : compiler did unroll those loops but due to the obvious load-use dependency it did not help HW.
> 
> I also tried:
> - Replacing the prefix carry based sum with two split prefix paths and combining them later to allow for higher ILP
> - Using OpenMP for the same

But they did not help.

```
 Performance counter stats for './main':

        20,454,449      task-clock                       #    0.956 CPUs utilized             
                 2      context-switches                 #   97.778 /sec                      
                 2      cpu-migrations                   #   97.778 /sec                      
             2,963      page-faults                      #  144.858 K/sec                     
       178,841,877      instructions                     #    1.90  insn per cycle            
                                                  #    0.09  stalled cycles per insn     (70.60%)
        94,205,351      cycles                           #    4.606 GHz                         (70.93%)
        16,186,843      stalled-cycles-frontend          #   17.18% frontend cycles idle        (70.91%)
        10,499,388      branches                         #  513.306 M/sec                       (70.59%)
           642,343      branch-misses                    #    6.12% of all branches             (70.69%)
        44,162,973      L1-dcache-loads                  #    2.159 G/sec                       (73.20%)
         7,389,526      L1-dcache-load-misses            #   16.73% of all L1-dcache accesses   (73.07%)

       0.021394340 seconds time elapsed

       0.015323000 seconds user
       0.006129000 seconds sys
```

```
    98.82%    12.59%  main     main                  [.] main
            |          
            |--86.23%--main
            |          |          
            |          |--63.26%--long long run_epoch<128>(std::vector<Packet, std::allocator<Packet> >&, std::vector<int, std::allocator<int> > const&, std::vector<int, std::allocator<int> >&, std::vector<int, std::allocator<int> > const&)
            |          |          |          
            |          |          |--39.18%--process_packets(std::vector<Packet, std::allocator<Packet> > const&, std::vector<int, std::allocator<int> > const&, std::vector<int, std::allocator<int> > const&)
            |          |          |          |          
            |          |          |           --27.55%--branchy_score(Packet const&, std::vector<int, std::allocator<int> > const&)
            |          |          |          
            |          |          |--15.68%--void refresh_history<128>(std::vector<int, std::allocator<int> >&, std::vector<Packet, std::allocator<Packet> > const&)
            |          |          |          |          
            |          |          |          |--0.62%--std::vector<Packet, std::allocator<Packet> >::operator[](unsigned long) const
            |          |          |          |          
            |          |          |           --0.62%--std::vector<Packet, std::allocator<Packet> >::size() const
            |          |          |          
            |          |           --7.60%--cold_column_probe(std::vector<int, std::allocator<int> > const&, int, int, int)
            |          |                     |          
            |          |                      --3.09%--std::vector<int, std::allocator<int> >::operator[](unsigned long) const
            |          |          
            |          |--6.68%--build_dependency_next(int)
            |          |          |          
            |          |          |--1.63%--std::vector<int, std::allocator<int> >::vector(unsigned long, std::allocator<int> const&)
            |          |          |          |...
            |          |          |--1.50%--std::vector<int, std::allocator<int> >::operator[](unsigned long)
            |          |          |          
            |          |           --0.99%--std::enable_if<std::__and_<std::__not_<std::__is_tuple_like<int> >, std::is_move_constructible<int>, std::is_move_assignable<int> >::value, void>::type std::swap<int>(int&, int&)
            |          |          
            |          |--3.98%--build_packets(int, int, int)
```

### Observations (2048)
- Major IPC gain reaching upto 1.9
- Dramatically reduced L1-dcache misses
- Reduced branch misses
- Runtime `~80ms`

---

## Result
~20ms on 128 (2.5x speedup)
~80ms on 2048 (6.5x speedup)