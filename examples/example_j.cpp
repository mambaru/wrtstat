#define WLOG_ENABLE_DEBUG_LOG ON
#include <signal.h>
#include <iostream>
#include <fstream>
#include <wrtstat/json/load.hpp>
#include <wrtstat/wrtstat_options.hpp>
#include <wrtstat/json/wrtstat_options_json.hpp>
#include <wjson/json.hpp>
#include <wjson/strerror.hpp>
#include <unistd.h>


int main(int argc, char* argv[])
{
  if ( argc!=2 )
  {
    std::cout << "Usage:" << std::endl;
    std::cout << "\t" << argv[0] << " file-name.conf\tDemo log with conf."  << std::endl;
    std::cout << "\t" << argv[0] << " [0|1|2|3]\tGenerate json-configuration."  << std::endl;
    std::cout << "Example:" << std::endl;
    std::cout << "\t" << argv[0] << " 0 | python -mjson.tool"  << std::endl;
    std::cout << "\t" << argv[0] << " 1 > example.conf"  << std::endl;
    std::cout << "\t" << argv[0] << " example.conf"  << std::endl;
    return 0;
  }
  
  wrtstat::wrtstat_options opt;
  
  if ( std::isdigit(argv[1][0]) )
  {
    std::cout << wrtstat::dump(opt) << std::endl;
    return 0;
  }
  
  
  std::string er;
  if ( !wrtstat::load(argv[1], &opt, &er) )
  {
    std::cerr << er << std::endl;
  }

  return 0;
}
