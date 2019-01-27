
#include "worker.hpp"
#include <iostream>
#include <thread>
#include <chrono>
void worker::start()
{
	m_running = true;
	m_count = 0;
	
	// Start the task
	m_result = (std::async([this](){
		m_callback(++m_count, "started");
		while (m_running || m_count <= 5)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			m_callback(++m_count, m_running ? "running" : "stopping");
		}
		m_callback(++m_count, "stopped");
	}));	
}

void worker::stop()
{
	m_running = false;
	// This is here to block - it does not return until the loop is finished.
	m_result.get();
	std::cout << "worker finished" << std::endl;
}
