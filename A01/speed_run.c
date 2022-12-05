//
// AED, August 2022 (Tomás Oliveira e Silva)
//
// First practical assignement (speed run)
//
// Compile using either
//   cc -Wall -O2 -D_use_zlib_=0 solution_speed_run.c -lm
// or
//   cc -Wall -O2 -D_use_zlib_=1 solution_speed_run.c -lm -lz
//
// Place your student numbers and names here
//   N.Mec. 102480  Name: Rúben Pequeno
//   N.Mec. 78106   Name: Tiago Ferreira
//   N.Mec. xxxxxx   Name: Vinicios Oliveira
//


//
// static configuration
//

#define _max_road_size_  800  // the maximum problem size
#define _min_road_speed_   2  // must not be smaller than 1, shouldnot be smaller than 2
#define _max_road_speed_   9  // must not be larger than 9 (only because of the PDF figure)


//
// include files --- as this is a small project, we include the PDF generation code directly from make_custom_pdf.c
//

#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include "../P02/elapsed_time.h"
#include "make_custom_pdf.c"


//
// road stuff
//

static int max_road_speed[1 + _max_road_size_]; // positions 0.._max_road_size_

static void init_road_speeds(void)
{
  double speed;
  int i;

  for(i = 0;i <= _max_road_size_;i++)
  {
    speed = (double)_max_road_speed_ * (0.55 + 0.30 * sin(0.11 * (double)i) + 0.10 * sin(0.17 * (double)i + 1.0) + 0.15 * sin(0.19 * (double)i));
    max_road_speed[i] = (int)floor(0.5 + speed) + (int)((unsigned int)random() % 3u) - 1;
    if(max_road_speed[i] < _min_road_speed_)
      max_road_speed[i] = _min_road_speed_;
    if(max_road_speed[i] > _max_road_speed_)
      max_road_speed[i] = _max_road_speed_;
  }
}


//
// description of a solution
//

typedef struct
{
  int n_moves;                         // the number of moves (the number of positions is one more than the number of moves)
  int positions[1 + _max_road_size_];  // the positions (the first one must be zero)
}
solution_t;

//
// the (very inefficient) recursive solution given to the students
//

static solution_t solution_1,solution_1_best;
static double solution_1_elapsed_time; // time it took to solve the problem
static unsigned long solution_1_count; // effort dispended solving the problem

static void solution_1_recursion(int move_number,int position,int speed,int final_position)
{
  int i,new_speed;

  // record move
  solution_1_count++;
  solution_1.positions[move_number] = position;
  // is it a solution?
  if(position == final_position && speed == 1)
  {
    // is it a better solution?
    if(move_number < solution_1_best.n_moves)
    {
      solution_1_best = solution_1;
      solution_1_best.n_moves = move_number;
    }
    return;
  }
  // no, try all legal speeds
  for(new_speed = speed - 1;new_speed <= speed + 1;new_speed++)
    if(new_speed >= 1 && new_speed <= _max_road_speed_ && position + new_speed <= final_position)
    {
      for(i = 0;i <= new_speed && new_speed <= max_road_speed[position + i];i++)
        ;
      if(i > new_speed)
        solution_1_recursion(move_number + 1,position + new_speed,new_speed,final_position);
    }
}

static void solve_1(int final_position)
{
  if(final_position < 1 || final_position > _max_road_size_)
  {
    fprintf(stderr,"solve_1: bad final_position\n");
    exit(1);
  }
  solution_1_elapsed_time = cpu_time();
  solution_1_count = 0ul;
  solution_1_best.n_moves = final_position + 100;
  solution_1_recursion(0,0,0,final_position);
  solution_1_elapsed_time = cpu_time() - solution_1_elapsed_time;
}

static solution_t solution_2,solution_2_best;
static double solution_2_elapsed_time; // time it took to solve the problem
static unsigned long solution_2_count; // effort dispended solving the problem

