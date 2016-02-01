#ifndef AMSI_NONLINEAR_ANALYSIS_H_
#define AMSI_NONLINEAR_ANALYSIS_H_
namespace amsi
{
  class Iteration
  {
  public:
    virtual void iterate() = 0;
  };
  class Convergence
  {
  public:
    virtual bool converged() = 0;
    virtual bool failed() {return false;}
  };
  bool numericalSolve(Iteration * it, Convergence * cn);
}
#endif