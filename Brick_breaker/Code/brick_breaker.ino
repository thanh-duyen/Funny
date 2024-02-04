#include "data.h"

TFT_eSPI tft = TFT_eSPI();

#define BG_COLOR TFT_BLACK

void update_block(){
  for(uint8_t i = 0; i < 16; i++){
    for(uint8_t j = 0; j < 24; j++){
      if(brick_data[i][j] != brick_data[i+16][j+24]){
        tft.fillRect(ball.start_x+j*BLOCK_SIZE,ball.start_y+i*BLOCK_SIZE,BLOCK_SIZE,BLOCK_SIZE,ball.color[brick_data[i][j]]);
        brick_data[i+16][j+24] = brick_data[i][j];
        break;
      }
    }
  }
}
void move_circle_init(uint32_t delay, uint16_t *color, uint16_t start_x, uint16_t start_y){
  for(uint8_t b = 0; b < 6; b++){
    for(uint8_t i = 0; i < 8; i++){
      for(uint8_t j = 0; j < 8; j++){
        brick_data[(b/3)*8+i][(b%3)*8+j] = b;
        brick_data[(b/3)*8+i+16][(b%3)*8+j+24] = b;
      }
    }
  }
  
  ball.running = true;
  ball.delay = delay;
  ball.start_x = start_x;
  ball.start_x = start_y;
  for(uint8_t i = 0; i < 6; i++){
    ball.x1[i] = ball.x2[i] = ball.old_x[i] = (i%3)*8*BLOCK_SIZE+20;
    ball.y1[i] = ball.y2[i] = ball.old_y[i] = (i/3)*8*BLOCK_SIZE+20;
    ball.time_run[i] = millis();
    ball.color[i] = color[i];
    ball.angle[i] = 225;
    float theta = PI*ball.angle[i]/180;
    ball.new_x[i] = ball.x1[i] + cos(theta)*1000;
    ball.new_y[i] = ball.y1[i] + sin(theta)*1000;
    ball.i[i] = 0.001;
  }
  ball.step = 0;
}
void move_circle_handle(){
  if(ball.step < 16*24){
    if(ball.running == true && millis() >= ball.time_run[0]){
      uint8_t col = ball.step%24;
      uint8_t row = ball.step/24;
      tft.fillRect(ball.start_x+col*BLOCK_SIZE,ball.start_y+row*BLOCK_SIZE,BLOCK_SIZE,BLOCK_SIZE,ball.color[row/8*3+col/8]);
      ball.step++;
      ball.time_run[0] = millis() + ball.delay;
    }
  }
  else{
    for(uint8_t i = 0; i < 6 && ball.running == true; i++){
      if(millis() >= ball.time_run[i]){
        while(ball.x2[i] == ball.x1[i] || ball.y2[i] == ball.y1[i]){
          ball.x2[i] = ball.old_x[i]+(ball.new_x[i]-ball.old_x[i])*ball.i[i];
          ball.y2[i] = ball.old_y[i]+(ball.new_y[i]-ball.old_y[i])*ball.i[i];
          ball.i[i] += 0.001;
          if(ball.y2[i] < RADIUS_CIRCLE){
            ball.y2[i] = RADIUS_CIRCLE;
            break;
          }
          else if(ball.y2[i] + RADIUS_CIRCLE >= 320){
            ball.y2[i] = 319 - RADIUS_CIRCLE;
            break;
          }
          else if(ball.x2[i] < RADIUS_CIRCLE){
            ball.x2[i] = RADIUS_CIRCLE;
            break;
          }
          else if(ball.x2[i] + RADIUS_CIRCLE >= 480){
            ball.x2[i] = 479 - RADIUS_CIRCLE;
            break;
          }
        }
        tft.drawCircle(ball.start_x + ball.x1[i], ball.start_y + ball.y1[i], RADIUS_CIRCLE, ball.color[i]);
        tft.drawCircle(ball.start_x + ball.x1[i], ball.start_y + ball.y1[i], RADIUS_CIRCLE-1, ball.color[i]);
        tft.drawCircle(ball.start_x + ball.x2[i], ball.start_y + ball.y2[i], RADIUS_CIRCLE, TFT_BLACK);
        tft.drawCircle(ball.start_x + ball.x2[i], ball.start_y + ball.y2[i], RADIUS_CIRCLE-1, TFT_BLACK);
        
        uint16_t data_bottom = brick_data[(ball.y2[i]+1+RADIUS_CIRCLE)/BLOCK_SIZE][(ball.x2[i])/BLOCK_SIZE];
        uint16_t data_top = brick_data[(ball.y2[i]-RADIUS_CIRCLE)/BLOCK_SIZE][(ball.x2[i])/BLOCK_SIZE];
        uint16_t data_right = brick_data[(ball.y2[i])/BLOCK_SIZE][(ball.x2[i]+1+RADIUS_CIRCLE)/BLOCK_SIZE];
        uint16_t data_left = brick_data[(ball.y2[i])/BLOCK_SIZE][(ball.x2[i]-RADIUS_CIRCLE)/BLOCK_SIZE];
        if(ball.y2[i] >= 318 - RADIUS_CIRCLE || data_bottom != i){
          ball.old_x[i] = ball.x2[i];
          ball.old_y[i] = ball.y2[i];
          if(ball.new_x[i] > ball.old_x[i]) ball.angle[i] = 315;
          else ball.angle[i] = 225;
          float theta = PI*ball.angle[i]/180;
          ball.new_x[i] = ball.x2[i] + cos(theta)*1000;
          ball.new_y[i] = ball.y2[i] + sin(theta)*1000;
          ball.i[i] = 0.001;
          if(data_bottom != i){
            brick_data[(ball.y2[i]+1+RADIUS_CIRCLE)/BLOCK_SIZE][(ball.x2[i])/BLOCK_SIZE] = i;
            uint16_t x = ball.x2[i]/BLOCK_SIZE*BLOCK_SIZE;
            uint16_t y = (ball.y2[i]+1+RADIUS_CIRCLE)/BLOCK_SIZE*BLOCK_SIZE;
            update_block();
          }
        }
        if(ball.y2[i] <= RADIUS_CIRCLE || data_top != i){
          ball.old_x[i] = ball.x2[i];
          ball.old_y[i] = ball.y2[i];
          if(ball.new_x[i] > ball.old_x[i]) ball.angle[i] = 45;
          else ball.angle[i] = 135;
          float theta = PI*ball.angle[i]/180;
          ball.new_x[i] = ball.x2[i] + cos(theta)*1000;
          ball.new_y[i] = ball.y2[i] + sin(theta)*1000;
          ball.i[i] = 0.001;
          if(data_top != i){
            brick_data[(ball.y2[i]-1-RADIUS_CIRCLE)/BLOCK_SIZE][(ball.x2[i])/BLOCK_SIZE] = i;
            uint16_t x = ball.x2[i]/BLOCK_SIZE*BLOCK_SIZE;
            uint16_t y = (ball.y2[i]-1-RADIUS_CIRCLE)/BLOCK_SIZE*BLOCK_SIZE;
            update_block();
          }
        }
        if(ball.x2[i] <= RADIUS_CIRCLE || data_left != i){
          ball.old_x[i] = ball.x2[i];
          ball.old_y[i] = ball.y2[i];
          if(ball.new_y[i] < ball.old_y[i]) ball.angle[i] = 315;
          else ball.angle[i] = 45;
          float theta = PI*ball.angle[i]/180;
          ball.new_x[i] = ball.x2[i] + cos(theta)*1000;
          ball.new_y[i] = ball.y2[i] + sin(theta)*1000;
          ball.i[i] = 0.001;
          if(data_left != i){
            brick_data[(ball.y2[i])/BLOCK_SIZE][(ball.x2[i]-1-RADIUS_CIRCLE)/BLOCK_SIZE] = i;
            uint16_t x = (ball.x2[i]-1-RADIUS_CIRCLE)/BLOCK_SIZE*BLOCK_SIZE;
            uint16_t y = ball.y2[i]/BLOCK_SIZE*BLOCK_SIZE;
            update_block();
          }
        }
        if(ball.x2[i] >= 478 - RADIUS_CIRCLE || data_right != i){
          ball.old_x[i] = ball.x2[i];
          ball.old_y[i] = ball.y2[i];
          if(ball.new_y[i] < ball.old_y[i]) ball.angle[i] = 225;
          else ball.angle[i] = 135;
          float theta = PI*ball.angle[i]/180;
          ball.new_x[i] = ball.x2[i] + cos(theta)*1000;
          ball.new_y[i] = ball.y2[i] + sin(theta)*1000;
          ball.i[i] = 0.001;
          if(data_right != i){
            brick_data[(ball.y2[i])/BLOCK_SIZE][(ball.x2[i]+1+RADIUS_CIRCLE)/BLOCK_SIZE] = i;
            uint16_t x = (ball.x2[i]+1+RADIUS_CIRCLE)/BLOCK_SIZE*BLOCK_SIZE;
            uint16_t y = ball.y2[i]/BLOCK_SIZE*BLOCK_SIZE;
            update_block();
          }
        }
        ball.x1[i] = ball.x2[i];
        ball.y1[i] = ball.y2[i];
        ball.time_run[i] = millis() + ball.delay;
      }
    }
  }
}
void setup(){
  Serial.begin(115200);
  pinMode(LED_LCD_PIN,OUTPUT);
  pinMode(POWER_EN_PIN,OUTPUT);
  digitalWrite(LED_LCD_PIN,HIGH);
  digitalWrite(POWER_EN_PIN,HIGH);
  delay(10);
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  uint16_t color[6] = {TFT_BLUE, TFT_GREEN, TFT_RED, TFT_YELLOW, TFT_WHITE, TFT_ORANGE};
  move_circle_init(5,color,0,0);
}
void loop(){
  move_circle_handle();
}
