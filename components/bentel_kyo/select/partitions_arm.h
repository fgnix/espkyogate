/*
* bentel_kyo - ESPHome component for Bentel KYO alarms
* Copyright (C) 2025 Lorenzo De Luca (me@lorenzodeluca.dev)
* Copyright (C) 2025 fgnix
*
* This program is distributed in the hope that it will be useful, but WITHOUT ANY
* WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
* PARTICULAR PURPOSE. See the GNU General Public License for more details.
*
*
* GNU Affero General Public License v3.0
*/

#pragma once
#include "../bentel_kyo.h"

#ifdef USE_SELECT

namespace esphome {
namespace bentel_kyo {

class PartitionArmSelect : public select::Select, public Parented<BentelKyo> {
	public:
		PartitionArmSelect(const uint8_t partition_id);

	protected:
		const uint8_t partition_id_;

		void control(const std::string &value) override;
};

}  // namespace bentel_kyo
}  // namespace esphome

#endif
