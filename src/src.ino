#include <SPI.h>
#include <SD.h>

#define REED 3
#define BUTTON 4
#define LED 5
#define CHIP_SELECT 10
#define CIRC 2.124
#define INTERVAL 1000
#define MAX_REV_TIME 40

byte exp_state = 0;
char record_buffer[40];
char output_file[12] = {'E','X','_', '0', '0', '0', '.', 'c', 's', 'v', '\0'};
float time, velocity, distance = 0;
unsigned long prev_time = 0;

void test();
void get_rev_time();
void get_velocity();
void get_distance();
void flush_data();
void log_data();
void generate_file();
void blink_led();

void setup(){
    pinMode(REED, INPUT);
    pinMode(BUTTON, INPUT);
    pinMode(LED, OUTPUT);
    pinMode(CHIP_SELECT, OUTPUT);
    Serial.begin(9600);
	if(!SD.begin(CHIP_SELECT)){
		Serial.println("Initialization failed");
        while(1);
    }
	Serial.println("Initialization succesful");
    File s = SD.open("out.csv", FILE_WRITE);
    s.close();

}


void loop(){
    File f;
    if(digitalRead(BUTTON)){
        delay(200);
        exp_state = !exp_state;
        if(exp_state){
            f = SD.open(output_file, FILE_WRITE);
            f.print("Hello");
            Serial.println("file opened");
        }else{
            f.close(); 
            Serial.println("file closed");
        }
        blink_led();
    }
    if(exp_state){ 
        if(digitalRead(REED)){
            delay(MAX_REV_TIME);
            test();
            log_data(f);
        }
    }
}

void test(){
    get_rev_time();
    get_velocity();
    get_distance();
}

void get_rev_time(){
    unsigned long time_in = millis();
    static unsigned long prev_time;
    time = (time_in - prev_time);
    time /= 1000;
    prev_time = time_in;
    Serial.println(time);
}

void get_velocity(){
    velocity = CIRC / time;
    Serial.println(velocity);
}

void get_distance(){
    distance += CIRC; 
    Serial.println(distance);
}

void log_data(File f){
    digitalWrite(LED, HIGH);
    f.print(distance);
    f.print(",");
    f.print(velocity);
    f.print(",");
    f.print(time);
    f.print("\n");
    digitalWrite(LED, LOW);
}

void generate_file(){	
    while(SD.exists(output_file) == true){		
	    if(output_file[5] == '9'){
			output_file[5] = '0';
			if(output_file[4] == '9'){
				output_file[4] = '0';
				if(output_file[3] == '9'){
					output_file[3] = '\0';
				}else{
					output_file[3]++;
				}
			}else{
				output_file[4]++;
			}
		}else{
			output_file[5]++;
		}
	}
}

void blink_led(){
    digitalWrite(LED, HIGH);
    delay(100);
    digitalWrite(LED, LOW);
}
