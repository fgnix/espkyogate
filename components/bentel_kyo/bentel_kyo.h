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
#include "esphome/core/automation.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/binary_sensor/binary_sensor.h"

#ifdef USE_TEXT_SENSOR
#include "esphome/components/text_sensor/text_sensor.h"
#endif

#ifdef USE_TIME
#include "esphome/core/time.h"
#include "esphome/components/time/real_time_clock.h"
#endif


namespace esphome {
namespace bentel_kyo {

static const size_t RX_BUF_STD_SIZE = 255;
static const char *const TAG = "bentel_kyo";

static const uint8_t MAX_ZONES = 32;
static const uint8_t MAX_PARTITIONS = 8; // (a.k.a. Areas)

static const char *const POLLING_HANDLER = "polling_handler";
static const char *const CLOCK_UPDATE_HANDLER = "clock_update_handler";

namespace partitions_arm_mode {
	static const char *const NO_CHANGE = "No change";
#ifdef LANG_IT
	static const char *const ARM_AWAY = "Inserisci";
	static const char *const ARMED_AWAY = "Inserito";
	static const char *const ARMED_STAY = "Parziale";
	static const char *const ARMED_STAY_0_DELAY = "Parziale 0 ritardo";
	static const char *const DISARM = "Disinserici";
	static const char *const DISARMED = "Disinserito";
#else
	static const char *const ARM_AWAY = "Away";
	static const char *const ARMED_AWAY = "Away";
	static const char *const ARMED_STAY = "Stay";
	static const char *const ARMED_STAY_0_DELAY = "Stay 0 delay";
	static const char *const DISARM = "Disarm";
	static const char *const DISARMED = "Disarmed";
#endif
	static const char *const UNKNOWN = "Unknown";
}

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

enum class PollingStatus {
	NOT_RUNNING = 0,
	WAITING = 1,
	REQUEST_STATUS_UPDATE = 2,
	READ_STATUS_UPDATE = 3,
	REQUEST_PARTITIONS_UPDATE = 4,
	READ_PARTITIONS_UPDATE = 5,
	REQUEST_CLOCK_UPDATE = 6,
	READ_CLOCK_UPDATE = 7,
};


class BentelKyo : public PollingComponent, public uart::UARTDevice {
	SUB_BINARY_SENSOR(operational)
	SUB_BINARY_SENSOR(global_alarm)

	// Warnings
	SUB_BINARY_SENSOR(warn_ac_power_loss)
	SUB_BINARY_SENSOR(warn_missing_bpi)
	SUB_BINARY_SENSOR(warn_fuse)
	SUB_BINARY_SENSOR(warn_low_battery)
	SUB_BINARY_SENSOR(warn_faulty_telephone_line)
	SUB_BINARY_SENSOR(warn_default_codes)
	SUB_BINARY_SENSOR(warn_wireless)

	// Tamper
	SUB_BINARY_SENSOR(tamper_zone)
	SUB_BINARY_SENSOR(tamper_fake_key)
	SUB_BINARY_SENSOR(tamper_bpi)
	SUB_BINARY_SENSOR(tamper_system)
	SUB_BINARY_SENSOR(tamper_jam)
	SUB_BINARY_SENSOR(tamper_wireless)


	public:
		BentelKyo(AlarmModel model, uint8_t max_zones, uint8_t max_partitions);

		// Standard ESPHome methods
		void setup() override;
		void update() override;
		void dump_config() override;
		float get_setup_priority() const override { return setup_priority::DATA; };

		// Clock
#ifdef USE_TIME
		void set_real_time_clock(time::RealTimeClock *rtc);
		void set_clock_update_interval(uint32_t interval_ms);
		void schedule_clock_update();
#endif

		// Configuration options
		void set_partitions_update_skip(uint8_t skip);

