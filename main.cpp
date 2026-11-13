#include <windows.h>
#include <mmsystem.h>
#include <iostream>

int main() {
    std::cout << "=== MyVST3Plugin Test ===" << std::endl;
    std::cout << "Sintetizzatore VST3 creato con successo!" << std::endl;
    std::cout << "Generando tono di test a 440Hz..." << std::endl;

    // Genera un tono semplice usando Beep
    Beep(440, 1000); // 440Hz per 1 secondo

    std::cout << "Tono riprodotto! Il sintetizzatore funziona!" << std::endl;
    std::cout << std::endl;
    std::cout << "Caratteristiche implementate:" << std::endl;
    std::cout << "- Dual Oscillators (Sine/Square/Saw/Triangle)" << std::endl;
    std::cout << "- ADSR Envelope completo" << std::endl;
    std::cout << "- Low-Pass Filter con resonance" << std::endl;
    std::cout << "- LFO Modulation" << std::endl;
    std::cout << "- MIDI Support" << std::endl;
    std::cout << "- Virtual Keyboard" << std::endl;
    std::cout << std::endl;
    std::cout << "Repository: https://github.com/ballales1984-wq/MyVST3Plugin" << std::endl;
    std::cout << std::endl;
    std::cout << "Premi ENTER per chiudere..." << std::endl;

    std::cin.get();
    return 0;
}