static bool no_execed_limit_speed (int position, int speed, int final_position) // Make sure you don't exceed the speed limit
{  
  int min = max_road_speed[position];

  for (int index = position+1; index <= position + speed && index <= final_position; index++)
  {
    if (max_road_speed[index] == _min_road_speed_)
    {
      min = _min_road_speed_;
      break;
    }
    
    if (min > max_road_speed[index])
    {
      min = max_road_speed[index];
    }    
  }
  return speed <= min; 
}

static bool no_execed_road_length (int position, int speed, int final_position) // checks that it does not exceed the limit of the road
{
  return position+speed <= final_position;
}

static void solution_2_recursion(int move_number,int position,int speed,int final_position)
{
  int new_speed;

  // record move
  solution_2_count++;
  solution_2.positions[move_number] = position;
  // is it a solution?
  if(position == final_position && speed == 1)
  {
    // is it a better solution?
    if(move_number < solution_2_best.n_moves)
    {
      solution_2_best = solution_2;
      solution_2_best.n_moves = move_number;
    }
    return;
  }  
  
  // restriction of some possibilities (the speed of final possition must be 1)
  
  if (speed*(speed + 1)/2 < final_position - position) 
  {
    for(new_speed = speed - 1;new_speed <= speed + 1;new_speed++)
    {
      if(new_speed >= 1 && new_speed <= _max_road_speed_ && no_execed_road_length(position,new_speed,final_position))
      {                 
        if(no_execed_limit_speed(position,new_speed,final_position))
        {              
          solution_2_recursion(move_number + 1,position + new_speed,new_speed,final_position);
        }             
      }
    }  
  }

  if (speed*(speed + 1)/2 == final_position - position)
  {
    for(new_speed = speed - 1;new_speed <= speed;new_speed++)
    {
      if(new_speed >= 1 && new_speed <= _max_road_speed_ && no_execed_road_length(position,new_speed,final_position))
      {                 
        if(no_execed_limit_speed(position,new_speed,final_position))
        {              
          solution_2_recursion(move_number + 1,position + new_speed,new_speed,final_position);
        }             
      }
    }  
  }

  if (speed*(speed + 1)/2 > final_position - position)
  {
    new_speed = speed - 1;
    
    if(new_speed >= 1 && new_speed <= _max_road_speed_ && no_execed_road_length(position,new_speed,final_position))
    {                 
      if(no_execed_limit_speed(position,new_speed,final_position))
      {              
        solution_2_recursion(move_number + 1,position + new_speed,new_speed,final_position);
      }             
    }      
  }    
}

static void solve_2(int final_position)
{   
  if(final_position < 1 || final_position > _max_road_size_)
  {
    fprintf(stderr,"solve_3: bad final_position\n");
    exit(1);
  }  
  solution_2_elapsed_time = cpu_time();
  solution_2_count = 0ul;
  solution_2_best.n_moves = final_position + 100;  
  solution_2_recursion(0,0,0,final_position);
  solution_2_elapsed_time = cpu_time() - solution_2_elapsed_time;
}

static solution_t solution_3;
static double solution_3_elapsed_time; // time it took to solve the problem
static unsigned long solution_3_count; // effort dispended solving the problem

static int distanceToStop(int speed){ // distance required to stop at a certain speed
  return speed*(speed+1)/2;  // for(speed; speed>0; speed--){sum+=speed}
}

static int verifyOverSpeed(int new_speed, int position){  // true if new speed goes over the road limit
  solution_3_count++;
  if(new_speed==0){
    return 0;
  }
  int i;
  for(i=0;i <= new_speed && new_speed <= max_road_speed[position + i];i++);
  if(i > new_speed){
    return verifyOverSpeed(new_speed-1, position + new_speed);  // check if it has time to stop before future road limits
  }
  return 1;
}

