
#include "manager.hpp"
#include <iostream>

void manager::init()
{
	std::cout << "init manager\n";
	m_stored_value = 0;
	m_worker.init([this](int val, const std::string& str){callback(val, str);});
	m_worker.start();
	m_worker.stop();
}

void manager::callback(int test, const std::string& str)
{
	m_stored_value = test;
	std::cout << "callback():  " << m_stored_value << " - " << str << std::endl;
}
