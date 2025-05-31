#ifndef CRYSTALOSCILLATOR_H
#define CRYSTALOSCILLATOR_H



class CrystalOscillator {
private:
  unsigned long cycles = 0;
public:
  CrystalOscillator& operator++() {
    cycles++;
    return *this;
  }

  unsigned long getCycles() const {
    return cycles;
  }

  void increment() {
    cycles++;
  }

  void increment(const unsigned long &cycleCount) {
    cycles += cycleCount;
  }

  void reset() {
    cycles = 0;
  }

  std::string toString() const {
    return "-- Clock --\n"
           "Cycle count: " + std::to_string(cycles) + '\n';
  }

  std::string toStringMD() const {
    return "# Clock\n"
           "Cycle count: " + std::to_string(cycles) + '\n';
  }
};



#endif //CRYSTALOSCILLATOR_H
