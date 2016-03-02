#include <ns3/log.h>
#include <ns3/pointer.h>
#include <ns3/math.h>
#include <cfloat>
#include <set>
#include <climits>
#include <vector>

#include <ns3/lte-amc.h>
#include <ns3/d2d-circle-ff-mac-scheduler.h>
#include <ns3/simulator.h>
#include <ns3/lte-common.h>
#include <ns3/lte-vendor-specific-parameters.h>
#include <ns3/boolean.h>
#include "ns3/lte-amc.h"
#include <ns3/lte-amc.h>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("D2dCircleFfMacScheduler");

static const int Type0AllocationRbg[4] = {
  10,       // RGB size 1
  26,       // RGB size 2
  63,       // RGB size 3
  110       // RGB size 4
};  // see table 7.1.6.1-1 of 36.213


NS_OBJECT_ENSURE_REGISTERED (D2dCircleFfMacScheduler);



class D2dCircleSchedulerMemberCschedSapProvider : public D2dFfMacCschedSapProvider
{
public:
  D2dCircleSchedulerMemberCschedSapProvider (D2dCircleFfMacScheduler* scheduler);

  // inherited from FfMacCschedSapProvider
  virtual void CschedCellConfigReq (const struct CschedCellConfigReqParameters& params);
  virtual void CschedUeConfigReq (const struct CschedUeConfigReqParameters& params);
  virtual void UpdateD2dInfo (const struct CschedUeConfigReqParameters& params);
  virtual d2dResUseInfo_s GetD2dResUseInfo ();


private:
  // D2dCircleSchedulerMemberCschedSapProvider ();
  D2dCircleFfMacScheduler* m_scheduler;
};

// D2dCircleSchedulerMemberCschedSapProvider::D2dCircleSchedulerMemberCschedSapProvider ()
// {
// }

D2dCircleSchedulerMemberCschedSapProvider::D2dCircleSchedulerMemberCschedSapProvider (D2dCircleFfMacScheduler* scheduler) : m_scheduler (scheduler)
{
}


void
D2dCircleSchedulerMemberCschedSapProvider::CschedCellConfigReq (const struct CschedCellConfigReqParameters& params)
{
  m_scheduler->DoCschedCellConfigReq (params);
}

void
D2dCircleSchedulerMemberCschedSapProvider::CschedUeConfigReq (const struct CschedUeConfigReqParameters& params)
{
  m_scheduler->DoCschedUeConfigReq (params);
}

void
D2dCircleSchedulerMemberCschedSapProvider::UpdateD2dInfo (const struct CschedUeConfigReqParameters& params)
{
  m_scheduler->DoUpdateD2dInfo (params);
}


d2dResUseInfo_s 
D2dCircleSchedulerMemberCschedSapProvider::GetD2dResUseInfo ()
{
  return m_scheduler->DoGetD2dResUseInfo ();
}



class D2dCircleSchedulerMemberSchedSapProvider : public D2dFfMacSchedSapProvider
{
public:
  D2dCircleSchedulerMemberSchedSapProvider (D2dCircleFfMacScheduler* scheduler);
  virtual void SchedReq(const struct SchedReqParameters& params);

private:
  D2dCircleSchedulerMemberSchedSapProvider ();
  D2dCircleFfMacScheduler* m_scheduler;
};



D2dCircleSchedulerMemberSchedSapProvider::D2dCircleSchedulerMemberSchedSapProvider ()
{
}


D2dCircleSchedulerMemberSchedSapProvider::D2dCircleSchedulerMemberSchedSapProvider (D2dCircleFfMacScheduler* scheduler)
  : m_scheduler (scheduler)
{
}

void
D2dCircleSchedulerMemberSchedSapProvider::SchedReq(const struct SchedReqParameters& params)
{
  m_scheduler->DoSchedReq (params);
}




D2dCircleFfMacScheduler::D2dCircleFfMacScheduler(): m_cschedSapUser (0), m_schedSapUser (0)
{
  m_cschedSapProvider = new D2dCircleSchedulerMemberCschedSapProvider (this);
  m_schedSapProvider = new D2dCircleSchedulerMemberSchedSapProvider (this);
  // m_amc = CreateObject <LteAmc> ();
}

