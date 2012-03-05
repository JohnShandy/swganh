/*
 This file is part of SWGANH. For more information, visit http://swganh.com
 
 Copyright (c) 2006 - 2011 The SWG:ANH Team

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef SWGANH_TRADE_TRADE_SERVICE_BINDING_H_
#define SWGANH_TRADE_TRADE_SERVICE_BINDING_H_

#include "swganh/trade/trade_service.h"

#include <boost/python.hpp>

using namespace swganh::trade;
using namespace boost::python;
using namespace std;

void exportTradeService()
{
	class_<::TradeService, std::shared_ptr<::TradeService>, boost::noncopyable>("TradeService", no_init)
		.def("request_trade", &::TradeService::RequestTrade, "Sends a SecureTrade ObjController to the target.")
		.def("begin_trade", &::TradeService::BeginTrade, "Sends BeginTradeMessage to both the actor and target.")
		;
}

#endif // SWGANH_TRADE_TRADE_SERVICE_BINDING_H_