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


bool BentelKyo::match_cmd_in_response(const uint8_t cmd[], const size_t cmd_len, const uint8_t rx_buf[],
                                      const size_t rx_len) const
{
	if (rx_len < cmd_len) {
		ESP_LOGD(TAG, "UART resonse is shorter than the command");
		return false;
	}

	for (int i=0; i<cmd_len; i++) {
		if (cmd[i] != rx_buf[i]) {
			ESP_LOGD(TAG, "UART response does not contins the command");
			return false;
		}
	}
	return true;
}

uint32_t BentelKyo::request_status_update() {
	write_UART(command::getStatus, sizeof(command::getStatus));
	return command_response_time::getStatus;
}

bool BentelKyo::read_status_update() {
	uint8_t buf[RX_BUF_STD_SIZE];
	int len = 0;

	len = read_UART(buf, RX_BUF_STD_SIZE);

	if (len == 0) {
		ESP_LOGE(TAG, "No UART response");
		return false;
	}
	if (len < 0)
		return false;

	switch (this->model_) {
		case AlarmModel::KYO_8G:
		case AlarmModel::KYO_8W:
		case AlarmModel::KYO_8GW:
		case AlarmModel::KYO_32:
		case AlarmModel::KYO_32G:
			return parse_long_status_update(buf, len);
	}

	ESP_LOGE(TAG, "Unspupported alarm type");
	return false;
}

uint32_t BentelKyo::request_partitions_update() {
	write_UART(command::getPartitionsStatus, sizeof(command::getPartitionsStatus));
	return command_response_time::getPartitionsStatus;
}

bool BentelKyo::read_partitions_update() {
	uint8_t buf[RX_BUF_STD_SIZE];
	int len = 0;

	len = read_UART(buf, RX_BUF_STD_SIZE);

	if (len == 0) {
		ESP_LOGE(TAG, "No UART response");
		return false;
	}
	if (len < 0)
		return false;

	switch (this->model_) {
		case AlarmModel::KYO_8G:
		case AlarmModel::KYO_8W:
		case AlarmModel::KYO_8GW:
		case AlarmModel::KYO_32:
		case AlarmModel::KYO_32G:
			return parse_long_partitions_update(buf, len);
	}

	ESP_LOGE(TAG, "Unspupported alarm type");
	return false;
}

