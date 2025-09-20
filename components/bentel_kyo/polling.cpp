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

void BentelKyo::polling_run() {
	if (this->polling_status_ != PollingStatus::WAITING) {
		ESP_LOGD(TAG, "Do not schedule next update. The last one is still running");
		return;
	}

	this->polling_loop_error_ = false;
	ESP_LOGD(TAG, "Starting polling loop...");
	this->polling_status_ = PollingStatus::REQUEST_STATUS_UPDATE;
	polling_next_step();
}

void BentelKyo::polling_next_step() {
	bool status = false;
	uint32_t next_timeout = 7; // 7 ms is the expected time of a complete polling of all sensor by ESPHome

	switch (this->polling_status_) {
		case PollingStatus::REQUEST_STATUS_UPDATE:
			next_timeout = request_status_update();
			status = true;
			this->polling_status_ = PollingStatus::READ_STATUS_UPDATE;
			break;

		case PollingStatus::READ_STATUS_UPDATE:
			status = read_status_update();
			this->polling_exec_count_++;
			this->polling_status_ = polling_fetch_next_step();
			break;

		case PollingStatus::REQUEST_PARTITIONS_UPDATE:
			next_timeout = request_partitions_update();
			status = true;
			this->polling_status_ = PollingStatus::READ_PARTITIONS_UPDATE;
			break;

		case PollingStatus::READ_PARTITIONS_UPDATE:
			status = read_partitions_update();
			this->polling_status_ = polling_fetch_next_step();
			break;

#ifdef USE_TIME
		case PollingStatus::REQUEST_CLOCK_UPDATE:
			next_timeout = request_clock_update();
			status = true;
			this->polling_status_ = PollingStatus::READ_CLOCK_UPDATE;
			break;

		case PollingStatus::READ_CLOCK_UPDATE:
			status = read_clock_update();
			this->polling_status_ = polling_fetch_next_step();

			if (this->clock_update_interval_ != UINT32_MAX) {
				cancel_timeout(CLOCK_UPDATE_HANDLER); // Cancel any pending timeout, if any
				ESP_LOGI(TAG, "Scheduling next clock update in %u ms", this->clock_update_interval_);
				set_timeout(CLOCK_UPDATE_HANDLER, this->clock_update_interval_, [this]() {
					this->schedule_clock_update();
				});
			}
			break;
#endif
	}

	// Operation failed
	if (!status) {
		ESP_LOGW(TAG, "An error occured durind polling");
		this->polling_loop_error_ = true;
	}

	// Schedule next operation if it was not the last one
	if (next_timeout && this->polling_status_ != PollingStatus::WAITING) {
		ESP_LOGD(TAG, "Schedule next polling step in %u ms", next_timeout);
		set_timeout(POLLING_HANDLER, next_timeout, [this]() { this->polling_next_step(); });
	}

	// Report as operational
	else if (!this->polling_loop_error_) {
		this->polling_error_count_ = 0;
		ESP_LOGD(TAG, "Polling loop finished successfully", next_timeout);
		if (this->operational_binary_sensor_ != nullptr)
			this->operational_binary_sensor_->publish_state(true);
	}
	else {
		if (this->polling_error_count_ < 250)
			this->polling_error_count_++;
		ESP_LOGW(TAG, "Polling loop had %u errors", this->polling_error_count_);
	}

	// Report as NOT operational
	if (this->polling_error_count_ > 3) {
		ESP_LOGE(TAG, "Polling loop is not working properly. [Error count: %u]", this->polling_error_count_);
		if (this->operational_binary_sensor_ != nullptr)
			this->operational_binary_sensor_->publish_state(false);
	}
}

void BentelKyo::polling_force_partitions_update() {
	ESP_LOGI(TAG, "Scheduling next partition update immediatelly");
	this->polling_exec_count_ = this->partitions_update_skip_+1;
}

PollingStatus BentelKyo::polling_fetch_next_step() {
	if (this->polling_status_ == PollingStatus::READ_STATUS_UPDATE
	    && this->polling_exec_count_ > this->partitions_update_skip_)
	{
		// It's time to run partition update
		ESP_LOGD(TAG, "Running partition update in polling loop...");
		this->polling_exec_count_ = 0;
		return PollingStatus::REQUEST_PARTITIONS_UPDATE;
	}

	if (this->polling_steps_scheduled_.empty()) {
		return PollingStatus::WAITING;
	}

	PollingStatus next_step = this->polling_steps_scheduled_.front();
	this->polling_steps_scheduled_.pop();
	return next_step;
}

}  // namespace bentel_kyo
}  // namespace esphome
