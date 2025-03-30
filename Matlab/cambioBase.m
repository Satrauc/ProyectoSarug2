function P = cambioBaseAB(A, B) % Genera la Matriz de cambio de base P
    % Calculamos Inversa de A 
    
    A_inv = invertirMatriz(A);

    % Multiplicamos A^(-1) * B = P 

    P = A_inv * B

end

function [A_inv] = invertirMatriz(A); % invierte una Matriz

    % Definir la matriz A (de tamaño m x m)
    
    % Verificar si la matriz es cuadrada
    [m, n] = size(A);

    if m ~= n
        disp('La matriz no es cuadrada, no se puede calcular su inversa.');
        return;
    end
    
    % Crear la matriz identidad I de tamaño m x m
    I = eye(m);
    
    % Usar el método de Gauss-Jordan para calcular la inversa
    augmented_matrix = [A I]; % Combinar A con la identidad
    for i = 1:m
        % Asegurarse de que el pivote no sea cero
        if augmented_matrix(i, i) == 0
            disp('El pivote es cero, la matriz no tiene inversa.');
            return;
        end
        
        % Dividir la fila i por el pivote para hacer que el pivote sea 1
        augmented_matrix(i, :) = augmented_matrix(i, :) / augmented_matrix(i, i);
        
        % Eliminar las demás filas en la columna i
        for j = 1:m
            if j ~= i
                augmented_matrix(j, :) = augmented_matrix(j, :) - augmented_matrix(j, i) * augmented_matrix(i, :);
            end
        end
    end
    
    % La matriz A ha sido transformada en la matriz identidad y la parte
    % derecha es la inversa de A
    A_inv = augmented_matrix(:, m+1:end);
    
    
end