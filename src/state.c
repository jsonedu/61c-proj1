#include "state.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

/* Helper function definitions */
static void set_board_at(game_state_t *state, unsigned int row, unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_state_t *state, unsigned int snum);
static char next_square(game_state_t *state, unsigned int snum);
static void update_tail(game_state_t *state, unsigned int snum);
static void update_head(game_state_t *state, unsigned int snum);

/* Task 1 */
game_state_t *create_default_state() {
  // TODO: Implement this function.
  game_state_t* default_state=malloc(sizeof(game_state_t));
  default_state->num_rows=18;
  default_state->board=malloc(18*sizeof(char*));
  int i;
  for(i=0;i<18;i++){
      default_state->board[i]=malloc(21);
      if(i==0||i==17){
          //default_state->board[i]=
          strcpy(default_state->board[i],"####################");
      }
      else{
          //default_state->board[i]=
          strcpy(default_state->board[i],"#                  #");
      }
  }
  //default_state->board[2]=
  strcpy(default_state->board[2],"# d>D    *         #");
  default_state->num_snakes=1;
  default_state->snakes=malloc(sizeof(snake_t));
  /*
  default_state->snakes[0].tail_row=2;
  default_state->snakes[0].tail_col=2;
  default_state->snakes[0].head_row=2;
  default_state->snakes[0].head_col=4;
  default_state->snakes[0].live=true;
  */
  default_state->snakes[0]=(snake_t){.tail_row=2,.tail_col=2,.head_row=2,.head_col=4,.live=true};
  return default_state;
}

/* Task 2 */
void free_state(game_state_t *state) {
  // TODO: Implement this function.
  int i;
  for(i=0;i<state->num_rows;i++){
      if(state->board[i]!=NULL){
          free(state->board[i]);
      }
  }
  if(state->board!=NULL){
      free(state->board);
  }
  if(state->num_snakes>0){
      free(state->snakes);
  }
  if(state!=NULL){
      free(state);
  }
  return;
}

/* Task 3 */
void print_board(game_state_t *state, FILE *fp) {
  // TODO: Implement this function.
  int i;
  for(i=0;i<state->num_rows;i++){
      fprintf(fp,"%s\n",state->board[i]);
  }
  return;
}

/*
  Saves the current state into filename. Does not modify the state object.
  (already implemented for you).
*/
void save_board(game_state_t *state, char *filename) {
  FILE *f = fopen(filename, "w");
  print_board(state, f);
  fclose(f);
}

/* Task 4.1 */

/*
  Helper function to get a character from the board
  (already implemented for you).
*/
char get_board_at(game_state_t *state, unsigned int row, unsigned int col) { return state->board[row][col]; }

/*
  Helper function to set a character on the board
  (already implemented for you).
*/
static void set_board_at(game_state_t *state, unsigned int row, unsigned int col, char ch) {
  state->board[row][col] = ch;
}

/*
  Returns true if c is part of the snake's tail.
  The snake consists of these characters: "wasd"
  Returns false otherwise.
*/
static bool is_tail(char c) {
  // TODO: Implement this function.
  return c=='w'||c=='a'||c=='s'||c=='d';
}

/*
  Returns true if c is part of the snake's head.
  The snake consists of these characters: "WASDx"
  Returns false otherwise.
*/
static bool is_head(char c) {
  // TODO: Implement this function.
  return c=='W'||c=='A'||c=='S'||c=='D'||c=='x';
}

/*
  Returns true if c is part of the snake.
  The snake consists of these characters: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
  // TODO: Implement this function.
  return is_tail(c)||is_head(c)||c=='^'||c=='<'||c=='v'||c=='>';
}

/*
  Converts a character in the snake's body ("^<v>")
  to the matching character representing the snake's
  tail ("wasd").
*/
static char body_to_tail(char c) {
  // TODO: Implement this function.
  if(c=='^'){
      return 'w';
  }
  if(c=='<'){
      return 'a';
  }
  if(c=='v'){
      return 's';
  }
  if(c=='>'){
      return 'd';
  }
  return '?';
}

