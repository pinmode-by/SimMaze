
#include "utils.h"

bool parseCommamdLine(int argc, char** argv,
                      optCmdLineMaze& opt) {
 
  // parse command line 
  // if parse == true -> fill struct <optCmdLineMaze>
  // if parse == false -> cout USAGE 
  opt.cmdSize = cv::Size(5, 5);  
  
  return true ;
}