bool BentelKyo::parse_long_status_update(const uint8_t buf[], const size_t len) {
	if (len != response_length::getStatus) {
		ESP_LOGE(TAG, "invalid message length %d. Check if the alarm model is correct", len);
		return false;
	}
	if (!match_cmd_in_response(command::getStatus, sizeof(command::getStatus), buf, len)) {
		ESP_LOGE(TAG, "Invalid response. Missing command at the beginning");
		return false;
	}

	// Validate checksum
	const uint8_t cks = compute_checksum(buf, response_length::getStatus-1);
	if (cks != buf[17]) {
		ESP_LOGE(TAG, "Invalid chekcsum in status response. Got 0x%x expected 0x%x", cks, buf[17]);
		return false;
	}

	uint8_t i;
	bool status;

	// Read zone status
	for (i = 0; i < this->used_zones_; i++) {
		if (this->zone_sensors_[i] == nullptr) {
			ESP_LOGV(TAG, "Zone ID %i: sensor not set. SKIP", i+1);
			continue;
		}

		if (i <= 7)
			status = (buf[9] >> i) & 1;
		else if (i >= 8 && i <= 15)
			status = (buf[8] >> (i - 8)) & 1;
		else if (i >= 16 && i <= 23)
			status = (buf[7] >> (i - 16)) & 1;
		else if (i >= 24)
			status = (buf[6] >> (i - 24)) & 1;

		if (status != this->zone_sensors_[i]->state) {
			ESP_LOGI(TAG, "Zone ID %i: Status %i", i+1, status);
		}
		this->zone_sensors_[i]->publish_state(status);
	}

	// Read zone tamper
	for (i = 0; i < this->used_zones_; i++) {
		if (this->zone_tamper_sensors_[i] == nullptr) {
			ESP_LOGV(TAG, "Zone ID %i: Tamper sensor not set. SKIP", i+1);
			continue;
		}

		if (i <= 7)
			status = (buf[13] >> i) & 1;
		else if (i >= 8 && i <= 15)
			status = (buf[12] >> (i - 8)) & 1;
		else if (i >= 16 && i <= 23)
			status = (buf[12] >> (i - 16)) & 1;
		else if (i >= 24)
			status = (buf[10] >> (i - 24)) & 1;

		if (status != this->zone_tamper_sensors_[i]->state) {
			ESP_LOGI(TAG, "Zone ID %i: Tamper status %i", i+1, status);
		}
		this->zone_tamper_sensors_[i]->publish_state(status);
	}

	// Read partition alarm status
	for (i = 0; i < this->used_partitions_; i++) {
		if (this->partition_alarm_sensors_[i] == nullptr) {
			ESP_LOGV(TAG, "Partition ID %i: Alarm sensor not set. SKIP", i+1);
			continue;
		}

		status = (buf[15] >> i) & 1;
		if (status != this->partition_alarm_sensors_[i]->state) {
			ESP_LOGI(TAG, "Partition ID %i: Alarm status %i", i+1, status);
		}
		this->partition_alarm_sensors_[i]->publish_state(status);
	}

	// Read warnings
	for (i = 0; i < 8; i++) {
		status = (buf[14] >> i) & 1;
		switch (i) {
			case 0:
				if (this->warn_ac_power_loss_binary_sensor_ != nullptr) {
					this->warn_ac_power_loss_binary_sensor_->publish_state(status);
				}
				break;

			case 1:
				if (this->warn_missing_bpi_binary_sensor_ != nullptr) {
					this->warn_missing_bpi_binary_sensor_->publish_state(status);
				}
				break;

			case 2:
				if (this->warn_fuse_binary_sensor_ != nullptr) {
					this->warn_fuse_binary_sensor_->publish_state(status);
				}
				break;

			case 3:
				if (this->warn_low_battery_binary_sensor_ != nullptr) {
					this->warn_low_battery_binary_sensor_->publish_state(status);
				}
				break;

			case 4:
				if (this->warn_faulty_telephone_line_binary_sensor_ != nullptr) {
					this->warn_faulty_telephone_line_binary_sensor_->publish_state(status);
				}
				break;

			case 5:
				if (this->warn_default_codes_binary_sensor_ != nullptr) {
					this->warn_default_codes_binary_sensor_->publish_state(status);
				}
				break;

			case 6:
				if (this->warn_wireless_binary_sensor_ != nullptr) {
					this->warn_wireless_binary_sensor_->publish_state(status);
				}
				break;
		}
	}


	// Read tamper
	for (i = 0; i < 8; i++) {
		status = (buf[16] >> i) & 1;
		switch(i) {
			case 2:
				if (this->tamper_zone_binary_sensor_ != nullptr) {
					this->tamper_zone_binary_sensor_->publish_state(status);
				}
				break;

			case 3:
				if (this->tamper_fake_key_binary_sensor_ != nullptr) {
					this->tamper_fake_key_binary_sensor_->publish_state(status);
				}
				break;

			case 4:
				if (this->tamper_bpi_binary_sensor_ != nullptr) {
					this->tamper_bpi_binary_sensor_->publish_state(status);
				}
				break;

			case 5:
				if (this->tamper_system_binary_sensor_ != nullptr) {
					this->tamper_system_binary_sensor_->publish_state(status);
				}
				break;

			case 6:
				if (this->tamper_jam_binary_sensor_ != nullptr) {
					this->tamper_jam_binary_sensor_->publish_state(status);
				}
				break;

			case 7:
				if (this->tamper_wireless_binary_sensor_ != nullptr) {
					this->tamper_wireless_binary_sensor_->publish_state(status);
				}
				break;
		}
	}

	ESP_LOGD(TAG, "Valid status update received");
	return true;
}

