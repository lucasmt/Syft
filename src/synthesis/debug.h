#include <iostream>

template<class T>
void report(const std::string& disclaimer, const T& data)
{
  std::cerr << disclaimer << data << std::endl;
}
