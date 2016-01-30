#ifndef MPI_T_H_
#define MPI_T_H_
#include <mpi.h>
#include <cstring> //memcpy
#include <iostream>
extern MPI_Comm AMSI_COMM_WORLD;
extern MPI_Comm AMSI_COMM_LOCAL;
typedef int rank_t;
namespace amsi
{
  template <typename T>
    MPI_Datatype mpi_type(T t = NULL);
  template<typename T>
    T comm_sum(T v, MPI_Comm cm = AMSI_COMM_LOCAL)
  {
    T sm = 0;
    MPI_Datatype tp = mpi_type(v);
    size_t sz = 1;
    if(tp == MPI_BYTE)
      sz *= sizeof(T);
    MPI_Allreduce(&v,&sm,sz,mpi_type<T>(),MPI_SUM,cm);
    return sm;
  }
  template <typename T>
    T comm_min(T v, MPI_Comm cm = AMSI_COMM_LOCAL)
  {
    T mn = 0;
    MPI_Allreduce(&v,&mn,1,mpi_type<T>(),MPI_MIN,cm);
    return mn;
  }
  template <typename T>
    T comm_max(T v, MPI_Comm cm = AMSI_COMM_LOCAL)
  {
    T mx = 0;
    MPI_Allreduce(&v,&mx,1,mpi_type<T>(),MPI_MAX,cm);
    return mx;
  }
  template <typename T>
    void send(T * msg, rank_t to, int cnt = 1, MPI_Comm cm = AMSI_COMM_LOCAL)
  {
    MPI_Send(msg,cnt,mpi_type<T>(),to,0,cm);
  }
  template <typename T>
    MPI_Request asend(T * msg, rank_t to, int cnt = 1, MPI_Comm cm = AMSI_COMM_LOCAL)
  {
    MPI_Request rqst;
    MPI_Isend(msg,cnt,mpi_type<T>(),to,0,cm,&rqst);
    return rqst;
  }
  template <typename T>
    MPI_Status recv(T * msg, rank_t frm = MPI_ANY_SOURCE, int cnt = 1, MPI_Comm cm = AMSI_COMM_LOCAL)
  {
    MPI_Status s;
    MPI_Recv(msg,cnt,mpi_type<T>(),frm,MPI_ANY_TAG,cm,&s);
    return s;
  }
  // test if there is a message from rank frm waiting to be recv'd return a sz > 0 if so
  template <typename T>
    int arecv_sz(rank_t & frm, MPI_Comm cm = AMSI_COMM_LOCAL)
  {
    int sz = 0;
    MPI_Status s;
    int flg = 0;
    MPI_Iprobe(frm,0,cm,&flg,&s);
    if(flg)
      MPI_Get_count(&s,mpi_type<T>(),&sz);
    frm = s.MPI_SOURCE;
    return sz;
  }
  // asynchronously recv a message from frm with cnt elements
  template <typename T>
    MPI_Request arecv(T * msg, rank_t frm = MPI_ANY_SOURCE, int cnt = 1, MPI_Comm cm = AMSI_COMM_LOCAL)
  {
    MPI_Request rqst;
    MPI_Irecv(msg,
              cnt,
              mpi_type<T>(),
              frm,
              MPI_ANY_TAG,
              cm,
              &rqst);
    return rqst;
  }
}
#include "amsiMPI_impl.h"
#endif