D2dCircleFfMacScheduler::~D2dCircleFfMacScheduler ()
{
  NS_LOG_FUNCTION (this);
}

void
D2dCircleFfMacScheduler::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  delete m_cschedSapProvider;
  delete m_schedSapProvider;
}



TypeId
D2dCircleFfMacScheduler::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::D2dCircleFfMacScheduler")
    .SetParent<Object> ()
    .SetGroupName("D2d")
    .AddConstructor<D2dCircleFfMacScheduler> ()
    .AddAttribute ("CqiTimerThreshold",
                   "The number of TTIs a CQI is valid (default 1000 - 1 sec.)",
                   UintegerValue (1000),
                   MakeUintegerAccessor (&D2dCircleFfMacScheduler::m_cqiTimersThreshold),
                   MakeUintegerChecker<uint32_t> ())
  ;
  return tid;
}



void
D2dCircleFfMacScheduler::SetD2dFfMacCschedSapUser (D2dFfMacCschedSapUser* s)
{
  m_cschedSapUser = s;
}

void
D2dCircleFfMacScheduler::SetD2dFfMacSchedSapUser (D2dFfMacSchedSapUser* s)
{
  m_schedSapUser = s;
}

D2dFfMacCschedSapProvider*
D2dCircleFfMacScheduler::GetD2dFfMacCschedSapProvider ()
{
  return m_cschedSapProvider;
}

D2dFfMacSchedSapProvider*
D2dCircleFfMacScheduler::GetD2dFfMacSchedSapProvider ()
{
  return m_schedSapProvider;
}


void
D2dCircleFfMacScheduler::DoCschedCellConfigReq (const struct D2dFfMacCschedSapProvider::CschedCellConfigReqParameters& params)
{
  NS_LOG_FUNCTION (this);
  // Read the subset of parameters used
  m_cschedCellConfig = params;

  D2dFfMacCschedSapUser::CschedCellConfigCnfParameters cnf;
  cnf.m_result = SUCCESS;
  m_cschedSapUser->D2dCschedCellConfigCnf (cnf);
  return;
}

void
D2dCircleFfMacScheduler::DoCschedUeConfigReq (const struct D2dFfMacCschedSapProvider::CschedUeConfigReqParameters& params)
{
  NS_LOG_FUNCTION (this);

  d2dlink d(params.m_srcrnti, params.m_srcposition, params.m_dstrnti, params.m_dstposition);

  if (params.m_d2dmode)
  {
  	/* ue enter d2d mode */
  	d2dlink_state sta;
  	sta.m_cqiList = params.m_cqiList;
  	sta.m_linkId = m_d2dlink_id_counter++;
    m_d2dlinks.insert(std::make_pair<d2dlink, d2dlink_state> (d, sta));
  	// m_d2dlinks[d] = sta;

  }
  else
  {
  	/* ue leave d2d mode */
  	m_d2dlinks.erase(d);
  }

  D2dFfMacCschedSapUser::CschedUeConfigCnfParameters cnf;
  cnf.m_result = SUCCESS;
  cnf.m_rnti = params.m_srcrnti;
  m_cschedSapUser->D2dCschedUeConfigCnf (cnf);

}

void
D2dCircleFfMacScheduler::DoUpdateD2dInfo (const struct D2dFfMacCschedSapProvider::CschedUeConfigReqParameters& params)
{
  NS_LOG_FUNCTION (this);
  d2dlink d(params.m_srcrnti, params.m_srcposition, params.m_dstrnti, params.m_dstposition);
  std::map<struct d2dlink, struct d2dlink_state>::iterator iter = m_d2dlinks.find(d);
  NS_ASSERT(iter != m_d2dlinks.end());
  iter->second.m_cqiList = params.m_cqiList;

}


struct d2dResUseInfo_s
D2dCircleFfMacScheduler::DoGetD2dResUseInfo ()
{
  struct d2dResUseInfo_s ret;
  ret.m_dru = m_resUsePercent;
  ret.m_upr = m_d2dlinks.size() * 1.0 / m_cschedCellConfig.m_rbs;
  return ret;
}


