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

namespace esphome {
namespace bentel_kyo {


void BentelKyo::write_UART(const uint8_t *const data, const size_t data_len) {
	// Clear RX buffer
	uint8_t buf;
	while (available() > 0)
		read_byte(&buf);

	// Send data
	write_array(data, data_len);
	ESP_LOGVV(TAG, "TX '%s'", format_hex_pretty(data, data_len).c_str());
}

int BentelKyo::read_UART(uint8_t buf[], const size_t size) {
	size_t len = 0; // amount of data already read and saved in buf
	size_t byte_to_read;

	if (size <= 1) {
		ESP_LOGE(TAG, "RX buffer too small to proceed");
		return -1;
	}

	// Clear RX buffer
	memset(buf, 0, size-1);

	// Read data from UART
	byte_to_read = available();
	while (byte_to_read > 0) {
		if (byte_to_read > size - len) {
			ESP_LOGE(TAG, "Too much data (%i B). Increase buffer size", len + byte_to_read);
			return -1;
		}

		if (!read_array(buf + len, byte_to_read)) {
			ESP_LOGE(TAG, "UART read error");
			return -1;
		}
		len += byte_to_read;
		byte_to_read = available();
	}

	if (len <= 0) {
		ESP_LOGVV(TAG, "No data from UART");
		return 0;
	}
	ESP_LOGVV(TAG, "RX '%s'", format_hex_pretty(buf, len).c_str());
	return len;
}

/*
 * The serial protocol uses for command with data a simple checksum; computed on the data only.
 * Since all commands are 6 B longs we skip that part
 */
uint8_t BentelKyo::compute_checksum(const uint8_t *const data, const size_t len) const {
	uint8_t i, cks = 0;
	for (i = 6; i < len; i++)
		cks += data[i];
	return cks;
}

}  // namespace bentel_kyo
}  // namespace esphome
