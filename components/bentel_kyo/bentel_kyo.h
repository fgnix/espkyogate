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
#include "esphome/components/uart/uart.h"
#include "esphome/components/binary_sensor/binary_sensor.h"



namespace esphome {
namespace bentel_kyo {

static const size_t RX_BUF_STD_SIZE = 255;
static const char *const TAG = "bentel_kyo";

static const uint8_t MAX_ZONES = 32;
static const uint8_t MAX_PARTITIONS = 8; // (a.k.a. Areas)

enum class AlarmModel {
	UNKNOWN = 0,
	KYO_4 = 1,
	KYO_8 = 2,
	KYO_8G = 3,
	KYO_8W = 4,
	KYO_8GW = 5,
	KYO_32 = 6,
	KYO_32G = 7,
};


class BentelKyo : public PollingComponent, public uart::UARTDevice {
	SUB_BINARY_SENSOR(operational)

	public:
		BentelKyo(AlarmModel model, uint8_t max_zones, uint8_t max_partitions);

		// Standard ESPHome methods
		void setup() override;
		void update() override;
		void dump_config() override;
		float get_setup_priority() const override { return setup_priority::DATA; };


	protected:
		// Alarm type and options
		const AlarmModel model_;
		const uint8_t max_partitions_;
		const uint8_t max_zones_;

		/*
		 * UART send and receive
		 */
		void write_UART(const uint8_t *const data, const size_t data_len);
		int read_UART(uint8_t buf[], const size_t size);
};

}  // namespace bentel_kyo
}  // namespace esphome

