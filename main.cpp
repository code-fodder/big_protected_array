#include <iostream>
#include <vector>
#include <thread>
#include <random>
#include <mutex>
#include <algorithm>
#include <atomic>
#include <chrono>

// Variables (err...ok, constants - but changes these to change the test)
constexpr int array_size = 1000;
constexpr int array_max_index (array_size - 1);
constexpr int num_workers = 8;
constexpr int work_time_ms = 2000;
constexpr bool use_protected = false;
constexpr int work_load_ms = 0;

#define ANALYSIS_ON 1

// The large array
std::array<std::atomic<long unsigned int>, array_size> big_array;
std::array<std::atomic<int>, num_workers> protected_indexes;
std::atomic<int> worker_index;

// Stops the threads
bool running = true;
std::atomic<long> collision_count;
std::atomic<long> lock_count;
std::atomic<long> amount_of_work_done;
std::chrono::duration<double> longest_lock_time;

// Random number generator
std::mt19937 rng;
std::uniform_int_distribution<std::mt19937::result_type> dist_1m(0, array_max_index);
#define get_rand() dist_1m(rng)

class array_protector
{
public:
	array_protector()
	{
		std::lock_guard<std::mutex> lock(mtx);
		allocated_id = ++worker_index;
	};

	void lock_index(int index)
	{
		#if (ANALYSIS_ON)
			auto start = std::chrono::high_resolution_clock::now();
		#endif

		while (!try_lock_index(index))
			;

		#if (ANALYSIS_ON)
			std::chrono::duration<double> elapsed = std::chrono::high_resolution_clock::now() - start;
			if (longest_lock_time < elapsed) { longest_lock_time = elapsed;};
		#endif
	}

	bool try_lock_index(int index)
	{
		std::lock_guard<std::mutex> lock(mtx);
		// Check if item is in the mutexed list
		if ( std::find(protected_indexes.begin(), protected_indexes.end(), index) == protected_indexes.end() ) 
		{
			// If its not then add it - now that array value is safe from other threads
			protected_indexes[(unsigned int) allocated_id] = index;
	
			#if (ANALYSIS_ON)
 				lock_count++;
			#endif
			return true;
		}
		#if (ANALYSIS_ON)
			collision_count++;
		#endif
		return false;
	};

	// Atomic free operation
	void free_index() { protected_indexes[(unsigned int) allocated_id] = -1; };

	int allocated_id;
private:
	std::mutex mtx;

};

void print_protected_indexes()
{
	std::cout << "protected index list: ";
	for (size_t i = 0; i < num_workers; i++)
	{
		if (protected_indexes[i] == -1)
			std::cout << "- ";
		else
			std::cout << protected_indexes[i] << " ";
	}
	std::cout << std::endl;
}

void print_big_array()
{
	std::cout << "big_array: ";
	for (size_t i = 0; i < 20; i++)
	{
		std::cout << big_array[i] << " ";
	}
	std::cout << std::endl;
}
void print_arrays(const std::string & desc)
{
	std::cout << "\n" << desc << std::endl;
	print_protected_indexes();
	print_big_array();
	std::cout << std::endl;
}

void worker_protected()
{
	array_protector ap;
	while (running)
	{
		unsigned int index = get_rand();
		ap.lock_index((int) index);
		big_array[index]++;
		amount_of_work_done++;
		std::this_thread::sleep_for(std::chrono::milliseconds(work_load_ms));
		ap.free_index();
	}
}

void worker_normal()
{
	array_protector ap;
	while (running)
	{
		unsigned int index = get_rand();
		big_array[index]++;
		amount_of_work_done++;
		std::this_thread::sleep_for(std::chrono::milliseconds(work_load_ms));
	}
}


int main()
{
	rng.seed(std::random_device()());
	worker_index = -1;
	std::fill(std::begin(protected_indexes), std::end(protected_indexes), -1);

	print_arrays("start values");

	std::array<std::thread, num_workers> worker_threads;
	for (auto &worker_thread : worker_threads)
	{
		if (use_protected)
		{
			worker_thread = std::thread([]{ worker_protected(); });
		}
		else
		{
			worker_thread = std::thread([]{ worker_normal(); });
		}
		
	}

	// Allow workers to do their thing...
	std::this_thread::sleep_for(std::chrono::milliseconds(work_time_ms / 2));
	// Halfway print out
	print_arrays("~halfway values");
	std::this_thread::sleep_for(std::chrono::milliseconds(work_time_ms / 2));
	running = false;

	for (auto &worker_thread : worker_threads)
	{
		worker_thread.join();
	}

	print_arrays("finish values");


	#if (ANALYSIS_ON)
		std::cout << "\n------------------------------------------\n"
			      << "Inputs:" << std::endl;
		std::cout << "array size:          " << array_size << std::endl;
		std::cout << "number of workers:   " << num_workers << std::endl;
		std::cout << "work duration:       " << work_time_ms << "ms" << std::endl;
		std::cout << "work load:           " << work_load_ms << "ms" << std::endl;
		std::cout << "using protection:    " << (use_protected ? "true" : "false") << std::endl;
		std::cout << "\nResults:" << std::endl;
		std::cout << "Amount of work done: " << amount_of_work_done << std::endl;
		if (use_protected)
		{
			std::cout << "Collisions:          " << collision_count << std::endl;
			std::cout << "Locks:               " << lock_count << std::endl;
			if (collision_count > 0)
			{
				std::cout << "Radio:               " << (lock_count / collision_count) << " locks per collision" << std::endl;
			}
			else
			{
				std::cout << "Radio:               " << "n/a (no collisions)" << std::endl;
			}
			std::cout << "longest lock time:   " << longest_lock_time.count() * 1000 << " ms\n";
		}
		std::cout << "\n" << std::endl;
	#endif

	return 0;
}

