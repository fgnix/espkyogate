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

namespace esphome {
namespace bentel_kyo {

namespace command {

	// Read Realtime Status and Trouble Status
	const uint8_t getStatus[6] = {0xf0, 0x04, 0xf0, 0x0a, 0x00, 0xee};

	// Partitions Status (305) - Outputs Status - Tamper Memory - Bypassed Zones - Zone Alarm Memory - Zone Tamper Memory
	const uint8_t getPartitionsStatus[6] = {0xf0, 0x02, 0x15, 0x12, 0x00, 0x19};

	const uint8_t updateClock[6] = {0x0f, 0x03, 0xf0, 0x05, 0x00, 0x07};

} // namespace command


namespace response_length {

	const uint8_t getStatus = 18;
	const uint8_t getPartitionsStatus = 26;
	const uint8_t updateClock = 6;

} // namespace response_length


// This is used as delay from when the request has been sent to when we check for the response
// In milliseconds (ms)
namespace command_response_time {

	const uint32_t getStatus = 100;
	const uint32_t getPartitionsStatus = 100;
	const uint32_t updateClock = 300;

}


} // namespace bentel_kyo
} // namespace esphome
