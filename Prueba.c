#include <stdio.h>
#include <math.h>

#define ConstPi 3.14159265358979323846

//static char help[] = "Genera una onda sinusoidal con crecimiento y terminación suaves, con un comienzo, duración, amplitud y frecuencia dadas. \n La onda está expresada en un dominio de 500 [s] de largo y muestreada a 100 [Hz].\n\n";

/*
	Include "petscksp.h" so that we can use KSP solvers.  Note that this file
	automatically includes:
		 petscsys.h    - base PETSc routines   petscvec.h - vectors
		 petscmat.h    - matrices              petscpc.h  - preconditioners
		 petscis.h     - index sets
		 petscviewer.h - viewers

	Note:  The corresponding parallel example is ex23.c
*/
#include <petscksp.h>

//Generación de onda
	#undef  __FUNCT__
	#define __FUNCT__ "onda"
	PetscErrorCode onda(PetscReal inicio,PetscReal duracion,PetscReal amplitud,PetscReal freq,PetscReal *y,PetscInt largo,PetscInt frec_muest)
	{
		PetscErrorCode ierrF = 0;
		PetscInt i;
		PetscReal t;

		if (inicio<1.0 || inicio >499.0)
		{
			PetscPrintf(PETSC_COMM_WORLD,"Inicio no puede ser menor a 1.0 o mayor a 499.0  \n");
			ierrF=1;
			return ierrF;
		}

		for(i=0; i<largo; i++)
		{
			t=(double) i/frec_muest;

			if(t<inicio)
			{
				y[i]=0.0;							//El arreglo creado con calloc debería ser zeros, pero mantenemos esto por si se usa malloc
			}
			else if(t<(inicio+duracion/20.0))
			{
				y[i]=(t-inicio)/(duracion/20.0)*amplitud*sin(2*ConstPi*freq*t);
			}
			else if(t<(inicio+duracion-duracion/20.0))
			{
				y[i]=amplitud*sin(2*ConstPi*freq*t);
			}
			else if(t<(inicio+duracion))
			{
				y[i]=(1.0-(t-(inicio+duracion-duracion/20.0))/(duracion/20.0))*amplitud*sin(2*ConstPi*freq*t);;
			}
			else
			{
				y[i]=0.0;
			}

			//y[i]=amplitud*sin(2*ConstPi*freq*t);
		}
		return ierrF;
	}
//


#undef __FUNCT__
#define __FUNCT__ "main"
int main(int argc, char *argv[])
{
	PetscErrorCode  ierr;
	ierr = PetscInitialize(&argc,&argv,0,0);CHKERRQ(ierr);                    //Always initialize PETSc
	PetscPrintf(PETSC_COMM_WORLD,"Prueba de la función de generación de señal \n");

	PetscInt commsize,rank;
	ierr = MPI_Comm_size(PETSC_COMM_WORLD,&commsize);CHKERRQ(ierr);
	ierr = MPI_Comm_rank(PETSC_COMM_WORLD,&rank);CHKERRQ(ierr);

	FILE *fp;
	PetscReal t_tot,frec_muest,*y;
	PetscInt largo;

	t_tot = 500.0;														//Dominio total de la señal
	frec_muest = 100.0;													//Frecuencia de muestreo de la señal

	largo = round(t_tot*frec_muest)+1;									//Numero total de puntos de muestreo. Se suma uno para considerar el punto inicial, por ejemplo, 1 segundo a 5 [Hz] es [0.0 0.2 0.4 0.6 0.8 1.0] = 6 puntos

	ierr = PetscCalloc1(largo,&y);CHKERRQ(ierr);

		 //onda(inicio,duracion,amplitud,freq,*y,largo,frec_muest)
	ierr = onda(10.0,200.0,2.5,5.0,y,largo,frec_muest);CHKERRQ(ierr);

	ierr = PetscFOpen(PETSC_COMM_WORLD,"onda.txt","w",&fp);CHKERRQ(ierr);

	for (int i=0; i<largo; i++)
	{
		 ierr = PetscFPrintf(PETSC_COMM_WORLD,fp,"%.8e \n",y[i]);CHKERRQ(ierr);
	}

	ierr = PetscFClose(PETSC_COMM_WORLD,fp);CHKERRQ(ierr);
	
	ierr = PetscFinalize();
	return ierr;
}
