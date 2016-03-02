#ifndef D2D_FF_MAC_SCHED_SAP_H
#define D2D_FF_MAC_SCHED_SAP_H

#include <stdint.h>
#include <vector>

#include "ff-mac-common.h"
#include <ns3/vector.h>


namespace ns3 {


class D2dFfMacSchedSapProvider
{
public:
  virtual ~D2dFfMacSchedSapProvider ();


    struct SchedReqParameters
    {
      uint16_t m_srcrnti;
      uint16_t m_dstrnti;
      Vector    m_srcposition;
      Vector    m_dstposition;

    };

    virtual void SchedReq(const struct SchedReqParameters& params) = 0;


};


class D2dFfMacSchedSapUser
{
public:
  virtual ~D2dFfMacSchedSapUser ();

  struct SchedConfigIndParameters
  {
    std::vector <struct D2dDciListElement_s> m_dciList;
  };

  virtual void SchedConfigInd (const struct SchedConfigIndParameters& params) = 0;

};




}

#endif