static void solution_3_iterative(int final_position)
{
  int speed = 0, position = 0, distance;
  for(position; position <= final_position; position+=speed){
    solution_3_count++;
    solution_3.positions[solution_3.n_moves++] = position;
    distance = distanceToStop(speed) + position - final_position;
    if (distance < -speed){ // try to increase speed
      speed++;  // start testing at speed+1
      for (int i=2; i>0; i--){
        if(verifyOverSpeed(speed, position)){  // decrease speed if it goes over speed limit
          speed--;
        } else{
          break;  // doesn't need to check slower speeds if higher is possible
        }
      }
    }
    else if(distance > 0 && speed>1){ // decrease speed because its reaching final position
      speed--;
    }
    else{ // mantain speed
      if (verifyOverSpeed(speed, position) && speed>1){  // decrease speed if it goes over speed limit
            speed--;
      }
    }
  }
  solution_3.n_moves--;
}

static void solve_3(int final_position)
{
  if(final_position < 1 || final_position > _max_road_size_)
  {
    fprintf(stderr,"solve_3: bad final_position\n");
    exit(1);
  }
  solution_3_elapsed_time = cpu_time();
  solution_3_count = 0ul;
  solution_3.n_moves = 0;
  solution_3_iterative(final_position);
  solution_3_elapsed_time = cpu_time() - solution_3_elapsed_time;
}

typedef struct data
{
  int position;
  int speed;
  int level;
}
data_t;

typedef struct node
{
  int position;
  int speed;
  int level;
  data_t prev;
}
node_t;

static data_t new_data(int position, int speed, int level)
{
  data_t n;
  n.position=position;
  n.speed=speed;
  n.level=level;

  return n;
}

static node_t new_node(int position, int speed, int level, data_t prev)
{
  node_t n;
  n.position=position;
  n.speed=speed;
  n.level=level;
  n.prev=prev;

  return n;
}

static bool no_execed_limit_speed4 (int position, int speed) //Make sure you don't exceed the speed limit
{  
  int min = max_road_speed[position];

  for (int index = position+1; index <= position + speed; index++)
  {
    if (max_road_speed[index] == _min_road_speed_)
    {
      min = _min_road_speed_;
      break;
    }
    
    if (min > max_road_speed[index])
    {
      min = max_road_speed[index];
    }    
  }
  return speed <= min; 
}

static void new_ramification(node_t children[],node_t *n) // create the children of a node
{    
  int new_speed;
  int index=0;
  bool cut = n->prev.speed != 1 || n->prev.position == 1; 

  if (n->speed==1 && n->position !=1)
  {
    if (no_execed_limit_speed4(n->position,n->speed) && cut)
    { 
      data_t data = new_data(n->position,n->speed,n->level);              
      children[index]=new_node(n->position+1,1,n->level+1,data);                
    }
  }
  else
  {
    for(new_speed = n->speed - 1;new_speed <= n->speed + 1;new_speed++)
    {
      if (new_speed >= 1 && new_speed <= _max_road_speed_)
      {          
        if (no_execed_limit_speed4(n->position,new_speed))
        { 
          data_t data = new_data(n->position,n->speed,n->level);                       
          children[index]=new_node(n->position+new_speed,new_speed,n->level+1,data);                
        }
      }      
      index++;
    }
  }           
}

int number_of_children(node_t line[], int size_line) // calculates the number of children of each node
{ 
  int n_childs = 0;
  node_t *ptr_line;
  for (int index = 0; index < size_line; index++)
  {
    ptr_line = &line[index];
    if (ptr_line->speed == 1 && ptr_line->prev.speed ==1)
    {
      if (ptr_line->prev.position == 1)
      {
        n_childs+=1;
      }
      else
      {
        n_childs+=0;
      }     
    } 
    else if (ptr_line->speed == 1 && ptr_line->prev.speed !=1)
    {
      n_childs+=1;
    }
    else
    {
      for(int new_speed = ptr_line->speed - 1;new_speed <= ptr_line->speed + 1;new_speed++)
      {
        if (new_speed >= 1 && new_speed <= _max_road_speed_)
        {          
          if (no_execed_limit_speed4(ptr_line->position,new_speed))
          { 
            n_childs+=1;                
          }
        } 
      }      
    }   
    
  }
  return n_childs;
}

