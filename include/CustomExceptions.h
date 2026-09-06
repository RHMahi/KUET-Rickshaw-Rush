#ifndef CUSTOM_EXCEPTIONS_H
#define CUSTOM_EXCEPTIONS_H

#include <iostream>
#include <string>

namespace KUET {
    class GameExceptionHandler {
    public:
        // Fuel check try-catch
        static bool checkFuelError(float fuel) {
            try {
                if (fuel <= 0.0f) {
                    throw "Chacha, tel shesh! Press R to call Mechanic.";
                }
                return false;
            } 
            catch (const char* errorMsg) {
                std::cout << "\n[EXCEPTION CAUGHT] " << errorMsg << "\n";
                return true;  
            }
        }

        // Time check try-catch
        static bool checkTimeError(int missionStatus) {
            try {
                if (missionStatus == 3) {
                    throw "Time Up! Mission Failed!";
                }
                return false; 
            } 
            catch (const char* errorMsg) {
                std::cout << "\n[EXCEPTION CAUGHT] " << errorMsg << "\n";
                return true;  
            }
        }
    };
}

#endif