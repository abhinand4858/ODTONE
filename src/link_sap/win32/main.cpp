//=============================================================================
// Brief   : Link SAP Win32 Main Entry Point
// Authors : Bruno Santos <bsantos@av.it.pt>
//
//
// Copyright (C) 2009 Universidade Aveiro - Instituto de Telecomunicacoes Polo Aveiro
//
// This file is part of ODTONE - Open Dot Twenty One.
//
// This software is distributed under a license. The full license
// agreement can be found in the file LICENSE in this distribution.
// This software may not be copied, modified, sold or distributed
// other than expressed in the named license agreement.
//
// This software is distributed without any warranty.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
#include "../link_sap.hpp"
#include "../interface/if_802_11.hpp"
#include <odtone/debug.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <iostream>
#include "win32.hpp"

// This file defines a macro that contains the path to the default
// configuration file
#ifndef LINK_SAP_CONFIG
#define LINK_SAP_CONFIG "link_sap.conf"
#endif

///////////////////////////////////////////////////////////////////////////////
using link_sap::nic::interface;
using link_sap::nic::if_802_11;
using link_sap::nic::if_id;
using link_sap::ushort;
using link_sap::uint;

void wlan_event_handler(const WLAN_NOTIFICATION_DATA& nd, boost::asio::io_service& ios, link_sap::link_sap* ls)
{
	std::cout << "wlan notification[" << nd.NotificationCode << "]\n";

}

int main(int argc, char** argv)
{
	odtone::setup_crash_handler();

	try {
		odtone::mih::config cfg(argc, argv, LINK_SAP_CONFIG);

		std::cout	<< "cfg(" LINK_SAP_CONFIG "): port=" << cfg.get<ushort>(odtone::mih::kConf_Port)
					<< " mihf.ip=\"" << cfg.get<std::string>(odtone::mih::kConf_MIHF_Ip)
					<< " mihf.local_port=\"" << cfg.get<ushort>(odtone::mih::kConf_MIHF_Local_Port)
					<< "\"\n";

		boost::asio::io_service ios;
		link_sap::link_sap ls(cfg, ios);

		link_sap::win32::handle lan = link_sap::win32::wlan_open();
		link_sap::win32::wlan_if_list iflst = link_sap::win32::wlan_enum_interfaces(lan);

		for (uint i = 0; i < iflst->dwNumberOfItems; ++i) {
			interface* it;

			it = new if_802_11(if_id(&iflst->InterfaceInfo[i].InterfaceGuid));
			ios.dispatch(boost::bind(&link_sap::link_sap::update, &ls, it));
		}

		ios.run();

	} catch(std::exception& e) {
		std::cerr << "error: " << e.what() << std::endl;
	}
}

// EOF ////////////////////////////////////////////////////////////////////////