bool BentelKyo::parse_long_partitions_update(const uint8_t buf[], const size_t len) {
	if (len != response_length::getPartitionsStatus) {
		ESP_LOGE(TAG, "invalid message length %d. Check if the alarm model is correct", len);
		return false;
	}
	if (!match_cmd_in_response(command::getPartitionsStatus, sizeof(command::getPartitionsStatus), buf, len)) {
		ESP_LOGE(TAG, "Invalid response. Missing command at the beginning");
		return false;
	}

	// Validate checksum
	const uint8_t cks = compute_checksum(buf, response_length::getPartitionsStatus-1);
	if (cks != buf[25]) {
		ESP_LOGE(TAG, "Invalid chekcsum in partition update response. Got 0x%x expected 0x%x", cks, buf[17]);
		return false;
	}

	uint8_t i;
	bool status, status_A, status_S, status_I, status_D;
	std::string text_status;

#ifdef USE_TEXT_SENSOR
	// Read partition armed status
	for (i = 0; i < this->used_partitions_; i++) {
		if (this->partition_armed_text_sensors_[i] == nullptr) {
			ESP_LOGV(TAG, "Partition ID %i: Armed status sensor not set. SKIP", i+1);
			continue;
		}

		text_status = "";
		status = true; // False. If status is unknown
		status_A = (buf[6] >> i) & 1; // "Away"
		status_S = (buf[7] >> i) & 1; // "Stay"
		status_I = (buf[8] >> i) & 1; // "Stay 0 delay"
		status_D = (buf[9] >> i) & 1; // "Disarmed"

		if (status_A)
			text_status = partitions_arm_mode::ARMED_AWAY;
		if (status_S) {
			if (!text_status.empty())
				status = false;
			text_status = partitions_arm_mode::ARMED_STAY;
		}
		if (status_I) {
			if (!text_status.empty())
				status = false;
			text_status = partitions_arm_mode::ARMED_STAY_0_DELAY;
		}
		if (status_D) {
			if (!text_status.empty())
				status = false;
			text_status = partitions_arm_mode::DISARMED;
		}

		if (!status) {
			ESP_LOGE(TAG, "Partition ID %i: Unable to determine armed status. Multiple flag set", i+1);
			this->partition_armed_text_sensors_[i]->publish_state(partitions_arm_mode::UNKNOWN);
			continue;
		}

		if (this->partition_armed_text_sensors_[i]->state != text_status) {
			ESP_LOGI(TAG, "Partition ID %i: Armed status '%s'", i+1, text_status.c_str());
			this->partition_armed_text_sensors_[i]->publish_state(text_status);
		}
	}
#endif

	// Read global alarm
	if (this->global_alarm_binary_sensor_ == nullptr) {
		ESP_LOGV(TAG, "Skipping global alarm: Sensor not set");
	} else {
		status = (buf[10] >> 5) & 1;
		if (this->global_alarm_binary_sensor_->state != status)
			ESP_LOGI(TAG, "Global alarm status: %i", status);
		this->global_alarm_binary_sensor_->publish_state(status);
	}

	// Read zone bypassed
	for (i = 0; i < this->used_zones_; i++) {
		if (this->zone_bypassed_sensors_[i] == nullptr) {
			ESP_LOGV(TAG, "Zone ID %i: zone bypassed sensor not set. SKIP", i+1);
			continue;
		}

		if (i <= 7)
			status = (buf[16] >> i) & 1;
		else if (i >= 8 && i <= 15)
			status = (buf[15] >> (i - 8)) & 1;
		else if (i >= 16 && i <= 23)
			status = (buf[14] >> (i - 16)) & 1;
		else if (i >= 24)
			status = (buf[13] >> (i - 24)) & 1;

		if (status != this->zone_bypassed_sensors_[i]->state) {
			ESP_LOGI(TAG, "Zone ID %i: Bypass %s", i+1, status ? "enabled" : "disabled");
		}
		this->zone_bypassed_sensors_[i]->publish_state(status);
	}

	// Read alarm memory
	for (i = 0; i < this->used_zones_; i++) {
		if (this->zone_alarm_memory_sensors_[i] == nullptr) {
			ESP_LOGV(TAG, "Zone ID %i: zone alarm memory sensor not set. SKIP", i+1);
			continue;
		}

		if (i <= 7)
			status = (buf[20] >> i) & 1;
		else if (i >= 8 && i <= 15)
			status = (buf[19] >> (i - 8)) & 1;
		else if (i >= 16 && i <= 23)
			status = (buf[18] >> (i - 16)) & 1;
		else if (i >= 24)
			status = (buf[17] >> (i - 24)) & 1;

		if (status != this->zone_alarm_memory_sensors_[i]->state) {
			ESP_LOGI(TAG, "Zone ID %i: Alarm memory %i", i+1, status);
		}
		this->zone_alarm_memory_sensors_[i]->publish_state(status);
	}

	// Read zone tamper memory sensors
	for (i = 0; i < this->used_zones_; i++) {
		if (this->zone_tamper_memory_sensors_[i] == nullptr) {
			ESP_LOGV(TAG, "Zone ID %i: Zone tamper memory sensor not set. SKIP", i+1);
			continue;
		}

		if (i <= 7)
			status = (buf[24] >> i) & 1;
		else if (i >= 8 && i <= 15)
			status = (buf[23] >> (i - 8)) & 1;
		else if (i >= 16 && i <= 23)
			status = (buf[22] >> (i - 16)) & 1;
		else if (i >= 24)
			status = (buf[21] >> (i - 24)) & 1;

		if (status != this->zone_tamper_memory_sensors_[i]->state) {
			ESP_LOGI(TAG, "Zone ID %i: Tamper memory %i", i+1, status);
		}
		this->zone_tamper_memory_sensors_[i]->publish_state(status);
	}

	ESP_LOGD(TAG, "Valid partition update received");
	return true;
}



}  // namespace bentel_kyo
}  // namespace esphome
