#ifndef AMSIMETA_H_
#define AMSIMETA_H_

#include "TaskManager.h"
#include "CommunicationManager.h"

#include <amsiUtil.h>

#include <string>

namespace amsi {

  extern TaskManager * tm;
  extern CommunicationManager * cm;

  class amsiMetaInit : virtual public amsiUtilInit
  {
  public:
    virtual void amsiInit(int,char**);
    virtual void amsiFree();
  };
  
  void configureFromFile(const std::string &);
}


#endif
