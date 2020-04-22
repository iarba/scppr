#ifndef SCPPR_LIB_LOG_H
#define SCPPR_LIB_LOG_H

#include <iostream>
#include <string>
#include <stdexcept>

#define scppr_ASSERT(value, fail_msg) if(!value){std::cout << fail_msg << std::endl << std::flush; throw std::runtime_error(fail_msg);}
#define scppr_LOG(msg) std::cout << msg << std::endl << std::flush;

#endif // SCPPR_LIB_LOG_H
