#define sizeMatriz 10
#include <Arduino.h>

class Matriz { //Clase Mariz
  private:
    uint8_t matriz[sizeMatriz][sizeMatriz];

  public:
    Matriz() {
        for (int i = 0; i < sizeMatriz; i++) {
            for (int j = 0; j < sizeMatriz; j++) {
                matriz[i][j] = 0;  // Inicializamos en ceros
            }
        }
    }

    void cargarMatriz(uint8_t datos[sizeMatriz][sizeMatriz]) {
        for (int i = 0; i < sizeMatriz; i++) {
            for (int j = 0; j < sizeMatriz; j++) {
                matriz[i][j] = datos[i][j];
            }
        }
    }

    uint8_t getValor(int i, int j) const {
        return matriz[i][j];
    }

    void setValor(int i, int j, uint8_t valor) {
        matriz[i][j] = valor;
    }

    void imprimirMatriz() {
        for (int i = 0; i < sizeMatriz; i++) {
            for (int j = 0; j < sizeMatriz; j++) {
                Serial.print(matriz[i][j]);
                Serial.print(" ");
            }
            Serial.println();
        }
        Serial.println();
    }
};

// Función de multiplicación de matrices
Matriz Multiplicacion(const Matriz &A, const Matriz &B) {
    Matriz C;  // Matriz resultado

    for (int i = 0; i < sizeMatriz; i++) {
        for (int j = 0; j < sizeMatriz; j++) {
            uint8_t sum = 0;
            for (int k = 0; k < sizeMatriz; k++) {
                sum += A.getValor(i, k) * B.getValor(k, j);
            }
            C.setValor(i, j, sum);
        }
    }

    return C;
}