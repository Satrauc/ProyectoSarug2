function [b, txChar, rxChar] = configBLE(deviceName, serviceUUID, txUUID, rxUUID)
    b = ble(deviceName);
    txChar = characteristic(b, serviceUUID, txUUID);  % Para leer notificaciones (ESP32 → MATLAB)
    rxChar = characteristic(b, serviceUUID, rxUUID);  % Para escribir datos (MATLAB → ESP32)
end