#include <iostream>
#include <vector>
#include <thread>
#include <random>
#include <mutex>
#include <algorithm>
#include <atomic>
#include <chrono>

#define ARRAY_SIZE 10
#define MAX_INDEX (ARRAY_SIZE - 1)

// The large array
std::vector<long unsigned int> big_array(ARRAY_SIZE);
//std::vector<std::atomic<int>> protected_indexes;
std::array<std::atomic<int>, 10> protected_indexes;
std::atomic<int> worker_index;

// Stops the threads
bool running = true;
std::atomic<long> collision_count;

// Random number generator
std::mt19937 rng;
std::uniform_int_distribution<std::mt19937::result_type> dist_1m(0, MAX_INDEX);
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
		while (!try_lock_index(index))
			;
	}

	bool try_lock_index(int index)
	{
		std::lock_guard<std::mutex> lock(mtx);
		// Check if item is in the mutexed list
		if ( std::find(protected_indexes.begin(), protected_indexes.end(), index) == protected_indexes.end() ) 
		{
			// If its not then add it - now that array value is safe from other threads
			protected_indexes[(unsigned int) allocated_id] = index;
			return true;
		}
		collision_count++;
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
	for (size_t i = 0; i < 10; i++)
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
	for (size_t i = 0; i < 10; i++)
	{
		std::cout << big_array[i] << " ";
	}
	std::cout << std::endl;
}

void worker()
{
	array_protector ap;
	std::cout << "worker " << ap.allocated_id << " - started" << std::endl;
	while (running)
	{
		unsigned int index = get_rand();
		ap.lock_index((int) index);
		big_array[index]++;
		ap.free_index();
	}
	std::cout << "worker " << ap.allocated_id << " - finished" << std::endl;
}

int main()
{
	std::cout << "starting\n";
	rng.seed(std::random_device()());
	worker_index = -1;
	std::fill(std::begin(protected_indexes), std::end(protected_indexes), -1);

	std::cout << "last: " << big_array[MAX_INDEX] << std::endl;
	std::cout << "RAND: " << get_rand() << std::endl;
	std::cout << "RAND: " << get_rand() << std::endl;
	std::cout << "RAND: " << get_rand() << std::endl;

	print_protected_indexes();
	print_big_array();

	std::thread t1 = std::thread([]{ worker(); });
	std::thread t2 = std::thread([]{ worker(); });

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	running = false;

	t1.join();
	t2.join();

	print_protected_indexes();
	print_big_array();

	std::cout << "collisions: " << collision_count << std::endl;
	return 0;
}