void inic_line (node_t n[], int length)
{
  for (int i = 0; i < length; i++)
  {
    n[i].position=0;
    n[i].speed=0;
    n[i].level=0;
  }
}

void transp_values(node_t n1[],node_t n2[], int length)
{
  for (int i = 0; i < length; i++)
  {
    n1[i].level=n2[i].level;
    n1[i].position=n2[i].position;
    n1[i].speed=n2[i].speed;
    n1[i].prev=n2[i].prev;
  }    
}

void print_data (data_t n)
{  
  printf("%d,%d,%d",n.position,n.speed,n.level);  
}

void print_nodes (node_t n[], int size)
{
  for (int i = 0; i < size; i++)
  {
    printf("%d,%d,%d, (",n[i].position,n[i].speed,n[i].level);
    print_data(n[i].prev);
    printf(") -> ");
  }
  printf("\n");
}

void solution_4 (int s[], int last_position)
{
  last_position++;
  s = calloc(last_position,sizeof(int));    
  
  int level = 1;
  int size_line_n = 1;
  int size_line_n_1 = 2;
  node_t *line_n = malloc(size_line_n*sizeof(node_t));
  node_t *line_n_1 = malloc(size_line_n_1*sizeof(node_t));

      
  inic_line(line_n,size_line_n);
  inic_line(line_n_1,size_line_n_1);
  node_t children[3];
  data_t roat = new_data(0,0,0);    
  line_n[0]=new_node(1,1,1,roat);

  for (int i = 0; i < 3; i++)
  {
    children[i].position=0;
    children[i].speed=0;
    children[i].level=0;
    children[i].prev=roat;
  }  
  
  node_t *ptr_line_n;
  node_t *ptr_children;
  while (s[last_position-1] == 0)
  {   
    
    int index_n_1 = 0;    
    for (int index_n = 0; index_n < size_line_n; index_n++)
    {      
      ptr_line_n = &line_n[index_n];     
      
      if (ptr_line_n->position != 0 || level == 0)
      {        
        new_ramification(children,ptr_line_n);      

        for (int i = 0; i < 3; i++)
        {
          ptr_children=&children[i];
          if (ptr_children->position != 0)
          {             
            line_n_1[index_n_1]=children[i];
            index_n_1++;            
          }                   
        }

        for (int i = 0; i < 3; i++)
        {
          children[i].position=0;
          children[i].speed=0;
          children[i].level=0;
          children[i].prev=roat;
        }
                   
      }   
        
    }
    index_n_1 = 0;
    level++;           
    size_line_n = size_line_n_1;
    size_line_n_1 = number_of_children(line_n_1,size_line_n_1);    
    free(line_n);
    line_n=malloc((size_line_n+1)*sizeof(node_t));    
    transp_values(line_n,line_n_1,size_line_n);    
    free(line_n_1);
    line_n_1= malloc((size_line_n_1)*sizeof(node_t));
    inic_line(line_n_1,size_line_n_1);

    for (int index_n = 0; index_n < size_line_n; index_n++)
    {   
      ptr_line_n = &line_n[index_n];            
      if (ptr_line_n->speed == 1)
      {
        if (s[line_n[index_n].position] == 0)
        {
          s[line_n[index_n].position]=line_n[index_n].level;
        }            
      }          
    }

        
        
    ptr_line_n=NULL;
      
  }
  free(line_n);
  free(line_n_1);

  for (int i = 0; i < last_position; i++)
  {
    printf("Final Position:%d | Best Moves:%d\n",i,s[i]);
  }   
}


//
// example of the slides
//

