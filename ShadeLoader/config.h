// 1 for enable 0 for disable 

#define ENABLE_DEBUG 1 //  debug mode 
#define AUTODESTRUCT 1  // auto destuct 
#define SUSPEND_DOWNLOAD 1 //  waiting time
#define PROTECTION 1 // anti VM / disable IF YOU USE IN ON VM !!! 



#if ENABLE_DEBUG
#include <iostream>
#define DEBUG_PRINT(x) std::cout << x << std::endl
#else
#define DEBUG_PRINT(x)
#endif

