#ifndef UTIL_USAGE__
#define UTIL_USAGE__
#include <cstddef>
#include <iosfwd>
#include <string>

#include <stdint.h>
#include <stdlib.h>

namespace util {

class Memory {
  public:
      Memory():vmem_s(0),pmem_s(0),vmem_e(0),pmem_e(0){ start(); }

      int    getVMEM()    {return vmem_e-vmem_s;}
      double getVMEMMB() {return (vmem_e-vmem_s)/1024.0;}
      int getPMEM(){return pmem_e-pmem_s;}

      void start(){
          update(vmem_s,pmem_s);
      }

      void stop(){
          update(vmem_e,pmem_e);
      }
  private:
      int vmem_s; //virtual mem
      int pmem_s; //peak? mem
      int vmem_e;
      int pmem_e;

      void update(int &vmem, int &pmem);
};

void PrintUsage(std::ostream &to);

// Determine how much physical memory there is.  Return 0 on failure.
uint64_t GuessPhysicalMemory();

// Parse a size like unix sort.  Sadly, this means the default multiplier is K.
uint64_t ParseSize(const std::string &arg);
} // namespace util
#endif // UTIL_USAGE__
