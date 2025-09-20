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

#include "action.h"

namespace esphome {
namespace bentel_kyo {

#ifdef USE_TIME
template<typename... Ts>
ClockUpdateAction<Ts...>::ClockUpdateAction(BentelKyo *component) : bentel_kyo_(component) {}

template<typename... Ts>
void ClockUpdateAction<Ts...>::play(Ts... x) { this->bentel_kyo_->schedule_clock_update(); }
#endif

template<typename... Ts>
AllAlarmsResetAction<Ts...>::AllAlarmsResetAction(BentelKyo *component): bentel_kyo_(component) {}

template<typename... Ts>
void AllAlarmsResetAction<Ts...>::play(Ts... x) { this->bentel_kyo_->schedule_all_alarms_reset(); }

template<typename... Ts>
PartitionsArmedEditAction<Ts...>::PartitionsArmedEditAction(BentelKyo *component)
	: bentel_kyo_(component)
{
	for (int i=0; i<MAX_PARTITIONS; i++)
		this->partition_mode_[i] = PartitionsArmMode::NO_CHANGE;
}

template<typename... Ts>
void PartitionsArmedEditAction<Ts...>::play(Ts... x) {
	for (int i=0; i<MAX_PARTITIONS; i++) {
		if (this->partition_mode_[i] != PartitionsArmMode::NO_CHANGE) {
			this->bentel_kyo_->partition_arm_edit(i+1, this->partition_mode_[i]);
		}
	}
	this->bentel_kyo_->schedule_partitions_arm_commit();
}

template<typename... Ts>
void PartitionsArmedEditAction<Ts...>::set_partition_armed_mode(const uint8_t partition_id, const PartitionsArmMode mode) {
	const uint8_t partition_num = partition_id - 1;
	if (partition_num >= MAX_PARTITIONS) {
		ESP_LOGE(TAG, "Partition ID %u but only %u partitions are supported", partition_id, MAX_PARTITIONS);
		return;
	}
	this->partition_mode_[partition_num] = mode;
}

} // namespace bentel_kyo
} // namespace esphome
