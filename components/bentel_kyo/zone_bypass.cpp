
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

uint32_t BentelKyo::request_zones_bypass_edit() {
	ESP_LOGI(TAG, "Requesting zones bypass edit...");
	uint8_t cmd[sizeof(command::editZonesBypass) + 9]; // 6 B cmd + 4 B bypassed + 4 B non-bypassed + 1B sum
	memcpy(cmd, command::editZonesBypass, sizeof(command::editZonesBypass));

	cmd[6] = (zone_uncommitted_bypass_enable_ >> 24) & 0xFF; // ZoneID 32-25
	cmd[7] = (zone_uncommitted_bypass_enable_ >> 16) & 0xFF; // ZoneID 24-17
	cmd[8] = (zone_uncommitted_bypass_enable_ >> 8) & 0xFF;  // ZoneID 16-09
	cmd[9] = (zone_uncommitted_bypass_enable_ >> 0) & 0xFF;  // ZoneID 08-01
	cmd[10] = (zone_uncommitted_bypass_disable_ >> 24) & 0xFF; // ZoneID 32-25
	cmd[11] = (zone_uncommitted_bypass_disable_ >> 16) & 0xFF; // ZoneID 24-17
	cmd[12] = (zone_uncommitted_bypass_disable_ >> 8) & 0xFF;  // ZoneID 16-09
	cmd[13] = (zone_uncommitted_bypass_disable_ >> 0) & 0xFF;  // ZoneID 08-01
	cmd[14] = compute_checksum(cmd, sizeof(cmd)-1);

#ifdef DRY_RUN
	ESP_LOGI(TAG, "[DRY-RUN] Edit zones bypass command not sent: '%s'", format_hex_pretty(cmd, sizeof(cmd)).c_str());
#else
	write_UART(cmd, sizeof(cmd));
#endif
	return command_response_time::editZonesBypass;
}

bool BentelKyo::read_zones_bypass_edit() {
#ifdef DRY_RUN
	ESP_LOGI(TAG, "[DRY-RUN] Zones bypass edited successfully");
#else
	if (!read_simple_ack(command::editZonesBypass, sizeof(command::editZonesBypass))) {
		ESP_LOGW(TAG, "Unable to edit zones bypass");
		return false;
	}
	ESP_LOGI(TAG, "Zones bypass edited successfully");
#endif
	zones_bypass_uncommited_reset();
	return true;
}

} // namespace bentel_kyo
} // namespace esphome
