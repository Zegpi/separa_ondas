#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <math.h>
#include <time.h>


/*Este código evoluciona una nueva red neuronal a partir de 2 redes existentes (en desarrollo)*/
int main(void)
{

//Variables
	int cont,num_redes_entrada,a,b,c,N1,N2,*n1,*n2,neuronas_capa_inicial,neuronas_capa_anterior,c1,c2,c_nuevo;
	float ***w1,***w2,***r1,***r2,***red_larga;
	DIR *carpeta;
	FILE *fid_red1,*fid_red2;
	size_t num_leidos;
	struct dirent *lista_archivo;
	char **archivos,ruta[256],nombre_red_1[256],nombre_red_2[256];
//

//Cargamos todos los nombres en la carpeta de redes
	//Revisamos la carpeta con los inputs

	sprintf(ruta,"%s","./redes");

	carpeta = opendir(ruta);
	if (carpeta == NULL)  //Es null sólo si falla (la carpeta no existe, está bloqueada, etc.)
	{
		printf("No se pudo abrir la carpeta seleccionada");
		return 0;
	}

	//Enumeramos los archivos en la carpeta
	cont=0;
	while (readdir(carpeta) != NULL)	//readdir es como fread, lee linea por linea hasta el final
	{
		cont++;
	}
	closedir(carpeta);
	num_redes_entrada=cont-2;			//Eliminamos 2 porque readdir pesca . y ..

	archivos=calloc(num_redes_entrada,sizeof(char*));
	for(int i=0;i<num_redes_entrada;i++)
	{
		archivos[i]=calloc(128,sizeof(char));				//Muy dificil que la ruta de un archivo tenga más de 128 caracteres, pero tener ojo igual
	}

	//Abrimos de nuevo, para empezar desde el principio
	carpeta = opendir(ruta);
	cont=0;
	while ((lista_archivo = readdir(carpeta)) != NULL)
	{
		if(cont>1)	//Esto se salta . y ..
		{
			sprintf(archivos[cont-2],"%s",lista_archivo->d_name);		//Si no hago sprintf aqui, archivo queda con punteros a los archivos, que despues son eliminados por closedir
			//archivos[cont-2]=lista_archivo->d_name;
		}
		cont++;
	}
	closedir(carpeta);		//Ahora "archivos" tiene el nombre de todos los archivos con ondas
	/*
	for (int i=0;i<num_redes_entrada;i++)
	{
		printf("El archivos número %d es %s \n",i+1,archivos[i]);
	}
	*/
//

//Seleccionamos 2 archivos al azar
	srand(time(NULL));					//Semilla del numero aleatorio usando el tiempo actual, con precision al segundo (no usar esto en fors, los numeros se repiten)

	a=(int)( ((double) rand() / RAND_MAX) * (num_redes_entrada) );
	b=(int)( ((double) rand() / RAND_MAX) * (num_redes_entrada) );

	while (b==a)
	{
		b=(int)( ((double) rand() / RAND_MAX) * (num_redes_entrada) );			//A veces a y b son iguales, corregimos eso
	}

	c=(a>b) ? b:a;	b=(a>b) ? a:b;	a=c;		//Aquí hacemos que a y b esten ordenados de menor a mayor
												//El operador ? se escribe asi (condicion) ? X:Y y significa: si (condicion en parentesis) es verdadera ? entonces X: si no, Y  
	//Generé 20000 de estos pares e hice un histograma, la distribución era casi perfectamente uniforme y no se sale de los límites.

	printf("Seleccionamos las redes %s y %s \n",archivos[a],archivos[b]);

	sprintf(nombre_red_1,"./redes/%s",archivos[a]);
	sprintf(nombre_red_2,"./redes/%s",archivos[b]);
//

//Abrimos y cargamos las dos redes
	//Abrimos el archivo que contiene la red neuronal
	fid_red1=fopen(nombre_red_1,"r");
	fid_red2=fopen(nombre_red_2,"r");

	//Leemos el número de capas y el número de neuronas por capa de ambas redes
	num_leidos=fread(&N1,sizeof(int),1,fid_red1);
	num_leidos=fread(&N2,sizeof(int),1,fid_red2);
	
	n1=calloc(N1,sizeof(int));
	for(int i=0;i<N1;i++)
	{
		num_leidos=fread(&n1[i],sizeof(int),1,fid_red1);
	}
	
	n2=calloc(N2,sizeof(int));
	for(int i=0;i<N2;i++)
	{
		num_leidos=fread(&n2[i],sizeof(int),1,fid_red2);
	}

	//Creamos arreglos para guardar los pesos
	neuronas_capa_inicial=20001;			//Num inputs, siempre debe ser 20001 a menos que se esté haciendo debug
	
	neuronas_capa_anterior=neuronas_capa_inicial;
	w1=calloc(N1,sizeof(float **));
	for(int i=0;i<N1;i++)
	{
		w1[i]=calloc(n1[i],sizeof(float *));

		for(int j=0;j<n1[i];j++)
		{
			w1[i][j]=calloc(neuronas_capa_anterior+1,sizeof(float));
		}
		neuronas_capa_anterior=n1[i];
	}

	neuronas_capa_anterior=neuronas_capa_inicial;
	w2=calloc(N2,sizeof(float **));
	for(int i=0;i<N2;i++)
	{
		w2[i]=calloc(n2[i],sizeof(float *));

		for(int j=0;j<n2[i];j++)
		{
			w2[i][j]=calloc(neuronas_capa_anterior+1,sizeof(float));
		}
		neuronas_capa_anterior=n2[i];
	}

	//Ahora leemos los archivos y los ponemos en w1 y w2
	neuronas_capa_anterior=neuronas_capa_inicial;
	for(int i=0;i<N1;i++)
	{
		for(int j=0;j<n1[i];j++)
		{
			for(int k=0;k<neuronas_capa_anterior+1;k++)
			{
				num_leidos=fread(&w1[i][j][k],sizeof(float),1,fid_red1);
			}
		}
		neuronas_capa_anterior=n1[i];
	}

	neuronas_capa_anterior=neuronas_capa_inicial;
	for(int i=0;i<N2;i++)
	{
		for(int j=0;j<n2[i];j++)
		{
			for(int k=0;k<neuronas_capa_anterior+1;k++)
			{
				num_leidos=fread(&w2[i][j][k],sizeof(float),1,fid_red2);
			}
		}
		neuronas_capa_anterior=n2[i];
	}

	fclose(fid_red1);
	fclose(fid_red2);				//La red neuronal 1 está en w1[i][j][k] y la red neuronal 2 está en w2[i][j][k]
	//Imprimí los valores en consola y estan bien leidos y cargados
//

//Combinamos ambas redes
	
	//Partimos del supuesto que N1==N2, si eso no se cumple, la cosa no anda.
	if(N1!=N2)
	{
		printf("El número de capas de ambas redes no es igual, este caso no se considera \n");
		return -1;
	}
	else
	{
		neuronas_capa_anterior=neuronas_capa_inicial;					//La capa inicial siempre tiene el mismo largo
		for(int i=0;i<N1;i++)		//Ciclo sobre las capas
		{
			//Determinamos que red tiene menos neuronas
			if(n1[i]>n2[i])
			{
				r1=w2;				//R1 es la red con menor número de neuronas.
				r2=w1;
				c1=n2[i];			//Número de neuronas en la red con menos neuronas.
				c2=n1[i];			//Número de neuronas en la red con más neuronas.
			}
			else
			{
				r1=w1;
				r2=w2;
				c1=n1[i];
				c2=n2[i];
			}
			c2=c1+7;
			c_nuevo=c1+ (int) ((c2-c1)*( (double) rand()/RAND_MAX ));

			printf("Para la capa %d las redes tenían %d y %d neuronas y la nueva red tendrá %d neuronas \n",i,c1,c2,c_nuevo);


		}

		
	}
	

//


	
return 0;
}