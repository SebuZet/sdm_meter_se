#include "sdm_meter_se.h"
#include "sdm_meter_registers.h"
#include "esphome/core/log.h"

namespace esphome
{
  namespace sdm_meter_se
  {

    static const char *const TAG = "sdm_meter_se";

    static const uint8_t MODBUS_CMD_READ_IN_REGISTERS = 0x04;
    static const uint8_t MODBUS_REGISTER_READ_CYCLES = 4;
    static const uint16_t MODBUS_REGISTER_START_ADDRESS[] = {0x0000, 0x0038, 0x00C8, 0x014E};
    static const uint16_t MODBUS_REGISTER_ENTITIES_COUNT[] = {0x0036, 0x0034, 0x0046, 0x0030};
    static const uint16_t MODBUS_REGISTER_MAX_BUFFER_SIZE = 800; // 0x1FC x 2 = 764

    SDMMeterSE::SDMMeterSE()
    {
      data.reserve(MODBUS_REGISTER_MAX_BUFFER_SIZE);
    }

    void SDMMeterSE::on_modbus_data(const std::vector<uint8_t> &data_in)
    {
      if (data_in.size() != MODBUS_REGISTER_ENTITIES_COUNT[read_cycle] * 2)
      {
        ESP_LOGW(TAG, "Invalid size for SDMMeterSE!");
        return;
      }

      memcpy(data.data() + MODBUS_REGISTER_START_ADDRESS[read_cycle] * 2, data_in.data(), data_in.size());
      ++read_cycle;
      ESP_LOGW(TAG, "Getting %d package of data...", read_cycle);

      if (read_cycle == MODBUS_REGISTER_READ_CYCLES)
      {
        read_cycle = 0;
        ESP_LOGW(TAG, "Got all data, updating sensors!");
      }
      else
      {
        update();
      }

      auto sdm_meter_get_float = [&](size_t i) -> float
      {
        uint32_t temp = encode_uint32(data[i], data[i + 1], data[i + 2], data[i + 3]);
        float f;
        memcpy(&f, &temp, sizeof(f));
        return f;
      };

      for (uint8_t i = 0; i < 3; i++)
      {
        auto phase = this->phases_[i];
        if (!phase.setup)
          continue;

        float voltage = sdm_meter_get_float(SDM_PHASE_1_VOLTAGE * 2 + (i * 4));
        float current = sdm_meter_get_float(SDM_PHASE_1_CURRENT * 2 + (i * 4));
        float active_power = sdm_meter_get_float(SDM_PHASE_1_ACTIVE_POWER * 2 + (i * 4));
        float apparent_power = sdm_meter_get_float(SDM_PHASE_1_APPARENT_POWER * 2 + (i * 4));
        float reactive_power = sdm_meter_get_float(SDM_PHASE_1_REACTIVE_POWER * 2 + (i * 4));
        float power_factor = sdm_meter_get_float(SDM_PHASE_1_POWER_FACTOR * 2 + (i * 4));
        float phase_angle = sdm_meter_get_float(SDM_PHASE_1_ANGLE * 2 + (i * 4));

        float voltage_next_phase = sdm_meter_get_float(SDM_LINE_1_TO_LINE_2_VOLTS * 2 + (i * 4));
        float active_energy = sdm_meter_get_float(SDM_L1_TOTAL_ACTIVE_ENERGY * 2 + (i * 4));
        float import_active_energy = sdm_meter_get_float(SDM_L1_IMPORT_ACTIVE_ENERGY * 2 + (i * 4));
        float export_active_energy = sdm_meter_get_float(SDM_L1_EXPORT_ACTIVE_ENERGY * 2 + (i * 4));
        float reactive_energy = sdm_meter_get_float(SDM_L1_TOTAL_REACTIVE_ENERGY * 2 + (i * 4));
        float import_reactive_energy = sdm_meter_get_float(SDM_L1_IMPORT_REACTIVE_ENERGY * 2 + (i * 4));
        float export_reactive_energy = sdm_meter_get_float(SDM_L1_EXPORT_REACTIVE_ENERGY * 2 + (i * 4));
        float demand_active_power = sdm_meter_get_float(SDM_PHASE_1_CURRENT_DEMAND * 2 + (i * 4));

        ESP_LOGD(
            TAG,
            "SDMMeterSE Phase %c: V=%.3f V, I=%.3f A, Active P=%.3f W, Apparent P=%.3f VA, Reactive P=%.3f VAR, PF=%.3f, "
            "PA=%.3f °",
            i + 'A', voltage, current, active_power, apparent_power, reactive_power, power_factor, phase_angle);

        if (phase.voltage_sensor_ != nullptr)
          phase.voltage_sensor_->publish_state(voltage);
        if (phase.current_sensor_ != nullptr)
          phase.current_sensor_->publish_state(current);
        if (phase.active_power_sensor_ != nullptr)
          phase.active_power_sensor_->publish_state(active_power);
        if (phase.apparent_power_sensor_ != nullptr)
          phase.apparent_power_sensor_->publish_state(apparent_power);
        if (phase.reactive_power_sensor_ != nullptr)
          phase.reactive_power_sensor_->publish_state(reactive_power);
        if (phase.power_factor_sensor_ != nullptr)
          phase.power_factor_sensor_->publish_state(power_factor);
        if (phase.phase_angle_sensor_ != nullptr)
          phase.phase_angle_sensor_->publish_state(phase_angle);

        if (phase.voltage_next_phase_sensor_ != nullptr)
          phase.voltage_next_phase_sensor_->publish_state(voltage_next_phase);
        if (phase.active_energy_sensor_ != nullptr)
          phase.active_energy_sensor_->publish_state(active_energy);
        if (phase.import_active_energy_sensor_ != nullptr)
          phase.import_active_energy_sensor_->publish_state(import_active_energy);
        if (phase.export_active_energy_sensor_ != nullptr)
          phase.export_active_energy_sensor_->publish_state(export_active_energy);
        if (phase.reactive_energy_sensor_ != nullptr)
          phase.reactive_energy_sensor_->publish_state(reactive_energy);
        if (phase.import_reactive_energy_sensor_ != nullptr)
          phase.import_reactive_energy_sensor_->publish_state(import_reactive_energy);
        if (phase.export_reactive_energy_sensor_ != nullptr)
          phase.export_reactive_energy_sensor_->publish_state(export_reactive_energy);
        if (phase.demand_active_power_sensor_ != nullptr)
          phase.demand_active_power_sensor_->publish_state(demand_active_power);
      }

      float frequency = sdm_meter_get_float(SDM_FREQUENCY * 2);
      float import_active_energy = sdm_meter_get_float(SDM_IMPORT_ACTIVE_ENERGY * 2);
      float export_active_energy = sdm_meter_get_float(SDM_EXPORT_ACTIVE_ENERGY * 2);
      float import_reactive_energy = sdm_meter_get_float(SDM_IMPORT_REACTIVE_ENERGY * 2);
      float export_reactive_energy = sdm_meter_get_float(SDM_EXPORT_REACTIVE_ENERGY * 2);

      ESP_LOGD(TAG, "SDMMeterSE: F=%.3f Hz, Im.A.E=%.3f Wh, Ex.A.E=%.3f Wh, Im.R.E=%.3f VARh, Ex.R.E=%.3f VARh", frequency,
               import_active_energy, export_active_energy, import_reactive_energy, export_reactive_energy);

      if (this->frequency_sensor_ != nullptr)
        this->frequency_sensor_->publish_state(frequency);
      if (this->import_active_energy_sensor_ != nullptr)
        this->import_active_energy_sensor_->publish_state(import_active_energy);
      if (this->export_active_energy_sensor_ != nullptr)
        this->export_active_energy_sensor_->publish_state(export_active_energy);
      if (this->import_reactive_energy_sensor_ != nullptr)
        this->import_reactive_energy_sensor_->publish_state(import_reactive_energy);
      if (this->export_reactive_energy_sensor_ != nullptr)
        this->export_reactive_energy_sensor_->publish_state(export_reactive_energy);
    }

