
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

#include "bentel_kyo.h"
#include "serial_commands.h"

namespace esphome {
namespace bentel_kyo {

void BentelKyo::zone_bypass(const uint8_t zone_id, const bool bypass) {
	const uint8_t zone_num = zone_id - 1;
	if (zone_num >= this->used_zones_) {
		ESP_LOGE(TAG, "Zone ID %u: Unable to bypass. Only %u zones are in use", zone_id, this->used_zones_);
		return;
	}
	if (bypass) {
		ESP_LOGI(TAG, "Zone ID %u: Enable bypass", zone_id);
		// Save the change only if the zone is not yet bypassed
		if (this->zone_bypassed_sensors_[zone_num] == nullptr || !this->zone_bypassed_sensors_[zone_num]->state)
			this->zone_uncommitted_bypass_enable_ |= (1 << zone_num);
		this->zone_uncommitted_bypass_disable_ &= ~(1 << zone_num);
	} else {
		ESP_LOGI(TAG, "Zone ID %u: Disable bypass", zone_id);
		this->zone_uncommitted_bypass_enable_ &= ~(1 << zone_num);
		// Save the change only if the zone is not yet included
		if (this->zone_bypassed_sensors_[zone_num] == nullptr || this->zone_bypassed_sensors_[zone_num]->state)
			this->zone_uncommitted_bypass_disable_ |= (1 << zone_num);
	}

	// Reset any uncommitted changes after some time
	cancel_timeout(ZONES_BYPASS_UNCOMMITTED_RESET_HANDLER); // Cancel any pending timeout, if any
	if (this->zone_uncommitted_bypass_enable_ != 0 || this->zone_uncommitted_bypass_disable_ != 0) {
		ESP_LOGI(TAG, "Scheduling reset of uncommitted zones bypass in %u s",
		         AUTO_RESET_UNCOMMITTED_INTERVAL / 1000);
		set_timeout(ZONES_BYPASS_UNCOMMITTED_RESET_HANDLER, AUTO_RESET_UNCOMMITTED_INTERVAL, [this]() {
			this->zones_bypass_uncommited_reset();
		});
	}
}

void BentelKyo::zones_bypass_uncommited_reset() {
	ESP_LOGI(TAG, "Resetting uncommitted zones bypass...");
	this->zone_uncommitted_bypass_enable_ = 0;
	this->zone_uncommitted_bypass_disable_ = 0;
}


} // namespace bentel_kyo
} // namespace esphome
