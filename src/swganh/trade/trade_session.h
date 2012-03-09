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

#ifndef SWGANH_TRADE_TRADE_SESSION_H_
#define SWGANH_TRADE_TRADE_SESSION_H_

#include <cstdint>
#include <list>
#include <memory>
#include <string>

namespace swganh {
namespace trade {

	struct TradeSession
	{
		uint64_t actor_id;
		uint64_t target_id;
		uint32_t actor_trade_credit_amount;
		uint32_t target_trade_credit_amount;
		std::list<uint64_t> actor_trade_items;
		std::list<uint64_t> target_trade_items;

		TradeSession()
			: actor_id(0)
			, target_id(0)
			, actor_trade_credit_amount(0)
			, target_trade_credit_amount(0)
		{}
	};

}} // namespace swganh::trade

#endif // SWGANH_TRADE_TRADE_SESSION_H_