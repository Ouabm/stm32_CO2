#ifndef RT_CO2reception
#define RT_CO2reception_HPP

#include "cadmium/modeling/devs/atomic.hpp"
#include "stm32h7xx_hal_dma.h"
#include "stm32h7xx_hal_adc.h"
#include "stm32h7xx_hal_dac.h"
extern "C" {
     #include "adc.h"
    }

using namespace std;

namespace cadmium {

    struct ReceptionState {
        float input;
        bool output;
        double sigma;

        ReceptionState(): input(0.0),output(false), sigma(0) {}
    };

    std::ostream& operator<<(std::ostream &out, const ReceptionState& state) {
        out << "Valeur analogique: " << state.output;
        return out;
    }

    class Reception : public Atomic<ReceptionState> {
    public:
    
        Port<bool> out;
        Port<float> in;
       

        // Période d’échantillonnage (en secondes)
        double pollingRate;

        // Constructeur
        Reception(const std::string& id) 
            : Atomic<ReceptionState>(id, ReceptionState()) {

            out = addOutPort<bool>("out");
            in =addInPort<float>("in");
                   
        }

        // Transition interne : lire une nouvelle valeur ADC
        void internalTransition(ReceptionState& state) const override {

           if(state.input<=500.0){
            state.output=true;
           }
           else{
            state.output=false;
           }
            state.sigma = 0.1;
        }

        // Transition externe
        void externalTransition(ReceptionState& state, double e) const override {
            if (!in->empty()) {
                for (const auto value : in->getBag()) {
                    state.input = value;
                }
            } 
        }

        // Fonction de sortie
        void output(const ReceptionState& state) const override {
           
                out->addMessage(state.output);
            
        }

        // Avance dans le temps
        [[nodiscard]] double timeAdvance(const ReceptionState& state) const override {
            return state.sigma;
        }
    };
}

#endif // RT_ANALOGINPUT_HPP
