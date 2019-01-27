#include "worker.hpp"

class manager
{
public:
	void init();
private:
	void callback(int, const std::string&);
	
	worker m_worker;
	int m_stored_value;
};