		// Attach zone sensors
		void set_zone_sensor(binary_sensor::BinarySensor *sensor, const uint8_t zone_id);
		void set_zone_tamper_sensor(binary_sensor::BinarySensor *sensor, const uint8_t zone_id);
		void set_zone_bypassed_sensor(binary_sensor::BinarySensor *sensor, const uint8_t zone_id);
		void set_zone_alarm_memory_sensor(binary_sensor::BinarySensor *sensor, const uint8_t zone_id);
		void set_zone_tamper_memory_sensor(binary_sensor::BinarySensor *sensor, const uint8_t zone_id);

		// Attach partition sensors
		void set_partition_alarm_sensor(binary_sensor::BinarySensor *sensor, const uint8_t partition_id);
#ifdef USE_TEXT_SENSOR
		void set_partition_armed_text_sensor(text_sensor::TextSensor *sensor, const uint8_t partition_id);
#endif


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

		// State machine status for polling updates
		PollingStatus polling_status_ = PollingStatus::NOT_RUNNING;
		std::queue<PollingStatus> polling_steps_scheduled_;
		bool polling_loop_error_; // True if an error occurred during the polling loop
		uint8_t polling_error_count_ = 0;
		// How may time skip the partition update.
		//   0 => update partition every status update
		//   1 => skip 1 time. Update partition every 2 status update
		uint8_t partitions_update_skip_ = 0;
		uint8_t polling_exec_count_ = 0;

		// Clock update
#ifdef USE_TIME
		time::RealTimeClock *rtc_ = nullptr; // If this is set; Time update is enabled
		uint32_t clock_update_interval_; // Interval in ms
#endif

		// Zones
		binary_sensor::BinarySensor *zone_sensors_[MAX_ZONES];
		binary_sensor::BinarySensor *zone_tamper_sensors_[MAX_ZONES];
		binary_sensor::BinarySensor *zone_bypassed_sensors_[MAX_ZONES];
		binary_sensor::BinarySensor *zone_alarm_memory_sensors_[MAX_ZONES];
		binary_sensor::BinarySensor *zone_tamper_memory_sensors_[MAX_ZONES];

		// Partitions
		binary_sensor::BinarySensor *partition_alarm_sensors_[MAX_PARTITIONS];
#ifdef USE_TEXT_SENSOR
		text_sensor::TextSensor *partition_armed_text_sensors_[MAX_PARTITIONS];
#endif


		/*
		 * Handle polling stages
		 */
		void polling_run();
		void polling_next_step();
		void polling_force_partitions_update();
		PollingStatus polling_fetch_next_step();

		/*
		 * Send serial request and parse serial response
		 */
		bool match_cmd_in_response(const uint8_t cmd[], const size_t cmd_len, const uint8_t rx_buf[],
		                           const size_t rx_len) const;
		uint32_t request_status_update();
		bool read_status_update();
		uint32_t request_partitions_update();
		bool read_partitions_update();
		bool read_simple_ack(const uint8_t *cmd, const uint8_t len);
#ifdef USE_TIME
		uint32_t request_clock_update();
		uint32_t request_clock_update(ESPTime time);
		bool read_clock_update();
#endif

		/*
		 * Parse UART responses
		 * Kyo 4 and Kyo 8 (no G / W / GW) use shorter reponses.
		 *
		 * parse_short are for Kyo 4 and Kyo 8 (only)
		 * parse_long methods are for the other models
		 *
		 */
		bool parse_long_status_update(const uint8_t buf[], const size_t len);
		bool parse_short_status_update(const uint8_t buf[], const size_t len);
		bool parse_long_partitions_update(const uint8_t buf[], const size_t len);
		bool parse_short_partitions_update(const uint8_t buf[], const size_t len);

		/*
		 * UART send and receive
		 */
		void write_UART(const uint8_t *const data, const size_t data_len);
		int read_UART(uint8_t buf[], const size_t size);
		uint8_t compute_checksum(const uint8_t *const data, const size_t len) const;
};

}  // namespace bentel_kyo
}  // namespace esphome

