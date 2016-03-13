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
  D2dCircleSchedulerMemberCschedSapProvider ();
  D2dCircleFfMacScheduler* m_scheduler;
};

D2dCircleSchedulerMemberCschedSapProvider::D2dCircleSchedulerMemberCschedSapProvider ()
{
}

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
  m_amc = CreateObject <LteAmc> ();
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
  NS_LOG_FUNCTION(this);
  m_cschedSapUser = s;
}

void
D2dCircleFfMacScheduler::SetD2dFfMacSchedSapUser (D2dFfMacSchedSapUser* s)
{
  NS_LOG_FUNCTION(this);
  m_schedSapUser = s;
}

D2dFfMacCschedSapProvider*
D2dCircleFfMacScheduler::GetD2dFfMacCschedSapProvider ()
{
  NS_LOG_FUNCTION(this);
  return m_cschedSapProvider;
}

D2dFfMacSchedSapProvider*
D2dCircleFfMacScheduler::GetD2dFfMacSchedSapProvider ()
{
  NS_LOG_FUNCTION(this);
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

void D2dCircleFfMacScheduler::UpdateResConflictMap(const d2dlink &link, const d2dlink_state& sta)
{
	std::vector<std::pair<d2dlink, d2dlink_state> >::iterator iter = m_d2dlinks.begin();
  for(;iter != m_d2dlinks.end(); ++iter)
  {
  	if (!FreeInterference(link, iter->first))
  	{
  		m_resConflictMap[std::pair<uint16_t, uint16_t>(sta.m_linkId, iter->second.m_linkId)] = true;
  		m_resConflictMap[std::pair<uint16_t, uint16_t>(iter->second.m_linkId, sta.m_linkId)] = true;
  	}
  	else
  	{
  		m_resConflictMap[std::pair<uint16_t, uint16_t>(sta.m_linkId, iter->second.m_linkId)] = false;
  		m_resConflictMap[std::pair<uint16_t, uint16_t>(iter->second.m_linkId, sta.m_linkId)] = false;
  	}
  }
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

  	UpdateResConflictMap(d, sta);

    m_d2dlinks.push_back(std::make_pair<d2dlink, d2dlink_state> (d, sta));

  }
  else
  {
  	/* ue leave d2d mode */
    std::vector<std::pair<d2dlink, d2dlink_state> >::iterator iter = m_d2dlinks.begin();
    for(;iter != m_d2dlinks.end(); ++iter)
    {
      if (iter->first == d)
      {
        m_d2dlinks.erase(iter);
        break;
      }
    }
  	
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

  std::vector<std::pair<d2dlink, d2dlink_state> >::iterator iter = m_d2dlinks.begin();
  for(;iter != m_d2dlinks.end(); ++iter)
    {
      if (iter->first == d)
      {
        break;
      }
    }
  // std::map<struct d2dlink, struct d2dlink_state>::iterator iter = m_d2dlinks.find(d);
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

 	m_rra = rand() % 2 == 1;

	std::sort(m_d2dlinks.begin(), m_d2dlinks.end(), comparator(m_rra));
	
	std::vector<std::pair<d2dlink, d2dlink_state> >::iterator iter = m_d2dlinks.begin();

  
	std::vector <bool> rbMap;
	rbMap.resize(m_cschedCellConfig.m_rbs, false);
	size_t  rbAllocIndex = 0;
	m_resRunOut = false;

	m_resAllocMap.clear();
	m_dciAllocMap.clear();

	for( ;iter != m_d2dlinks.end(); ++iter)
	{
		D2dDciListElement_s d2ddci;

		d2ddci.m_linkId = iter->second.m_linkId;
		d2ddci.m_tx = iter->first.m_src.m_rnti;
		d2ddci.m_rx = iter->first.m_dst.m_rnti;

		if (iter == m_d2dlinks.begin())
		{
		  d2ddci.m_rbLen = m_cschedCellConfig.m_rbsperuser;
		  d2ddci.m_rbStart = 0;
		  for(uint8_t i = 0; i < m_cschedCellConfig.m_rbsperuser; ++i)
		  {
		    rbMap[i] = true;
		  }
		  // NS_LOG_DEBUG("alloc new RB "<<0<< " and " << 1);
		  NS_LOG_DEBUG("link " << iter->first.m_src.m_rnti << " to "<< iter->first.m_dst.m_rnti<<" alloc new RB "<<0<< " and " << 1);


                
		}
		else 
		{
			std::vector<std::pair<d2dlink, d2dlink_state> >::iterator iter_bef = iter - 1;
			int index_bef = rbAllocIndex - 1;

			while(1) 
			{
        
				if (iter_bef == m_d2dlinks.begin())
				{
					//
					if (iter_bef->first.m_src == iter->first.m_src || CanUseSameRes(iter->second, iter_bef->second) )
					{
						/* 可以使用相同的资源 */
						NS_LOG_DEBUG("link " << iter->first.m_src.m_rnti << " to "<< iter->first.m_dst.m_rnti<< " reuse "
							"link " << iter_bef->first.m_src.m_rnti << " to "<< iter_bef->first.m_dst.m_rnti);
						d2ddci.m_rbStart = ret.m_dciList[index_bef].m_rbStart;
						d2ddci.m_rbLen = ret.m_dciList[index_bef].m_rbLen;
						break;
					}


					/* 无法复用现有资源，分配新资源 */
					d2ddci.m_rbLen = m_cschedCellConfig.m_rbsperuser;
					d2ddci.m_rbStart = 0;
					int i = 0;
					for(; i < m_cschedCellConfig.m_rbs; ++i)
					{
						if( rbMap[i] == false )
							break;
					}

					if(i + m_cschedCellConfig.m_rbsperuser > m_cschedCellConfig.m_rbs )
					{
						/* 资源不足，无法继续分配 */
						m_resRunOut = true;
						NS_LOG_DEBUG("d2d resource out of use");
						goto alloc_end;
					}
					else
					{
						d2ddci.m_rbLen = m_cschedCellConfig.m_rbsperuser;
						d2ddci.m_rbStart = i;
						for(int j = 0; j < m_cschedCellConfig.m_rbsperuser; ++j)
						{
							rbMap[i++] = true;
						}
						NS_LOG_DEBUG("alloc new RB "<<(uint32_t)d2ddci.m_rbStart << " and " << (uint32_t)d2ddci.m_rbStart+1);
						break;
					}
    		
				}
				else
				{
					/* 寻找可复用的现有资源 */
					if ( iter_bef->first.m_src == iter->first.m_src || CanUseSameRes(iter->second, iter_bef->second) )
					{
						/* 可以使用相同的资源 */
						NS_LOG_DEBUG("link " << iter->first.m_src.m_rnti << " to "<< iter->first.m_dst.m_rnti<< " reuse "
							"link " << iter_bef->first.m_src.m_rnti << " to "<< iter_bef->first.m_dst.m_rnti);
						d2ddci.m_rbStart = ret.m_dciList[index_bef].m_rbStart;
						d2ddci.m_rbLen = ret.m_dciList[index_bef].m_rbLen;
						break;
					}

					--iter_bef;
					--index_bef;
				}

			}
	      
		}

		rbAllocIndex++;

		m_resAllocMap[d2ddci.m_rbStart].push_back(iter->second.m_linkId);
		m_dciAllocMap[iter->second.m_linkId] = d2ddci.m_rbStart;

		uint8_t cqi = iter->second.m_cqiList[0];

		NS_ASSERT( cqi != 0);

		d2ddci.m_cqi = cqi;

		d2ddci.m_mcs = m_amc->GetMcsFromCqi (cqi);

		ret.m_dciList.push_back(d2ddci);
    
	}

alloc_end:

	int sum = 0;
	for (uint8_t i = 0; i < rbMap.size(); ++i)
	{
		if (rbMap[i])
		{
			sum++;
		}
	}

	m_resUsePercent = sum * 1.0 / rbMap.size();

	NS_ASSERT(m_schedSapUser != 0);

	NS_LOG_DEBUG("D2dCircleFfMacScheduler::DoSchedReq :: " << m_d2dlinks.size() << " d2dlinks ");
	NS_LOG_DEBUG("d2d resource usage == " << m_resUsePercent);
	m_schedSapUser->SchedConfigInd (ret);

}


bool
D2dCircleFfMacScheduler::FreeInterference(const d2dlink& d1, const d2dlink& d2)
{
  
  double d1_x = ( d1.m_src.m_position.x + d1.m_dst.m_position.x) / 2.0;
  double d1_y = ( d1.m_src.m_position.y + d1.m_dst.m_position.y) / 2.0;

  double d2_x = ( d2.m_src.m_position.x + d2.m_dst.m_position.x) / 2.0;
  double d2_y = ( d2.m_src.m_position.y + d2.m_dst.m_position.y) / 2.0;

  double distance = sqrt( pow(d1_x - d2_x, 2) + pow(d1_y - d2_y, 2) );


  if(distance - m_cschedCellConfig.m_interDistance > 1e-2)
    return true;
  else
    return false;
}

bool
D2dCircleFfMacScheduler::CanUseSameRes(const d2dlink_state& d1, const d2dlink_state& d2)
{
	/* can d1 reuse d2's resource ? */

	if (m_resConflictMap[std::pair<uint16_t, uint16_t>(d1.m_linkId, d2.m_linkId)])
	{
		return false;
	}

	uint8_t rbStart = m_dciAllocMap[d2.m_linkId];

	for( uint16_t i = 0; i < m_resAllocMap[rbStart].size(); ++i)
	{
		if(m_resConflictMap[std::pair<uint16_t, uint16_t>(m_resAllocMap[rbStart][i], d1.m_linkId)])
			return false;

	}

	return true;

}









}