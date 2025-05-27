function closeBLE(b)
    % Verifica si la conexión existe y es válida
    if exist('b', 'var') && isvalid(b)
        try
            % Intenta cerrar la conexión
            clear b;
            msgbox('Conexión BLE cerrada correctamente.', 'BLE Desconectado');
        catch e
            errordlg(['Error al cerrar la conexión BLE: ', e.message], 'Error de Desconexión');
        end
    else
        warndlg('La conexión BLE no es válida o ya fue cerrada.', 'Advertencia');
    end

    % Intenta limpiar también rxChar y txChar si existen
    varsToClear = {'rxChar', 'txChar', 'b'};
    for i = 1:numel(varsToClear)
        if evalin('base', ['exist(''', varsToClear{i}, ''', ''var'')'])
            evalin('base', ['clear ', varsToClear{i}]);
        end
    end

    disp('Variables rxChar y txChar eliminadas del workspace.');
end
