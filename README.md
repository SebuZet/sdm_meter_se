# SDM Meter component for SDM630 v2 (SolarEdge meter bridge)

This component is based on official ESPHome addond.
It includes extra parametres for SDM630 v2.

# Usage example

```yaml

substitutions:
  devicename: esphome-sdm630
  upper_devicename: SDM630 Bridge
  
esphome:
  name: ${devicename}
  platform: ESP32
  board: esp32-poe-iso
  on_boot:
    then:
      - output.turn_off: gpio_modbus_de
      - output.turn_off: gpio_modbus_re

external_components:
  - source: github://SebuZet/sdm_meter_se

ethernet:
  type: LAN8720
  mdc_pin: GPIO23
  mdio_pin: GPIO18
  clk_mode: GPIO17_OUT
  phy_addr: 0
  power_pin: GPIO12

api:
  password: !secret esphome_api_pass
ota:
  password: !secret esphome_api_pass

web_server:
  port: 80

output:
  - platform: gpio
    pin: 14
    id: gpio_modbus_de
  - platform: gpio
    pin: 5
    id: gpio_modbus_re

uart:
  rx_pin: 36
  tx_pin: 4
  baud_rate: 9600
  stop_bits: 1
  
modbus:
  flow_control_pin: 14

sensor:
  - platform: uptime
    name: $upper_devicename Uptime
  - platform: sdm_meter_se
    phase_a:
      current:
        name: "SDM630 L1 Current"
      voltage:
        name: "SDM630 L1 Voltage"
      active_power:
        name: "SDM630 L1 Power"
      power_factor:
        name: "SDM630 L1 Power Factor"
      apparent_power:
        name: "SDM630 L1 Apparent Power"
      reactive_power:
        name: "SDM630 L1 Reactive Power"
      phase_angle:
        name: "SDM630 L1 Phase Angle"
      voltage_next_phase:
        name: "SDM630 L1 To L2 Current"
      active_energy:
        name: "SDM630 L1 Active Energy"
      import_active_energy:
        name: "SDM630 L1 Import Active Energy"
      export_active_energy:
        name: "SDM630 L1 Export Active Energy"
      reactive_energy:
        name: "SDM630 L1 Reactive Energy"
      import_reactive_energy:
        name: "SDM630 L1 Import Reactive Energy"
      export_reactive_energy:
        name: "SDM630 L1 Export Reactive Energy"
      demand_active_power:
        name: "SDM630 L1 Power Active Demand"
    phase_b:
      current:
        name: "SDM630 L2 Current"
      voltage:
        name: "SDM630 L2 Voltage"
      active_power:
        name: "SDM630 L2 Power"
      power_factor:
        name: "SDM630 L2 Power Factor"
      apparent_power:
        name: "SDM630 L2 Apparent Power"
      reactive_power:
        name: "SDM630 L2 Reactive Power"
      phase_angle:
        name: "SDM630 L2 Phase Angle"
      voltage_next_phase:
        name: "SDM630 L2 To L3 Current"
      active_energy:
        name: "SDM630 L2 Active Energy"
      import_active_energy:
        name: "SDM630 L2 Import Active Energy"
      export_active_energy:
        name: "SDM630 L2 Export Active Energy"
      reactive_energy:
        name: "SDM630 L2 Reactive Energy"
      import_reactive_energy:
        name: "SDM630 L2 Import Reactive Energy"
      export_reactive_energy:
        name: "SDM630 L2 Export Reactive Energy"
      demand_active_power:
        name: "SDM630 L2 Power Active Demand"
    phase_c:
      current:
        name: "SDM630 L3 Current"
      voltage:
        name: "SDM630 L3 Voltage"
      active_power:
        name: "SDM630 L3 Power"
      power_factor:
        name: "SDM630 L3 Power Factor"
      apparent_power:
        name: "SDM630 L3 Apparent Power"
      reactive_power:
        name: "SDM630 L3 Reactive Power"
      phase_angle:
        name: "SDM630 L3 Phase Angle"
      voltage_next_phase:
        name: "SDM630 L3 To L1 Current"
      active_energy:
        name: "SDM630 L3 Active Energy"
      import_active_energy:
        name: "SDM630 L3 Import Active Energy"
      export_active_energy:
        name: "SDM630 L3 Export Active Energy"
      reactive_energy:
        name: "SDM630 L3 Reactive Energy"
      import_reactive_energy:
        name: "SDM630 L3 Import Reactive Energy"
      export_reactive_energy:
        name: "SDM630 L3 Export Reactive Energy"
      demand_active_power:
        name: "SDM630 L3 Power Active Demand"
    frequency:
      name: "SDM630 Frequency"
    import_active_energy:
      name: "SDM630 Import Active Energy"
    export_active_energy:
      name: "SDM630 Export Active Energy"
    import_reactive_energy:
      name: "SDM630 Import Reactive Energy"
    export_reactive_energy:
      name: "SDM630 Export Reactive Energy"
    update_interval: 10s

```
