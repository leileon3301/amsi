#ifndef AMSI_PLANMIGRATION_H_
#define AMSI_PLANMIFRATION_H_
#include "amsiMetaConfig.h"
#include <amsiMPI.h>
#include <vector>
namespace amsi
{
  class DataDistribution;
  class CommPattern;
  void CommPattern_PlanMigration_Test(std::vector<int> & m_index,
                                      std::vector<int> & m_send_to,
                                      CommPattern * pattern,
                                      const DataDistribution * dd,
                                      int rank,
                                      MPI_Comm task_comm);
  void CommPattern_PlanMigration_Full(std::vector<int> & m_index,
                                      std::vector<int> & m_send_to,
                                      CommPattern * pattern,
                                      const DataDistribution * dd,
                                      int rank,
                                      MPI_Comm task_comm);
# ifdef ZOLTAN
  void CommPattern_PlanMigration_Zoltan(std::vector<int> & m_index,
                                        std::vector<int> & m_send_to,
                                        CommPattern * pattern,
                                        const DataDistribution * dd,
                                        int rank,
                                        MPI_Comm task_comm);
# endif
}
#endif