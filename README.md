### Epoller DEMO
For the purpose of learning, I simply imitated Shuo Chen's muduo network library.
Use polymorphism to replace the original muduo::net::Channel and std::bind.
Provides a timer for receiving any type of calling object and return a std::future.

Demo needs to depend on nlohmann::json library, you can use the real ip address to replace the parsing of json
