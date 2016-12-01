#include <cstdlib>
#include <iostream>
#include <cerrno>
#include <unistd.h>
using namespace std;
#include "mpi.h"

int main ( int argc, char *argv[] ) 
{
  int msgOut;
  int msgIn;
  int runtime = 10;
  bool send_signal = false;
  int id;
  int p;  
  MPI::Status status;
  int tag = 1;
  //  Initialize MPI.
  MPI::Init ( argc, argv );
  //  Get the number of processes.
  p = MPI::COMM_WORLD.Get_size ( );
  //  Determine the rank of this process.
  id = MPI::COMM_WORLD.Get_rank ( );
  
  int fork_layout[p-1];
  for(int i = 0; i < p-1; i++)
  {
	  fork_layout[i] = 1;	  
  }  
  //Prints the fork array.
  std::cout << "Available Forks: ";
  for(int i = 0; i < p-1; i++)
  {
	  std::cout << fork_layout[i] << " ";
  }
  std::cout <<std::endl;  
  //Safety check - need at least 2 philosophers to make sense
  if (p < 3) 
  {
	    MPI::Finalize ( );
	    std::cerr << "Need at least 2 philosophers! Try again" << std::endl;
	    return 1; //non-normal exit
  }
  srand(id + time(NULL)); //ensure different seeds...  
  while( runtime > 0)
  {
	  //  Setup Fork Master (Ombudsman) and Philosophers
	  if ( id == 0 ) //Master
	  {		  
		//let the philosophers check in
		for (int i = 1; i < p; i++) 
		{
			MPI::COMM_WORLD.Recv ( &msgIn, 1, MPI::INT, MPI::ANY_SOURCE, tag, status );
			if(msgIn == 1)
			{
				if(fork_layout[status.Get_source() - 1] == 1 && fork_layout[status.Get_source() % (p - 1)] == 1)
				{
					fork_layout[status.Get_source() - 1] = 0;
					fork_layout[status.Get_source() %(p - 1)] = 0;					
					msgOut = 2;	//eat.
					sleep(1);
					MPI::COMM_WORLD.Send ( &msgOut, 1, MPI::INT, 0, tag );
					send_signal = true;
					std::cout << "TEST 3" <<std::endl;						
					std::cout << "Master is commanding to eat. Message: " << msgOut << " To Philosopher ";
					std::cout << status.Get_source() << std::endl;	
					
					std::cout << "Available Forks: ";
					  for(int i = 0; i < p-1; i++)
					  {
						  std::cout << fork_layout[i] << " ";
					  }
					  std::cout <<std::endl;
				}
				else
				{
					msgOut = 4;
					sleep(1);
					std::cout << "TEST 4" <<std::endl;					
					std::cout << "Master is commanding to wait. Message: " << msgOut << " To Philosopher ";
					std::cout << status.Get_source() << std::endl;
					MPI::COMM_WORLD.Send ( &msgOut, 1, MPI::INT, 0, tag );
					send_signal = true;					
					std::cout << "Available Forks: ";
					  for(int i = 0; i < p-1; i++)
					  {
						  std::cout << fork_layout[i] << " ";
					  }
					  std::cout <<std::endl;
				}
			}
			else if(msgIn == 3)
			{
				fork_layout[status.Get_source() - 1] = 1;
				fork_layout[status.Get_source() %(p - 1)] = 1;				
				std::cout << "Available Forks: ";
				for(int i = 0; i < p-1; i++)
				{
				  std::cout << fork_layout[i] << " ";
				}
				std::cout <<std::endl;
			}
		}
	  }
	  else //I'm a philosopher
	  {
		  std::cout << "check1" << std::endl;
		  if(send_signal == true)
		  {
			  send_signal = false;
			  std::cout << "check2" << std::endl;
		  }
		  std::cout << "check3" << std::endl;	
		///pick a number between 0 and the number of processes (one more than # of philosophers)
		///int msgOut = rand() % p; 		
		///getsource() % (p-1)
		/*if(start == true)
		{			
			msgOut = 1; //hungry
			sleep(1);
			MPI::COMM_WORLD.Send ( &msgOut, 1, MPI::INT, 0, tag );
			std::cout << "Philospher:  " << id << " - hungry. Message: " << msgOut << std::endl;	
			///std::cout << "TEST " <<std::endl;			
			start = false;
		}*/
			msgOut = 1; //hungry
			sleep(1);
			std::cout << "Philospher:  " << id << " - hungry. Message: " << msgOut << std::endl;	
			MPI::COMM_WORLD.Send ( &msgOut, 1, MPI::INT, 0, tag );
			sleep(1);
			std::cout << "TEST 1" <<std::endl;	
			MPI::COMM_WORLD.Recv ( &msgIn, 1, MPI::INT, MPI::ANY_SOURCE, tag, status );			
			std::cout << "TEST 2 " <<std::endl;				
			if(msgIn == 4)
			{ 
				std::cout << "Philospher:  " << id << "- wait heard. Message: " << msgOut << std::endl;
			}
			else if(msgIn == 2)
			{
				std::cout << "Philospher:  " << id << "- wait heard. Message: " << msgOut << std::endl;
				//response_wait = false;
			}	
//***		//  MPI::COMM_WORLD.Recv ( &msgIn, 1, MPI::INT, MPI::ANY_SOURCE, tag, status );
			msgOut = 3; 
			sleep(1);
			//MPI::COMM_WORLD.Send ( &msgOut, 1, MPI::INT, 0, tag );
			std::cout << "Philospher:  " << id << "- wait heard. Message: " << msgOut << std::endl;
			MPI::COMM_WORLD.Send ( &msgOut, 1, MPI::INT, 0, tag );
	   }		
		//std::cout << "Philospher:  " << id << " - sending Message: " << msgOut << std::endl;		
		//check in with master - send a random number (node 0 = master)
		MPI::COMM_WORLD.Send ( &msgOut, 1, MPI::INT, 0, tag ); 
	  }	  
	  runtime--;
  //}  
  //  Terminate MPI.
  MPI::Finalize ( );
  return 0;
}
