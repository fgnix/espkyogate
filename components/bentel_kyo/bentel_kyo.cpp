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

BentelKyo::BentelKyo(AlarmModel model, uint8_t max_zones, uint8_t max_partitions)
	: model_(model), max_partitions_(max_partitions), max_zones_(max_zones)
{
	for (int i = 0; i < MAX_ZONES; i++) {
		this->zone_sensors_[i] = nullptr;
		this->zone_tamper_sensors_[i] = nullptr;
		this->zone_bypassed_sensors_[i] = nullptr;
		this->zone_alarm_memory_sensors_[i] = nullptr;
		this->zone_tamper_memory_sensors_[i] = nullptr;
	}
	for (int i = 0; i < MAX_PARTITIONS; i++) {
		this->partition_alarm_sensors_[i] = nullptr;
#ifdef USE_TEXT_SENSOR
		this->partition_armed_text_sensors_[i] = nullptr;
#endif
	}
}

void BentelKyo::setup(){
	if (this->operational_binary_sensor_ != nullptr)
		this->operational_binary_sensor_->publish_state(false);
}

void BentelKyo::update(){

}

void BentelKyo::dump_config(){
	ESP_LOGCONFIG(TAG,
	              "Bentel Kyo:\n"
	              "  model: %i\n"
	              "  max_zones: %i\n"
	              "  max_partitions: %i\n",
	              this->model_,
	              this->max_zones_,
	              this->max_partitions_);
}

void BentelKyo::set_zone_sensor(binary_sensor::BinarySensor *sensor, const uint8_t zone_id) {
	const uint8_t zone_num = zone_id - 1; // On YAML config Zone ID starts from 1
	if (zone_num >= this->max_zones_) {
		ESP_LOGE(TAG, "Zone ID %u: unable to set zone_sensor. Only %u zones are supported", zone_id, this->max_zones_);
		return;
	}
	if (this->zone_sensors_[zone_num] != nullptr) {
		ESP_LOGE(TAG, "Zone ID %u: Unable to set zone_sensor. Already set", zone_id);
		return;
	}
	this->zone_sensors_[zone_num] = sensor;
	if (zone_num >= this->used_zones_)
		this->used_zones_ = zone_num + 1;
}
void BentelKyo::set_zone_tamper_sensor(binary_sensor::BinarySensor *sensor, const uint8_t zone_id){
	const uint8_t zone_num = zone_id - 1; // On YAML config Zone ID starts from 1
	if (zone_num >= this->max_zones_) {
		ESP_LOGE(TAG, "Zone ID %u: Unable to set zone_tamper_sensor. Only %u zones are supported",
		         zone_id, this->max_zones_);
		return;
	}
	if (this->zone_tamper_sensors_[zone_num] != nullptr) {
		ESP_LOGE(TAG, "Zone ID %u: Unable to set zone_tamper_sensor. Already set", zone_id);
		return;
	}
	this->zone_tamper_sensors_[zone_num] = sensor;
	if (zone_num >= this->used_zones_)
		this->used_zones_ = zone_num + 1;
}
void BentelKyo::set_zone_bypassed_sensor(binary_sensor::BinarySensor *sensor, const uint8_t zone_id){
	const uint8_t zone_num = zone_id - 1; // On YAML config Zone ID starts from 1
	if (zone_num >= this->max_zones_) {
		ESP_LOGE(TAG, "Zone ID %u: Unable to set zone_bypassed_sensor. Only %u zones are supported",
		         zone_id, this->max_zones_);
		return;
	}
	if (this->zone_bypassed_sensors_[zone_num] != nullptr) {
		ESP_LOGE(TAG, "Zone ID %u: Unable to set zone_bypassed_sensor. Already set", zone_id);
		return;
	}
	this->zone_bypassed_sensors_[zone_num] = sensor;
	if (zone_num >= this->used_zones_)
		this->used_zones_ = zone_num + 1;
}
void BentelKyo::set_zone_alarm_memory_sensor(binary_sensor::BinarySensor *sensor, const uint8_t zone_id){
	const uint8_t zone_num = zone_id - 1; // On YAML config Zone ID starts from 1
	if (zone_num >= this->max_zones_) {
		ESP_LOGE(TAG, "Zone ID %u: Unable to set zone_alarm_memory_sensor. Only %u zones are supported",
		         zone_id, this->max_zones_);
		return;
	}
	if (this->zone_alarm_memory_sensors_[zone_num] != nullptr) {
		ESP_LOGE(TAG, "Zone ID %u: Unable to set zone_alarm_memory_sensor. Already set", zone_id);
		return;
	}
	this->zone_alarm_memory_sensors_[zone_num] = sensor;
	if (zone_num >= this->used_zones_)
		this->used_zones_ = zone_num + 1;
}
void BentelKyo::set_zone_tamper_memory_sensor(binary_sensor::BinarySensor *sensor, const uint8_t zone_id){
	const uint8_t zone_num = zone_id - 1; // On YAML config Zone ID starts from 1
	if (zone_num >= this->max_zones_) {
		ESP_LOGE(TAG, "Zone ID %u: Unable to set zone_tamper_memory_sensor. Only %u zones are supported",
		         zone_id, this->max_zones_);
		return;
	}
	if (this->zone_tamper_memory_sensors_[zone_num] != nullptr) {
		ESP_LOGE(TAG, "Zone ID %u: Unable to set zone_tamper_memory_sensor. Already set", zone_id);
		return;
	}
	this->zone_tamper_memory_sensors_[zone_num] = sensor;
	if (zone_num >= this->used_zones_)
		this->used_zones_ = zone_num + 1;
}

void BentelKyo::set_partition_alarm_sensor(binary_sensor::BinarySensor *sensor, const uint8_t partition_id) {
	const uint8_t partition_num = partition_id - 1; // On YAML config Partition ID starts from 1
	if (partition_num >= this->max_partitions_) {
		ESP_LOGE(TAG, "Partition ID %u: Unable to set partition_alarm_sensor. Only %u partitions are supported",
		         partition_id, this->max_partitions_);
		return;
	}
	if (this->partition_alarm_sensors_[partition_num] != nullptr) {
		ESP_LOGE(TAG, "Partition ID %u: Unable to set partition_alarm_sensor. Already set", partition_id);
		return;
	}
	this->partition_alarm_sensors_[partition_num] = sensor;
	if (partition_num >= this->used_partitions_)
		this->used_partitions_ = partition_num + 1;
}
#ifdef USE_TEXT_SENSOR
void BentelKyo::set_partition_armed_text_sensor(text_sensor::TextSensor *sensor, const uint8_t partition_id) {
	const uint8_t partition_num = partition_id - 1; // On YAML config Partition ID starts from 1
	if (partition_num >= this->max_partitions_) {
		ESP_LOGE(TAG, "Partition ID %u: Unable to set partition_armed_text_sensor. Only %u partitions are supported",
		         partition_id, this->max_partitions_);
		return;
	}
	if (this->partition_armed_text_sensors_[partition_num] != nullptr) {
		ESP_LOGE(TAG, "Partition ID %u: Unable to set partition_armed_text_sensor. Already set", partition_id);
		return;
	}
	this->partition_armed_text_sensors_[partition_num] = sensor;
	if (partition_num >= this->used_partitions_)
		this->used_partitions_ = partition_num + 1;
}
#endif

}  // namespace bentel_kyo
}  // namespace esphome
