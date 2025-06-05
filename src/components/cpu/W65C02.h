#ifndef W65C02_H
#define W65C02_H

#include <format>
#include <functional>
#include <list>
#include <map>
#include <queue>

#include "../../Main.h"
#include "../Component.h"

/* CPU
 * Diagram: https://davidmjc.github.io/6502/bd.svg
 * Datasheet: https://eater.net/datasheets/w65c02s.pdf
 */

class W65C02 : public Component {
   public:
    W65C02() : registers(*this), instructions(*this) {
        // All these methods are already implemented
        // decodeLogic[0x00] = [this]() -> void { return instructions.I_00(); };
    }

    // This methode represents the reset button
    void reset() noexcept;

    // make RW flag visible to memory
    [[nodiscard]] bool getRW() const { return registers.RW; }  // make RW flag visible to memory

   private:
    struct Registers {
        Byte A;  // Accumulator

        // Index registers
        Byte X;  // X index
        Byte Y;  // Y index
        Byte S;  // Stack pointer

        Word PC;  // Program Counter

        // Processor flags NV-BDIZC (7-0)
        struct s_P {
            bool N;               // Negative
            bool V;               // Overflow
            const bool B = true;  // Break (always set)
            bool D;               // Decimal
            bool I;               // Interrupt Disable
            bool Z;               // Zero
            bool C;               // Carry

            Byte getByte() const {
                return (N << 7) + (V << 6) + (B << 4) + (D << 3) + (I << 2) + (Z << 1) + C;
            }

            std::string toStringMD() const {
                return "### Processor flags\n"
                       "| Register | Value (bin) |\n"
                       "|----------|-------------|\n"
                       "| N (Negative) | " +
                       std::format("{:B}", N) + " |\n" + "| V (Overflow) | " +
                       std::format("{:B}", V) + " |\n" + "| B (Break) | " + std::format("{:B}", B) +
                       " |\n" + "| D (Decimal) | " + std::format("{:B}", D) + " |\n" +
                       "| I (Interrupt Disable) | " + std::format("{:B}", I) + " |\n" +
                       "| Z (Zero) | " + std::format("{:B}", Z) + " |\n" + "| C (Carry) |" +
                       std::format("{:B}", C) + " |\n";
            }
        } P;

        bool RW;  // true = Read; false = Write

        // This reference is needed to be able to access non-static members of W65C02
        explicit Registers(W65C02& parent) : CPU(parent) {}

        std::string toStringMD() const {
            return "## Registers\n"
                   "| Register | Value (bin) | Value (dec) | Value (hex)|\n"
                   "|----------|-------------|-------------|------------|\n"
                   "| A (Accumulator) | " +
                   std::format("{:B}", A) + " | " + std::to_string(A) + " | " +
                   std::format("{:X}", A) + " |\n" + "| X (X index) | " + std::format("{:B}", X) +
                   " | " + std::to_string(X) + " | " + std::format("{:X}", X) + " |\n" +
                   "| Y (Y index) | " + std::format("{:B}", Y) + " | " + std::to_string(Y) + " | " +
                   std::format("{:X}", Y) + " |\n" + "| S (Stack pointer) |" +
                   std::format("{:B}", S) + " | " + std::to_string(S) + " | " +
                   std::format("{:X}", S) + " |\n" + "| PC (Program Counter) | " +
                   std::format("{:B}", PC) + " | " + std::to_string(PC) + " | " +
                   std::format("{:X}", PC) + " |\n" + "| RW (Read/Write) | " +
                   std::format("{:B}", RW) + " | " + std::to_string(RW) + " | " +
                   std::format("{:X}", RW) + " |\n" + P.toStringMD();
        }

       private:
        W65C02& CPU;
    } registers;

    void fetch();

    // This map maps the opcode to the methode in Instructions
    std::map<Byte, std::function<void()>> decodeLogic;

    // This queue consist of arrays with 2 Elements, these represent the Ph1 and Ph2
    std::queue<std::array<std::function<void()>, 2>,
               std::list<std::array<std::function<void()>, 2>>>
        callQueue;

    /* Instruction Set
     * This class contains the hole 6502 instructions set including the WDC extensions */
    class Instructions {
       public:
        void I_00();  // BRK impl
        void I_01();  // ORA x,ind
        void I_02();  // -
        void I_03();  // -
        void I_04();  // TSB zpg
        void I_05();  // ORA zpg
        void I_06();  // ASL zpg
        void I_07();  // RMB0 zpg
        void I_08();  // PHP impl
        void I_09();  // ORA #
        void I_0A();  // ASL A
        void I_0B();  // -
        void I_0C();  // TSB abs
        void I_0D();  // ORA abs
        void I_0E();  // ASL abs
        void I_0F();  // BBR0 rel

        void I_10();  // BPL rel
        void I_11();  // ORA ind,Y
        void I_12();  // ORA (zpg)
        void I_13();  // -
        void I_14();  // TRB zpg
        void I_15();  // ORA zpg,X
        void I_16();  //
        void I_17();  //
        void I_18();  //
        void I_19();  //
        void I_1A();  //
        void I_1B();  //
        void I_1C();  //
        void I_1D();  //
        void I_1E();  //
        void I_1F();  //

        void I_20();  //
        void I_21();  //
        void I_22();  //
        void I_23();  //
        void I_24();  //
        void I_25();  //
        void I_26();  //
        void I_27();  //
        void I_28();  //
        void I_29();  //
        void I_2A();  //
        void I_2B();  //
        void I_2C();  //
        void I_2D();  //
        void I_2E();  //
        void I_2F();  //