    void SDMMeterSE::update()
    {
      this->send(
          MODBUS_CMD_READ_IN_REGISTERS,
          MODBUS_REGISTER_START_ADDRESS[read_cycle],
          MODBUS_REGISTER_ENTITIES_COUNT[read_cycle]);
    }

    void SDMMeterSE::dump_config()
    {
      ESP_LOGCONFIG(TAG, "SDM Meter:");
      ESP_LOGCONFIG(TAG, "  Address: 0x%02X", this->address_);
      for (uint8_t i = 0; i < 3; i++)
      {
        auto phase = this->phases_[i];
        if (!phase.setup)
          continue;
        ESP_LOGCONFIG(TAG, "  Phase %c", i + 'A');
        LOG_SENSOR("    ", "Voltage", phase.voltage_sensor_);
        LOG_SENSOR("    ", "Current", phase.current_sensor_);
        LOG_SENSOR("    ", "Active Power", phase.active_power_sensor_);
        LOG_SENSOR("    ", "Apparent Power", phase.apparent_power_sensor_);
        LOG_SENSOR("    ", "Reactive Power", phase.reactive_power_sensor_);
        LOG_SENSOR("    ", "Power Factor", phase.power_factor_sensor_);
        LOG_SENSOR("    ", "Phase Angle", phase.phase_angle_sensor_);
      }
      LOG_SENSOR("  ", "Frequency", this->frequency_sensor_);
      LOG_SENSOR("  ", "Import Active Energy", this->import_active_energy_sensor_);
      LOG_SENSOR("  ", "Export Active Energy", this->export_active_energy_sensor_);
      LOG_SENSOR("  ", "Import Reactive Energy", this->import_reactive_energy_sensor_);
      LOG_SENSOR("  ", "Export Reactive Energy", this->export_reactive_energy_sensor_);
    }

  } // namespace sdm_meter_se
} // namespace esphome
