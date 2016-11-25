#ifndef AMSI_FEA_H_
#define AMSI_FEA_H_
#include "amsiBoundaryConditions.h"
#include "amsiLAS.h"
#include "amsiMPI.h"
#include <cstring> // memset
#include <iostream>
#include <list>
#include <map>
#include <vector>
#include <mpi.h>
namespace amsi
{
  typedef int ldof_type;
  typedef long gdof_type;
  // TODO: replace with functor subclassed on field library... or something
  bool isFixed(int);
  template <typename T>
    void Model_PrintInfo(T model, std::ostream & out);
  template <typename T>
    void Mesh_PrintInfo(T mesh, std::ostream & out);
  class FEA
  {
  protected:
    // additional global dofs from constraints
    int constraint_dofs;
    // the number of dofs on this processor
    int local_dof_count;
    // the first locally-owned dof (using the global numbering)
    int first_local_dof;
    // the total number of dofs in the analysis
    int global_dof_count;
    // the number of fixed locally-fixed dofs
    int fixed_dofs;
    // whether the dofs have been numbered or not
    bool numbered;
    // the current simulation time (for dynamics) or psuedo-time (for statics)
    // might want to pull this out entirely somehow... just pass in to pertinent member functions
    // especially as it's really only used for nonlinear FEA, for linear FEA it is just set to 1
    double T;
    // name of this analysis
    std::string name;
    // dimensionality of the analysis
    int analysis_dim;
    // mpi communicator containing only those processes undertaking this analysis
    MPI_Comm analysis_comm;
    std::vector<BCQuery*> dir_bcs;
    std::vector<BCQuery*> neu_bcs;
  public:
    FEA(MPI_Comm cm = AMSI_COMM_SCALE);
    virtual void Adapt() {};
    virtual void ApplyBC_Dirichlet() {};
    virtual void ApplyBC_Neumann(LAS *) {};
    virtual void Assemble(LAS *) {};
    void setSimulationTime(double t)
    {
      T = t;
      std::cout << "Simulation time updated: " << T << std::endl;
    }
    template <typename NODE_TYPE>
      void AssembleDOFs(LAS * las,
                        int num_elemental_dofs,
                        int * dof_numbers,
                        const NODE_TYPE * node_values,
                        double * Ke,
                        double * fe,
                        bool includes_body_forces) const;
    virtual void GetDOFInfo(int& global,int& local,int& offset)
    {
      global = global_dof_count;
      local = local_dof_count;
      offset = first_local_dof;
    };
    virtual void RenumberDOFs() {};
    virtual void UpdateDOFs(const double * sol) = 0;
  private:
  };
  void assembleMatrix(LAS * las,
                      int rw_cnt,
                      int * rw_nms,
                      int cl_cnt,
                      int * cl_nms,
                      double * Ke);
  void assembleVector(LAS * las,
                      int rw_cnt,
                      int * rw_nms,
                      double * fe);
  template <typename NODE_TYPE>
    void FEA::AssembleDOFs(LAS * las,
                           int num_elemental_dofs,
                           int * dof_numbers,
                           const NODE_TYPE * node_values,
                           double * Ke,
                           double * fe,
                           bool includes_body_forces) const
  {
    if(Ke != NULL)
    {
      if(!includes_body_forces)
      {
        double * bf = new double[num_elemental_dofs]();
        for(int ii = 0; ii < num_elemental_dofs; ii++)
        {
          const int & global_i = dof_numbers[ii];
          if(!isFixed(global_i))
          {
            for(int jj = 0; jj < num_elemental_dofs; jj++)
            {
              const double & val = Ke[ii * num_elemental_dofs + jj];
              double j_val = node_values[jj / analysis_dim][jj % analysis_dim];
              if(j_val != 0.0)
                bf[ii] += -val * j_val;
            }
          }
        }
        assembleVector(las,num_elemental_dofs,dof_numbers,&bf[0]);
        delete [] bf;
      }
      assembleMatrix(las,
                     num_elemental_dofs,dof_numbers,
                     num_elemental_dofs,dof_numbers,
                     &Ke[0]);
    }
    /// Modification of fe to correctly account for nonzero dirichlet boundary conditions
    double * dirichletValue = new double[num_elemental_dofs]();
    for (int ii = 0; ii < num_elemental_dofs; ii++)
    {
      if (dof_numbers[ii] < 0)
        dirichletValue[ii] = node_values[ii / analysis_dim][ii % analysis_dim];
    }
    double * dfe = new double[num_elemental_dofs]();
    for (int ii = 0; ii < num_elemental_dofs; ii++)
      for (int jj = 0 ; jj < num_elemental_dofs; jj++)
        dfe[ii] = dfe[ii] + Ke[ii * num_elemental_dofs + jj] * dirichletValue[ii];
    for (int ii = 0; ii < num_elemental_dofs; ii++)
      fe[ii] = fe[ii] + dfe[ii];
    delete [] dirichletValue;
    delete [] dfe;
    assembleVector(las,num_elemental_dofs,dof_numbers,&fe[0]);
  }
}
#endif