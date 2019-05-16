#ifndef APPLICATION_RTC_STORAGE_H_
#define APPLICATION_RTC_STORAGE_H_

#include <vector>
#include "user_interface.h" // this is for the RTC memory read/write functions

class RTCStorage {
public:
    typedef struct {
    int battery;
    double temperature;
    double humidity;
    double pressure;
    int age;
    } rtc_data;
    rtc_data rtcMem;

    RTCStorage(void);
    bool IsFull();
    void StoreRTCData(rtc_data data);
    std::vector<rtc_data> GetRTCData();
    void Clear();

private:
    bool IsDataValid();
    void InitializeData();
    void WriteCounter(int counter);
    static const int valid_flag_address = 64;
    static const int counter_address = 65;
    static const int memory_start_address = 66;
    static const int valid_flag_value = 0xDEADBEEF;
    static const int memory_lenght = 126;
    static const int RTCData_size_words = sizeof(rtc_data)/4;
    int counter;
    
};
#endif // APPLICATION_RTC_STORAGE_H_