/*
  Converts a character in the snake's head ("WASD")
  to the matching character representing the snake's
  body ("^<v>").
*/
static char head_to_body(char c) {
  // TODO: Implement this function.
  if(c=='W'){
      return '^';
  }
  if(c=='A'){
      return '<';
  }
  if(c=='S'){
      return 'v';
  }
  if(c=='D'){
      return '>';
  }
  return '?';
}

/*
  Returns cur_row + 1 if c is 'v' or 's' or 'S'.
  Returns cur_row - 1 if c is '^' or 'w' or 'W'.
  Returns cur_row otherwise.
*/
static unsigned int get_next_row(unsigned int cur_row, char c) {
  // TODO: Implement this function.
  if(c=='v'||c=='s'||c=='S'){
      return cur_row+1;
  }
  if(c=='^'||c=='w'||c=='W'){
      return cur_row-1;
  }
  return cur_row;
}

/*
  Returns cur_col + 1 if c is '>' or 'd' or 'D'.
  Returns cur_col - 1 if c is '<' or 'a' or 'A'.
  Returns cur_col otherwise.
*/
static unsigned int get_next_col(unsigned int cur_col, char c) {
  // TODO: Implement this function.
  if(c=='>'||c=='d'||c=='D'){
      return cur_col+1;
  }
  if(c=='<'||c=='a'||c=='A'){
      return cur_col-1;
  }
  return cur_col;
}

/*
  Task 4.2

  Helper function for update_state. Return the character in the cell the snake is moving into.

  This function should not modify anything.
*/
static char next_square(game_state_t *state, unsigned int snum) {
  // TODO: Implement this function.
  /*unsigned int i;
  unsigned int j;
  for(i=0;i<state->num_rows;i++){
      for(j=0;j<strlen((char*)state->board[i]);j++){
          char c=state->board[i][j];
          if(is_head(c)){
              return state->board[get_next_row(i,c)][get_next_col(j,c)];
          }
      }
  }*/
  char head=state->board[state->snakes[snum].head_row][state->snakes[snum].head_col];
  return state->board[get_next_row(state->snakes[snum].head_row,head)][get_next_col(state->snakes[snum].head_col,head)];
  return '?';
}

/*
  Task 4.3

  Helper function for update_state. Update the head...

  ...on the board: add a character where the snake is moving

  ...in the snake struct: update the row and col of the head

  Note that this function ignores food, walls, and snake bodies when moving the head.
*/
static void update_head(game_state_t *state, unsigned int snum) {
  // TODO: Implement this function.
  char snake_head=state->board[state->snakes[snum].head_row][state->snakes[snum].head_col];
  unsigned int next_row=get_next_row(state->snakes[snum].head_row,snake_head);
  unsigned int next_col=get_next_col(state->snakes[snum].head_col,snake_head);
  state->board[next_row][next_col]=snake_head;
  state->board[state->snakes[snum].head_row][state->snakes[snum].head_col]=head_to_body(snake_head);
  state->snakes[snum].head_row=next_row;
  state->snakes[snum].head_col=next_col;
  return;
}

/*
  Task 4.4

  Helper function for update_state. Update the tail...

  ...on the board: blank out the current tail, and change the new
  tail from a body character (^<v>) into a tail character (wasd)

  ...in the snake struct: update the row and col of the tail
*/
static void update_tail(game_state_t *state, unsigned int snum) {
  // TODO: Implement this function.
 char snake_tail=state->board[state->snakes[snum].tail_row][state->snakes[snum].tail_col];
  unsigned int next_row=get_next_row(state->snakes[snum].tail_row,snake_tail);
  unsigned int next_col=get_next_col(state->snakes[snum].tail_col,snake_tail);
  state->board[next_row][next_col]=body_to_tail(state->board[next_row][next_col]);
  state->board[state->snakes[snum].tail_row][state->snakes[snum].tail_col]=' ';
  state->snakes[snum].tail_row=next_row;
  state->snakes[snum].tail_col=next_col;
  return;
}

