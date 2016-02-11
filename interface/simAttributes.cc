#include "simAttributes.h"
#include <cassert>
namespace amsi
{
  char const * sim_attr_css[] =
  {
    "problem definition",
    "output"
  };
  void initCase(pModel mdl, pACase cs)
  {
    pPList chdrn = AttNode_children(cs);
    pACase chd = NULL;
    void * it = NULL;
    while((chd = (pACase)PList_next(chdrn,&it)))
      AttCase_setModel(chd,mdl);
    AttCase_setModel(cs,mdl);
    AttCase_associate(cs,NULL);
  }
  void freeCase(pACase cs)
  {
    AttCase_unassociate(cs);
  }
  const char * attRepTypeString(AttRepType tp)
  {
    switch(tp)
    {
    case Att_case:
      return "Att_case";
    case Att_group:
      return "Att_group";
    case Att_model:
      return "Att_model";
    case Att_int:
      return "Att_int";
    case Att_double:
      return "Att_double";
    case Att_string:
      return "Att_string";
    case Att_void:
      return "Att_void";
    case Att_list:
      return "Att_list";
    case Att_refnode:
      return "Att_refnode";
    case Att_gentity:
      return "Att_gentity";
    case Att_tensor0:
      return "Att_tensor0";
    case Att_tensor1:
      return "Att_tensor1";
    case Att_tensor2:
      return "Att_tensor2";
    case Att_tensor3:
      return "Att_tensor3";
    case Att_tensor4:
      return "Att_tensor4";
    case Att_resource:
      return "Att_resource";
    case Att_process:
      return "Att_process";
    default:
      return "Error: unknow AttRepType!";
    };
  }
  void describeNode(pANode nd)
  {
    std::cout << "Attribute node name: " << AttNode_name(nd)
              << " infotype: " << AttNode_infoType(nd)
              << " imageclass: " << AttNode_imageClass(nd)
              << " reptype: " << amsi::attRepTypeString(AttNode_repType(nd)) << std::endl;
  }
  void initAttributeCase(pGModel mdl,const char * att_cs)
  {
    std::cout << "Initializing attribute case " << att_cs << std::endl;
    pACase cs = AMAN_findCase(SModel_attManager(mdl),att_cs);
    if(cs)
    {
      pPList chdrn = AttNode_children(cs);
      pACase child;
      void * iter = NULL;
      while((child = (pACase)PList_next(chdrn,&iter)))
      {
        AttCase_setModel(child,mdl);
      }
      AttCase_setModel(cs,mdl);
      AttCase_associate(cs,NULL);
      PList_delete(chdrn);
    }
  }
  void clearModelAttributes(pGModel mdl)
  {
    // get the associated attribute case, call AttCase_unassociate()
  }
  bool isIncrementallyLoaded(pGEntity ent, const char * attr)
  {
    bool result = false;
    pAttribute force_constraint = GEN_attrib(ent,"force constraint");
    if(force_constraint)
      result = !AttributeTensor1_constant(static_cast<pAttributeTensor1>(Attribute_childByType(force_constraint,"direction")));
    pAttribute disp_constraint = GEN_attrib(ent,"displacement constraint");
    if(disp_constraint)
    {
      pAttribute constraint_set = Attribute_childByType(disp_constraint,"Set");
      pPList children = Attribute_children(constraint_set);
      void * iter = NULL;
      pAttribute att;
      while((att = static_cast<pAttribute>(PList_next(children,&iter))) && !result)
      {
        pAttributeDouble disp_attribute =
          static_cast<pAttributeDouble>(Attribute_childByType(att,"Total Displacement"));
        result = !AttributeDouble_constant(disp_attribute);
      }
      PList_delete(children);
    }
    return result;
  }
  bool requiresIncrementalLoading(pGModel mdl, const char * attr)
  {
    bool result = false;
    pGEntity entity;
    for(GFIter gfiter = GM_faceIter(mdl); (entity = GFIter_next(gfiter)) && !result;)
      result = isIncrementallyLoaded(entity,attr);
    for(GEIter geiter = GM_edgeIter(mdl); (entity = GEIter_next(geiter)) && !result;)
      result = isIncrementallyLoaded(entity,attr);
    for(GVIter gviter = GM_vertexIter(mdl); (entity = GVIter_next(gviter)) && !result;)
      result = isIncrementallyLoaded(entity,attr);
    return result;
  }
  bool hasAttribute(pGEntity entity, const char * attr)
  {
    return GEN_attrib(entity,attr) != NULL;
  }
  void getWithAttribute(pGModel mdl, const char * attr, std::list<pGEntity> & ents)
  {
    pGEntity entity;
    for(GRIter griter = GM_regionIter(mdl); (entity = GRIter_next(griter)); )
      if(hasAttribute(entity,attr))
        ents.push_back(entity);
    for(GFIter gfiter = GM_faceIter(mdl); (entity = GFIter_next(gfiter)); )
      if(hasAttribute(entity,attr))
        ents.push_back(entity);
    for(GEIter geiter = GM_edgeIter(mdl); (entity = GEIter_next(geiter)); )
      if(hasAttribute(entity,attr))
        ents.push_back(entity);
    for(GVIter gviter = GM_vertexIter(mdl); (entity = GVIter_next(gviter)); )
      if(hasAttribute(entity,attr))
        ents.push_back(entity);
  }
  void applyRegionAttributes(pGModel mdl, Analysis::FEA * fea)
  {
    pGEntity rgn = NULL;
    for(GRIter griter = GM_regionIter(mdl); (rgn = GRIter_next(griter)); )
    {
      int tg = GEN_tag(rgn);
      std::cout << "Processing region " << tg << ":" << std::endl;
      pPList atts = GEN_attributes(rgn,"");
      pAttribute att = NULL;
      void * iter = NULL;
      while( (att = (pAttribute)PList_next(atts,&iter)) )
      {
        char * nm = Attribute_name(att);
        char * inf_tp = Attribute_infoType(att);
        std::cout << "\tAttribute " << nm << " of type " << inf_tp << std::endl;
      }
      PList_delete(atts);
    }
  }
  void applyAttributes(Analysis::SimFEA* fea)
  {
    pGModel mdl = fea->getGeometricDomain();
    applyRegionAttributes(mdl,fea);
  }
  void writeAttMan(pAManager attm, const char * fnm)
  {
    assert(fnm);
    AMAN_write(attm,fnm,13);
  }
}