static void example_1(void)
{
  int i,final_position;

  srandom(0xAED2022);
  init_road_speeds();
  final_position = 30;
  solve_3(final_position);
  make_custom_pdf_file("example1.pdf",final_position,&max_road_speed[0],solution_1_best.n_moves,&solution_1_best.positions[0],solution_1_elapsed_time,solution_2_count,"Plain recursion1");
  printf("mad road speeds:");
  for(i = 0;i <= final_position;i++)
    printf(" %d",max_road_speed[i]);
  printf("\n");
  printf("positions:");
  for(i = 0;i <= solution_1_best.n_moves;i++)
    printf(" %d",solution_1_best.positions[i]);
  printf("\n");
}

static void example_2(void)
{
  int i,final_position;

  srandom(0xAED2022);
  init_road_speeds();
  final_position = 30;
  solve_2(final_position);
  make_custom_pdf_file("example2.pdf",final_position,&max_road_speed[0],solution_2_best.n_moves,&solution_2_best.positions[0],solution_2_elapsed_time,solution_2_count,"Plain recursion2");
  printf("mad road speeds:");
  for(i = 0;i <= final_position;i++)
    printf(" %d",max_road_speed[i]);
  printf("\n");
  printf("positions:");
  for(i = 0;i <= solution_2_best.n_moves;i++)
    printf(" %d",solution_2_best.positions[i]);
  printf("\n");
}

static void example_3(void)
{
  int i,final_position;

  srandom(0xAED2022);
  init_road_speeds();
  final_position = 30;
  solve_3(final_position);
  make_custom_pdf_file("example3.pdf",final_position,&max_road_speed[0],solution_3.n_moves,&solution_3.positions[0],solution_3_elapsed_time,solution_3_count,"Iterative");
  printf("mad road speeds:");
  for(i = 0;i <= final_position;i++)
    printf(" %d",max_road_speed[i]);
  printf("\n");
  printf("positions:");
  for(i = 0;i <= solution_3.n_moves;i++)
    printf(" %d",solution_3.positions[i]);
  printf("\n");
}




//
// main program
//

