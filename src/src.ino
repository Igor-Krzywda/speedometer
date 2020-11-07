#include <SPI.h>
#include <SD.h>

#define REED 3
#define BUTTON 4
#define LED 5
#define CHIP_SELECT 10
#define CIRC 2.124
#define INTERVAL_EXPERIMENT 100
#define INTERVAL_SENSOR 40

byte exp_state = 0;
char record_buffer[40];
char output_file[12] = {'E','X','_', '0', '0', '0', '.', 'c', 's', 'v', '\0'};
float velocity, distance = 0, time = 0;
unsigned long prev_time_experiment = 0, prev_time_sensor = 0;

// utilities
void generate_file();
void blink_led();
void format_data();
void write_data();
// gathering / formatting data
void read_distance();
void read_data();
void log_data();

void test_sd(){
    for(int i = 0; i < 10; ++i){
        generate_file();
        File f = SD.open(output_file, FILE_WRITE);
        f.print("hi");
        f.close();
        delay(100);
    }
}


void read_data(){
    unsigned long current_time = millis();
    unsigned long rev_time = current_time - prev_time_sensor;
    if(rev_time > INTERVAL_SENSOR){
        if(digitalRead(REED)){
            prev_time_sensor = current_time;
            distance += CIRC;
            velocity = CIRC / ((float)rev_time / 1000);
        }
    }
}


void format_data(){
    char str_temp[3][6];
    dtostrf(velocity, 4, 2, str_temp[0]);
    dtostrf(distance, 4, 2, str_temp[1]);
    dtostrf(time, 4, 2, str_temp[2]);
    sprintf(record_buffer, "%s,%s,%s\n\0", str_temp[0], str_temp[1], str_temp[2]);
}

void write_data(){
    File f = SD.open(output_file, FILE_WRITE);
    f.print(record_buffer);
    f.close();
}

void log_data(){
    unsigned long current_time = millis();
    if((current_time - prev_time_experiment) > INTERVAL_EXPERIMENT){
        digitalWrite(LED, HIGH);
        prev_time_experiment = current_time;     
        time += 0.1; 
        format_data();
        write_data();
        digitalWrite(LED, LOW);
    }
}

void generate_file(){	
    // str to int, increment, back
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
}

void loop(){
    if(digitalRead(BUTTON)){
        delay(200);
        exp_state = !exp_state;
        if(exp_state){
            blink_led();
            generate_file();
            distance = time = velocity = 0;
        }else{
            blink_led();
        }
    }
    if(exp_state){ 
        read_data();
        log_data();
    }
}
