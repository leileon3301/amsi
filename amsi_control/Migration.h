#ifndef MIGRATION_H_
#define MIGRATION_H_

#include "CommPattern.h"
#include "ControlService.h"

#ifdef ZOLTAN
#include <zoltan.h>
#endif

namespace amsi
{
  
class Migration
{
protected:
  MPI_Comm comm;
  int comm_size;

  int algo;
  void (*usr_algo)();

  std::vector<int> send_to;
  std::vector<int> recvd_from; 

# ifdef ZOLTAN
  Zoltan_Struct * zs;
# endif
  
public:
  enum MigrationAlgorithm
  {
    NO_ALGO = 0,
    USER_ALGO = 1,
    RANDOM_ALGO = 2,
    FULL_ALGO = 3,
    ZOLTAN_ALGO = 4 
  };
  
  Migration(MPI_Comm c,
	    int a = 0,
	    void (*ua)() = NULL);
  
  virtual void plan(std::vector<int> & to_serialize,
	    int local_size,
	    double * local_weights = NULL);

  virtual void execute(std::vector< std::vector<char> > & data);
};


class ScaleSensitiveMigration : public Migration
{
private:
  CommPattern * cp;
  std::vector<int> send_indices;
public:
  ScaleSensitiveMigration(CommPattern * p,
			  MPI_Comm c,
			  int a = 0,
			  void (*ua)() = NULL);
  
  virtual void plan(std::vector<int> & to_serialize,
		    int local_size,
		    double * local_weights = NULL);

  virtual void execute(std::vector< std::vector<char> > & data);
};

}//namespace
#endif