## Notes:

 - Class Manager has a standard member function: `void callback(int, const std::string&);`
 - Manager initialises worker passing lambda to the callback: `m_worker.init([this](int val, const std::string& str){callback(val, str);});`
 - Worker init function is a template function: `template<typename functor> void init(functor fn)`
 - Work init stores the functor: `m_callback = fn;`
 - m_callback is of wrapper type `std::function<void (int, const std::string&)>`

And that's it - now worker can call `m_callback(int, string)` which, via the magic of lambdas, calls the manager member `callback(int, string)`.