void
D2dCircleFfMacScheduler::DoSchedReq(const struct D2dFfMacSchedSapProvider::SchedReqParameters& params)
{
 	NS_LOG_FUNCTION (this);

  D2dFfMacSchedSapUser::SchedConfigIndParameters ret;

 	// m_rra = rand() % 2 == 1;

 	// std::sort(m_d2dlinks.begin(), m_d2dlinks.end(), comparator(m_rra));
 	
 	// std::map<struct d2dlink, struct d2dlink_state>::iterator iter = m_d2dlinks.begin();

  
  // std::vector <bool> rbMap;
  // rbMap.resize(m_cschedCellConfig.m_rbs, false);
  // size_t  rbAllocIndex = 0;
  // m_resRunOut = false;

 	// for( ;iter != m_d2dlinks.end(); ++iter)
 	// {
  //   D2dDciListElement_s d2ddci;

  //   d2ddci.m_linkId = iter->second.m_linkId;
  //   d2ddci.m_tx = iter->first.m_src.m_rnti;
  //   d2ddci.m_rx = iter->first.m_dst.m_rnti;

  //   if (iter == m_d2dlinks.begin())
  //   {
  //     d2ddci.m_rbLen = m_cschedCellConfig.m_rbsperuser;
  //     d2ddci.m_rbStart = 0;
  //     for(int i = 0; i < m_cschedCellConfig.m_rbsperuser; ++i)
  //     {
  //       rbMap[i] = true;
  //     }
      
        
  //   }
  //   else 
  //   {
  //     std::map<struct d2dlink, struct d2dlink_state>::iterator iter_bef = iter - 1;
  //     int index_bef = rbAllocIndex - 1;

  //     while(1) 
  //     {
        
  //       if (iter_bef == m_d2dlinks.begin())
  //       {
  //         /* 无法复用现有资源，分配新资源 */
  //         d2ddci.m_rbLen = m_cschedCellConfig.m_rbsperuser;
  //         d2ddci.m_rbStart = 0;
  //         int i = 0;
  //         for(; i < m_cschedCellConfig.m_rbs; ++i)
  //         {
  //           if( rbMap[i] == false )
  //             break;
  //         }

  //         if(i + m_cschedCellConfig.m_rbsperuser > m_cschedCellConfig.m_rbs )
  //         {
  //           /* 资源不足，无法继续分配 */
  //           m_resRunOut = true;
  //           goto alloc_end;
  //         }
  //         else
  //         {
  //           d2ddci.m_rbLen = m_cschedCellConfig.m_rbsperuser;
  //           d2ddci.m_rbStart = i;
  //           for(int j = 0; j < m_cschedCellConfig.m_rbsperuser; ++j)
  //           {
  //             rbMap[i++] = true;
  //           }
  //         }

  //         break;
  //       }
  //       else
  //       {
  //         /* 寻找可复用的现有资源 */
  //         if (iter_bef->first.m_src == iter->first.m_src || FreeInterference(iter_bef->first, iter->first) )
  //         {
  //           /* 可以使用相同的资源 */
  //           d2ddci.m_rbStart = ret.m_dciList[index_bef].m_rbStart;
  //           d2ddci.m_rbLen = ret.m_dciList[index_bef].m_rbLen;
  //           break;
  //         }

  //         --iter_bef;
  //         --index_bef;
  //       }

  //     }
      
  //   }

  //   rbAllocIndex++;


  //   uint8_t cqi = iter->second.m_cqiList[0];

  //   NS_ASSERT( cqi != 0);
 
  //   d2ddci.m_mcs = m_amc->GetMcsFromCqi (cqi);

  //   ret.m_dciList.push_back(d2ddci);
    
 	// }

// alloc_end:
//   int sum = 0;
//   for (int i = 0; i < rbMap.size(); ++i)
//   {
//     if (rbMap[i])
//     {
//       sum++;
//     }
//   }

//   m_resUsePercent = sum * 1.0 / rbMap.size();
  m_schedSapUser->SchedConfigInd (ret);

}


bool
D2dCircleFfMacScheduler::FreeInterference(const d2dlink& d1, const d2dlink& d2)
{
  double distance = sqrt( pow(d1.m_src.m_position.x - d2.m_src.m_position.x, 2) + pow(d1.m_src.m_position.y - d2.m_src.m_position.y, 2) );

  if(distance - m_cschedCellConfig.m_interDistance > 1e-2)
    return true;
  else
    return false;
}











}