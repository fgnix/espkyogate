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
#include "bentel_kyo.h"

namespace esphome {
namespace bentel_kyo {


#ifdef USE_TIME
template<typename... Ts> class ClockUpdateAction : public Action<Ts...> {
	public:
		ClockUpdateAction(BentelKyo *component);
		void play(Ts... x) override;

	protected:
		BentelKyo *bentel_kyo_;
};
template class ClockUpdateAction<>;
#endif

template<typename... Ts> class AllAlarmsResetAction : public Action<Ts...> {
	public:
		AllAlarmsResetAction(BentelKyo *component);
		void play(Ts... x) override;

	protected:
		BentelKyo *bentel_kyo_;
};
template class AllAlarmsResetAction<>;

template<typename... Ts> class PartitionsArmedEditAction : public Action<Ts...> {
	public:
		PartitionsArmedEditAction(BentelKyo *component);
		void play(Ts... x) override;
		void set_partition_armed_mode(const uint8_t partition_id, const PartitionsArmMode mode);

	protected:
		BentelKyo *bentel_kyo_;
		PartitionsArmMode partition_mode_[MAX_PARTITIONS];
};
template class PartitionsArmedEditAction<>;

} // namespace bentel_kyo
} // namespace esphome
