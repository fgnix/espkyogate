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

uint32_t BentelKyo::request_all_alarms_reset() {
	ESP_LOGI(TAG, "Requesting all alarms reset...");
	uint8_t cmd[sizeof(command::resetAlarms) + 3]; // 6 B cmd + 2 B parameters + 1B sum
	memcpy(cmd, command::resetAlarms, sizeof(command::resetAlarms));

	cmd[6] = 0xFF;
	cmd[7] = 0x00;
	cmd[8] = 0xFF; // Checksum. Computed statically

#ifdef DRY_RUN
	ESP_LOGI(TAG, "[DRY-RUN] Reset all alarms command not sent: '%s'",
	         format_hex_pretty(cmd, sizeof(cmd)).c_str());
#else
	write_UART(cmd, sizeof(cmd));
#endif
	return command_response_time::resetAlarms;
}

bool BentelKyo::read_all_alarms_reset() {
#ifdef DRY_RUN
	ESP_LOGI(TAG, "[DRY-RUN] Alarms reset successfully");
#else
	if (!read_simple_ack(command::resetAlarms, sizeof(command::resetAlarms))) {
		ESP_LOGW(TAG, "Unable to reset all alarms");
		return false;
	}
	ESP_LOGI(TAG, "Alarms reset successfully");
#endif
	return true;
}


}  // namespace bentel_kyo
}  // namespace esphome

#endif
