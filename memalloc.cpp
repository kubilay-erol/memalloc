
#pragma once 

#include <cstdint>
#include <new>

constexpr uint32_t buffer_size{32*8*1000}; //preallocate 256'000 bytes compile time

constexpr uint32_t data_buffer_size{buffer_size/32};

constexpr uint32_t conf_buffer_size{buffer_size/(8*32)};


enum class limit : uint32_t {
    lower = 0,
    upper = 1,
};

class memalloc {

public:

    static uint32_t ipow(uint32_t base, uint32_t exp) {

    uint32_t result = 1;

    while (exp > 0)
    {
        if (exp & 1)
            result *= base;

        base *= base;
        exp >>= 1;
    }

    return result;

    }

    static uint32_t* alloc(uint32_t bytes) {

        uint32_t* data_ptr = mem_add(bytes);
        
        if (data_ptr == nullptr) {//return a data pointer to delete later
            return nullptr;
        }

        return data_ptr;
    }

    static int32_t dealloc(uint32_t* data_ptr) {
        if (data_ptr == nullptr) {
            return -1;
        }
        return mem_free(data_ptr);

    }


private:

    alignas(4) static inline uint8_t main_pool[buffer_size] = {0};

    alignas(4) static inline uint8_t data_buffer[data_buffer_size] = {0};

    alignas(4) static inline uint8_t conf_buffer[conf_buffer_size] = {0};

    static inline uint32_t mem_block_c = 0;
    static inline uint32_t ptr_c = 0;

    static int32_t check(uint32_t base_ref = 0) {//returns the number of first empty bit
                                                    //dont call directly
        uint32_t bitmask = base_ref%8;
        uint32_t bit_arr_in = (base_ref - bitmask)/8;
        bool is_taken = 0;

        if (conf_buffer_size <= bit_arr_in) {
            return -2;
        }

        is_taken = *(conf_buffer + conf_buffer_size - bit_arr_in - 1) & (1U << bitmask);
        
        if (is_taken == 0) {
            return 1;
        }

        if (is_taken == 1) {
            return -1;
        }

        return -2;

    }

    static void* seq_mem(uint32_t bytes) {//finds sequantial memory blocks, dont call directly

        uint32_t i = 0;

        mem_block_c = 0;

        ptr_c = 0;
        
        if (bytes == 0) {
            return nullptr;
        }
        
        while (i <= conf_buffer_size*8) {
            int32_t ck = check(i);

            if (ck == -2) {
                return nullptr;
            }

            if (ck == 1) {
                mem_block_c += 1;
                if (mem_block_c*32 >= bytes) { 

                    return reinterpret_cast<uint8_t*>(main_pool) + ptr_c*32;
                };
            }
            else {
                ptr_c += 1 + mem_block_c;
                mem_block_c = 0;
            }

            i += 1;
        }

        if (mem_block_c*32 >= bytes) {

            return reinterpret_cast<uint8_t*>(main_pool) + ptr_c*32;
        
        }

        return nullptr;

    }

    static uint32_t* mem_add(uint32_t bytes) {//register memory addresses
                                                        //runs to resolve memory block as reserved
        void* ptr_s = seq_mem(bytes);
        
        if (ptr_s == nullptr) {
            return nullptr;
        }        
        
        void* ptr_e = (uint8_t*)(ptr_s) + mem_block_c*32;

        if ((ptr_s == nullptr) || (ptr_e == nullptr)) {
            return nullptr;
        }

        uint32_t* ptr_data_s = (uint32_t*)((uint8_t*)(data_buffer) + ptr_c*32);

        uint32_t* ptr_data_e = (uint32_t*)((uint8_t*)(data_buffer) + (ptr_c + mem_block_c)*32);

        *ptr_data_s = (uint32_t)ptr_s;
        *ptr_data_e = (uint32_t)ptr_e;

        for (uint32_t i = 1; i < mem_block_c; ++i) {
            
            ptr_data_s[i] = 1;
        }

        uint32_t bitmsk = 0;
        uint32_t n = ptr_c;

        while (n <= ptr_c + mem_block_c) {

            if (bitmsk == 8) {
                n += 1;
                bitmsk = 0;
            }

            if (n > ptr_c + mem_block_c) {
                break;
            }

            *(conf_buffer + conf_buffer_size - n - 1) |= (1U << bitmsk);

            bitmsk += 1;
        }

        return ptr_data_s;
    }

    static int32_t mem_free(uint32_t* data_ptr) {

        if ((*data_ptr == 1) || (*data_ptr == 0)) {
            return -1;
        }

        int32_t i = 1;

        
        data_ptr[0] = 0;

        uint32_t c = ((uint8_t*)data_ptr - data_buffer)/4;

        while(i < c) {
           
            data_ptr[i] = 0;
          
            i += 1;
        }

        data_ptr[c] = 0;

        int32_t bitmsk = 0;
        
        i += 1;

        while (i >= 0) {

            if (bitmsk == 8) {
                i -= 1;
                bitmsk = 0;
            }

            *(conf_buffer + conf_buffer_size - i - c - 1) &= ~(1U << bitmsk);

            bitmsk += 1;
        }

        return 1;
        
    }

};

struct animal {
    uint32_t* ptr_d = nullptr;
    uint32_t a = 0;
};



int main() {

    uint32_t* data_ptr = memalloc::alloc(sizeof(animal));

    animal* ptr = new ((uint32_t*)(*data_ptr)) animal;

    ptr->ptr_d = data_ptr;

    ptr->a = (uint32_t)sizeof(animal);

};

