puerto = serialport("COM4", 115200);
pause(2);

% Valores de ejemplo
ValorX1 = 12.3; ValorY1 = 45.6; ValorAngulo1 = 90.1;
ValorX2 = 10.0; ValorY2 = 20.0; ValorAngulo2 = 30.0;
Bx = 5.5; By = 6.6;
Cx = 7.7; Cy = 8.8;

mensaje = sprintf("GEX:%.2f|GEY:%.2f|GEA:%.2f;LEX:%.2f|LEY:%.2f|LEA:%.2f;BX:%.2f|BY:%.2f;CX:%.2f|CY:%.2f", ...
    ValorX1, ValorY1, ValorAngulo1, ValorX2, ValorY2, ValorAngulo2, Bx, By, Cx, Cy);

writeline(puerto, mensaje);
pause(1);

if puerto.NumBytesAvailable > 0
    respuesta = readline(puerto);
    disp("Respuesta desde ESP32:");
    disp(respuesta);
else
    disp("No se recibió respuesta.");
end

clear puerto;

