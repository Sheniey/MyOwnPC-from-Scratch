
#pragma once
#include <Arduino.h>

struct SortAlghoritms {
    template<typename T>
    void insertion(T* arr, uint16_t left, uint16_t right) {
        for (uint16_t i = left + 1; i <= right; i++) {
            unsigned int temp = arr[i];
            uint16_t j = i - 1;
            while (j >= left && arr[j] > temp) { arr[j + 1] = arr[j]; j--; }
        arr[j + 1] = (T)temp;
        }
    }

    template<typename T>
    void merge(T* arr, uint16_t left, uint16_t middle, uint16_t right) {
        uint16_t len1 = middle - left + 1;
        uint16_t len2 = right - middle;
        unsigned int _left[len1], _right[len2];
        
        for (uint16_t i = 0; i < len1; i++) _left[i] = arr[left + i];
        for (uint16_t i = 0; i < len2; i++) _right[i] = arr[middle + 1 + i];

        uint16_t i = 0; uint16_t j = 0; uint16_t k = left;

        while (i < len1 && j < len2) {
            if (_left[i] <= _right[j]) {
                arr[k] = (T)_left[i];
                i++;
            } else {
                arr[k] = (T)_right[j];
                j++;
            }
            k++;
        }

        while (i < len1) {
            arr[k] = (T)_left[i];
            k++; i++;
        }

        while (j < len2) {
            arr[k] = (T)_right[j];
            k++; j++;
        }
    }

    template<typename T>
    void timsort(T* arr, uint16_t len, const uint8_t RUN = 48) {
        for (uint16_t i = 0; i < len; i += RUN) this->insertion(arr, i, std::min((i + RUN - 1), (len - 1)));

        for (uint16_t size = RUN; size < len; size = 2 * size) {
            for (uint16_t left = 0; left < len; left += 2 * size) {
                uint16_t middle = left + size - 1;
                uint16_t right = std::min((left + 2 * size - 1), (n - 1));

                if (middle < right) this->merge(arr, left, middle, right);
            }
        }
    }
};

namespace Utils {
    template<typename T>
    bool into(int val, T* arr, uint16_t len) {
        for (uint16_t i = 0; i < len; i++) {
            if (val == arr[i]) return true;
        }
        return false;
    }

    template<typename T>
    T find(int val, T* arr, uint16_t len) {
        for (uint16_t i = 0; i < len; i++) {
            if (val == arr[i]) return i;
        }
        return false;
    }

    template<typename T>
    T min(T* arr, uint16_t len) {
    int minimum = arr[0];
        for (uint16_t i = 1; i < len; ++i) {
            if (arr[i] < minimum) minimum = arr[i];
        }
        return minimum;
    }

    template<typename T>
    void shiftLeftArray(T* arr, uint16_t len, uint16_t n = 1) {
        if (n <= 1) return;
        len = len % n;
        if (len == 0) return;

        unsigned int temp[len];

        for (int i = 0; i < len; i++) {
            temp[i] = arr[i];
        }
        for (int i = 0; i < n - len; i++) {
            arr[i] = arr[i + len];
        }
        for (int i = 0; i < len; i++) {
            arr[n - len + i] = temp[i];
        }
    }

    template<typename T>
    void shiftRightArray(T* arr, uint16_t len, uint16_t n = 1) {
        if (n <= 1) return;
        len = len % n;
        if (len == 0) return;

        unsigned int temp[len];

        for (uint16_t i = 0; i < len; i++) {
            temp[i] = arr[n - len + i];
        }
        for (uint16_t i = n - 1; i >= len; i--) {
            arr[i] = arr[i - len];
        }
        for (uint16_t i = 0; i < len; i++) {
            arr[i] = temp[i];
        }
    }

    template<typename T>
    struct __typeTag__ {};

    template<typename T>
    constexpr __typeTag__<T> typeof(T) { return {}; }

    /** [-] FEATURE:
    * A === B | typeof(B) == typeof(A) => A = B : true
    */
    template<typename T>
    constexpr bool operator==(__typeTag__<T>, __typeTag__<T>) { return true; }

    /** [-] FEATURE:
    * A === B | typeof(B) != typeof(A) => A = B : false
    */
    template<typename T, typename U>
    constexpr bool operator==(__typeTag__<T>, type_tag<U>) { return false; }

    /** [*] NOTE:
    * +--------------------------------------------------------------+
    * | Aqui Aplica Muy Bien La Frase... "Si Funciona, No lo Toques" |
    * +--------------------------------------------------------------+
    * - Estas dos funciones me daban problemas porque no debes de definir funciones no templadas en el header (.h);
    * debes de definirlas en C++ (.cpp), cuyo archivo no se puede exportar.
    * - El problema da cuando intento importar estas unciones en "cache.cpp" pero me lo detecta como Non-Defined (No Definidas),
    * esto es por que las definimos en el .cpp pero recuerda que ese archivo no se puede exportar entonces tenia un archivo que 
    * pide cosas hechas en un archivo que no debe de tener cosas hechas y su contraparte si tiene cosas hechas pero que no puedes
    * importar.
    * - Entonces de pura curiosidad decidi convertir estas funciones a funciones anonimas (lambda), no me preguntes como o por que,
    * pero sirve de puta madre... 
    * PD: Bendito compilador.
    */
    auto i8toi16 = [](int8_t lo, int8_t hi) -> int16_t {
        return ((uint16_t)hi << 8) | lo;
    };

    auto i16toi8 = [](int16_t word) -> int8_t[2] {
        int8_t hi = (word >> 8) & 0xFF;
        int8_t lo = word & 0xFF;
        return {hi, lo};
    };

    auto i16toi32 = [](int16_t lo, int16_t hi) -> int32_t {
        return ((uint32_t)hi << 16) | lo;
    };

    auto i16toi64 = [](int16_t w0, int16_t w1, int16_t w2, int16_t w3) -> int64_t {
        int64_t res = 0;
        res |= (int64_t)(uint16_t)w3 << 48;
        res |= (int64_t)(uint16_t)w2 << 32;
        res |= (int64_t)(uint16_t)w1 << 16;
        res |= (int64_t)(uint16_t)w0;
        return res;
    }
};
