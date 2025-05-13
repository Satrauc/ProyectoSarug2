deviceName = "ESP32_BLE";
serviceUUID = "12345678-1234-1234-1234-1234567890ab";
txUUID = "abcd1234-ab12-cd34-ef56-abcdef123456";
rxUUID = "dcba4321-ba21-dc43-fe65-fedcba654321";

[b, txChar, rxChar] = configBLE("ESP32_BLE", "12345678-1234-1234-1234-1234567890ab", "abcd1234-ab12-cd34-ef56-abcdef123456", "dcba4321-ba21-dc43-fe65-fedcba654321");

sendBLE(rxChar, 'Hola ESP32');

pause(1);  % Esperar por una respuesta

mensaje = readBLE(txChar);
disp("Respuesta desde ESP32: " + mensaje);

closeBLE(b);