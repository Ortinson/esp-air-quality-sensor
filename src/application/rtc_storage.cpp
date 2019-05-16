#include "rtc_storage.h"

RTCStorage::RTCStorage() {
    if(!this->IsDataValid())
        this->InitializeData();
    system_rtc_mem_read( this->counter_address, &this->counter, sizeof(this->counter));
}

void RTCStorage::StoreRTCData(RTCStorage::rtc_data data){
    system_rtc_mem_write(this->memory_start_address + this->counter * RTCData_size_words, &data, sizeof(data));
    this->WriteCounter(this->counter + 1);
}

bool RTCStorage::IsFull(){    
    if (this->counter >= this->memory_lenght / RTCData_size_words)
        return true;
    return false;
}

std::vector<RTCStorage::rtc_data> RTCStorage::GetRTCData(){
    std::vector<RTCStorage::rtc_data> output = std::vector<RTCStorage::rtc_data>();
    int c = this->counter;
    while (c>0){
        RTCStorage::rtc_data *measurement = new RTCStorage::rtc_data;
        system_rtc_mem_read(this->memory_start_address + (c - 1) * RTCData_size_words, measurement, sizeof(*measurement));
        output.push_back(*measurement);
        c--;
    }
    return output;
}

bool RTCStorage::IsDataValid(){
    int readed_flag;
    system_rtc_mem_read( this->valid_flag_address, &readed_flag, sizeof(readed_flag));
    if (readed_flag == this->valid_flag_value)
        return true;
    return false;
}

void RTCStorage::InitializeData(){
    int v = valid_flag_value;
    system_rtc_mem_write(this->valid_flag_address, &v, sizeof(v));
    this->WriteCounter(0);
}

void RTCStorage::WriteCounter(int c){
    this->counter = c;
    system_rtc_mem_write(this->counter_address, &c, sizeof(c));
}

void RTCStorage::Clear(){
    this->InitializeData();
}
