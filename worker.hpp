#include <iostream>
#include <functional>
#include <future>

typedef std::function<void (int, const std::string&)> callback_t;

class worker
{
public:
	template<typename functor>
	void init(functor fn)
	{
		m_callback = fn;
		std::cout << "init worker\n";
	}
	
	void start();
	void stop();
	
private:
	callback_t m_callback;
	std::future<void> m_result;
	bool m_running;
	int m_count;

};