/* Task 4.5 */
void update_state(game_state_t *state, int (*add_food)(game_state_t *state)) {
  // TODO: Implement this function.
  unsigned int i;
  for(i=0;i<state->num_snakes;i++){
      if(state->snakes[i].live){
          char next_cell=next_square(state,i);
          if(next_cell=='*'){
              //add_food(state);
              update_head(state,i);
              add_food(state);
          }else if(next_cell==' '){
              update_head(state,i);
              update_tail(state,i);
          }else{
              state->snakes[i].live=false;
              state->board[state->snakes[i].head_row][state->snakes[i].head_col]='x';
          }
      }
  }
  return;
}

/* Task 5.1 */
char *read_line(FILE *fp) {
  // TODO: Implement this function.
  int buffer_size=20;
  char buffer[buffer_size];
  char* result=NULL;
  //result[0]='\0';
  size_t size=0;
  if(fp==NULL){
      return NULL;
  }
  char* check=fgets(buffer,buffer_size,fp);
  while(check!=NULL){
      size+=strlen(buffer);
      result=realloc(result, size+1);
      strcpy(result+(size-strlen(buffer)),buffer);
      //break when fgets encounters newline and doesn't fill buffer
      if(strchr(result,'\n')!=NULL){
        break;
      }
      check=fgets(buffer,buffer_size,fp);
  }
  //fgets must've failed or immediately hit a newline for this to occur
  if(check==NULL){
      return NULL;
  }
  return result;
}

/* Task 5.2 */
game_state_t *load_board(FILE *fp) {
  // TODO: Implement this function.
  game_state_t* gst_ptr=malloc(sizeof(game_state_t));
  gst_ptr->num_rows=0;
  gst_ptr->board=malloc(sizeof(char*));
  while(true){
      char* temp=read_line(fp);
      if(temp==NULL){
          break;
      }
      gst_ptr->num_rows+=1;
      gst_ptr->board=realloc(gst_ptr->board,gst_ptr->num_rows*sizeof(char*));
      temp[strlen(temp)-1]='\0';
      gst_ptr->board[gst_ptr->num_rows-1]=malloc(strlen(temp)+1);
      gst_ptr->board[gst_ptr->num_rows-1]=strcpy(gst_ptr->board[gst_ptr->num_rows-1],temp);
      free(temp);
  }
  gst_ptr->num_snakes=0;
  return gst_ptr;
}

/*
  Task 6.1

  Helper function for initialize_snakes.
  Given a snake struct with the tail row and col filled in,
  trace through the board to find the head row and col, and
  fill in the head row and col in the struct.
*/
static void find_head(game_state_t *state, unsigned int snum) {
  // TODO: Implement this function.
  unsigned int curr_row=state->snakes[snum].tail_row;
  unsigned int curr_col=state->snakes[snum].tail_col;

  unsigned int next_row=curr_row;
  unsigned int next_col=next_col;
  while(!is_head(state->board[curr_row][curr_col])&&is_snake(state->board[curr_row][curr_col])){
      next_row=get_next_row(curr_row,state->board[curr_row][curr_col]);
      next_col=get_next_col(curr_col,state->board[curr_row][curr_col]);

      curr_row=next_row;
      curr_col=next_col;
  }
  state->snakes[snum].head_row=curr_row;
  state->snakes[snum].head_col=curr_col;
  return;
}

/* Task 6.2 */
game_state_t *initialize_snakes(game_state_t *state) {
  // TODO: Implement this function.
  unsigned int i;
  unsigned int j;
  state->num_snakes=0;
  state->snakes=malloc(0);
  for(i=0;i<state->num_rows;i++){
      for(j=0;j<strlen(state->board[i]);j++){
          if(is_tail(state->board[i][j])){
              state->num_snakes+=1;
              state->snakes=realloc(state->snakes,sizeof(snake_t)*state->num_snakes);
              if(state->snakes==NULL){
                  return NULL;
              }
              state->snakes[state->num_snakes-1]=(snake_t){.tail_row=i,.tail_col=j,.live=true};
              find_head(state,state->num_snakes-1);
          }
      }
  }
  return state;
}
