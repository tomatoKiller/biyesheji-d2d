#ifndef D2D_FF_MAC_CSCHED_SAP_H
#define D2D_FF_MAC_CSCHED_SAP_H


#include <stdint.h>
#include <vector>
#include "ff-mac-common.h"
#include "ns3/core-module.h"
#include <ns3/lte-d2d-common.h>
#include "ns3/lte-d2d-common.h"

namespace ns3 {



class D2dFfMacCschedSapProvider
{
public:
	virtual ~D2dFfMacCschedSapProvider();

	struct CschedCellConfigReqParameters
	{
		
		uint8_t	 m_rbs;  //分配给D2D通信的RB数量
    uint8_t  m_rbsperuser;  //每个UE需要多少个RB进行D2D通信
    uint16_t m_interDistance;  //D2D 通信对之间的干扰距离
	};

	struct CschedUeConfigReqParameters
	{
		bool m_d2dmode;
    uint16_t  m_srcrnti;
    Vector m_srcposition;  
    uint16_t  m_dstrnti;  // d2d communication target
    Vector m_dstposition;
    std::vector <uint8_t> m_cqiList;  // cqi  on all the d2d rb
	};


	virtual void CschedCellConfigReq (const struct CschedCellConfigReqParameters& params) = 0;

	virtual void CschedUeConfigReq (const struct CschedUeConfigReqParameters& params) = 0;
  virtual void UpdateD2dInfo (const struct CschedUeConfigReqParameters& params) = 0;

  virtual struct d2dResUseInfo_s GetD2dResUseInfo () = 0;
};


class D2dFfMacCschedSapUser
{
public:
  virtual ~D2dFfMacCschedSapUser ();

  struct CschedCellConfigCnfParameters
  {
    enum Result_e m_result;
  };


  struct CschedUeConfigCnfParameters
  {
    uint16_t  m_rnti;
    enum Result_e m_result;

  };


  virtual void D2dCschedCellConfigCnf (const struct CschedCellConfigCnfParameters& params) = 0;

  virtual void D2dCschedUeConfigCnf (const struct CschedUeConfigCnfParameters& params) = 0;

};














}


#endif