        void I_30();  //
        void I_31();  //
        void I_32();  //
        void I_33();  //
        void I_34();  //
        void I_35();  //
        void I_36();  //
        void I_37();  //
        void I_38();  //
        void I_39();  //
        void I_3A();  //
        void I_3B();  //
        void I_3C();  //
        void I_3D();  //
        void I_3E();  //
        void I_3F();  //

        void I_40();  //
        void I_41();  //
        void I_42();  //
        void I_43();  //
        void I_44();  //
        void I_45();  //
        void I_46();  //
        void I_47();  //
        void I_48();  //
        void I_49();  //
        void I_4A();  //
        void I_4B();  //
        void I_4C();  //
        void I_4D();  //
        void I_4E();  //
        void I_4F();  //

        void I_50();  //
        void I_51();  //
        void I_52();  //
        void I_53();  //
        void I_54();  //
        void I_55();  //
        void I_56();  //
        void I_57();  //
        void I_58();  //
        void I_59();  //
        void I_5A();  //
        void I_5B();  //
        void I_5C();  //
        void I_5D();  //
        void I_5E();  //
        void I_5F();  //

        void I_60();  //
        void I_61();  //
        void I_62();  //
        void I_63();  //
        void I_64();  //
        void I_65();  //
        void I_66();  //
        void I_67();  //
        void I_68();  //
        void I_69();  //
        void I_6A();  //
        void I_6B();  //
        void I_6C();  //
        void I_6D();  //
        void I_6E();  //
        void I_6F();  //

        void I_70();  //
        void I_71();  //
        void I_72();  //
        void I_73();  //
        void I_74();  //
        void I_75();  //
        void I_76();  //
        void I_77();  //
        void I_78();  //
        void I_79();  //
        void I_7A();  //
        void I_7B();  //
        void I_7C();  //
        void I_7D();  //
        void I_7E();  //
        void I_7F();  //

        void I_80();  //
        void I_81();  //
        void I_82();  //
        void I_83();  //
        void I_84();  //
        void I_85();  //
        void I_86();  //
        void I_87();  //
        void I_88();  //
        void I_89();  //
        void I_8A();  //
        void I_8B();  //
        void I_8C();  //
        void I_8D();  //
        void I_8E();  //
        void I_8F();  //

        void I_90();  //
        void I_91();  //
        void I_92();  //
        void I_93();  //
        void I_94();  //
        void I_95();  //
        void I_96();  //
        void I_97();  //
        void I_98();  //
        void I_99();  //
        void I_9A();  //
        void I_9B();  //
        void I_9C();  //
        void I_9D();  //
        void I_9E();  //
        void I_9F();  //

        void I_A0();  //
        void I_A1();  //
        void I_A2();  //
        void I_A3();  //
        void I_A4();  //
        void I_A5();  //
        void I_A6();  //
        void I_A7();  //
        void I_A8();  //
        void I_A9();  //
        void I_AA();  //
        void I_AB();  //
        void I_AC();  //
        void I_AD();  //
        void I_AE();  //
        void I_AF();  //

        void I_B0();  //
        void I_B1();  //
        void I_B2();  //
        void I_B3();  //
        void I_B4();  //
        void I_B5();  //
        void I_B6();  //
        void I_B7();  //
        void I_B8();  //
        void I_B9();  //
        void I_BA();  //
        void I_BB();  //
        void I_BC();  //
        void I_BD();  //
        void I_BE();  //
        void I_BF();  //

        void I_C0();  //
        void I_C1();  //
        void I_C2();  //
        void I_C3();  //
        void I_C4();  //
        void I_C5();  //
        void I_C6();  //
        void I_C7();  //
        void I_C8();  //
        void I_C9();  //
        void I_CA();  //
        void I_CB();  //
        void I_CC();  //
        void I_CD();  //
        void I_CE();  //
        void I_CF();  //

        void I_D0();  //
        void I_D1();  //
        void I_D2();  //
        void I_D3();  //
        void I_D4();  //
        void I_D5();  //
        void I_D6();  //
        void I_D7();  //
        void I_D8();  //
        void I_D9();  //
        void I_DA();  //
        void I_DB();  //
        void I_DC();  //
        void I_DD();  //
        void I_DE();  //
        void I_DF();  //

        void I_E0();  //
        void I_E1();  //
        void I_E2();  //
        void I_E3();  //
        void I_E4();  //
        void I_E5();  //
        void I_E6();  //
        void I_E7();  //
        void I_E8();  //
        void I_E9();  //
        void I_EA();  //
        void I_EB();  //
        void I_EC();  //
        void I_ED();  //
        void I_EE();  //
        void I_EF();  //

        void I_F0();  //
        void I_F1();  //
        void I_F2();  //
        void I_F3();  //
        void I_F4();  //
        void I_F5();  //
        void I_F6();  //
        void I_F7();  //
        void I_F8();  //
        void I_F9();  //
        void I_FA();  //
        void I_FB();  //
        void I_FC();  //
        void I_FD();  //
        void I_FE();  //
        void I_FF();  //

        // This reference is needed to be able to access non-static members of W65C02
        explicit Instructions(W65C02& parent) : CPU(parent) {}

       private:
        W65C02& CPU;
    } instructions;

   public:
    void onClockCycle(Phase phase) override;
    std::string toStringMD() const;
};

#endif  // W65C02_H
