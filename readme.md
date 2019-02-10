## To Compile / Run:
 - `make` - compiles
 - `make test` - compiles and runs
 - Edit the constexpr values at the top of main to change the test parameters

## Summary:
When the workload is 1ms (the time taken to perform each action) then the amount of work done was:

 - 9808 for protected
 - 8117 for normal

 **Note** these values varied, sometimes the normal was higher, there appered no clear winner.

When the workload is 0ms (basically increment a few counters) then the amount of work done was:

 - 9791264 for protected
 - 29307829 for normal

So here you can see that using the mutexed protection slows down the work by a factor of about a third (1/3). This ratio is consistant between tests.

I also ran the same tests for 1 worker, and the same ratios roughly held true. However when I make the array smaller (~1000 elements) the amount of work done is still roughly the same for when the work load is 1ms. But when the workload is very light I got results like:

 - 5621311
 - 39157931

 Which is about 7 times slower.

### Conclusion
 - The larger the array then less collisions occur - the performance is better.
 - The longer the work load is (per item) then the less noticable the difference is with using the protecting mechanism.

It appears that the locking is generally only adding an overhead that is 2-3 times slower then incrementing a few counters. This is probably skewed by actual collisions because (from the results) the longest lock time recorded was a hugh 40ms - but this was when there was the work time was very fast so, many collisions occured (~8 successful locks per collision).


## Results for large array (100,000,000) and very fast workload:

### Protected Results
```
start values
protected index list: - - - - - - - - 
big_array: 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 


~halfway values
protected index list: - - - 88637840 - 37905973 13908272 77751725 
big_array: 0 0 1 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 


finish values
protected index list: - - - - - - - - 
big_array: 0 0 1 0 0 0 0 0 0 0 2 1 0 0 0 1 0 0 0 0 


------------------------------------------
Inputs:
array size:          100000000
number of workers:   8
work duration:       2000ms
work load:           0ms
using protection:    true

Results:
Amount of work done: 9791264
Collisions:          1123132
Locks:               9791264
Radio:               8 locks per collision
longest lock time:   39.4504 ms

```

### Normal Results
```
start values
protected index list: - - - - - - - - 
big_array: 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 


~halfway values
protected index list: - - - - - - - - 
big_array: 1 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 


finish values
protected index list: - - - - - - - - 
big_array: 1 0 0 0 0 0 0 1 0 0 0 0 0 2 1 0 0 0 0 0 


------------------------------------------
Inputs:
array size:          100000000
number of workers:   8
work duration:       2000ms
work load:           0ms
using protection:    false

Results:
Amount of work done: 29307829
```

## Results for large array (100,000,000) and 1ms work load:

### Protected Results
```
start values
protected index list: - - - - - - - - 
big_array: 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 


~halfway values
protected index list: 35904480 70342294 79050180 28948495 62313801 47437651 73381099 44950954 
big_array: 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 


finish values
protected index list: - - - - - - - - 
big_array: 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 


------------------------------------------
Inputs:
array size:          100000000
number of workers:   8
work duration:       2000ms
work load:           1ms
using protection:    true

Results:
Amount of work done: 9808
Collisions:          100816
Locks:               9808
Radio:               0 locks per collision
longest lock time:   2.95311 ms
```

### Normal Results
```
start values
protected index list: - - - - - - - - 
big_array: 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 


~halfway values
protected index list: - - - - - - - - 
big_array: 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 


finish values
protected index list: - - - - - - - - 
big_array: 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 


------------------------------------------
Inputs:
array size:          100000000
number of workers:   8
work duration:       2000ms
work load:           1ms
using protection:    false

Results:
Amount of work done: 8117

```
