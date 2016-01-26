#ifndef AMSI_DISTRIBUTED_H_
#define AMSI_DISTRIBUTED_H_
#include "amsiMPI.h"
namespace amsi
{
  class Distributed
  {
  protected:
    MPI_Comm cm;
    bool vld;
  public:
     Distributed(MPI_Comm c = MPI_COMM_NULL, bool v = false)
      : cm(c)
      , vld(false)
    { }
    bool isLocallyValid() {return vld;}
    bool isValid()
    {
      int all = 0;
      MPI_Allreduce(&vld,&all,1,MPI_INTEGER,MPI_SUM,cm);
      return all > 0;
    }
    void setValid(bool v) { vld = v; }
    MPI_Comm getComm() { return cm; }
    virtual void synchronize() = 0;
  };
}
#endif
