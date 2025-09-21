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

#ifdef USE_TIME

namespace esphome {
namespace bentel_kyo {

const std::string time_format = "%d/%m/%Y_%H:%M:%S";

uint32_t BentelKyo::request_clock_update(ESPTime time) {
	const std::string time_str = time.strftime(time_format);
	ESP_LOGD(TAG, "Updating clock to %s", time_str.c_str());

	if (!time.fields_in_range() || time.year < 2000 || time.year >= 2100) {
		ESP_LOGE(TAG, "Invalid time");
		return 0;
	}

	uint8_t cmd[sizeof(command::updateClock) + 7]; // 6 byte of command + 7 bytes of parameters
	memcpy(cmd, command::updateClock, sizeof(command::updateClock));

	// Set time
	cmd[6] = time.day_of_month;
	cmd[7] = time.month;
	cmd[8] = time.year - 2000;
	cmd[9] = time.hour;
	cmd[10] = time.minute;
	cmd[11] = time.second;
	cmd[12] = compute_checksum(cmd, sizeof(cmd)-1);

#ifdef DRY_RUN
	ESP_LOGI(TAG, "[DRY-RUN] Update clock command not sent: '%s'", format_hex_pretty(cmd, sizeof(cmd)).c_str());
#else
	write_UART(cmd, sizeof(cmd));
#endif
	return command_response_time::updateClock;
}

bool BentelKyo::read_clock_update() {
#ifdef DRY_RUN
	ESP_LOGI(TAG, "[DRY-RUN] Clock updated successfully");
#else
	if (!read_simple_ack(command::updateClock, sizeof(command::updateClock))) {
		ESP_LOGW(TAG, "Unable to update the clock");
		return false;
	}
	ESP_LOGI(TAG, "Clock updated successfully");
#endif
	return true;
}


}  // namespace bentel_kyo
}  // namespace esphome

#endif
