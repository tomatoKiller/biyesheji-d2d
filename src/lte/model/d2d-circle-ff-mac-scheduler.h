

#ifndef D2D_CIRCLE_FF_MAC_SCHEDULER_H
#define D2D_CIRCLE_FF_MAC_SCHEDULER_H

#include <ns3/d2d-ff-mac-csched-sap.h>
#include <ns3/d2d-ff-mac-sched-sap.h>
#include <ns3/lte-d2d-common.h>
#include "ns3/lte-amc.h"
#include <algorithm>
#include <vector>
#include <list>
#include <map>
#include <cmath>


namespace ns3 {



// class comparator
// {
// public:
//   comparator(bool rra): m_rra(rra){}
//   bool operator ()(const d2dlink& a, const d2dlink& b)
//   {
//     double angle_a = a.m_src.m_position.y * 1.0 / a.m_src.m_position.x;
//       double angle_b = b.m_src.m_position.y * 1.0 / b.m_src.m_position.x;

//     if(a.m_src.m_rnti == b.m_src.m_rnti) return false;

//     if( angle_a < angle_b)
//       return true;
//     else if( std::fabs(angle_b - angle_a) < 1e-4 )
//     {
//       //相同角度
//       double radiu_a = std::sqrt( std::pow(a.m_src.m_position.x, 2) + std::pow(a.m_src.m_position.y, 2) );
//       double radiu_b = std::sqrt( std::pow(b.m_src.m_position.x, 2) + std::pow(b.m_src.m_position.y, 2) );

//       //如果rra 为true， 则从径向朝外分配资源
//       if(m_rra)
//         return radiu_a < radiu_b;
//       else
//         return radiu_b < radiu_a;
//     }
//     else
//       return false;
//   }

// private:
//   bool m_rra;
// };



class D2dCircleFfMacScheduler: public Object
{
public:

  D2dCircleFfMacScheduler ();

  /**
   * Destructor
   */
  ~D2dCircleFfMacScheduler ();

  // inherited from Object
  virtual void DoDispose (void);
  static TypeId GetTypeId (void);

  // inherited from FfMacScheduler
  void SetD2dFfMacCschedSapUser (D2dFfMacCschedSapUser* s);
  void SetD2dFfMacSchedSapUser (D2dFfMacSchedSapUser* s);
  D2dFfMacCschedSapProvider* GetD2dFfMacCschedSapProvider ();
  D2dFfMacSchedSapProvider* GetD2dFfMacSchedSapProvider ();


  friend class D2dCircleSchedulerMemberCschedSapProvider;
  friend class D2dCircleSchedulerMemberSchedSapProvider;


private:

  void DoCschedCellConfigReq (const struct D2dFfMacCschedSapProvider::CschedCellConfigReqParameters& params);

  void DoCschedUeConfigReq (const struct D2dFfMacCschedSapProvider::CschedUeConfigReqParameters& params);

  struct d2dResUseInfo_s DoGetD2dResUseInfo ();

  void DoUpdateD2dInfo (const struct D2dFfMacCschedSapProvider::CschedUeConfigReqParameters& params);

  
  bool FreeInterference(const d2dlink& d1, const d2dlink& d2);

  void DoSchedReq(const struct D2dFfMacSchedSapProvider::SchedReqParameters& params);


  uint32_t m_cqiTimersThreshold; // # of TTIs for which a CQI canbe considered valid


  D2dFfMacCschedSapProvider::CschedCellConfigReqParameters m_cschedCellConfig;


  std::map <uint16_t,uint8_t> m_uesTxMode; // txMode of the UEs

  std::map<struct d2dlink, struct d2dlink_state> m_d2dlinks;  //all the d2d links in the cell

  bool m_rra;   //  radius random resource allocation

  bool m_resRunOut;

  uint16_t  m_d2dlink_id_counter;

  uint8_t m_ulGrantMcs; // MCS for UL grant (default 0)

  double m_resUsePercent;

  D2dFfMacCschedSapUser *m_cschedSapUser;
  D2dFfMacSchedSapUser *m_schedSapUser;

  D2dFfMacCschedSapProvider* m_cschedSapProvider;
  D2dFfMacSchedSapProvider* m_schedSapProvider;

};

} // namespace ns3

#endif 
