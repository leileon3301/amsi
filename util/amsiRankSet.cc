#include "amsiRankSet.h"
#include "amsiPArray.h"
#include "amsiUUID.h"
#include <cassert>
#include <algorithm>
namespace amsi
{
  void SimpleRankSet::toArray(rank_t * rs) const
  {
    assert(rs);
    std::copy(s.begin(),s.end(),rs);
  }
  RankSet * SimpleRankSet::copy()
  {
    RankSet * result = new SimpleRankSet(*this);
    return result;
  }
  void intersection(const RankSet * a, const RankSet * b, RankSet * rslt)
  {
    int asz = a->size();
    int bsz = b->size();
    rank_t ar[asz];
    rank_t br[bsz];
    a->toArray(ar);
    b->toArray(br);
    int rsz = asz > bsz ? asz : bsz;
    rank_t rs[rsz];
    rank_t * nsz = std::set_intersection(ar,ar+asz,
                                         br,br+bsz,
                                         rs);
    insertFrom(rslt,&rs[0],nsz);
  }
  void merge(const RankSet * a, const RankSet * b, RankSet * rslt)
  {
    rank_t asz = a->size();
    rank_t bsz = b->size();
    rank_t as[asz];
    rank_t bs[bsz];
    a->toArray(as);
    b->toArray(bs);
    insertFrom(rslt,&as[0],as+asz);
    insertFrom(rslt,&bs[0],bs+bsz);
  }
  void diff(const RankSet * a, const RankSet * b, RankSet * rslt)
  {
    rank_t asz = a->size();
    rank_t bsz = b->size();
    rank_t as[asz];
    rank_t bs[bsz];
    a->toArray(as);
    b->toArray(bs);
    rank_t ns[asz];
    rank_t * nsz = std::set_difference(as,as+asz,
                                       bs,bs+bsz,
                                       ns);
    insertFrom(rslt,ns,nsz);
  }
  void getGlobalRanks(RankSet * rslt, MPI_Comm cm)
  {
    int sz = 0;
    int rnk = -1;
    int grnk = -1;
    MPI_Comm_size(cm,&sz);
    MPI_Comm_rank(cm,&rnk);
    MPI_Comm_rank(AMSI_COMM_WORLD,&grnk);
    int * rnks = new int[sz]();
    MPI_Allgather(&grnk,1,mpi_type<int>(),
                  &rnks[0],1,mpi_type<int>(),
                  cm);
    insertFrom(rslt,rnks,rnks+sz);
    delete [] rnks;
  }
  // collective on the rankset in amsi comm world
  // requires the mpi3 standard
  MPI_Comm makeComm(RankSet * rs)
  {
    int rnk = -1;
    MPI_Comm_rank(AMSI_COMM_WORLD,&rnk);
    MPI_Group wrld_grp;
    MPI_Comm_group(AMSI_COMM_WORLD,&wrld_grp);
    rank_t sz = rs->size();
    rank_t grp_rnks[sz];
    rs->toArray(grp_rnks);
    MPI_Group sbgrp = MPI_GROUP_EMPTY;
    if(rs->contains(rnk))
      MPI_Group_incl(wrld_grp,rs->size(),grp_rnks,&sbgrp);
    // abs protects against overflow which would give a negative value
    // really we only need tg to be the same on all processes in sbgrp
    // which this achieves, though there are undoubtedly better ways to
    // accomplish this
    //int tg = abs((int)generateRangeUUID((rank_t*)grp_rnks,grp_rnks+sz));
    MPI_Comm result;
    MPI_Comm_create(AMSI_COMM_WORLD,sbgrp,&result);
    //TODO: this is in the mpi3 standard and allows the creation to not be collective over amsi_comm_world
    //MPI_Comm_create_group(AMSI_COMM_WORLD,sbgrp,tg,&result);
    return result;
  }
}
