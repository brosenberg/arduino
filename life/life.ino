// Conway's Game of Life
//  Automatically resets if the board reaches a steady state.
//  If the board survives for over 50 cycles, it 'wins' and resets.
// Modified from code found here: http://www.idolstarastronomer.com/Home/char-total_control_lighting
// Assumes a strand of 50 LEDs arrayed in a 7x7 square, with the final LED left in a continual off state.
#include <SPI.h>
#include <TCL.h>

const int ACTUAL_LEDS = 50;
const int LEDS = 49; 
const int COLORS = 6; // 5 colors and black.
const int BLACK = 0; // Define the colors for your code
const int RED = 1;
const int ORANGE = 2;
const int YELLOW = 3;
const int GREEN = 4;
const int BLUE = 5;
int TIMESTEP = 500;
const int MAXSTEPS = 51;
int steps = 0;
int flashes = 0;
int current_color = 1;

byte color_values[COLORS][3]; // This will store the RGB values of various colors
unsigned long change_time;
int board[ACTUAL_LEDS];
int next_board[LEDS];

void setup() {
  int i;
  unsigned long time;
  
  TCL.begin();
  
  // Set up RGB values for each color we have defined above
  color_values[BLACK][0] = 0x00;
  color_values[BLACK][1] = 0x00;
  color_values[BLACK][2] = 0x00;
  
  color_values[RED][0]=0xff;
  color_values[RED][1]=0x00;
  color_values[RED][2]=0x00;
  
  color_values[ORANGE][0]=0xff;
  color_values[ORANGE][1]=0x60;
  color_values[ORANGE][2]=0x00;
  
  color_values[YELLOW][0]=0xff;
  color_values[YELLOW][1]=0xb0;
  color_values[YELLOW][2]=0x00;
  
  color_values[GREEN][0]=0x00;
  color_values[GREEN][1]=0x80;
  color_values[GREEN][2]=0x00;
  
  color_values[BLUE][0]=0x00;
  color_values[BLUE][1]=0x00;
  color_values[BLUE][2]=0xff;

  for(i=0;i<ACTUAL_LEDS;i++)
    board[i]=BLACK;
  write_board();
}

int is_alive(int cell) {
  int i;
  int neighbors = 0;
  neighbors += board[(cell+8)%LEDS];
  neighbors += board[(cell+7)%LEDS];
  neighbors += board[(cell+6)%LEDS];
  neighbors += board[(cell+1)%LEDS];
  neighbors += board[(cell-1)%LEDS];
  neighbors += board[(cell-6)%LEDS];
  neighbors += board[(cell-7)%LEDS];
  neighbors += board[(cell-8)%LEDS];
  if ( (board[cell] && (neighbors<2||neighbors>3)) ||
       (!board[cell] && neighbors!=3) ) return 0;
  return 1;
}

void randomize_board() {
  int i;
  steps = 0;
  current_color = ((current_color+1)%(COLORS-1))+1;

  for(i=0;i<LEDS;i++)
    board[i]=random(0,2);
}

void flash_board() {
  int i;
  current_color = (current_color+1)%(COLORS-1)+1;

  if ( flashes++ == 0 ) {
    TIMESTEP/=2;
    for(i=0;i<LEDS;i++)
      board[i]=0;
  }
  for(i=0;i<LEDS;i++)
    if (board[i])
      board[i] = 0;
    else
     board[i] = 1;
  if ( flashes == 28 ) {
    steps++;
    flashes = 0;
    TIMESTEP*=2;
  } 
}

void step_board() {
  int i;

  if ( ++steps == MAXSTEPS ) {
    randomize_board();
    return;
  }
  for(i=0;i<LEDS;i++)
    if (is_alive(i))
      next_board[i] = 1;
    else
      next_board[i]= 0;
  for (i=0;i<LEDS;i++)
    if (board[i]!=next_board[i]) {
      i=0;
      break;
    }
  if (i)
    randomize_board();
  else
    for (i=0;i<LEDS;i++)
      board[i]=next_board[i];
}

void write_board() {
  int i,r;

  TCL.sendEmptyFrame();
  for(r=0;r<7;r++)
    if (r%2)
      for(i=6;i>=0;i--)
        if (board[i+(r*7)])
          TCL.sendColor(color_values[current_color][0],
                        color_values[current_color][1],
                        color_values[current_color][2]);
        else
          TCL.sendColor(0,0,0);
    else
      for(i=0;i<7;i++)
        if (board[i+(r*7)])
          TCL.sendColor(color_values[current_color][0],
                        color_values[current_color][1],
                        color_values[current_color][2]);
        else
          TCL.sendColor(0,0,0);
  TCL.sendColor(0,0,0);
  TCL.sendEmptyFrame();
}

void loop() {
  unsigned long time;
  
  time=millis();
  if (change_time<time) {
    change_time=time+TIMESTEP;
    if ( steps == MAXSTEPS-1 )
      flash_board();
    else
      step_board();
    write_board();
  }
}
