int last_time = 0;

void setup(){
    
}

void loop(){
int cur_time = millis();
    if(cur_time - last_time > 2000){
        last_time =cur_time;    
    }
    
}
    