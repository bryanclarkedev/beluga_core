#include "beluga_rtc_sd2405.h"
#include "beluga_string.h"
namespace beluga_core
{

    //copy-and-sep for operator= per https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
    void swap(rtc_sd2405& first, rtc_sd2405& second) // nothrow
    {
        // enable ADL (not necessary in our case, but good practice)
        using std::swap;

        swap(static_cast<beluga_core::mechanism<int> &>(first), static_cast<beluga_core::mechanism<int> &>(second));

        swap(first._rtc, second._rtc);   
        swap(first._set_time, second._set_time);
        swap(first._day_count_to_string, second._day_count_to_string);
    }

    //copy-and-swap for operator= per https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
    rtc_sd2405& rtc_sd2405::operator=(rtc_sd2405 other) 
    {
        swap(*this, other); 
        return *this;
    }

    bool rtc_sd2405::read_config()
    {
        bool config_ok = false;
        std::string config_val;

        int sda_pin, scl_pin;
        bool sda_ok = _ini_ptr->get_config_value(beluga_utils::globals_section_key, "sda_pin", &config_val );
        if(sda_ok)
        {
            sda_pin = beluga_utils::string_to_int(config_val);
        }
        bool scl_ok = _ini_ptr->get_config_value(beluga_utils::globals_section_key, "scl_pin", &config_val );
        if(scl_ok)
        {
            scl_pin = beluga_utils::string_to_int(config_val);
        }
        if(sda_ok && scl_ok)
        {
            Wire.setPins(sda_pin, scl_pin);
        }
        
        // config_ok = _ini_ptr->get_config_value(_config_file_section, "enable_serial_debug", &config_val );
        // if(config_ok)
        // {
        //     set_serial_debug_enable(config_val);
        // } 
        bool set_time_ok = _ini_ptr->get_config_value(_config_file_section, "set_time", &config_val);   
        if(set_time_ok)
        {
            _set_time = beluga_utils::string_to_bool(config_val);
        }


        add_new_value("year");
        add_new_value("month");
        add_new_value("day");
        add_new_value("hour");
        add_new_value("minute");
        add_new_value("second");
        add_new_value("week");

        _day_count_to_string[0] = "Sunday";
        _day_count_to_string[1] = "Monday";
        _day_count_to_string[2] = "Tuesday";
        _day_count_to_string[3] = "Wednesday";
        _day_count_to_string[4] = "Thursday";
        _day_count_to_string[5] = "Friday";
        _day_count_to_string[6] = "Saturday";
        


        try
        {
            if (_rtc.begin() != SD2405_OK)
            {
                beluga_utils::debug_print_loop_forever("Error setting up rtc_sd2405");
            }
        }catch(...)
        {
            beluga_utils::debug_print_loop_forever("Excepted setting up rtc_sd2405");
        }
        if(_set_time)
        {
            // Set the RTC time automatically: Calibrate RTC time by your computer time
            // _rtc.adjustRtc(F(__DATE__), F(__TIME__));
            // Set the RTC time manually
            // _rtc.adjustRtc(2017,6,19,1,12,7,0);  //Set time: 2017/6/19, Monday, 12:07:00

            //TODO: Parse __DATE__ and __TIME__
                //  adjust to your needs.
                _rtc.setSeconds(00);
                _rtc.setMinutes(25);
                _rtc.setHours(20);
                _rtc.setWeekDay(5);   //  4 = Thursday
                _rtc.setDay(3);
                _rtc.setMonth(01);
                _rtc.setYear(26);
                _rtc.enableWriteRTC();
                _rtc.write();
        }
        
        return true;
    }


    bool rtc_sd2405::run(void * p )
    {
        _rtc.read();
        std::stringstream ss;
        int year, month, day, hour, minute, second, week;
        year = (int) _rtc.year();
        month = (int) _rtc.month();
        day = (int) _rtc.day();
        hour = (int) _rtc.hours();
        minute = (int) _rtc.minutes();
        second = (int) _rtc.seconds();
        //week = (int) day / 7;

        set_value(year, "year");
        set_value(month, "month");
        set_value(day, "day");
        set_value(hour, "hour");
        set_value(minute, "minute");
        set_value(second, "second");
       // set_value(week, "week");
        

        return true;        
    }

    bool rtc_sd2405::get_time_string(std::string & time_str_return)
    {
        int h, m, s;
        bool ok_h = get_value(h, "hour");
        bool ok_m = get_value(m, "minute");
        bool ok_s = get_value(s, "second");
        
        if(ok_h && ok_m && ok_s)
        {
            std::string h_str = get_padded_time_digit(h);
            std::string m_str = get_padded_time_digit(m);
            std::string s_str = get_padded_time_digit(s);
            _ss.str("");
            _ss << h_str << ":" << m_str << ":" << s_str;
            time_str_return = _ss.str();
            return true;
        }
        return false;
    } 
    
    bool rtc_sd2405::get_date_string(std::string & time_str_return)
    {
        int yy, mm, dd;
        bool ok_yy = get_value(yy, "year");
        bool ok_mm = get_value(mm, "month");
        bool ok_dd = get_value(dd, "day");
        
        if(ok_yy && ok_mm && ok_dd)
        {
            std::string yy_str = "20" + get_padded_time_digit(yy);
            std::string mm_str = get_padded_time_digit(mm);
            std::string dd_str = get_padded_time_digit(dd);
            _ss.str("");
            _ss << yy_str << ":" << mm_str << ":" << dd_str;
            time_str_return = _ss.str();
            return true;
        }
        return false;
    } 
    
    bool rtc_sd2405::get_date_time_string(std::string & date_time_str_return)
    {
        bool date_ok, time_ok;
        std::string date_str, time_str;
        date_ok = get_date_string(date_str);
        time_ok = get_time_string(time_str);
        if(date_ok && time_ok)
        {
            _ss.str("");
            _ss << date_str << "," << time_str;
            date_time_str_return = _ss.str();
            return true;
        }
        return false;
        
    }
              
    std::string rtc_sd2405::get_padded_time_digit(int i)
    {
        _ss.str("");
        if(i <= 9)
        {
           _ss << "0";
        }
        _ss << i;
        return _ss.str();
    }

}