#include "test.h"
#include "amsiInterface.h"
#include "simAnalysis.h"
#include "simAttributes.h"
#include "simBoundaryConditions.h"
#include "apf.h"
#include "apfSIM.h"
#include "apfBoundaryConditions.h"
#include <mpi.h>
#include <cassert>
#include <fstream>
int main(int argc, char ** argv)
{
  int failed = 0;
  assert(argc == 3);
  amsi::use_simmetrix = true;
  amsi::interfaceInit(argc,argv);
  pGModel mdl = GM_load(argv[1],0,NULL);
  pParMesh sm_msh = PM_load(argv[2],sthreadNone,mdl,NULL);
  pMesh prt = PM_mesh(sm_msh,0);
  // get all analysis cases
  std::vector<pACase> css;
  amsi::getTypeCases(SModel_attManager(mdl),"analysis",std::back_inserter(css));
  // only run the first attribute case
  amsi::initCase(mdl,css[0]);
  pACase pd = (pACase)AttNode_childByType((pANode)css[0],"problem definition");
  apf::Mesh * msh =  apf::createMesh(sm_msh);
  apf::Field * u = apf::createLagrangeField(msh,"displacement",apf::VECTOR,1);
  apf::Numbering * nm = apf::createNumbering(u);
  int tps[] = {amsi::SURFACE_TRACTION, amsi::PRESSURE};
  std::vector<amsi::SimBCQuery*> neu_qrys;
  amsi::buildSimBCQueries(pd,amsi::NUEMANN,&tps,&tps+2,std::back_inserter(neu_qrys));
  int fxd = amsi::applySimNeumannBCs(nm,prt,dir_qrys.begin(),dir_qrys.end(),0.0);
  failed += test_neq("Fixed dofs",0,fxd);
  amsi::freeCase(css[0]);
  amsi::interfaceFree();
  return failed;
}
