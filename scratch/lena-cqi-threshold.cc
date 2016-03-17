/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Manuel Requena <manuel.requena@cttc.es>
 */


#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/lte-module.h"
#include "ns3/config-store.h"
#include <ns3/buildings-helper.h>
#include <sstream>
 #include <fstream>
//#include "ns3/gtk-config-store.h"

using namespace ns3;

std::ofstream positionFile("UePosition");


static void 
CourseChange (std::string foo, Ptr<const MobilityModel> mobility)
{
  Vector pos = mobility->GetPosition ();
  Vector vel = mobility->GetVelocity ();
  positionFile << Simulator::Now () <<" " <<foo<< ", model=" << mobility << ", POS: x=" << pos.x << ", y=" << pos.y
            << ", z=" << pos.z << "; VEL:" << vel.x << ", y=" << vel.y
            << ", z=" << vel.z << std::endl;
}

int main (int argc, char *argv[])
{	
    CommandLine cmd;
    cmd.Parse (argc, argv);

    int runnum = 0;
    std::string ulOutputName = "macOutput/Ul/MacStats_";
    std::string dlOutputName = "macOutput/Dl/MacStats_";

    // LogComponentEnable("LteEnbRrc",LOG_LEVEL_DEBUG);
    // LogComponentEnable("LteEnbMac",LOG_LEVEL_DEBUG);
    // LogComponentEnable("LteEnbPhy",LOG_LEVEL_ALL);
    // LogComponentEnable("LteUePhy",LOG_LEVEL_DEBUG);
    // LogComponentEnable("LteSpectrumValueHelper",LOG_LEVEL_FUNCTION);
    // LogComponentEnable("LteUeMac",LOG_LEVEL_DEBUG);
    // LogComponentEnable("D2dCircleFfMacScheduler",LOG_LEVEL_ALL);
    // LogComponentEnable("LteSpectrumPhy",LOG_LEVEL_FUNCTION);
    // LogComponentEnable("MultiModelSpectrumChannel",LOG_LEVEL_DEBUG);
    // LogComponentEnable("MultiModelSpectrumChannel",LOG_LEVEL_LOGIC);
    // LogComponentEnable("D2dCircleSchedulerMemberSchedSapProvider",LOG_LEVEL_ALL);


	
  // to save a template default attribute file run it like this:
  // ./waf --command-template="%s --ns3::ConfigStore::Filename=input-defaults.txt --ns3::ConfigStore::Mode=Save --ns3::ConfigStore::FileFormat=RawText" --run src/lte/examples/lena-first-sim
  //
  // to load a previously created default attribute file
  // ./waf --command-template="%s --ns3::ConfigStore::Filename=input-defaults.txt --ns3::ConfigStore::Mode=Load --ns3::ConfigStore::FileFormat=RawText" --run src/lte/examples/lena-first-sim

    runnum = 0;
    while(runnum++ < 10)
  {
    if (runnum == 2)
    {
      break;
    }
    Config::SetDefault ("ns3::ConfigStore::Filename", StringValue ("input-defaults.txt"));
    Config::SetDefault ("ns3::ConfigStore::Mode", StringValue ("Load"));
    Config::SetDefault ("ns3::ConfigStore::FileFormat", StringValue ("RawText"));

    int ueNum = 100 + (runnum - 1) * 20;

    std::stringstream ss;
    ss<<ueNum;
    std::string str;
    ss>>str;
    Config::SetDefault("ns3::MacStatsCalculator::UlOutputFilename", StringValue (ulOutputName + str));
    Config::SetDefault("ns3::MacStatsCalculator::DlOutputFilename", StringValue (dlOutputName + str));


    ConfigStore inputConfig;
    inputConfig.ConfigureDefaults ();
    // inputConfig.ConfigureAttributes();

    // parse again so you can override default values from the command line
    cmd.Parse (argc, argv);

    Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();
    // lteHelper->SetPathlossModelType ("ns3::FriisPropagationLossModel");

    lteHelper->SetFadingModel("ns3::TraceFadingLossModel");
    lteHelper->SetFadingModelAttribute(
      "TraceFilename", StringValue ("src/lte/model/fading-traces/fading_trace_EPA_3kmph.fad"));
    lteHelper->SetFadingModelAttribute ("TraceLength", TimeValue (Seconds (10.0)));
    lteHelper->SetFadingModelAttribute ("SamplesNum", UintegerValue (10000));
    lteHelper->SetFadingModelAttribute ("WindowSize", TimeValue (Seconds (0.5)));
    lteHelper->SetFadingModelAttribute ("RbNum", UintegerValue (50));



    NodeContainer enbNodes;
    NodeContainer ueNodes;
    enbNodes.Create (1);
    ueNodes.Create (ueNum);

    // Install Mobility Model
    MobilityHelper mobility;
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
    positionAlloc->Add (Vector (0.0, 0.0, 35));
    mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    mobility.SetPositionAllocator(positionAlloc);
    mobility.Install (enbNodes);
    BuildingsHelper::Install (enbNodes);


    mobility.SetPositionAllocator ("ns3::RandomDiscPositionAllocator",
                                 "X", StringValue ("0.0"),
                                 "Y", StringValue ("0.0"),
                                 "Theta", StringValue("ns3::UniformRandomVariable[Min=0.0|Max=6.2830]"),
                                 "Rho", StringValue ("ns3::UniformRandomVariable[Min=50|Max=500]"));

      
  mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Mode", StringValue ("Time"),
                             "Time", StringValue ("0.5"),
                             "Speed", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"),
                             "Bounds", StringValue ("-500|500|-500|500"));
  mobility.Install (ueNodes);

  Config::Connect ("/NodeList/*/$ns3::MobilityModel/CourseChange",
                   MakeCallback (&CourseChange));



  BuildingsHelper::Install (ueNodes);

  // Create Devices and install them in the Nodes (eNB and UE)
  NetDeviceContainer enbDevs;
  NetDeviceContainer ueDevs;
  // lteHelper->SetSchedulerType ("ns3::RrFfMacScheduler");
  // lteHelper->SetSchedulerType ("ns3::PfFfMacScheduler");
  // lteHelper->SetSchedulerAttribute ("CqiTimerThreshold", UintegerValue (2));
  enbDevs = lteHelper->InstallEnbDevice (enbNodes);
  ueDevs = lteHelper->InstallUeDevice (ueNodes);
  
  // lteHelper->EnableRlcTraces();
  lteHelper->EnableMacTraces();

  // Attach a UE to a eNB
  lteHelper->Attach (ueDevs, enbDevs.Get (0));
  
  // Simulator::Schedule (Seconds (0.010), &ChangePosition, ueNodes.Get (0));
  // Simulator::Schedule (Seconds (0.020), &ChangePosition, ueNodes.Get (0));

    // Activate a data radio bearer
    enum EpsBearer::Qci q = EpsBearer::GBR_CONV_VOICE;
    EpsBearer bearer (q);
    lteHelper->ActivateDataRadioBearer (ueDevs, bearer);

    Simulator::Stop (Seconds (1.7));

    Simulator::Run ();

    Simulator::Destroy ();


  }

  positionFile.close();
    return 0;
}
