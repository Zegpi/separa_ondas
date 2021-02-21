/*  C  */

#include <stdio.h>
#include <mpi.h>
#include <dirent.h>
#include <math.h>
#include <time.h>
#include <unistd.h>

int main (int argc, char *argv[]) {

	/* MPI Parameters */
	int rank, size, len, ierr, i, j;
	char name[MPI_MAX_PROCESSOR_NAME];

	/* All ranks initiate the message-passing environment. */
	/* Each rank obtains information about itself and its environment. */
	MPI_Init(&argc, &argv);                 /* start MPI           */
	MPI_Comm_size(MPI_COMM_WORLD, &size);   /* get number of ranks */
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);   /* get rank            */
	MPI_Get_processor_name(name, &len);     /* get run-host name   */



	if(rank==0)
	{
		printf("Soy el nodo 0, el numero total de nodos es %d \n",size);

		//Aqui enviaremos los datos
		//(Posible) Estructura
		//while(true) //Pq la cosa debe correr hasta el infinito o hasta que yo lo corte
		//bool enviando[size-1],trabajando[size-1]
		//
		//for(i=1:size-1)   //i empieza de 1 pq no se le manda nada al nodo 0
		//if enviando[i]==0 && trabajando[i]==0
		//{Elegir que enviar al proceso i (esto será todo un tema)
		//MPI_Isend(array,i, etc)
		//enviando[i]=1;
		//}
		//Ver si podemos usar sleep(1min) aqui o si sleep detiene el envío
		//Ahora revisamos si terminamos de enviar
		//
		//for (i=1:size-1)
		//if enviando[i]==1 && trabajando[i] ==0
		//{
		//MPI_TEST(request[i],flag)
		//if flag==true
		//enviando[i]=0; trabajando[i]=1;
		//
		//
		//

		int repeticiones=0, *enviando, *trabajando, *envio;
		float *datos;
		MPI_Request *mpireq;
		MPI_Status *estado;

		ierr = MPI_Alloc_mem(sizeof(int)*(size-1),MPI_INFO_NULL,&enviando);
		ierr = MPI_Alloc_mem(sizeof(int)*(size-1),MPI_INFO_NULL,&trabajando);
		ierr = MPI_Alloc_mem(sizeof(int)*(size-1),MPI_INFO_NULL,&envio);
		ierr = MPI_Alloc_mem(sizeof(float)*(10),MPI_INFO_NULL,&datos);
		ierr = MPI_Alloc_mem(sizeof(MPI_Request)*(size-1),MPI_INFO_NULL,&mpireq);
		ierr = MPI_Alloc_mem(sizeof(MPI_Status)*(size-1),MPI_INFO_NULL,&estado);

		//Crear arreglos para guardar el estado de trabajo y envio
		for(i=0;i<size-1;i++)
		{
			enviando[i]=0;
			trabajando[i]=0;
		}

		while(repeticiones<100)
		{
			for(i=1;i<size;i++)
			{
				if(enviando[i-1]==0 && trabajando[i-1]==0)
				{
					for(j=0;j<10;j++)
					{
						datos[j]=(float) repeticiones + (float) i;
					}
					// int MPI_Isend(void *buffer,int count,MPI_Datatype type,int dest,int tag,MPI_Communicator comm,MPI_Request *request);
					ierr = MPI_Isend(datos,10,MPI_FLOAT,i,0,MPI_COMM_WORLD,&mpireq[i-1]);
					enviando[i-1]=1;
				}

				if(enviando[i-1]==1)
				{
					//int MPI_Test(MPI_Request *request, int *flag, MPI_Status *status);
					ierr = MPI_Test(&mpireq[i-1],&envio[i-1],&estado[i-1]);

					if(envio[i-1]!=0)
					{
						enviando[i-1]=0;
						trabajando[i-1]=1;
					}
				}
			}
			repeticiones++;

			sleep(1);

			if(repeticiones%10==0)
			{
				printf("\n\n");
				for(i=0;i<size-1;i++)
				{
				enviando[i]=0;
				trabajando[i]=0;
				}
			}
		}
	}
	else
	{
		//Aqui recibimos los datos
		//
		//MPI_Irecv(args)
		//MPI_Wait(args)
		//
		//Procesamos una AI (por ahora se modela como sleep(rand(segundos entre 1 y 2 minutos))
		//
		//Evaluamos la AI
		//Enviamos el resultado al nodo 0
		//Esperamos si el nodo 0 nos dice si nuestra AI queda en la tabla de posiciones o no

		while(true)		//Esperar nuevos datos siempre	
		{
			float *datos;
			ierr = MPI_Alloc_mem(sizeof(float)*(10),MPI_INFO_NULL,&datos);
			MPI_Request req;
			MPI_Status estado;

			// int MPI_Irecv(void *buffer, int count, MPI_Datatype datatype, int source, int tag, MPI_Communicator comm, MPI_Request *request);
			ierr = MPI_Irecv(datos,10,MPI_FLOAT,0,MPI_ANY_TAG,MPI_COMM_WORLD,&req);
			ierr = MPI_Wait(&req,&estado);

			printf("Soy el nodo %d y recibi el arreglo [",rank);
			for(i=0;i<10;i++)
			{
				printf(" %f ",datos[i]);
			}
			printf("] \n");

			if(rank==2)
			{
				sleep(25);
			}
			else
			{
				sleep(0.5);
			} 
		}
	}

	//printf("Runhost:%s   Rank:%d of %d ranks   hello, world\n", name,rank,size);

	MPI_Finalize();                         /* terminate MPI       */
	return 0;
}