#Introducción

Vamos a describir como podemos construir un contenedor para alojar un programa C++ de forma segura. Las enseñanzas son extrapolables a otro tipo de servicios.

Veremos lo siguiente:

- Imagen para compilar. Crearemos una imagen para compilar programas C++. La imagen tendra todas las herramientas y librerias necesarias para crear nuestra aplicación
- Crear un contenedor
- Crear un contenedor "super ligero"
- Seguridad

## Imagen para compilar

Creamos la imagen para construir nuestros programas c++:

```ps
docker build . -t cpp-build-base:0.1.0
```

Se crean la imagen de ubuntu y nuestra imagen para compilar programas c++:

```ps
docker images

REPOSITORY          TAG                 IMAGE ID            CREATED             SIZE
cpp-build-base      0.1.0               64e09874cfc0        17 minutes ago      424MB
node                14.2.0-alpine3.10   f9eb26ceb3c5        3 weeks ago         116MB
ubuntu              bionic              c3c304cb4f22        5 weeks ago         64.2MB
```

## Crear una contenedor

Ya podemos crear nuestra aplicación. El dockerfile para crear la imagen es el siguiente:

```txt
FROM cpp-build-base:0.1.0 AS build

WORKDIR /src

COPY CMakeLists.txt hola.cpp hola.h ./

RUN cmake . && make

FROM ubuntu:bionic

WORKDIR /opt/hola

COPY --from=build /src/hola ./

CMD ["./hola"]
```

Puntos a destacar. Etiquetamos la imagen con `AS build`:

```txt
FROM cpp-build-base:0.1.0 AS build
```

Mas adelante, a la hora de crear la imagen propiamente dicha, hacemos referencia a la etiqueta anterior para identificar desde donde obtener el código `COPY --from=build /src/hola ./`:

```txt
COPY --from=build /src/hola ./
```

Podemos ahora crear nuestro contenedor con el programa de c++:

```ps
docker build . -t hola:1.0.0
```

Podemos ver las imagenes que se han creado:

```ps
docker images

REPOSITORY          TAG                 IMAGE ID            CREATED             SIZE
<none>              <none>              dcce199f2b6a        5 seconds ago       424MB
hola                1.0.0               bf86d73193b7        4 minutes ago       64.2MB
cpp-build-base      0.1.0               64e09874cfc0        17 minutes ago      424MB
node                14.2.0-alpine3.10   f9eb26ceb3c5        3 weeks ago         116MB
ubuntu              bionic              c3c304cb4f22        5 weeks ago         64.2MB
```

En particular hay dos images:

```ps
<none>              <none>              dcce199f2b6a        5 seconds ago       424MB
hola                1.0.0               bf86d73193b7        4 minutes ago       64.2MB
```

La primera es una imagen intermedia que se ha creado para compilar nuestra imagen. La segunda es el contenedor con el programa c++. Esta imagen intermedia la podemos borrar, no se necesita para nada. Para ejecutar nuestra imagen:

```ps
docker run -it hola:1.0.0

Hola Mundo!!
```

## Scratch

La imagen que se ha creado para nuestro programa puede ser aún más pequeña. Para eso podemos usar como base _`scratch`_ en lugar de _`ubuntu:bionic`_. La imagen `scratch` es minima, por lo tanto necesitamos que todas las dependencias que el ejecutable precisa están incluidas en la salida de la compilación. Para esto lo primero que tenemos que hacer es que la compilación sea estática. Actualizamos el `CMAKELists.txt`:

```txt
cmake_minimum_required (VERSION 3.8)

project ("hola")

# Add source to this project's executable.
SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
SET(BUILD_SHARED_LIBS OFF)
SET(CMAKE_EXE_LINKER_FLAGS "-static")
add_executable (hola "hola.cpp" "hola.h")
```

La parte clave es:

```txt
SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
SET(BUILD_SHARED_LIBS OFF)
SET(CMAKE_EXE_LINKER_FLAGS "-static")
```

Esto hace que nuestro programa incluya todas las dependencias. Con esto lo siguiente sera actualizar el dockerfile:

```txt
FROM cpp-build-base:0.1.0 AS build

WORKDIR /src

COPY CMakeLists.txt hola.cpp hola.h ./

RUN cmake . && make

FROM scratch

WORKDIR /opt/hola

COPY --from=build /src/hola ./

CMD ["./hola"]
```

Notese que se utiliza `FROM scratch`.

