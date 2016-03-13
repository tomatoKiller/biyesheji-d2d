#ifndef LTE_D2D_COMMON_H
#define LTE_D2D_COMMON_H

#include <ns3/vector.h>
#include <ns3/d2d-ff-mac-csched-sap.h>
#include <ns3/d2d-ff-mac-sched-sap.h>
#include "ns3/d2d-ff-mac-csched-sap.h"

namespace ns3 {


struct d2dLinkSrsConfig
{
	uint16_t m_srcRnti;
	uint16_t m_dstRnti;
	uint32_t m_srsConfig;
};


struct d2due
{
  uint16_t m_rnti;
  Vector m_position;

  bool operator==(const d2due& rhs)
  {
    return m_rnti == rhs.m_rnti;
  }

  d2due(uint16_t rnti_a, Vector pos_a):m_rnti(rnti_a), m_position(pos_a){}
};


// class D2dFfMacCschedSapProvider;

struct d2dlink
{
  d2dlink(uint16_t rnti_a, Vector pos_a, uint16_t rnti_b, Vector pos_b):m_src(rnti_a, pos_a), m_dst(rnti_b, pos_b){}

  bool operator < (const d2dlink& b) const
  {
    double angle_a = m_src.m_position.y * 1.0 / m_src.m_position.x;
    double angle_b = b.m_src.m_position.y * 1.0 / b.m_src.m_position.x;

    if(m_src.m_rnti == b.m_src.m_rnti) return false;

    if( angle_a < angle_b)
      return true;
    else if( std::fabs(angle_b - angle_a) < 1e-4 )
    {
      //相同角度
      double radiu_a = std::sqrt( std::pow(m_src.m_position.x, 2) + std::pow(m_src.m_position.y, 2) );
      double radiu_b = std::sqrt( std::pow(b.m_src.m_position.x, 2) + std::pow(b.m_src.m_position.y, 2) );

      //如果rra 为true， 则从径向朝外分配资源

      return radiu_a < radiu_b;

    }
    else
      return false;
  }

  // class D2dFfMacCschedSapProvider;
  // typename D2dFfMacCschedSapProvider::SchedReqParameters;

  // d2dlink(const struct D2dFfMacCschedSapProvider::CschedUeConfigReqParameters& params):m_src(params.m_srcrnti, params.m_srcposition),m_dst(params.m_dstrnti, params.m_dstposition)
  // {
  //   // m_src.m_rnti = params.m_srcrnti;
  //   // m_src.m_position = params.m_srcposition;
  //   // m_dst.m_rnti = params.m_dstrnti;
  //   // m_dst.m_position = params.m_dstposition;
  // }

  bool operator==(const d2dlink& rhs)
  {
    return m_src == rhs.m_src && m_dst == rhs.m_dst ;
  }

  d2due  m_src;
  d2due  m_dst;

};

struct d2dResUseInfo_s
{
  double m_dru;   //d2d 资源使用率
  double m_upr;   //d2d 每个RB承载的链路数量
};


 struct d2dlink_state
 {
    uint16_t  m_linkId;
    std::vector< uint8_t >   m_cqiList;
    uint8_t   m_part;
 };





}


#endif