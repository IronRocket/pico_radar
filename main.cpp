#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include <iostream>
#include <string>
#include <map>

const float hertz_servo_const = 13.333333;
const float accuracy = 0.7;

class Ultrasonic{
    private:
        unsigned int trigger;
        unsigned int echo;
    public:

        void init(int tri, int ech){
            this->trigger = tri;
            this->echo = ech;
            gpio_init(trigger);
            gpio_init(echo);
            gpio_set_dir(trigger, GPIO_OUT);
            gpio_set_dir(echo, GPIO_IN);


        }
        float getdistance(){
            float signalon = 0;

            gpio_put(trigger,true);
            sleep_us(10);
            gpio_put(trigger,false);

            while (gpio_get(echo) == 0){
            }
            while (gpio_get(echo) == 1){
                signalon++;
                sleep_us(1);
            }

            return (signalon/29)/2;

        }

};



class Servo{
    private:
        unsigned short int servo_pin;
        int frequency;
        int duty;
    public:

        void init(int servo_pin_){
            this->servo_pin = servo_pin_;
            this->frequency = 39062;
            gpio_set_function(servo_pin,GPIO_FUNC_PWM);
            pwm_config config = pwm_get_default_config();
            pwm_config_set_clkdiv(&config, 64.f);
            pwm_config_set_wrap(&config, frequency);


            pwm_init(pwm_gpio_to_slice_num(servo_pin),&config,true);


            position(400);

        }


        void position(int duty_){
            this->duty = duty_;
            pwm_set_gpio_level(servo_pin,(duty/20000.f)*frequency);
        }

};

std::map<float,float> update_radar(Servo ser, Ultrasonic sen, std::map<float,float> &radar_map, float accuracy = 0.05){
    for (float i = 0; i<120;i += 1/accuracy){
        ser.position(i*hertz_servo_const);
        sleep_ms(10/accuracy);
        radar_map[i] = sen.getdistance();
        
    }
    return radar_map;
}






int main() {
    stdio_init_all();
    //Initializing servo
    Servo serv;
    serv.init(15);
    serv.position(400);

    //Initializing ultrasonic sensor
    Ultrasonic sensor;
    sensor.init(0,1);
    std::map<float, float> radar_map = {
    };
    sleep_ms(5000);

    while (true) {
        radar_map = update_radar(serv,sensor, radar_map, accuracy);
        
        //Sends data over serial where its read by the python application named gui.py
        for (auto const &pair: radar_map) {
            printf("%g:",pair.first);
            printf("%g\n",pair.second);
        }
        
        //The servo is then reset to its original position
        serv.position(400);
        sleep_ms(500);

        
    }
}