int main(int argc,char *argv[argc + 1])
{
# define _time_limit_  3600.0
  int n_mec,final_position,print_this_one;
  char file_name[64];

  if(argc == 2){
    if(argv[1][0] == '-' && argv[1][1] == 'e' && argv[1][2] == 'x'){  // examples
      example_1();
      example_2();
      example_3();
      return 0;
    } else if (argv[1][0] == '-' && argv[1][1] == 'i'){ // run just iterative solution(3) since others are much slower
      n_mec = atoi(argv[1]);
      srandom((unsigned int)n_mec);
      init_road_speeds();
      // run all solution methods for all interesting sizes of the problem
      final_position = 1;
      solution_3_elapsed_time = 0.0;
      printf("    + --- ---------------- --------- +\n");
      printf("    |                plain recursion |\n");
      printf("--- + --- ---------------- --------- +\n");
      printf("  n | sol            count  cpu time |\n");
      printf("--- + --- ---------------- --------- +\n");
      while(final_position <= _max_road_size_)
      {
        print_this_one = (final_position == 10 || final_position == 20 || final_position == 50 || final_position == 100 || final_position == 200 || final_position == 400 || final_position == 800) ? 1 : 0;
        printf("%3d |",final_position);
        if(solution_3_elapsed_time < _time_limit_)
        {
          solve_3(final_position);
          if(print_this_one != 0)
          {
            sprintf(file_name,"%03d_3.pdf",final_position);
            make_custom_pdf_file(file_name,final_position,&max_road_speed[0],solution_3.n_moves,&solution_3.positions[0],solution_3_elapsed_time,solution_3_count,"Iterative");
          }
          printf(" %3d %16lu %9.3e |",solution_3.n_moves,solution_3_count,solution_3_elapsed_time);
        }
        else
        {
          solution_3.n_moves = -1;
          printf("                                |");
        }
        printf("\n");
        fflush(stdout);
        // new final_position
        if(final_position < 50)
          final_position += 1;
        else if(final_position < 100)
          final_position += 5;
        else if(final_position < 200)
          final_position += 10;
        else
          final_position += 20;
      }
      printf("--- + --- ---------------- --------- +\n");
      return 0;
        }
    
  }
  // generate the example data

  // initialization
  n_mec = (argc < 2) ? 0xAED2022 : atoi(argv[1]);
  srandom((unsigned int)n_mec);
  init_road_speeds();
  int aux1 = 0, aux2 = 0, aux3 = 0;
  // run all solution methods for all interesting sizes of the problem
  final_position = 1;
  solution_1_elapsed_time = 0.0;
  printf("    + --- ---------------- --------- + --- ---------------- --------- + --- ---------------- --------- +\n");
  printf("    |               Plain recursion1 |               Plain recursion2 |                Plain iterative |\n");
  printf("--- + --- ---------------- --------- + --- ---------------- --------- + --- ---------------- --------- +\n");
  printf("  n | sol            count  cpu time | sol            count  cpu time | sol            count  cpu time |\n");
  printf("--- + --- ---------------- --------- + --- ---------------- --------- + --- ---------------- --------- +\n");
  while(final_position <= _max_road_size_/* && final_position <= 20*/)
  {
    print_this_one = (final_position == 10 || final_position == 20 || final_position == 50 || final_position == 100 || final_position == 200 || final_position == 400 || final_position == 800) ? 1 : 0;
    printf("%3d |",final_position);
    // first solution method (very bad)
    if(solution_1_elapsed_time < _time_limit_ && aux1 == 0)
    {
      solve_1(final_position);
      if(print_this_one != 0)
      {
        sprintf(file_name,"%03d_1.pdf",final_position);
        make_custom_pdf_file(file_name,final_position,&max_road_speed[0],solution_1_best.n_moves,&solution_1_best.positions[0],solution_1_elapsed_time,solution_1_count,"Plain recursion 1");
      }
      printf(" %3d %16lu %9.3e |",solution_1_best.n_moves,solution_1_count,solution_1_elapsed_time);
    }
    else
    {
      aux1+=1;
      solution_1_best.n_moves = -1;
      printf("                                |");
    }
    // second solution method (less bad)
    
    if(solution_2_elapsed_time < _time_limit_ && aux2 == 0)
    {
      solve_2(final_position);
      if(print_this_one != 0)
      {
        sprintf(file_name,"%03d_2.pdf",final_position);
        make_custom_pdf_file(file_name,final_position,&max_road_speed[0],solution_2_best.n_moves,&solution_2_best.positions[0],solution_2_elapsed_time,solution_2_count,"Plain recursion 2");
      }
      printf(" %3d %16lu %9.3e |",solution_2_best.n_moves,solution_2_count,solution_2_elapsed_time);
    }
    else
    {
      aux2+=1;
      solution_2_best.n_moves = -1;
      printf("                                |");
    }

    if(solution_3_elapsed_time < _time_limit_ && aux3 == 0)
    {
      solve_3(final_position);
      if(print_this_one != 0)
      {
        sprintf(file_name,"%03d_3.pdf",final_position);
        make_custom_pdf_file(file_name,final_position,&max_road_speed[0],solution_3.n_moves,&solution_3.positions[0],solution_3_elapsed_time,solution_3_count,"Iterative");
      }
      printf(" %3d %16lu %9.3e |",solution_3.n_moves,solution_3_count,solution_3_elapsed_time);
    }
    else
    {
      aux3+=1;
      solution_3.n_moves = -1;
      printf("                                |");
    }

    // done
    printf("\n");
    fflush(stdout);
    // new final_position
    if(final_position < 50)
      final_position += 1;
    else if(final_position < 100)
      final_position += 5;
    else if(final_position < 200)
      final_position += 10;
    else
      final_position += 20;
  }
  printf("--- +");
  for(int i = 0; i<3; i++){ // i < number of solutions
    printf(" --- ---------------- --------- +");
  }
  printf("\n");
  // failed attempt
  //solution_4(100);
  return 0;
# undef _time_limit_
}
