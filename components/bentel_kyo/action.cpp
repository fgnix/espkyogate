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

} // namespace bentel_kyo
} // namespace esphome
