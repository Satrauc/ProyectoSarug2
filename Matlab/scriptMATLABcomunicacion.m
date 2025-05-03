puerto = serialport("COM4", 115200); % Usa "COM5" si es necesario
pause(2); % Espera un poco para que el puerto esté listo

% Enviamos un mensaje al ESP32
writeline(puerto, "Hola desde MatLab");

% Esperamos la respuesta
pause(1); % Dale tiempo al ESP32 para responder

if puerto.NumBytesAvailable > 0
    respuesta = readline(puerto);
    disp("Respuesta desde ESP32:");
    disp(respuesta);
else
    disp("No se recibió respuesta.");
end

clear puerto;


