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


		// Attach zone sensors
		void set_zone_sensor(binary_sensor::BinarySensor *sensor, const uint8_t zone_id);
		void set_zone_tamper_sensor(binary_sensor::BinarySensor *sensor, const uint8_t zone_id);
		void set_zone_bypassed_sensor(binary_sensor::BinarySensor *sensor, const uint8_t zone_id);
		void set_zone_alarm_memory_sensor(binary_sensor::BinarySensor *sensor, const uint8_t zone_id);
		void set_zone_tamper_memory_sensor(binary_sensor::BinarySensor *sensor, const uint8_t zone_id);


	protected:
		// Alarm type and options
		const AlarmModel model_;
		const uint8_t max_partitions_;
		const uint8_t max_zones_;
		/*
		 * Zone and Partition IDs are used to indetify them starting from 1. Like in the ESPHome configuration
		 * Zone and Partition Nums starts from 0 and are used to identify them in the arrays.
		 */
		uint8_t used_partitions_ = 0;
		uint8_t used_zones_ = 0;

		// Zones
		binary_sensor::BinarySensor *zone_sensors_[MAX_ZONES];
		binary_sensor::BinarySensor *zone_tamper_sensors_[MAX_ZONES];
		binary_sensor::BinarySensor *zone_bypassed_sensors_[MAX_ZONES];
		binary_sensor::BinarySensor *zone_alarm_memory_sensors_[MAX_ZONES];
		binary_sensor::BinarySensor *zone_tamper_memory_sensors_[MAX_ZONES];

		/*
		 * UART send and receive
		 */
		void write_UART(const uint8_t *const data, const size_t data_len);
		int read_UART(uint8_t buf[], const size_t size);
};

}  // namespace bentel_kyo
}  // namespace esphome

