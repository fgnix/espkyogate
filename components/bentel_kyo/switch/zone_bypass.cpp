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

#include "zone_bypass.h"

#ifdef USE_SWITCH

namespace esphome {
namespace bentel_kyo {

ZoneBypassSwitch::ZoneBypassSwitch(const uint8_t zone_id): zone_id_(zone_id) {}

void ZoneBypassSwitch::write_state(bool state) {
	this->parent_->zone_bypass(this->zone_id_, state);
	this->publish_state(state);
}

}  // namespace bentel_kyo
}  // namespace esphome

#endif
