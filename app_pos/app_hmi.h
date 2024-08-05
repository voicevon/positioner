#pragma  once

// pcf85176
#include "f5529_i2c.h"

enum Enum_lcd_flag{
    FLAG_HART = 1,
};

class Hmi{
    public:
        void setup_keys();
        void setup_pcf85176();

        int scan_keys();
        void print_int(int x);
        void print_num(double x);
        void print_str(const char* s);
        void show_flag(Enum_lcd_flag flag, bool is_turn_on);

    private:
        void Hmi::pcf_write_byte(unsigned char address, unsigned char value);
};
