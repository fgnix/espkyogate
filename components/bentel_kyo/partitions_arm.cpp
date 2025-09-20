
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

void BentelKyo::partition_arm_edit(const uint8_t partition_id, const PartitionsArmMode mode) {
	const uint8_t partition_num = partition_id - 1;
	if (partition_num >= this->used_partitions_) {
		ESP_LOGE(TAG, "Partition ID %u: Unable to change aremd status. Only %u partitions are in use",
		         partition_id, this->used_partitions_);
		return;
	}

	const uint8_t set_mask = 1 << partition_num;
	const uint8_t unset_mask = ~set_mask;
	const char *text_set_mode = partitions_arm_mode::NO_CHANGE;

	// Unset current partition from all previous changes
	this->partitions_arm_uncommitted_away_ &= unset_mask;
	this->partitions_arm_uncommitted_stay_ &= unset_mask;
	this->partitions_arm_uncommitted_stay_0_delay_ &= unset_mask;
	this->partitions_arm_uncommitted_disarm_ &= unset_mask;

	switch(mode) {
		case PartitionsArmMode::DISARM:
			text_set_mode = (const char*)partitions_arm_mode::DISARM;
			this->partitions_arm_uncommitted_disarm_ |= set_mask;
			break;

		case PartitionsArmMode::AWAY:
			text_set_mode = (const char*)partitions_arm_mode::ARM_AWAY;
			this->partitions_arm_uncommitted_away_ |= set_mask;
			break;

		case PartitionsArmMode::STAY:
			text_set_mode = (const char*)partitions_arm_mode::ARMED_STAY;
			this->partitions_arm_uncommitted_stay_ |= set_mask;
			break;

		case PartitionsArmMode::STAY_0_DELAY:
			text_set_mode = (const char*)partitions_arm_mode::ARMED_STAY_0_DELAY;
			this->partitions_arm_uncommitted_stay_0_delay_ |= set_mask;
			break;
	}
	ESP_LOGI(TAG, "Partition ID %u: Marking partition as %s. Change to be committed", partition_id, text_set_mode);

	// Auto reset uncommitted changes after 20 minutes
	cancel_timeout(PARTITIONS_ARM_UNCOMMITTED_RESET_HANDLER); // Cancel any pending timeout, if any
	if (this->partitions_arm_uncommitted_away_ != 0 || this->partitions_arm_uncommitted_stay_ != 0
	    || this->partitions_arm_uncommitted_stay_0_delay_ != 0 || this->partitions_arm_uncommitted_disarm_ != 0)
	{
		ESP_LOGD(TAG, "Scheduling reset of partitions arm uncommitted changes in %u s",
		         AUTO_RESET_UNCOMMITTED_INTERVAL / 1000);
		set_timeout(PARTITIONS_ARM_UNCOMMITTED_RESET_HANDLER, AUTO_RESET_UNCOMMITTED_INTERVAL, [this]() {
			this->partitions_arm_uncommited_reset();
		});
	}
}

uint32_t BentelKyo::compute_new_partitions_armed_status() const {
	uint32_t new_status = this->partitions_armed_status_;

	// Process totally
	new_status |=  (this->partitions_arm_uncommitted_away_ << 24);
	new_status &= ~(this->partitions_arm_uncommitted_away_ << 16);
	new_status &= ~(this->partitions_arm_uncommitted_away_ << 8);
	new_status &= ~(this->partitions_arm_uncommitted_away_ << 0);

	// Process pratially
	new_status &= ~(this->partitions_arm_uncommitted_stay_ << 24);
	new_status |=  (this->partitions_arm_uncommitted_stay_ << 16);
	new_status &= ~(this->partitions_arm_uncommitted_stay_ << 8);
	new_status &= ~(this->partitions_arm_uncommitted_stay_ << 0);

	// Process pratially no delay
	new_status &= ~(this->partitions_arm_uncommitted_stay_0_delay_ << 24);
	new_status &= ~(this->partitions_arm_uncommitted_stay_0_delay_ << 16);
	new_status |=  (this->partitions_arm_uncommitted_stay_0_delay_ << 8);
	new_status &= ~(this->partitions_arm_uncommitted_stay_0_delay_ << 0);

	// Process disarm
	new_status &= ~(this->partitions_arm_uncommitted_disarm_ << 24);
	new_status &= ~(this->partitions_arm_uncommitted_disarm_ << 16);
	new_status &= ~(this->partitions_arm_uncommitted_disarm_ << 8);
	new_status |=  (this->partitions_arm_uncommitted_disarm_ << 0);

	return new_status;
}

