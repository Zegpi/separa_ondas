Para comenzar a usar

En WSL hacer "echo 0 | sudo tee /proc/sys/kernel/yama/ptrace_scope" para configurar el acceso de memoria como le gusta a MPI.

Suponiendo que el código está en Prueba1.c hacer

"mpicxx Prueba1.c -o Prueba1" para compilar y luego

"mpirun -np 6 ./Prueba1" para correr usando 6 procesadores (cambiar el numero de acuerdo al computador)
