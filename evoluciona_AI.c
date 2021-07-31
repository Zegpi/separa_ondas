#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <math.h>
#include <time.h>
#include <limits.h>

int MinInt(int a[],int num)
{
	int min_a=INT_MAX;

	for(int i=0;i<num;i++)
	{
		if(a[i]<min_a)
		{
			min_a=a[i];
		}
	}

	return min_a;
}

int MaxInt(int a[],int num)
{
	int max_a=INT_MIN;

	for(int i=0;i<num;i++)
	{
		if(a[i]>max_a)
		{
			max_a=a[i];
		}
	}

	return max_a;
}

/*Este código evoluciona una nueva red neuronal a partir de 2 redes existentes (en desarrollo)*/
int main(void)
{

//Variables
	int cont,num_redes_entrada,a,b,c,N1,N2,*n1,*n2,nodos_capa_inicial,nodos_capa_anterior1,nodos_capa_anterior2,nodos_capa_anterior_nueva,c1,c2,,d1,d2,c_nuevo,delta_capa,limites[3],
		lim_c1,lim_c2,lim_d1,lim_d2;
	float ***w1,***w2,***w_nueva,***r1,***r2,***red_larga;
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

	//Leemos el número de capas y el número de nodos por capa de ambas redes
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
	nodos_capa_inicial=20001;			//Num inputs, siempre debe ser 20001 a menos que se esté haciendo debug
	
	nodos_capa_anterior1=nodos_capa_inicial;
	w1=calloc(N1,sizeof(float **));
	for(int i=0;i<N1;i++)
	{
		w1[i]=calloc(n1[i],sizeof(float *));

		for(int j=0;j<n1[i];j++)
		{
			w1[i][j]=calloc(nodos_capa_anterior1+1,sizeof(float));
		}
		nodos_capa_anterior1=n1[i];
	}

	nodos_capa_anterior1=nodos_capa_inicial;
	w2=calloc(N2,sizeof(float **));
	for(int i=0;i<N2;i++)
	{
		w2[i]=calloc(n2[i],sizeof(float *));

		for(int j=0;j<n2[i];j++)
		{
			w2[i][j]=calloc(nodos_capa_anterior1+1,sizeof(float));
		}
		nodos_capa_anterior1=n2[i];
	}

	//Ahora leemos los archivos y los ponemos en w1 y w2
	nodos_capa_anterior1=nodos_capa_inicial;
	for(int i=0;i<N1;i++)
	{
		for(int j=0;j<n1[i];j++)
		{
			for(int k=0;k<nodos_capa_anterior1+1;k++)
			{
				num_leidos=fread(&w1[i][j][k],sizeof(float),1,fid_red1);
			}
		}
		nodos_capa_anterior1=n1[i];
	}

	nodos_capa_anterior1=nodos_capa_inicial;
	for(int i=0;i<N2;i++)
	{
		for(int j=0;j<n2[i];j++)
		{
			for(int k=0;k<nodos_capa_anterior1+1;k++)
			{
				num_leidos=fread(&w2[i][j][k],sizeof(float),1,fid_red2);
			}
		}
		nodos_capa_anterior1=n2[i];
	}

	fclose(fid_red1);
	fclose(fid_red2);				//La red neuronal 1 está en w1[i][j][k] y la red neuronal 2 está en w2[i][j][k]
	//Imprimí los valores en consola y estan bien leidos y cargados
//

//Combinamos ambas redes
	
	//Creamos arreglo para guardar la nueva red
	w_nueva=calloc(N1,sizeof(float **));

	//Partimos del supuesto que N1==N2, si eso no se cumple, la cosa no anda.
	if(N1!=N2)
	{
		printf("El número de capas de ambas redes no es igual, este caso no se considera \n");
		return -1;
	}
	else
	{
		//Sabemos cuantos nodos tiene la capa anterior, porque el número de nodos de entrada es constante
		nodos_capa_anterior1=nodos_capa_inicial;
		nodos_capa_anterior2=nodos_capa_inicial+1000;
		nodos_capa_anterior_nueva=nodos_capa_inicial+500;
		
		for(int i=0;i<N1;i++)		//Hacemos un ciclo sobre las capas
		{
			//Determinamos que red tiene menos nodos en la capa actual
			if(n1[i]>n2[i])
			{
				r1=w2;						//r1 es la red con menor número de nodos.
				r2=w1;						//r2 es la red con más nodos
				c1=n2[i];					//Número de nodos en la red con menos nodos.
				c2=n1[i];					//Número de nodos en la red con más nodos.
				d1=nodos_capa_anterior2;
				d2=nodos_capa_anterior1;
			}
			else
			{
				r1=w1;				//idem a sección anterior
				r2=w2;
				c1=n1[i];
				c2=n2[i];
				d1=nodos_capa_anterior1;
				d2=nodos_capa_anterior2;
			}
			
			//Elegimos cuantos nodos tendrá la nueva red en la capa actual
			c2=c2+6;		//BORRAR ESTO!!!
			c_nuevo=c1 + (int) ((c2-c1+1)*( (double) rand()/RAND_MAX ));						//Probé esto con 30000 corridas y genera valores homogeneamente distribuidos entre c1 y c2, técnicamente
																							//podría generar el valor c2+1 si rand() es exactamente igual a RAND_MAX, pero la posibilidad es ínfima. 

			//Aleatoriamente podemos añadir o quitar un nodo 
			if( ((double)rand()/RAND_MAX)>=0.97 )
			{
				delta_capa=1;
			}
			else if( ((double)rand()/RAND_MAX)<=0.02 )
			{
				delta_capa=-1;
			}
			else
			{
				delta_capa=0;																//Generé 300000 de estos y aproximadamente el 2% son 1 y el 2% son -1, así que está ok.
			}
			c_nuevo=c_nuevo+delta_capa;
			//printf("Para la capa %d las redes tenían %d y %d nodos y la nueva red tendrá %d nodos \n",i,c1,c2,c_nuevo);

			//Ya sabemos cuantos nodos tiene la capa actual, creamos el arreglo para contenerla
			w_nueva[i]=calloc(c_nuevo,sizeof(float *));
			for(int j=0;j<c_nuevo;j++)
			{
				w_nueva[i][j]=calloc(nodos_capa_anterior_nueva+1,sizeof(float));
			}
			
			//Determinamos los límites de los ciclos
			//Límites para las filas
			limites[0]=c1;	limites[1]=c_nuevo;
			lim_c1=MinInt(limites,2);

			limites[0]=c2;	limites[1]=c_nuevo;
			lim_c2=MinInt(limites,2);

			//Límites para las columnas
			limites[0]=nodos_capa_anterior1;	limites[1]=nodos_capa_anterior2;	limites[2]=nodos_capa_anterior_nueva;
			lim_d1=MinInt(limites,3);

			limites[0]=nodos_capa_anterior1;	limites[1]=nodos_capa_anterior2;	limites[2]=MaxInt(limites,2);
			limites[0]=nodos_capa_anterior_nueva;	limites[1]=limites[2];
			lim_d2=MinInt(limites,2);

			
			
			printf("c1=%d, c2=%d, c_nuevo=%d, d1=%d, d2=%d, d_nuevo=%d \n",c1,c2,c_nuevo,nodos_capa_anterior1,nodos_capa_anterior2,nodos_capa_anterior_nueva);
			printf("Los limites para la capa %d son lim_c1=%d, lim_c2=%d, lim_d1=%d, lim_d2=%d \n",i,lim_c1,lim_c2,lim_d1,lim_d2);

			for(int k1=0;k1<lim_c1;k1++)
			{
				
			}

			//Los nodos de la capa actual se convierten en los nodos de la capa anterior
			nodos_capa_anterior1=n1[i];
			nodos_capa_anterior2=n2[i];
			nodos_capa_anterior_nueva=c_nuevo;
				
		}
	}
	

//


	
return 0;
}