void BentelKyo::partitions_arm_uncommited_reset() {
	ESP_LOGI(TAG, "Resetting partitions arm uncommitted changes...");
	this->partitions_arm_uncommitted_away_ = 0;
	this->partitions_arm_uncommitted_stay_ = 0;
	this->partitions_arm_uncommitted_stay_0_delay_ = 0;
	this->partitions_arm_uncommitted_disarm_ = 0;
}

bool BentelKyo::request_partitions_arm_edit(uint32_t &expected_response_time) {
	const uint32_t new_armed_status = compute_new_partitions_armed_status();
	if (new_armed_status == this->partitions_armed_status_) {
		ESP_LOGI(TAG, "Skip change partition armed status. The requested status is already enabled");
		partitions_arm_uncommited_reset();
		return false;
	}
	ESP_LOGI(TAG, "Changing partition armed status...");

	const uint8_t arm_away = (new_armed_status >> 24) & 0xFF;
	const uint8_t arm_stay = (new_armed_status >> 16) & 0xFF;
	const uint8_t arm_stay_0_delay = (new_armed_status >> 8) & 0xFF;
	const uint8_t disarm = (new_armed_status >> 0) & 0xFF;
	ESP_LOGD(TAG, "Changing partition armed status '%X.%X.%X.%X' => '%X.%X.%X.%X'",
	              (this->partitions_armed_status_ >> 24) & 0xFF,
	              (this->partitions_armed_status_ >> 16) & 0xFF,
	              (this->partitions_armed_status_ >> 8) & 0xFF,
	              (this->partitions_armed_status_ >> 0) & 0xFF,
	              arm_away,
	              arm_stay,
	              arm_stay_0_delay,
	              disarm);

	// Create the actual serial command
	uint8_t cmd[sizeof(command::editPartitionsArmed) + 5]; // 6 B of command + 3 B of parameters + 2 B of checksum
	memcpy(cmd, command::editPartitionsArmed, sizeof(command::editPartitionsArmed));

	cmd[6] = arm_away;
	cmd[7] = arm_stay;
	cmd[8] = arm_stay_0_delay;
	cmd[9] = disarm;
	cmd[10] = compute_checksum(cmd, sizeof(cmd)-1);

	// Since each partition must be set exactly 1 time and there are 8 partitions the sum must always be 0xFF.
	// Makeing sure that is actually 0xFF is like controlling that each one is set exactly 1 time.
	if (cmd[10] != 0xFF) {
		ESP_LOGE(TAG, "Invalid partition aremd checksum. It MUST always be 0xFF");
		return false;
	}

#ifdef DRY_RUN
	ESP_LOGI(TAG, "[DRY-RUN] Update partition armed status command not sent: '%s'",
	         format_hex_pretty(cmd, sizeof(cmd)).c_str());
#else
	write_UART(cmd, sizeof(cmd));
#endif
	expected_response_time = command_response_time::editPartitionsArmed;
	return true;
}

bool BentelKyo::read_partitions_arm_edit() {
#ifdef DRY_RUN
	ESP_LOGI(TAG, "[DRY-RUN] Partitions arm edited successfully!");
#else
	if (!read_simple_ack(command::editPartitionsArmed, sizeof(command::editPartitionsArmed))) {
		ESP_LOGW(TAG, "Unable to change partition armed status");
		return false;
	}
	ESP_LOGI(TAG, "Partitions arm edited successfully!");
#endif
	partitions_arm_uncommited_reset();
	return true;
}


} // namespace bentel_kyo
} // namespace esphome
