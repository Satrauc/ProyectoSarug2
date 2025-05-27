function sendBLE(rxChar, msg)
    if ischar(msg)
        write(rxChar, uint8(msg));
    else
        error('El mensaje debe ser texto (char).');
    end
end