Compilamos nuestra imagen:

```ps
docker build . -t hola:2.0.0
```

Veamos las imagenes:

```ps
docker images                                                                                                            

REPOSITORY          TAG                 IMAGE ID            CREATED              SIZE
hola                1.0.0               0787ce7d9fed        19 seconds ago       64.2MB
hola                2.0.0               87054d311c19        About a minute ago   2.25MB
cpp-build-base      0.1.0               64e09874cfc0        About an hour ago    424MB
node                14.2.0-alpine3.10   f9eb26ceb3c5        3 weeks ago          116MB
ubuntu              bionic              c3c304cb4f22        5 weeks ago          64.2MB
```

Podemos ver como el tamaño de la imagen se reduce de `64.2MB` a `2.25MB`. Para ejecutar la imagen:

```ps
docker run -it hola:2.0.0
```

Podemos ver que 

```ps
PS [EUGENIO] >docker run -it hola:1.0.0 sh                                                                                             # ls /
bin  boot  dev  etc  home  lib  lib64  media  mnt  opt  proc  root  run  sbin  srv  sys  tmp  usr  var
# id
uid=0(root) gid=0(root) groups=0(root)
#                                 
```

Sin embargo con la imagen que hemos creado sobre scratch:

```ps
docker run -it hola:2.0.0 sh                                                                                             C:\Program 

Files\Docker\Docker\resources\bin\docker.exe: Error response from daemon: OCI runtime create failed: container_linux.go:349: starting container process caused "exec: \"sh\": executable file not found in $PATH": unknown.
```

Esto es, la imagen tiene exclusivamente nuestro código y nada más.

## Seguridad

### Digests

Cada imagen tiene un hash que nos permite asegurar que el contenedor que estamos viendo se realemente el que buscamos:

```ps
docker images --digests

REPOSITORY          TAG                 DIGEST                                                                    IMAGE ID            CREATED             SIZE
hola                1.0.0               <none>                                                                    0787ce7d9fed        34 minutes ago      64.2MB
hola                2.0.0               <none>                                                                    87054d311c19        35 minutes ago      2.25MB
cpp-build-base      0.1.0               <none>                                                                    64e09874cfc0        2 hours ago         424MB
node                14.2.0-alpine3.10   sha256:f0c9fe9c259397d68cf8f7b01dd6459fa81cf6f45cbdbda629f9608a7df91127   f9eb26ceb3c5        3 weeks ago         116MB
ubuntu              bionic              sha256:3235326357dfb65f1781dbc4df3b834546d8bf914e82cce58e6e6b676e23ce8f   c3c304cb4f22        5 weeks ago         64.2MB
```

Hasta que no subamos nuestras imagenes al registry no se les aplicará el digest. Las subimos. Primero ponemos el tag:

```ps
docker tag hola:2.0.0 egsmartin/hola:2.0.0
```

Nos aseguramos de hacer login en el registro, y hacemos un push:

```ps
docker login

Authenticating with existing credentials...
Login Succeeded
```

```ps
docker push egsmartin/hola:2.0.0

The push refers to repository [docker.io/egsmartin/hola]
503adaed0665: Pushing [==================================================>]  2.254MB                                                   01901ee9a59b: Pushed                
```

Vemos ahora la imagen:

```ps
PS [EUGENIO] >docker images --digests
REPOSITORY          TAG                 DIGEST                                                                    IMAGE ID            CREATED             SIZE
hola                1.0.0               <none>                                                                    0787ce7d9fed        42 minutes ago      64.2MB
hola                2.0.0               <none>                                                                    87054d311c19        43 minutes ago      2.25MB
egsmartin/hola      2.0.0               sha256:50c2688fe861d6a32d0d6a30bed6c232503844424e712a7ba644e486f27cb2fe   87054d311c19        43 minutes ago      2.25MB
cpp-build-base      0.1.0               <none>                                                                    64e09874cfc0        2 hours ago         424MB
node                14.2.0-alpine3.10   sha256:f0c9fe9c259397d68cf8f7b01dd6459fa81cf6f45cbdbda629f9608a7df91127   f9eb26ceb3c5        3 weeks ago         116MB
ubuntu              bionic              sha256:3235326357dfb65f1781dbc4df3b834546d8bf914e82cce58e6e6b676e23ce8f   c3c304cb4f22        5 weeks ago         64.2MB
```

### Usuario

### Privilegios 

```ps
grep Cap /proc/self/status
```